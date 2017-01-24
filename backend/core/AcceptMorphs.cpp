#include "AcceptMorphs.hpp"
#include "inout.h"

AcceptMorphs::AcceptMorphs(
        MoleculeVector &morphs,
        std::vector<bool> &survivors,
        CandidateMap &candidates,
        SmileSet &modifiedParents,
        int candidatesToKeepMax
        ) :
mMorphs(morphs),
mSurvivors(survivors),
mCandidates(candidates),
mModifiedParents(modifiedParents),
mSurvivorCount(0),
mCandidatesToKeepMax(candidatesToKeepMax)
{
    assert(mMorphs.size() == mSurvivors.size());
}

AcceptMorphs::AcceptMorphs(AcceptMorphs &toSplit, tbb::split) :
mCandidates(toSplit.mCandidates),
mMorphs(toSplit.mMorphs),
mSurvivors(toSplit.mSurvivors),
mModifiedParents(toSplit.mModifiedParents),
mSurvivorCount(0),
mCandidatesToKeepMax(toSplit.mCandidatesToKeepMax)
{
}

void AcceptMorphs::operator()(const tbb::blocked_range<size_t> &r, tbb::pre_scan_tag)
{
    for (size_t idx = r.begin(); idx != r.end(); ++idx) {
        if (mSurvivors[idx]) {
            ++mSurvivorCount;
        }
    }
}

void AcceptMorphs::operator()(
        const tbb::blocked_range<size_t> &r, tbb::final_scan_tag)
{
    for (size_t idx = r.begin(); idx != r.end(); ++idx) {
        if (!mSurvivors[idx]) {
            continue;
        }
        if (mCandidatesToKeepMax > 0 && mSurvivorCount >= mCandidatesToKeepMax) {
            continue;
        }

        CandidateMap::accessor ac;
        mCandidates.insert(ac, mMorphs[idx].smiles);
        ac->second = mMorphs[idx];
        ac.release();

        // Update parent molecule.
        if (mCandidates.find(ac, mMorphs[idx].parentSmile)) {
            ac->second.descendants.insert(mMorphs[idx].smiles);
            ac->second.historicDescendants.insert(mMorphs[idx].smiles);
            SmileSet::const_accessor dummy;
            mModifiedParents.insert(dummy, ac->second.smiles);
        } else {
            assert(false);
        }
        ++mSurvivorCount;
    }
}

void AcceptMorphs::reverse_join(AcceptMorphs &toJoin)
{
    mSurvivorCount += toJoin.mSurvivorCount;
}

void AcceptMorphs::assign(AcceptMorphs &toAssign)
{
    mSurvivorCount = toAssign.mSurvivorCount;
}