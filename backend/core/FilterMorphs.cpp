#include "FilterMorphs.hpp"
#include "MolpherMolecule.h"

#include <assert.h>
#include <vector>

FilterMorphs::Filter::~Filter() {
    
}

/**
 * Require molecule weight to be within given range.
 */
struct WeightFilter : public FilterMorphs::Filter
{

    WeightFilter(double min, double max) : mMin(min), mMax(max)
    {
    }

    ~WeightFilter()
    {

    }

    bool filter(const MolpherMolecule& molecule) const
    {
        return (molecule.molecularWeight > mMin) ||
                (molecule.molecularWeight < mMax);
    }
private:
    double mMin;
    double mMax;
};

/**
 * Remove duplicity based on molecule SMILES.
 */
struct UniqueFilter : public FilterMorphs::Filter
{

    UniqueFilter(FilterMorphs::CandidateMap &candidates)
    : mCandidates(candidates)
    {
    }

    ~UniqueFilter()
    {

    }

    bool filter(const MolpherMolecule& molecule) const
    {
        FilterMorphs::CandidateMap::const_accessor dummy;
        if (mCandidates.find(dummy, molecule.smiles)) {
            return false;
        }
        return true;
    }
private:
    FilterMorphs::CandidateMap &mCandidates;
};

FilterMorphs::FilterMorphs(MorphDerivationMap &morphDerivations,
        MoleculeVector &morphs,
        std::vector<bool> &survivors) :
mMorphDerivations(morphDerivations),
mMorphs(morphs),
mSurvivors(survivors)
{
    assert(mMorphs.size() == mSurvivors.size());
}

void FilterMorphs::operator()(const tbb::blocked_range<size_t> &r) const
{
    for (size_t idx = r.begin(); idx != r.end(); ++idx) {
        // Assume it's okay and search for filter that fails.
        const MolpherMolecule &morph = mMorphs[idx];
        mSurvivors[idx] = true;
        for (auto iter = mFilters.begin(); iter != mFilters.end(); ++iter) {
            if (!(*iter)->filter(morph)) {
                mSurvivors[idx] = false;
                break;
            }
        }
    }
}

void FilterMorphs::addWeight(double min, double max)
{
    mFilters.push_back(std::shared_ptr<Filter>(new WeightFilter(min, max)));
}

void FilterMorphs::addUnique(CandidateMap &candidates)
{
    mFilters.push_back(std::shared_ptr<Filter>(new UniqueFilter(candidates)));
}

//        double acceptProbability = 1.0;
//        bool isTarget = (mMorphs[idx].smiles.compare(mSnp.target.smiles) == 0);
//        if (idx >= mSnp.params.cntCandidatesToKeep && !isTarget) {
//            acceptProbability =
//                    0.25 - (idx - mSnp.params.cntCandidatesToKeep) /
//                    ((mGlobalMorphCount - mSnp.params.cntCandidatesToKeep) * 4.0);
//        }
//
//        bool mightSurvive =
//                SynchRand::GetRandomNumber(0, 99) < (int) (acceptProbability * 100);
//        if (mightSurvive) {
//            bool isDead = false;
//            bool badWeight = false;
//            bool badSascore = false;
//            bool alreadyExists = false;
//            bool alreadyTriedByParent = false;
//            bool tooManyProducedMorphs = false;
//
//            // Tests are ordered according to their cost.
//
//            isDead = (badWeight || badSascore || alreadyExists ||
//                    alreadyTriedByParent || tooManyProducedMorphs);
//            if (!isDead) {
//                badWeight =
//                        (mMorphs[idx].molecularWeight <
//                        mSnp.params.minAcceptableMolecularWeight) ||
//                        (mMorphs[idx].molecularWeight >
//                        mSnp.params.maxAcceptableMolecularWeight);
//            }
//
//            isDead = (badWeight || badSascore || alreadyExists ||
//                    alreadyTriedByParent || tooManyProducedMorphs);
//            if (!isDead) {
//                PathFinderContext::CandidateMap::const_accessor dummy;
//                if (mCtx.candidates.find(dummy, mMorphs[idx].smiles)) {
//                    alreadyExists = true;
//                }
//            }
//
//            isDead = (badWeight || badSascore || alreadyExists ||
//                    alreadyTriedByParent || tooManyProducedMorphs);
//            if (!isDead) {
//                PathFinderContext::CandidateMap::const_accessor ac;
//                if (mCtx.candidates.find(ac, mMorphs[idx].parentSmile)) {
//                    alreadyTriedByParent = (
//                            ac->second.historicDescendants.find(mMorphs[idx].smiles)
//                            !=
//                            ac->second.historicDescendants.end());
//                } else {
//                    assert(false);
//                }
//            }
//            isDead = (badWeight || badSascore || alreadyExists ||
//                    alreadyTriedByParent || tooManyProducedMorphs);
//            if (!isDead) {
//                PathFinderContext::MorphDerivationMap::const_accessor ac;
//                if (mCtx.morphDerivations.find(ac, mMorphs[idx].smiles)) {
//                    tooManyProducedMorphs =
//                            (ac->second > mSnp.params.cntMaxMorphs);
//                }
//            }
//
//            isDead = (badWeight || badSascore || alreadyExists ||
//                    alreadyTriedByParent || tooManyProducedMorphs);
//            mSurvivors[idx] = !isDead;
//    }