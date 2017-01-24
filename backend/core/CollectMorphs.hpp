#pragma once

#include <string>

#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

#include "MolpherMolecule.h"

/**
 * Can be called in parallel to collect molecules into the given
 * molecule vector.
 */
class CollectMorphs
{
public:
    typedef tbb::concurrent_vector<MolpherMolecule> MoleculeVector;
    typedef tbb::concurrent_hash_map<std::string, bool /*dummy*/> SmileSet;
public:
    CollectMorphs(MoleculeVector &morphs);
    void operator()(const MolpherMolecule &morph);
    /**
     * @return
     */
    unsigned int WithdrawCollectAttemptCount();
private:
    SmileSet mDuplicateChecker;
    MoleculeVector &mMorphs;
    tbb::atomic<unsigned int> mCollectAttemptCount;
};