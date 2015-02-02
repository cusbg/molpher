/* 
 * File:   PathFinderActivity.h
 * Author: Krwemil
 *
 * Created on 11. říjen 2014, 15:27
 */

#include <cassert>
#include <cmath>
#include <cfloat>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <tbb/task_scheduler_init.h>
#include <tbb/tbb_exception.h>
#include <tbb/partitioner.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_sort.h>

#include <GraphMol/RDKitBase.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Substruct/SubstructMatch.h>

#include "inout.h"
#include "auxiliary/SynchRand.h"
#include "coord/ReducerFactory.h"
#include "chem/morphing/Morphing.hpp"
#include "../chem/scaffold/Scaffold.hpp"
#include "../chem/scaffold/ScaffoldDatabase.hpp"
#include "JobManager.h"
#include "PathFinderActivity.h"
#include "PaDELDescriptorCalculator.h"

PathFinderActivity::PathFinderActivity(
        tbb::task_group_context *tbbCtx, JobManager *jobManager, int threadCnt
        ) :
mTbbCtx(tbbCtx),
mJobManager(jobManager),
mThreadCnt(threadCnt) {
}

PathFinderActivity::~PathFinderActivity() {
}

bool PathFinderActivity::Cancelled() {
    return mTbbCtx->is_group_execution_cancelled();
}

PathFinderActivity::FindLeaves::FindLeaves(MoleculeVector &leaves) :
mLeaves(leaves) {
}

void PathFinderActivity::FindLeaves::operator()(
        const PathFinderContext::CandidateMap::range_type &candidates) const {
    PathFinderContext::CandidateMap::iterator it;
    for (it = candidates.begin(); it != candidates.end(); it++) {
        if (!it->second.parentSmile.empty()) {
            it->second.itersWithoutDistImprovement++;
        }
        bool isLeaf = it->second.descendants.empty();
        if (isLeaf) {
            mLeaves.push_back(it->second);
        }
    }
}

PathFinderActivity::CollectMorphs::CollectMorphs(MoleculeVector &morphs) :
mMorphs(morphs) {
    mCollectAttemptCount = 0;
}

void PathFinderActivity::CollectMorphs::operator()(const MolpherMolecule &morph) {
    ++mCollectAttemptCount; // atomic
    SmileSet::const_accessor dummy;
    if (mDuplicateChecker.insert(dummy, morph.smile)) {
        mMorphs.push_back(morph);
    } else {
        // ignore duplicate
    }
}

unsigned int PathFinderActivity::CollectMorphs::WithdrawCollectAttemptCount() {
    unsigned int ret = mCollectAttemptCount;
    mCollectAttemptCount = 0;
    return ret;
}

void MorphCollector2(MolpherMolecule *morph, void *functor) {
    PathFinderActivity::CollectMorphs *collect =
            (PathFinderActivity::CollectMorphs *) functor;
    (*collect)(*morph);
}

// return true if "a" is closes to target then "b"

bool PathFinderActivity::CompareMorphs::operator()(
        const MolpherMolecule &a, const MolpherMolecule &b) const {
    /* Morphs are rated according to their proximity to the connecting line
     between their closest decoy and the target (i.e. sum of both distances is
     minimal on the connecting line between decoy and target). When sums for
     both morphs are equal, it is possible (but not necessary) that both
     morphs lie on the same connecting line. In that case, morphs are
     rated only according to their proximity to the target. Such comparison
     should allow convergence to the target even in the late stages of the
     algorithm when majority of morphs lie on the connecting line between
     decoy closest to the target and the target itself. */

    double aSum = a.distToTarget + a.distToClosestDecoy;
    double bSum = b.distToTarget + b.distToClosestDecoy;

    bool approximatelyEqual = (
            fabs(aSum - bSum) <= (32 * DBL_EPSILON * fmax(fabs(aSum), fabs(bSum))));

    if (approximatelyEqual) {
        return a.distToTarget < b.distToTarget;
    } else {
        return aSum < bSum;
    }
}

//PathFinderActivity::FilterMorphs::FilterMorphs(PathFinderContext &ctx,
//        size_t globalMorphCount, MoleculeVector &morphs, std::vector<bool> &survivors
//        ) :
//mCtx(ctx),
//mGlobalMorphCount(globalMorphCount),
//mMorphs(morphs),
//mMorphCount(morphs.size()),
//mSurvivors(survivors) {
//    assert(mMorphs.size() == mSurvivors.size());
//}

PathFinderActivity::FilterMorphs::FilterMorphs(PathFinderContext &ctx,
        size_t globalMorphCount, MoleculeVector &morphs, std::vector<bool> &survivors,
        std::vector<bool> &next, std::vector<bool> &killedOutsideMOOP
        ) :
mCtx(ctx),
mGlobalMorphCount(globalMorphCount),
mMorphs(morphs),
mMorphCount(morphs.size()),
mSurvivors(survivors),
mNext(next),
mKilledOutsideMOOP(killedOutsideMOOP){
    assert(mMorphs.size() == mSurvivors.size());
    assert(mMorphs.size() == mNext.size());
    assert(mMorphs.size() == mKilledOutsideMOOP.size());
}

void PathFinderActivity::FilterMorphs::operator()(const tbb::blocked_range<size_t> &r) const {
    for (size_t idx = r.begin(); idx != r.end(); ++idx) {             
        
//        double acceptProbability = 1.0;
//        bool isTarget = !mCtx.ScaffoldMode() ?
//                (mMorphs[idx].smile.compare(mCtx.target.smile) == 0) :
//                (mMorphs[idx].scaffoldSmile.compare(mCtx.target.scaffoldSmile) == 0);
//        if (idx >= mCtx.params.cntCandidatesToKeep && !isTarget) {
//            acceptProbability =
//                    0.25 - (idx - mCtx.params.cntCandidatesToKeep) /
//                    ((mGlobalMorphCount - mCtx.params.cntCandidatesToKeep) * 4.0);
//        }

//        bool mightSurvive =
//                SynchRand::GetRandomNumber(0, 99) < (int) (acceptProbability * 100);
//        if (mightSurvive) {
        
        if (true) {
            bool isDead = false;
            bool badWeight = false;
            bool badSascore = false;
            bool alreadyExists = false;
            bool alreadyTriedByParent = false;
            bool tooManyProducedMorphs = false;
            bool isNotOptimal = false;
            bool isTooFarFromEtalon = false;
            
            // MOOP
            if (!isDead && mNext[idx] && !mKilledOutsideMOOP[idx]) {
                MolpherMolecule first = mMorphs[idx];
                for (size_t second_idx = 0; second_idx != mMorphCount; ++second_idx) {
                    MolpherMolecule second = mMorphs[second_idx];
                    if (first.id.compare(second.id) != 0 && mNext[second_idx]) {
                        bool check(true);
                        std::vector<double>::iterator it;
                        std::vector<double>::size_type desc_idx(0);
                        for (it = first.etalonDistances.begin(); it != first.etalonDistances.end(); it++, desc_idx++) {
//                            SynchCout(mCtx.relevantDescriptorNames[counter] + ": " + NumberToString(*it));
                            if (*it < second.etalonDistances[desc_idx]) {
                                check = false;
                                break;
                            }
                        }
                        if (check) {
                            isNotOptimal = true;
                            break;
                        }
                    }
                }
                if (isNotOptimal) {
                    SynchCout(first.id + ": dominated solution (distance: " + NumberToString(first.distToEtalon) + ")");
                }
                mNext[idx] = isNotOptimal;
            }

            // Tests are ordered according to their cost.
            // Added test for SAScore
            
            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);
            
            if (!isDead) {
                badWeight =
                        (mMorphs[idx].molecularWeight <
                        mCtx.params.minAcceptableMolecularWeight) ||
                        (mMorphs[idx].molecularWeight >
                        mCtx.params.maxAcceptableMolecularWeight);
                if (badWeight) {
                    SynchCout(mMorphs[idx].id + ": Bad weight\n");
                }
            }

            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);

            if (!isDead) {
                if (mCtx.params.useSyntetizedFeasibility) {
                    badSascore = mMorphs[idx].sascore > 6.0; // questionable, it is recommended value from Ertl
                    // in case of badSascore print message
                    if (badSascore) {
                        std::stringstream ss;
                        ss << "bad sasscore: " << mMorphs[idx].id << " - " << mMorphs[idx].smile << " : " << mMorphs[idx].sascore;
                        SynchCout( ss.str() );
                    }
                }
            }

            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);
            
            if (!isDead) {
                if (!mCtx.ScaffoldMode()) {
                    PathFinderContext::CandidateMap::const_accessor dummy;
                    if (mCtx.candidates.find(dummy, mMorphs[idx].smile)) {
                        alreadyExists = true;
                    }
                } else {
                    PathFinderContext::ScaffoldSmileMap::const_accessor dummy;
                    bool isInCandidates = mCtx.candidateScaffoldMolecules.find(dummy, mMorphs[idx].scaffoldSmile);
                    dummy.release();
                    bool isOnPath = mCtx.pathScaffoldMolecules.find(dummy, mMorphs[idx].scaffoldSmile);
                    if (isInCandidates ||
                            (isOnPath && mMorphs[idx].scaffoldSmile.compare(mCtx.target.scaffoldSmile) != 0)) {
                        alreadyExists = true;
                    }
                }
            }

            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);
            
            if (!isDead) {
                PathFinderContext::CandidateMap::const_accessor ac;
                if (mCtx.candidates.find(ac, mMorphs[idx].parentSmile)) {
                    alreadyTriedByParent = (
                            ac->second.historicDescendants.find(mMorphs[idx].smile)
                            !=
                            ac->second.historicDescendants.end());
                } else {
                    assert(false);
                }
            }
            
            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);
            
//            if (!isDead) {
//                PathFinderContext::MorphDerivationMap::const_accessor ac;
//                if (mCtx.morphDerivations.find(ac, mMorphs[idx].smile)) {
//                    tooManyProducedMorphs =
//                            (ac->second > mCtx.params.cntMaxMorphs);
//                }
//            }

            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);
            
//            if (!isDead) {
//                isTooFarFromEtalon = mMorphs[idx].distToEtalon > mCtx.params.maxAcceptableEtalonDistance;
//            }

            isDead = (badWeight || badSascore || alreadyExists || isNotOptimal || mKilledOutsideMOOP[idx] ||
                    alreadyTriedByParent || tooManyProducedMorphs || isTooFarFromEtalon);
            
            if (badWeight || badSascore || alreadyExists || 
                    alreadyTriedByParent || tooManyProducedMorphs || 
                    isTooFarFromEtalon) {
                mKilledOutsideMOOP[idx] = true;
            }
            mSurvivors[idx] = !isDead;
        }
    }
    
}

PathFinderActivity::AcceptMorphs::AcceptMorphs(
        MoleculeVector &morphs, std::vector<bool> &survivors,
        PathFinderContext &ctx, SmileSet &modifiedParents
        ) :
mMorphs(morphs),
mSurvivors(survivors),
mCtx(ctx),
mModifiedParents(modifiedParents),
mSurvivorCount(0) {
    assert(mMorphs.size() == mSurvivors.size());
}

PathFinderActivity::AcceptMorphs::AcceptMorphs(
        AcceptMorphs &toSplit, tbb::split
        ) :
mCtx(toSplit.mCtx),
mMorphs(toSplit.mMorphs),
mSurvivors(toSplit.mSurvivors),
mModifiedParents(toSplit.mModifiedParents),
mSurvivorCount(0) {
}

void PathFinderActivity::AcceptMorphs::operator()(
        const tbb::blocked_range<size_t> &r, tbb::pre_scan_tag) {
    for (size_t idx = r.begin(); idx != r.end(); ++idx) {
        if (mSurvivors[idx]) {
            ++mSurvivorCount;
        }
    }
}

void PathFinderActivity::AcceptMorphs::operator()(
        const tbb::blocked_range<size_t> &r, tbb::final_scan_tag) {
    for (size_t idx = r.begin(); idx != r.end(); ++idx) {
        if (mSurvivors[idx]) {
//            if (mSurvivorCount < mCtx.params.cntCandidatesToKeepMax) {
            if (true) {
                PathFinderContext::CandidateMap::accessor ac;

                if (!mCtx.ScaffoldMode()) {
                    mCtx.candidates.insert(ac, mMorphs[idx].smile);
                    ac->second = mMorphs[idx];
                    ac.release();
                } else {
                    PathFinderContext::ScaffoldSmileMap::accessor acScaff;
                    bool success = mCtx.candidateScaffoldMolecules.insert(
                        acScaff, mMorphs[idx].scaffoldSmile);
                    if (!success) {
                        // the scaffold morph is already in candidate tree (it is strange
                        // that it does not happen when scaffold hopping is turned off)
                        continue;
                    }
                    acScaff->second = mMorphs[idx].smile;

                    mCtx.candidates.insert(ac, mMorphs[idx].smile);
                    ac->second = mMorphs[idx];
                    ac.release();
                }

                if (mCtx.candidates.find(ac, mMorphs[idx].parentSmile)) {
                    ac->second.descendants.insert(mMorphs[idx].smile);
                    ac->second.historicDescendants.insert(mMorphs[idx].smile);
                    SmileSet::const_accessor dummy;
                    mModifiedParents.insert(dummy, ac->second.smile);
                } else {
                    assert(false);
                }
            }
            ++mSurvivorCount;
        }
    }
}

void PathFinderActivity::AcceptMorphs::reverse_join(AcceptMorphs &toJoin) {
    mSurvivorCount += toJoin.mSurvivorCount;
}

void PathFinderActivity::AcceptMorphs::assign(AcceptMorphs &toAssign) {
    mSurvivorCount = toAssign.mSurvivorCount;
}

PathFinderActivity::UpdateTree::UpdateTree(PathFinderContext &ctx) :
mCtx(ctx) {
}

void PathFinderActivity::UpdateTree::operator()(
        const SmileSet::range_type &modifiedParents) const {
    PathFinderActivity::SmileSet::iterator itParent;
    for (itParent = modifiedParents.begin();
            itParent != modifiedParents.end(); itParent++) {

        // Determine what child is the closest to the etalon.
        double minDistance = DBL_MAX;
        PathFinderContext::CandidateMap::accessor acParent;
        if (mCtx.candidates.find(acParent, itParent->first)) {

            std::set<std::string>::iterator itChild;
            for (itChild = acParent->second.descendants.begin();
                    itChild != acParent->second.descendants.end();
                    itChild++) {

                PathFinderContext::CandidateMap::const_accessor acChild;
                if (mCtx.candidates.find(acChild, (*itChild))) {
                    if (acChild->second.distToEtalon < minDistance) {
                        minDistance = acChild->second.distToEtalon;
                    }
                } else {
                    assert(false);
                }

            }

        } else {
            assert(false);
        }

        // Update the tree branch towards root.
        while ((!mCtx.ScaffoldMode() && !acParent->second.parentSmile.empty()) ||
                (mCtx.ScaffoldMode() && acParent->first.compare(mCtx.tempSource.smile) != 0)) {
            if (minDistance < acParent->second.distToEtalon) {
                acParent->second.itersWithoutDistImprovement = 0;
            }
            std::string smile = acParent->second.parentSmile;
            acParent.release();
            mCtx.candidates.find(acParent, smile);
            assert(!acParent.empty());
        }

    }
}

PathFinderActivity::PruneTree::PruneTree(PathFinderContext &ctx, SmileSet &deferred) :
mCtx(ctx),
mDeferred(deferred) {
}

void PathFinderActivity::PruneTree::operator()(
        const std::string &smile, tbb::parallel_do_feeder<std::string> &feeder) const {
    PathFinderContext::CandidateMap::accessor ac;
    mCtx.candidates.find(ac, smile);
    assert(!ac.empty());

    SmileSet::const_accessor dummy;
    bool deferred = mDeferred.find(dummy, smile);
    bool prune = (deferred ||
            (ac->second.itersWithoutDistImprovement > mCtx.params.itThreshold));
    if (prune) {

        bool tooManyDerivations = false;
        PathFinderContext::MorphDerivationMap::const_accessor acDerivations;
        if (mCtx.morphDerivations.find(acDerivations, smile)) {
            tooManyDerivations = (acDerivations->second > mCtx.params.cntMaxMorphs);
        }

        bool pruneThis = (deferred || tooManyDerivations);

        if (pruneThis) {
            PathFinderContext::CandidateMap::accessor acParent;
            mCtx.candidates.find(acParent, ac->second.parentSmile);
            assert(!acParent.empty());

            acParent->second.descendants.erase(smile);
            acParent.release();
            ac.release();

            EraseSubTree(smile);
        } else {
            std::set<std::string>::const_iterator it;
            for (it = ac->second.descendants.begin();
                    it != ac->second.descendants.end(); it++) {
                EraseSubTree(*it);
            }
            ac->second.descendants.clear();
            ac->second.itersWithoutDistImprovement = 0;
        }

    } else {
        std::set<std::string>::const_iterator it;
        for (it = ac->second.descendants.begin();
                it != ac->second.descendants.end(); it++) {
            feeder.add(*it);
        }
    }
}

void PathFinderActivity::PruneTree::EraseSubTree(const std::string &root) const {
    std::deque<std::string> toErase;
    toErase.push_back(root);

    while (!toErase.empty()) {
        std::string current = toErase.front();
        toErase.pop_front();

        PathFinderContext::CandidateMap::accessor ac;
        mCtx.candidates.find(ac, current);
        assert(!ac.empty());

        std::set<std::string>::const_iterator it;
        for (it = ac->second.descendants.begin();
                it != ac->second.descendants.end(); it++) {
            toErase.push_back(*it);
        }

        mCtx.prunedDuringThisIter.push_back(current);
        if (mCtx.ScaffoldMode()) {
            bool success = mCtx.candidateScaffoldMolecules.erase(
                    ac->second.scaffoldSmile);
            assert(success);
        }
        mCtx.candidates.erase(ac);
    }
}

PathFinderActivity::AccumulateTime::AccumulateTime(PathFinderContext &ctx) :
mCtx(ctx) {
    mTimestamp = std::clock();
}

unsigned int PathFinderActivity::AccumulateTime::GetElapsedSeconds(bool reset) {
    clock_t current = std::clock();
    unsigned int seconds =
            (unsigned int) ((current - mTimestamp) / CLOCKS_PER_SEC);
    if (reset) {
        mTimestamp = current;
    }
    return seconds;
}

void PathFinderActivity::AccumulateTime::ReportElapsedMiliseconds(
        const std::string &consumer, bool reset) {
    clock_t current = std::clock();
#if PATHFINDER_REPORTING == 1
    std::ostringstream stream;
    stream << mCtx.jobId << "/" << mCtx.iterIdx + 1 << ": " <<
            consumer << " consumed " << current - mTimestamp << " msec.";
    SynchCout(stream.str());
#endif
    if (reset) {
        mTimestamp = current;
    }
}

void PathFinderActivity::AccumulateTime::Reset() {
    mTimestamp = std::clock();
}

/**
 * Accept single morph with given index do not control anything.
 * @param idx Index or morph to accept.
 * @param morphs List of morphs.
 * @param ctx Context.
 * @param modifiedParents Parent to modify.
 *
 * Unused method (scaffold hopping is not implemented here).
 */
void acceptMorph2(
        size_t idx,
        PathFinderActivity::MoleculeVector &morphs,
        PathFinderContext &ctx,
        PathFinderActivity::SmileSet &modifiedParents) {
    PathFinderContext::CandidateMap::accessor ac;
    ctx.candidates.insert(ac, morphs[idx].smile);
    ac->second = morphs[idx];
    ac.release();

    if (ctx.candidates.find(ac, morphs[idx].parentSmile)) {
        ac->second.descendants.insert(morphs[idx].smile);
        ac->second.historicDescendants.insert(morphs[idx].smile);
        PathFinderActivity::SmileSet::const_accessor dummy;
        modifiedParents.insert(dummy, ac->second.smile);
    } else {
        assert(false);
    }
}

/**
 * Accept morphs from list. If there is no decoy the PathFinder::AcceptMorphs is
 * used. Otherwise for each decoy the same number of best candidates is accepted.
 * @param morphs Candidates.
 * @param survivors Survive index.
 * @param ctx Context.
 * @param modifiedParents
 * @param decoySize Number of decoy used during exploration.
 */
void acceptMorphs2(PathFinderActivity::MoleculeVector &morphs,
        std::vector<bool> &survivors,
        PathFinderContext &ctx,
        PathFinderActivity::SmileSet &modifiedParents,
        int decoySize) {
    PathFinderActivity::AcceptMorphs acceptMorphs(morphs, survivors, ctx, modifiedParents);
    // FIXME
    // Current TBB version does not support parallel_scan cancellation.
    // If it will be improved in the future, pass task_group_context
    // argument similarly as in parallel_for.
    tbb::parallel_scan(
            tbb::blocked_range<size_t>(0, morphs.size()),
            acceptMorphs, tbb::auto_partitioner());
    SynchCout("Acceptance ratio (iteration #" 
            + NumberToString(ctx.iterIdx + 1) + "): " 
            + NumberToString(acceptMorphs.mSurvivorCount) + "/" 
            + NumberToString(morphs.size()) + ".");
    return;
}

void PathFinderActivity::operator()() {
    SynchCout(std::string("PathFinder thread started."));

    tbb::task_scheduler_init scheduler;
    if (mThreadCnt > 0) {
        scheduler.terminate();
        scheduler.initialize(mThreadCnt);
    }

    bool canContinueCurrentJob = false;
    bool pathFound = false;
    std::vector<std::string> startMols;
    
    while (true) {

        if (!canContinueCurrentJob) {
            if (mJobManager->GetJob(mCtx)) {
                canContinueCurrentJob = true;
                pathFound = false;

                // Initialize the first iteration of a job.
                if (mCtx.candidates.empty()) {
                    assert(mCtx.iterIdx == 0);
                    assert(mCtx.candidateScaffoldMolecules.empty());
                    
                    if (mCtx.params.startMolMaxCount == 0) {
                        mCtx.params.startMolMaxCount = mCtx.actives.size();
                    }

                    PathFinderContext::CandidateMap::accessor ac;
                    if (!mCtx.ScaffoldMode()) {
                        int counter = 1;
                        for ( 
                                PathFinderContext::CandidateMap::const_iterator it = mCtx.actives.begin();
                                it != mCtx.actives.end(); it++
                                ) {
                            mCtx.candidates.insert(ac, it->first);
                            startMols.push_back(it->first);
                            //SynchCout(it->first);
                            ac->second = it->second;
                            
                            ++counter;
                            if (counter > mCtx.params.startMolMaxCount) {
                                break;
                            }
                        }
                        //mCtx.candidates.insert(ac, mCtx.source.smile);
                        //ac->second = mCtx.source;
                    } else {
                        assert(mCtx.scaffoldSelector == SF_MOST_GENERAL);

                        Scaffold *scaff = ScaffoldDatabase::Get(mCtx.scaffoldSelector);

                        std::string scaffSource;
                        scaff->GetScaffold(mCtx.source.smile, &scaffSource);
                        mCtx.tempSource.scaffoldSmile = scaffSource;
                        std::string scaffTarget;
                        scaff->GetScaffold(mCtx.target.smile, &scaffTarget);
                        mCtx.target.scaffoldSmile = scaffTarget;

                        mCtx.candidates.insert(ac, mCtx.tempSource.smile);
                        ac->second = mCtx.tempSource;

                        PathFinderContext::ScaffoldSmileMap::accessor acScaff;
                        mCtx.candidateScaffoldMolecules.insert(acScaff, scaffSource);
                        acScaff->second = mCtx.source.smile;
                        acScaff.release();

                        mCtx.pathScaffoldMolecules.insert(acScaff, scaffSource);
                        acScaff->second = mCtx.source.smile;
                        acScaff.release();
                        mCtx.pathScaffoldMolecules.insert(acScaff, scaffTarget);
                        acScaff->second = mCtx.target.smile;

                        std::string scaffDecoy;
                        std::vector<MolpherMolecule>::iterator it;
                        for (it = mCtx.decoys.begin(); it != mCtx.decoys.end(); ++it) {
                            scaff->GetScaffold(it->smile, &scaffDecoy);
                            it->scaffoldSmile = scaffDecoy;
                            it->scaffoldLevelCreation = mCtx.scaffoldSelector;
                        }

                        delete scaff;
                    }
                }
            } else {
                break; // Thread termination.
            }
        }

        try {

            if (!Cancelled()) {
                mJobManager->GetFingerprintSelector(mCtx.fingerprintSelector);
                mJobManager->GetSimCoeffSelector(mCtx.simCoeffSelector);
                mJobManager->GetDimRedSelector(mCtx.dimRedSelector);
                mJobManager->GetChemOperSelectors(mCtx.chemOperSelectors);
                mJobManager->GetParams(mCtx.params);
                mJobManager->GetDecoys(mCtx.decoys);
                mCtx.prunedDuringThisIter.clear();
            }

            AccumulateTime molpherStopwatch(mCtx);
            AccumulateTime stageStopwatch(mCtx);

            MoleculeVector leaves;
            FindLeaves findLeaves(leaves);
            if (!Cancelled()) {
                tbb::parallel_for(
                        PathFinderContext::CandidateMap::range_type(mCtx.candidates),
                        findLeaves, tbb::auto_partitioner(), *mTbbCtx);
                stageStopwatch.ReportElapsedMiliseconds("FindLeaves", true);
            }

            /* TODO MPI
             MASTER
             prepare light snapshot (PathFinderContext::ContextToLightSnapshot)
             broadcast light snapshot
             convert leaves to std::vector
             scatter leaves over cluster
             convert node-specific part back to MoleculeVector

             SLAVE
             broadcast light snapshot
             convert light snapshot into context (PathFinderContext::SnapshotToContext)
             scatter leaves over cluster
             convert node-specific part back to MoleculeVector
             */

            MoleculeVector morphs;
            CollectMorphs collectMorphs(morphs);
            Scaffold *scaff = mCtx.ScaffoldMode() ?
                    ScaffoldDatabase::Get(mCtx.scaffoldSelector) : NULL;
            std::vector<ChemOperSelector> chemOperSelectors =
                    !mCtx.ScaffoldMode() || mCtx.scaffoldSelector == SF_ORIGINAL_MOLECULE ?
                        mCtx.chemOperSelectors : scaff->GetUsefulOperators();
            for (MoleculeVector::iterator it = leaves.begin(); it != leaves.end(); it++) {
                MolpherMolecule &candidate = (*it);
                unsigned int morphAttempts = mCtx.params.cntMorphs;
//                if (candidate.distToTarget < mCtx.params.distToTargetDepthSwitch) {
//                    morphAttempts = mCtx.params.cntMorphsInDepth;
//                }

                if (!Cancelled()) {
                    morphs.reserve(morphs.size() + morphAttempts);

                    GenerateMorphs(
                            candidate,
                            morphAttempts,
//                            mCtx.fingerprintSelector,
//                            mCtx.simCoeffSelector,
                            chemOperSelectors,
//                            mCtx.target,
                            mCtx.decoys,
                            *mTbbCtx,
                            &collectMorphs,
                            MorphCollector2,
                            scaff);
                    PathFinderContext::MorphDerivationMap::accessor ac;

                    if (mCtx.morphDerivations.find(ac, candidate.smile)) {
                        ac->second += collectMorphs.WithdrawCollectAttemptCount();
                    } else {
                        mCtx.morphDerivations.insert(ac, candidate.smile);
                        ac->second = collectMorphs.WithdrawCollectAttemptCount();
                    }
                }

                if (Cancelled()) {
                    break;
                }
            }
            delete scaff;
            morphs.shrink_to_fit();

            if (!Cancelled()) {
                stageStopwatch.ReportElapsedMiliseconds("GenerateMorphs", true);
            }
            
            // prepare directory for descriptor computation
            std::string output_dir(mJobManager->GetStorageDir());
            std::string storage_dir(GenerateDirname(output_dir, mCtx.jobId, mCtx.proteinTargetName + "_" + NumberToString(mCtx.iterIdx)));
            try {
                boost::filesystem::create_directories(storage_dir);
            } catch (boost::filesystem::filesystem_error &exc) {
                SynchCout(exc.what());
            }      
            
            PaDELdesc::PaDELDescriptorCalculator calculator(
                "../dependencies/padel/"
                , storage_dir
                , mCtx.relevantDescriptorNames
            );
            
            unsigned int counter(1);
            for (MoleculeVector::iterator it = morphs.begin(); it != morphs.end(); it++) {
                MolpherMolecule &morph = (*it);
                morph.id = "MORPH_" + NumberToString(mCtx.iterIdx) + "_" + NumberToString(counter++);
                calculator.addMol(morph.id, morph.smile);
                morph.descriptorsFilePath = calculator.getOutputFilePath();
                morph.relevantDescriptorNames = mCtx.relevantDescriptorNames;
            }
            
            // compute descriptors using PaDEL
            calculator.computeDescriptors();
            
            // load and normalize the data + compute etalon distances 
            // TODO: could be concurrent (use the tbb::concurrent_* structures in PathFinderContext) -> concurrent file IO needed too
            for (MoleculeVector::iterator it = morphs.begin(); it != morphs.end(); it++) {
                MolpherMolecule &morph = (*it);
                morph.SaveDescriptors(calculator.getDescValues(morph.id));
                morph.normalizeDescriptors(mCtx.normalizationCoefficients);
                morph.ComputeEtalonDistances(mCtx.etalonValues, mCtx.activesDescriptors[0]);
            }
            
            if (!Cancelled()) {
                stageStopwatch.ReportElapsedMiliseconds("ComputeDescriptors", true);
            }
            
//            CompareMorphs compareMorphs;
//            if (!Cancelled()) {
//                /* FIXME
//                 Current TBB version does not support parallel_sort cancellation.
//                 If it will be improved in the future, pass task_group_context
//                 argument similarly as in parallel_for. */
//                tbb::parallel_sort(
//                        morphs.begin(), morphs.end(), compareMorphs);
//                stageStopwatch.ReportElapsedMiliseconds("SortMorphs", true);
//            }

            /* TODO MPI
             MASTER
             gather snapshots from slaves
             update mCtx.morphDerivations according to gathered snapshots (consider using TBB for this)
             convert morphs to std::vector
             gather other morphs from slaves
               - each vector is pre-sorted and without duplicates
             integrate all morph vectors into final vector (consider using TBB for this)
               - check for cross-vector duplicates
               - merge sort

             SLAVE
             convert context to full snapshot (PathFinderContext::ContextToSnapshot)
             gather snapshot to master
             convert morphs to std::vector
             gather morphs back to master
             */

            /* TODO MPI
             MASTER
             prepare full snapshot (PathFinderContext::ContextToSnapshot)
             broadcast snapshot
             broadcast morph vector complete size
             scatter morph vector over cluster
             convert node-specific part back to MoleculeVector

             SLAVE
             broadcast snapshot
             convert snapshot into context (PathFinderContext::SnapshotToContext)
             broadcast morph vector complete size
             scatter morph vector over cluster
             convert node-specific part back to MoleculeVector
             */

            // filtering now solves an MOOP to mark survivors
            std::vector<bool> survivors;
            std::vector<bool> next; // morphs scheduled for next MOOP run
            std::vector<bool> killedOutsideMOOP; // morphs scheduled for next MOOP run
            survivors.resize(morphs.size(), true);
            next.resize(morphs.size(), true);
            killedOutsideMOOP.resize(morphs.size(), false);
            FilterMorphs filterMorphs(mCtx, morphs.size(), morphs, survivors, next, killedOutsideMOOP);
            
            if (!Cancelled()) {
                if (mCtx.params.useSyntetizedFeasibility) {
                    SynchCout("\tUsing syntetize feasibility");
                }
                unsigned int counter = 0;
                while (mCtx.params.maxMOOPruns > counter) {
                    SynchCout("MOOP run #" + NumberToString(counter + 1));
                    tbb::parallel_for(
                        tbb::blocked_range<size_t>(0, morphs.size()),
                        filterMorphs, tbb::auto_partitioner(), *mTbbCtx);              
                    stageStopwatch.ReportElapsedMiliseconds("FilterMorphs", true);
                    unsigned int next_c = 0;
                    unsigned int accepted_c = 0;
                    unsigned int killed_outside_MOOP_c = 0;
                    for (unsigned int idx = 0; idx != next.size(); idx++) {
                        if (next[idx]) ++next_c;
                        if (survivors[idx]) ++accepted_c;
                        if (killedOutsideMOOP[idx]) ++killed_outside_MOOP_c;
                    }
                    SynchCout("Next MOOP run: " + NumberToString(next_c));
                    SynchCout("Killed outside MOOP: " + NumberToString(killed_outside_MOOP_c));
                    SynchCout("Survivors overall: " + NumberToString(accepted_c));
                    if (next_c == 0) break;
                    ++counter;
                }
            }

            /* TODO MPI
             MASTER
             gather survivors vector from slaves

             SLAVE
             gather survivors vector back to master
             */

            // Now we need to accept morphs ie. move the lucky one from
            // morphs -> survivors
            SmileSet modifiedParents;
            acceptMorphs2(morphs, survivors, mCtx, modifiedParents, mCtx.decoys.size());
            stageStopwatch.ReportElapsedMiliseconds("AcceptMorphs", true);

            UpdateTree updateTree(mCtx);
            if (!Cancelled()) {
                tbb::parallel_for(SmileSet::range_type(modifiedParents),
                        updateTree, tbb::auto_partitioner(), *mTbbCtx);
                stageStopwatch.ReportElapsedMiliseconds("UpdateTree", true);
            }

//            if (!Cancelled()) {
//                if (!mCtx.ScaffoldMode()) {
//                    PathFinderContext::CandidateMap::const_accessor acTarget;
//                    mCtx.candidates.find(acTarget, mCtx.target.smile);
//                    pathFound = !acTarget.empty();
//                } else {
//                    PathFinderContext::ScaffoldSmileMap::const_accessor acTarget;
//                    mCtx.candidateScaffoldMolecules.find(acTarget, mCtx.target.scaffoldSmile);
//                    pathFound = !acTarget.empty();
//                }
//                if (pathFound) {
//                    std::stringstream ss;
//                    ss << mCtx.jobId << "/" << mCtx.iterIdx + 1 << ": ";
//                    !mCtx.ScaffoldMode() ?
//                            ss << "- - - Path has been found - - -" :
//                            ss << "- - - Subpath has been found - - -";
//                    SynchCout(ss.str());
//                }
//            }

            SmileSet deferredSmiles;
            PruneTree pruneTree(mCtx, deferredSmiles);
            if (!Cancelled()) {
                // Prepare deferred visual pruning.
                std::vector<MolpherMolecule> deferredMols;
                mJobManager->GetPruned(deferredMols);
                std::vector<MolpherMolecule>::iterator it;
                for (it = deferredMols.begin(); it != deferredMols.end(); it++) {
                    SmileSet::const_accessor dummy;
                    if (it->smile == mCtx.source.smile ||
                            (mCtx.ScaffoldMode() && it->smile == mCtx.tempSource.smile)) {
                        continue;
                    }
                    deferredSmiles.insert(dummy, it->smile);
                }
                deferredMols.clear();

//                pruningQueue.push_back(!mCtx.ScaffoldMode() ?
//                        mCtx.source.smile : mCtx.tempSource.smile);
                for (std::vector<std::string>::iterator sourceIt = startMols.begin();
                        sourceIt != startMols.end(); sourceIt++) {
                    SmileVector pruningQueue;
                    pruningQueue.push_back(*sourceIt);
                    tbb::parallel_do(
                            pruningQueue.begin(), pruningQueue.end(), pruneTree, *mTbbCtx);
                    assert(!mCtx.ScaffoldMode() ?
                            true :
                            mCtx.candidates.size() == mCtx.candidateScaffoldMolecules.size());
                }
                stageStopwatch.ReportElapsedMiliseconds("PruneTree", true);
            }

            // calculation of dimension reduction
//            if (!Cancelled() && mCtx.params.useVisualisation) {
//                DimensionReducer::MolPtrVector molsToReduce;
//                int numberOfMolsToReduce = 0;
//                if (!mCtx.ScaffoldMode()) {
//                    numberOfMolsToReduce = mCtx.candidates.size() +
//                            mCtx.decoys.size() + 2;
//                } else {
//                    numberOfMolsToReduce = mCtx.candidates.size() +
//                            mCtx.decoys.size() + mCtx.pathMolecules.size() + 3;
//                }
//                molsToReduce.reserve(numberOfMolsToReduce);
//                // add all candidate
//                PathFinderContext::CandidateMap::iterator itCandidates;
//                for (itCandidates = mCtx.candidates.begin();
//                        itCandidates != mCtx.candidates.end(); itCandidates++) {
//                    molsToReduce.push_back(&itCandidates->second);
//                }
//                // add all decoys
//                std::vector<MolpherMolecule>::iterator itDecoys;
//                for (itDecoys = mCtx.decoys.begin();
//                        itDecoys != mCtx.decoys.end(); itDecoys++) {
//                    molsToReduce.push_back(&(*itDecoys));
//                }
//                molsToReduce.push_back(&mCtx.source);
//                molsToReduce.push_back(&mCtx.target);
//                if (mCtx.ScaffoldMode()) {
//                    std::vector<MolpherMolecule>::iterator itPathMols;
//                    for (itPathMols = mCtx.pathMolecules.begin();
//                            itPathMols != mCtx.pathMolecules.end(); itPathMols++) {
//                        molsToReduce.push_back(&(*itPathMols));
//                    }
//                    molsToReduce.push_back(&mCtx.tempSource);
//                }
//                // reduce ..
//                DimensionReducer *reducer =
//                        ReducerFactory::Create(mCtx.dimRedSelector);
//                reducer->Reduce(molsToReduce,
//                        mCtx.fingerprintSelector, mCtx.simCoeffSelector, *mTbbCtx);
//                ReducerFactory::Recycle(reducer);
//
//                stageStopwatch.ReportElapsedMiliseconds("DimensionReduction", true);
//            }

            if (!Cancelled()) {
                // find the closes molecule
                double distance = DBL_MAX;
                PathFinderContext::CandidateMap::iterator itCandidates;
                std::string bestID;
                for (itCandidates = mCtx.candidates.begin();
                        itCandidates != mCtx.candidates.end(); itCandidates++) {
                    if (itCandidates->second.distToEtalon < distance) {
                        distance = itCandidates->second.distToEtalon;
                        bestID = itCandidates->second.id;
                    }

//                    if (itCandidates->second.distToEtalon == 0) {
//                        std::stringstream ss;
//                        ss << mCtx.jobId << "/" << mCtx.iterIdx + 1 << ": "
//                                << "Zero distance: " << itCandidates->second.smile;
//                        SynchCout(ss.str());
//                    }
                }
                std::stringstream ss;
                ss << mCtx.jobId << "/" << mCtx.iterIdx + 1 << ": "
                        << "The min. distance to etalon: " << distance
                        << " (" << bestID << ")";
                SynchCout(ss.str());
            }

            if (!Cancelled()) {
                mCtx.iterIdx += 1;
                mCtx.elapsedSeconds += molpherStopwatch.GetElapsedSeconds();

                if (canContinueCurrentJob) {
                    bool itersDepleted = (mCtx.params.cntIterations <= mCtx.iterIdx);
                    bool timeDepleted = (mCtx.params.timeMaxSeconds <= mCtx.elapsedSeconds);
                    canContinueCurrentJob = (!itersDepleted && !timeDepleted);

                    if (itersDepleted) {
                        std::stringstream ss;
                        ss << mCtx.jobId << "/" << mCtx.iterIdx + 1 << ": "
                                << "The max number of iterations has been reached.";
                        SynchCout(ss.str());
                    }
                    if (timeDepleted) {
                        std::stringstream ss;
                        ss << mCtx.jobId << "/" << mCtx.iterIdx + 1 << ": "
                                << "We run out of time.";
                        SynchCout(ss.str());
                    }
                    if (!canContinueCurrentJob) {
                        IterationSnapshot snp;
                        PathFinderContext::ContextToSnapshot(mCtx, snp);
                        MoleculeVector final_leaves;
                        FindLeaves findFinalLeaves(final_leaves);
                        if (!Cancelled()) {
                            tbb::parallel_for(
                                    PathFinderContext::CandidateMap::range_type(mCtx.candidates),
                                    findFinalLeaves, tbb::auto_partitioner(), *mTbbCtx);
                            stageStopwatch.ReportElapsedMiliseconds("FindFinalLeaves", true);
                        }
                        for (MoleculeVector::iterator it = final_leaves.begin(); it != final_leaves.end(); it++) {
                            MolpherMolecule &leaf_mol = (*it);
                            std::string mol_name(leaf_mol.id);
                            std::string filename = GenerateFilename(output_dir, 
                                    mCtx.jobId, mol_name + ".path");
                            WriteMolpherPath(
                                    filename,
                                    leaf_mol.smile,
                                    snp.candidates,
                                    mCtx.pathMolecules
                                );
                        }
                    }
                }
            }

        } catch (tbb::tbb_exception &exc) {
            SynchCout(std::string(exc.what()));
            canContinueCurrentJob = false;
        }

        canContinueCurrentJob = mJobManager->CommitIteration(
                mCtx, canContinueCurrentJob, pathFound);

    }

    SynchCout(std::string("PathFinder thread terminated."));
}
