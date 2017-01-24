#pragma once

#include <tbb/parallel_scan.h>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

#include "MolpherMolecule.h"

/**
 * Accept survivors to the candidates.
 *
 * The number of accepted molecules can be limited.
 */
class AcceptMorphs
{
public:
    typedef tbb::concurrent_vector<MolpherMolecule> MoleculeVector;
    typedef tbb::concurrent_hash_map<std::string, MolpherMolecule> CandidateMap;
    typedef tbb::concurrent_hash_map<std::string, bool /*dummy*/> SmileSet;
public:
    /**
     * @param morphs
     * @param survivors
     * @param candidates
     * @param modifiedParents
     * @param candidatesToKeepMax -1 to no limit.
     */
    AcceptMorphs(MoleculeVector &morphs,
            std::vector<bool> &survivors,
            CandidateMap &candidates,
            SmileSet &modifiedParents,
            int candidatesToKeepMax);
    AcceptMorphs(AcceptMorphs &toSplit, tbb::split);
    void operator()(const tbb::blocked_range<size_t> &r, tbb::pre_scan_tag);
    void operator()(const tbb::blocked_range<size_t> &r, tbb::final_scan_tag);
    void reverse_join(AcceptMorphs &toJoin);
    void assign(AcceptMorphs &toAssign);
private:
    MoleculeVector &mMorphs;
    std::vector<bool> &mSurvivors;
    CandidateMap &mCandidates;
    SmileSet &mModifiedParents;
    unsigned int mSurvivorCount;
    int mCandidatesToKeepMax;
};
