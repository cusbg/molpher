
#include "PruneTree.hpp"

#include <assert.h>
#include <deque>

PruneTree::PruneTree(CandidateMap &candidates,
        MorphDerivationMap &morphDerivationMap,
        PrunedMoleculeVector prunedMolecules,
        int iterationThreshold, int maxMorphsCount,
        SmileSet &deferred) :
mCandidates(candidates),
mMorphDerivationMap(morphDerivationMap),
mPrunedMolecules(prunedMolecules),
mIterationThreshold(iterationThreshold),
mMaxMorphsCount(maxMorphsCount),
mDeferred(deferred)
{
}

void PruneTree::operator()(
        const std::string &smile, tbb::parallel_do_feeder<std::string> &feeder) const
{
    CandidateMap::accessor ac;
    mCandidates.find(ac, smile);
    assert(!ac.empty());

    SmileSet::const_accessor dummy;
    bool deferred = mDeferred.find(dummy, smile);
    bool prune = (deferred ||
            (ac->second.itersWithoutDistImprovement > mIterationThreshold));
    if (prune) {

        bool tooManyDerivations = false;
        MorphDerivationMap::const_accessor acDerivations;
        if (mMorphDerivationMap.find(acDerivations, smile)) {
            tooManyDerivations = (acDerivations->second > mMaxMorphsCount);
        }

        bool pruneThis = (deferred || tooManyDerivations);

        if (pruneThis) {
            CandidateMap::accessor acParent;
            mCandidates.find(acParent, ac->second.parentSmile);
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

void PruneTree::EraseSubTree(const std::string &root) const
{
    std::deque<std::string> toErase;
    toErase.push_back(root);

    while (!toErase.empty()) {
        std::string current = toErase.front();
        toErase.pop_front();

        CandidateMap::accessor ac;
        mCandidates.find(ac, current);
        assert(!ac.empty());

        std::set<std::string>::const_iterator it;
        for (it = ac->second.descendants.begin();
                it != ac->second.descendants.end(); it++) {
            toErase.push_back(*it);
        }

        mPrunedMolecules.push_back(current);
        mCandidates.erase(ac);
    }
}