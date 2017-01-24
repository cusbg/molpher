#pragma once

#include <tbb/blocked_range.h>
#include <tbb/parallel_do.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

#include "MolpherMolecule.h"

class PruneTree
{
public:
    typedef tbb::concurrent_hash_map<std::string, bool /*dummy*/> SmileSet;
    typedef tbb::concurrent_hash_map<std::string, MolpherMolecule> CandidateMap;
    typedef tbb::concurrent_hash_map<std::string, unsigned int> MorphDerivationMap;
    typedef tbb::concurrent_vector<std::string> PrunedMoleculeVector;
public:
    PruneTree(CandidateMap &candidates, MorphDerivationMap &morphDerivationMap,
            PrunedMoleculeVector prunedMolecules, int iterationThreshold,
            int maxMorphsCount, SmileSet &deferred);
    void operator()(const std::string &smile,
            tbb::parallel_do_feeder<std::string> &feeder) const;
protected:
    void EraseSubTree(const std::string &root) const;
private:
    CandidateMap &mCandidates;
    MorphDerivationMap &mMorphDerivationMap;
    PrunedMoleculeVector &mPrunedMolecules;
    SmileSet &mDeferred;
    /**
     * Max level of iterations that morph may not improve before
     * application of pruning.
     */
    int mIterationThreshold;
    /**
     * Maximum number of morphs to keep.
     */
    int mMaxMorphsCount;
};
