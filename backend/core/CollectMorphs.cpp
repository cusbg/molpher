#include "CollectMorphs.hpp"

CollectMorphs::CollectMorphs(MoleculeVector &morphs) : mMorphs(morphs)
{
    mCollectAttemptCount = 0;
}

void CollectMorphs::operator()(const MolpherMolecule &morph)
{
    ++mCollectAttemptCount; // Atomic operation.
    SmileSet::const_accessor dummy;
    if (mDuplicateChecker.insert(dummy, morph.smiles)) {
        mMorphs.push_back(morph);
    } else {
        // Ignore duplicate.
    }
}

unsigned int CollectMorphs::WithdrawCollectAttemptCount()
{
    unsigned int ret = mCollectAttemptCount;
    mCollectAttemptCount = 0;
    return ret;
}
