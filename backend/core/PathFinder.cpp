/*
 Copyright (c) 2012 Petr Koupy

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <cmath>
#include <cfloat>
#include <string>
#include <sstream>

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
#include "chem/morphing/Morphing.hpp"
#include "PathFinder.h"
#include "chem/morphing/CalculateDistances.hpp"
#include "chem/morphing/FilterScript.hpp"

#include "FindLeaves.hpp"
#include "CollectMorphs.hpp"
#include "CompareMorphs.hpp"
#include "FilterMorphs.hpp"
#include "AcceptMorphs.hpp"
#include "UpdateTree.hpp"

#include "script_execution.hpp"

typedef tbb::concurrent_vector<MolpherMolecule> MoleculeVector;
typedef tbb::concurrent_vector<std::string> SmileVector;
typedef tbb::concurrent_hash_map<std::string, bool /*dummy*/> SmileSet;

//
// MOOP operation.
//

/**
 *
 * @param molecules
 * @param survivors If false molecule is ignored.
 * @return True for molecules that are Pareto optimal.
 */
std::vector<bool> moop(MoleculeVector& molecules, std::vector<bool>& active)
{

}

void moop(MoleculeVector& molecules, std::vector<bool>& survivors,
        int moopRuns)
{
    // True for molecules used in MOOP.
    std::vector<bool> actives(survivors);
    // The list of optimal (sub-)molecules.
    std::vector<bool> pass(survivors.size(), false);

    for (int i = 0; i < moopRuns; ++i) {
        std::vector<bool> optimal = moop(molecules, actives);
        for (int i = 0; i < survivors.size(); ++i) {
            // For next run disabled accepted in this run.
            actives[i] = actives[i] && !optimal[i];
            // Add accepted to the result.
            pass[i] = pass[i] || optimal[i];
        }
    }
    // Update survivors.
    for (int i = 0; i < survivors.size(); ++i) {
        survivors[i] = survivors[i] && pass[i];
    }
}

class MoopFilter
{
public:

    MoopFilter(MoleculeVector& morphs,
            std::vector<bool>& actives,
            std::vector<bool>& pass)
    : mMorphs(morphs), mActives(actives), mPass(pass)
    {

    }

    operator()(const tbb::blocked_range<size_t> &r) const
    {
        for (size_t idx = r.begin(); idx != r.end(); ++idx) {
            if (!mActives[idx]) {
                continue;
            }
            bool isOptimal = true;
            MolpherMolecule* left = &mMorphs[idx];
            for (int right_idx = 0; right_idx < mMorphs.size(); ++right_idx) {
                if (!mActives[right_idx]) {
                    continue;
                }
                MolpherMolecule* right = &mMorphs[right_idx];
                if (left == right) {
                    continue;
                }
                int bad = 0;
                int equal = 0;
                // Check if we are dominated by someone.
                auto right_value = right->descriptors.begin();
                for (auto left_value = left->descriptors.begin();
                        left_value != left->descriptors.end();
                        ++left_value) {
                    if (*left_value >= *right_value) {
                        ++bad;
                        if (*left_value == *right_value) {
                            ++equal;
                        }
                    }
                    ++right_value;
                }
                int all = left->descriptors.size();
                isOptimal = !(all == bad && all != equal);
                if (!isOptimal) {
                    break;
                }
            }
            // We need molecule to be optimal in order to pass the filter.
            mPass[idx] = isOptimal;
        }
    }
    /**
     * Molecules to filter.
     */
    MoleculeVector& mMorphs;
    /**
     * If true molecule is used in another run of MOOP.
     */
    std::vector<bool>& mActives;
    /**
     * True for all (sub-)optimal molecules.
     * The default value is false.
     */
    std::vector<bool>& mPass;
};

/**
 * We need to keep data in parallel ready structures.
 */
struct PathFinderContext
{
public:
    typedef tbb::concurrent_hash_map<std::string, MolpherMolecule> CandidateMap;
    typedef tbb::concurrent_hash_map<std::string, unsigned int> MorphDerivationMap;
    typedef tbb::concurrent_vector<std::string> PrunedMoleculeVector;
public:
    CandidateMap candidates;
    MorphDerivationMap morphDerivations;
    PrunedMoleculeVector prunedDuringThisIter;
    IterationSnapshot &mSnp;
public:
    PathFinderContext(IterationSnapshot &snp);
    /**
     * Save content of the iteration.
     */
    void save() const;
};

PathFinderContext::PathFinderContext(IterationSnapshot &snp) : mSnp(snp)
{
    CandidateMap::accessor ac;
    for (auto iter = snp.sourceMols.begin();
            iter != snp.sourceMols.end(); ++iter) {
        candidates.insert(ac, iter->second.smiles);
        ac->second = iter->second;
    }

    for (auto it = snp.morphDerivations.begin();
            it != snp.morphDerivations.end(); it++) {
        morphDerivations.insert(*it);
    }

    for (auto it = snp.prunedDuringThisIter.begin();
            it != snp.prunedDuringThisIter.end(); it++) {
        prunedDuringThisIter.push_back(*it);
    }
}

void PathFinderContext::save() const
{
    // Update the iteration snapshot.

    mSnp.candidates.clear();
    for (auto iter = candidates.begin(); iter != candidates.end(); iter++) {
        mSnp.candidates.insert(*iter);
    }

    mSnp.morphDerivations.clear();
    for (auto iter = morphDerivations.begin();
            iter != morphDerivations.end(); iter++) {
        mSnp.morphDerivations.insert(*iter);
    }

    mSnp.prunedDuringThisIter.clear();
    for (auto iter = prunedDuringThisIter.begin();
            iter != prunedDuringThisIter.end(); iter++) {
        mSnp.prunedDuringThisIter.push_back(*iter);
    }

    //

    std::string fileName = GenerateFilename(
            mSnp.storagePath, mSnp.iterIdx);
    WriteSnapshotToFile(fileName, mSnp);
}

//
// @TODO Move to other files.
//

//class PruneTree
//{
//public:
//    PruneTree(PathFinderContext &ctx, SmileSet &deferred);
//    void operator()(const std::string &smiles,
//            tbb::parallel_do_feeder<std::string> &feeder) const;
//protected:
//    /**
//     * Delete given molecule and all molecules in a subtree.
//     *
//     * @param root
//     */
//    void EraseSubTree(const std::string &root) const;
//private:
//    PathFinderContext &mCtx;
//    /**
//     * ??
//     */
//    SmileSet &mDeferred;
//    /**
//     * Number of iterations for which molecule is protected.
//     */
//    int iterationThreshold;
//};
//
//PruneTree::PruneTree(PathFinderContext &ctx)
//: mCtx(ctx)
//{
//
//}
//
//void PruneTree::operator()(const std::string &smiles,
//        tbb::parallel_do_feeder<std::string> &feeder) const
//{
//    PathFinderContext::CandidateMap::accessor ac;
//    mCtx.candidates.find(ac, smiles);
//    assert(!ac.empty());
//
//    SmileSet::const_accessor dummy;
//    bool prune = ac->second.itersWithoutDistImprovement > iterationThreshold;
//    if (prune) {
//
//        bool tooManyDerivations = false;
//        PathFinderContext::MorphDerivationMap::const_accessor acDerivations;
//        if (mCtx.morphDerivations.find(acDerivations, smile)) {
//            tooManyDerivations = (acDerivations->second > mCtx.params.cntMaxMorphs);
//        }
//
//        bool pruneThis = (deferred || tooManyDerivations);
//
//        if (pruneThis) {
//            PathFinderContext::CandidateMap::accessor acParent;
//            mCtx.candidates.find(acParent, ac->second.parentSmile);
//            assert(!acParent.empty());
//
//            acParent->second.descendants.erase(smile);
//            acParent.release();
//            ac.release();
//
//            EraseSubTree(smile);
//        } else {
//            std::set<std::string>::const_iterator it;
//            for (it = ac->second.descendants.begin();
//                    it != ac->second.descendants.end(); it++) {
//                EraseSubTree(*it);
//            }
//            ac->second.descendants.clear();
//            ac->second.itersWithoutDistImprovement = 0;
//        }
//
//    } else {
//        std::set<std::string>::const_iterator it;
//        for (it = ac->second.descendants.begin();
//                it != ac->second.descendants.end(); it++) {
//            feeder.add(*it);
//        }
//    }
//}
//
//void PruneTree::EraseSubTree(const std::string &root) const
//{
//    std::deque<std::string> toErase;
//    toErase.push_back(root);
//
//    while (!toErase.empty()) {
//        std::string current = toErase.front();
//        toErase.pop_front();
//
//        PathFinderContext::CandidateMap::accessor ac;
//        mCtx.candidates.find(ac, current);
//        assert(!ac.empty());
//
//        std::set<std::string>::const_iterator it;
//        for (it = ac->second.descendants.begin();
//                it != ac->second.descendants.end(); it++) {
//            toErase.push_back(*it);
//        }
//
//        mCtx.prunedDuringThisIter.push_back(current);
//        mCtx.candidates.erase(ac);
//    }
//}

//
// @TODO Move to other files.
//

class AccumulateTime
{
public:
    AccumulateTime(IterationSnapshot &snp);
    unsigned int GetElapsedSeconds(bool reset = false);
    void ReportElapsedMiliseconds(
            const std::string &consumer, bool reset = false);
    void Reset();

private:
    IterationSnapshot &mSnp;
    clock_t mTimestamp;
};

AccumulateTime::AccumulateTime(IterationSnapshot &ctx) :
mSnp(ctx)
{
    mTimestamp = std::clock();
}

unsigned int AccumulateTime::GetElapsedSeconds(bool reset)
{
    clock_t current = std::clock();
    unsigned int seconds =
            (unsigned int) ((current - mTimestamp) / CLOCKS_PER_SEC);
    if (reset) {
        mTimestamp = current;
    }
    return seconds;
}

void AccumulateTime::ReportElapsedMiliseconds(
        const std::string &consumer, bool reset)
{
    clock_t current = std::clock();
    std::ostringstream stream;
    stream << mSnp.iterIdx + 1 << ": " <<
            consumer << " consumed " << current - mTimestamp << " msec.";
    SynchCout(stream.str());
    if (reset) {
        mTimestamp = current;
    }
}

void AccumulateTime::Reset()
{
    mTimestamp = std::clock();
}

//
//
//

PathFinder::PathFinder(
        tbb::task_group_context *tbbCtx, IterationSnapshot &iterationSnapshot, int threadCnt
        ) :
mTbbCtx(tbbCtx),
mSnp(iterationSnapshot),
mThreadCnt(threadCnt)
{
}

PathFinder::~PathFinder()
{
}

void MorphCollector(MolpherMolecule *morph, void *functor)
{
    CollectMorphs *collect = (CollectMorphs *) functor;
    (*collect)(*morph);
}

std::vector<ChemOperSelector> getChemOperSelectors(IterationSnapshot &snp)
{
    std::vector<ChemOperSelector> chemOperSelectors;
    chemOperSelectors.resize(snp.params.chemOperSelectors.size(), (ChemOperSelector) 0);
    for (size_t i = 0; i < snp.params.chemOperSelectors.size(); ++i) {
        chemOperSelectors[i] = (ChemOperSelector) snp.params.chemOperSelectors[i];
    }
    return chemOperSelectors;
}

//
//
//
//

void PathFinder::operator()()
{
    SynchCout(std::string("PathFinder started."));

    tbb::task_scheduler_init scheduler;
    if (mThreadCnt > 0) {
        scheduler.terminate();
        scheduler.initialize(mThreadCnt);
    }

    // TODO Initialize

    PathFinderContext ctx(mSnp);

    while (true) {

        try {
            mSnp.prunedDuringThisIter.clear();

            AccumulateTime molpherStopwatch(mSnp);
            AccumulateTime stageStopwatch(mSnp);

            MoleculeVector leaves;
            FindLeaves findLeaves(leaves);
            tbb::parallel_for(
                    PathFinderContext::CandidateMap::range_type(ctx.candidates),
                    findLeaves, tbb::auto_partitioner(), *mTbbCtx);
            stageStopwatch.ReportElapsedMiliseconds("FindLeaves", true);

            MoleculeVector morphs;
            CollectMorphs collectMorphs(morphs);
            std::vector<ChemOperSelector> chemOperSelectors = getChemOperSelectors(mSnp);
            for (MoleculeVector::iterator it = leaves.begin(); it != leaves.end(); it++) {
                MolpherMolecule &candidate = (*it);
                unsigned int morphAttempts = mSnp.params.cntMorphs;

                if (mSnp.params.scoreIsDistance) {
                    if (candidate.score < mSnp.params.scoreToTargetDepthSwitch) {
                        // We are close enough.
                        morphAttempts = mSnp.params.cntMorphsInDepth;
                    }
                } else {
                    if (candidate.score > mSnp.params.scoreToTargetDepthSwitch) {
                        // The score is high enough.
                        morphAttempts = mSnp.params.cntMorphsInDepth;
                    }
                }

                morphs.reserve(morphs.size() + morphAttempts);

                GenerateMorphs(candidate, morphAttempts,
                        chemOperSelectors, *mTbbCtx,
                        &collectMorphs, MorphCollector,
                        mSnp.iterIdx);
                PathFinderContext::MorphDerivationMap::accessor ac;

                if (ctx.morphDerivations.find(ac, candidate.smiles)) {
                    ac->second += collectMorphs.WithdrawCollectAttemptCount();
                } else {
                    ctx.morphDerivations.insert(ac, candidate.smiles);
                    ac->second = collectMorphs.WithdrawCollectAttemptCount();
                }
            }
            morphs.shrink_to_fit();
            stageStopwatch.ReportElapsedMiliseconds("GenerateMorphs", true);

            {
                std::stringstream ss;
                ss << mSnp.iterIdx + 1 << ":"
                        << " Candidates: " << mSnp.candidates.size()
                        << " Leaves: " << leaves.size()
                        << " Morphs: " << morphs.size();
                SynchCout(ss.str());
            }

            // TODO We may do filtering here, in order to reduce
            // the number of components we are computing similarity for.

            if (mSnp.params.scoreScriptCommand != "") {
                CalculateDistancesScript calculateDistances(morphs,
                        mSnp.params.scoreScriptCommand,
                        mSnp.storagePath,
                        mSnp.iterIdx, mSnp.params.scoreIsDistance);
                calculateDistances();
                stageStopwatch.ReportElapsedMiliseconds("ScoreCalculation", true);
            }

            if (mSnp.params.cntCandidatesToKeepMax != -1) {
                // We do not accept all, thus we need to sort the candidates.
                CompareMorphs compareMorphs(mSnp.params.scoreIsDistance);
                tbb::parallel_sort(morphs.begin(), morphs.end(), compareMorphs);
                stageStopwatch.ReportElapsedMiliseconds("SortMorphs", true);
            }

            std::vector<bool> survivors;
            survivors.resize(morphs.size(), false);
            FilterMorphs filterMorphs(ctx.morphDerivations, morphs, survivors);
            filterMorphs.addUnique(ctx.candidates);
            filterMorphs.addWeight(mSnp.params.minAcceptableMolecularWeight,
                    mSnp.params.maxAcceptableMolecularWeight);

            tbb::parallel_for(
                    tbb::blocked_range<size_t>(0, morphs.size()),
                    filterMorphs, tbb::auto_partitioner(), *mTbbCtx);
            stageStopwatch.ReportElapsedMiliseconds("FilterMorphs", true);

            if (mSnp.params.filterScriptCommand != "") {
                FilterScript filterScript(morphs,
                        mSnp.params.filterScriptCommand,
                        mSnp.storagePath,
                        mSnp.iterIdx,
                        survivors);
                filterScript();
                stageStopwatch.ReportElapsedMiliseconds("FilterScript", true);
            }

            if (mSnp.params.descriptorsScriptCommand != "") {
                compute_descriptors(morphs, survivors,
                        mSnp.params.descriptorsScriptCommand,
                        mSnp.storagePath,
                        mSnp.iterIdx);
                stageStopwatch.ReportElapsedMiliseconds("DescriptorScript", true);
            }

            if (mSnp.params.mooopPruns != -1) {
                std::vector<bool> actives(survivors);
                std::vector<bool> pass(survivors.size(), false);

                MoopFilter filter(morphs, actives, pass);

                for (int i = 0; i < mSnp.params.mooopPruns; ++i) {
                    // Print some statistics.
                    {
                        int activesCount = 0;
                        for (int i = 0; i < survivors.size(); ++i) {
                            if (actives[i]) {
                                ++activesCount;
                            }
                        }
                        std::stringstream ss;
                        ss << mSnp.iterIdx + 1 << ": " <<
                                "MOOP (" << i << ") input : " << activesCount;
                        SynchCout(ss.str());
                    }
                    //
                    tbb::parallel_for(
                            tbb::blocked_range<size_t>(0, morphs.size()),
                            filter, tbb::auto_partitioner(), *mTbbCtx);
                    // Another statistics.
                    {
                        int activesCount = 0;
                        for (int i = 0; i < survivors.size(); ++i) {
                            if (pass[i]) {
                                ++activesCount;
                            }
                        }
                        std::stringstream ss;
                        ss << mSnp.iterIdx + 1 << ": " <<
                                "MOOP (" << i << ") pass : " << activesCount;
                        SynchCout(ss.str());
                    }
                }

                for (int i = 0; i < survivors.size(); ++i) {
                    survivors[i] = survivors[i] && pass[i];
                }

                stageStopwatch.ReportElapsedMiliseconds("MOOP", true);
            }

            SmileSet modifiedParents;
            size_t candidatesBefore = ctx.candidates.size();

            AcceptMorphs acceptMorphs(morphs, survivors, ctx.candidates,
                    modifiedParents, mSnp.params.cntCandidatesToKeepMax);
            tbb::parallel_scan(tbb::blocked_range<size_t>(0, morphs.size()),
                    acceptMorphs, tbb::auto_partitioner());
            stageStopwatch.ReportElapsedMiliseconds("AcceptMorphs", true);

            size_t candidatesAfter = ctx.candidates.size();
            if (candidatesAfter == candidatesBefore) {
                std::stringstream ss;
                ss << mSnp.jobId << "/" << mSnp.iterIdx + 1 << ": ";
                ss << "No new molecules accepted!";
                SynchCout(ss.str());
                return;
            }

            //
            // Update tree.
            //

            UpdateTree updateTree(ctx.candidates);
            tbb::parallel_for(SmileSet::range_type(modifiedParents),
                    updateTree, tbb::auto_partitioner(), *mTbbCtx);
            stageStopwatch.ReportElapsedMiliseconds("UpdateTree", true);

//            PruneTree pruneTree(ctx);
//            SmileVector pruningQueue;
//            for (auto iter = mSnp.candidates.begin();
//                    iter ~= mSnp.candidates.end(); ++iter) {
//                pruningQueue.push_back(iter->second.smiles);
//            }
//            tbb::parallel_do(pruningQueue.begin(), pruningQueue.end(),
//                    pruneTree, *mTbbCtx);
//            stageStopwatch.ReportElapsedMiliseconds("PruneTree", true);

            //
            // Print score information.
            //

            double minDistance = std::numeric_limits<float>::max();
            double maxDistance = std::numeric_limits<float>::min();
            IterationSnapshot::CandidateMap::iterator itCandidates;
            for (itCandidates = mSnp.candidates.begin();
                    itCandidates != mSnp.candidates.end(); itCandidates++) {

                // Skip starting molecules.
                if (itCandidates->second.parentSmile == "") {
                    continue;
                }

                if (itCandidates->second.score < minDistance) {
                    minDistance = itCandidates->second.score;
                }

                if (itCandidates->second.score > maxDistance) {
                    maxDistance = itCandidates->second.score;
                }
            }
            std::stringstream ss;
            ss << mSnp.iterIdx + 1 << ": "
                    << "Score min: " << minDistance
                    << " max: " << maxDistance;
            SynchCout(ss.str());

            //
            // Update and save iteration.
            //

            mSnp.iterIdx += 1;
            mSnp.elapsedSeconds += molpherStopwatch.GetElapsedSeconds();

            ctx.save();

            //
            // Check termination condition.
            //

            if (mSnp.params.cntIterations <= mSnp.iterIdx) {
                std::stringstream ss;
                ss << mSnp.jobId << "/" << mSnp.iterIdx + 1 << ": "
                        << "The max number of iterations has been reached.";
                SynchCout(ss.str());
                return;
            }

            if (mSnp.params.timeMaxSeconds <= mSnp.elapsedSeconds) {
                std::stringstream ss;
                ss << mSnp.jobId << "/" << mSnp.iterIdx + 1 << ": "
                        << "We run out of time.";
                SynchCout(ss.str());
                return;
            }

        } catch (tbb::tbb_exception &exc) {
            SynchCout(std::string(exc.what()));
            break;
        }

    }

    SynchCout(std::string("PathFinder thread terminated."));
}
