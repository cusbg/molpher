#pragma once

#include <tbb/parallel_scan.h>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

#include "MolpherMolecule.h"

class UpdateTree
{
public:
    typedef tbb::concurrent_hash_map<std::string, bool /*dummy*/> SmileSet;
    typedef tbb::concurrent_hash_map<std::string, MolpherMolecule> CandidateMap;
public:
    UpdateTree(CandidateMap &candidates);
    void operator()(const SmileSet::range_type &modifiedParents) const;
private:
    CandidateMap &mCandidates;
};
