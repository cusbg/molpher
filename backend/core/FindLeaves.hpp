#pragma once

#include <string>

#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

#include "MolpherMolecule.h"

/**
 * Store leaves from the candidate map to the given molecule vector.
 */
class FindLeaves
{
public:
    typedef tbb::concurrent_vector<MolpherMolecule> MoleculeVector;
    typedef tbb::concurrent_hash_map<std::string, MolpherMolecule> CandidateMap;
public:
    FindLeaves(MoleculeVector &leaves);
    void operator()(const CandidateMap::range_type &candidates) const;
private:
    tbb::concurrent_vector<MolpherMolecule> &mLeaves;
};