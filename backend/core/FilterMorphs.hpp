#pragma once

#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

#include "MolpherMolecule.h"

/**
 * Filter given molecules for basic chemical properties.
 *
 * For each candidate save the "survive" flag to the survive variable.
 */
class FilterMorphs
{
public:
    typedef tbb::concurrent_hash_map<std::string, MolpherMolecule> CandidateMap;
    typedef tbb::concurrent_hash_map<std::string, unsigned int> MorphDerivationMap;
    typedef tbb::concurrent_vector<MolpherMolecule> MoleculeVector;
public:
    FilterMorphs(
            MorphDerivationMap &morphDerivations,
            MoleculeVector &morphs,
            std::vector<bool> &survivors
            );
    void operator()(const tbb::blocked_range<size_t> &r) const;
public:
    // TODO Add probability filter !
    void addWeight(double min, double max);
    void addUnique(CandidateMap &candidates);
public:

    struct Filter
    {
        virtual ~Filter();
        /**
         * @param molecule
         * @return True if given molecule should be preserved.
         */
        virtual bool filter(const MolpherMolecule& molecule) const = 0;
    };
private:
    MorphDerivationMap &mMorphDerivations;
    MoleculeVector &mMorphs;
    std::vector<bool> &mSurvivors;
    std::vector<std::shared_ptr<Filter>> mFilters;
};

// @TODO Implement filters here !!