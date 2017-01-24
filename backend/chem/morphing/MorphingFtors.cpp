/*
 Copyright (c) 2012 Peter Szepe

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cfloat>

#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Descriptors/MolDescriptors.h>

#include "main.hpp"
#include "auxiliary/SynchRand.h"
#include "chem/ChemicalAuxiliary.h"
#include "chem/morphing/MorphingFtors.hpp"

/*
 Added code for working with SAScore
 */

CalculateMorphs::CalculateMorphs(
    MorphingData &data,
    std::vector<MorphingStrategy *> &strategies,
    ChemOperSelector *opers,
    RDKit::RWMol **newMols,
    std::string *smiles,
    double *weights,
    tbb::atomic<unsigned int> &kekulizeFailureCount,
    tbb::atomic<unsigned int> &sanitizeFailureCount,
    tbb::atomic<unsigned int> &morphingFailureCount
    ) :
    mData(data),
    mStrategies(strategies),
    mOpers(opers),
    mNewMols(newMols),
    mSmiles(smiles),
    mWeights(weights),
    mKekulizeFailureCount(kekulizeFailureCount),
    mSanitizeFailureCount(sanitizeFailureCount),
    mMorphingFailureCount(morphingFailureCount)
{
    // no-op
}

void CalculateMorphs::operator()(const tbb::blocked_range<int> &r) const
{
//    DEBUG_REPORT("CalculateMorphs::operator")

    for (int i = r.begin(); i != r.end(); ++i) {
        int randPos = SynchRand::GetRandomNumber(mStrategies.size() - 1);
        MorphingStrategy *strategy = mStrategies[randPos];
        mOpers[i] = strategy->GetSelector();

        try {
            strategy->Morph(mData, &mNewMols[i]);
        } catch (const std::exception &exc) {
            ++mMorphingFailureCount; // atomic
            delete mNewMols[i];
            mNewMols[i] = NULL;
        }

        if (mNewMols[i]) {
            try {
                RDKit::RWMol *copy = new RDKit::RWMol();
                CopyMol(*(mNewMols[i]), *copy);
                delete mNewMols[i];
                mNewMols[i] = copy;

                mNewMols[i]->clearComputedProps();
                RDKit::MolOps::cleanUp(*(mNewMols[i]));
                mNewMols[i]->updatePropertyCache();
                RDKit::MolOps::Kekulize(*(mNewMols[i]));
                RDKit::MolOps::adjustHs(*(mNewMols[i]));
                // We initialize the ring system, so the fingerprints
                // can be computed for given molecule.
                mNewMols[i]->getRingInfo()->initialize();
                mSmiles[i] = RDKit::MolToSmiles(*(mNewMols[i]));
                mWeights[i] = RDKit::Descriptors::calcExactMW(*(mNewMols[i]));
            } catch (const ValueErrorException &exc) {
                ++mKekulizeFailureCount; // atomic
                delete mNewMols[i];
                mNewMols[i] = NULL;
            } catch (const RDKit::MolSanitizeException &exc) {
                ++mSanitizeFailureCount; // atomic
                delete mNewMols[i];
                mNewMols[i] = NULL;
            } catch (const std::exception &exc) {
                ++mSanitizeFailureCount; // atomic
                delete mNewMols[i];
                mNewMols[i] = NULL;
            }
        }
    }
}

ReturnResults::ReturnResults(
    RDKit::RWMol **newMols,
    std::string *smiles,
    std::string &parentSmile,
    ChemOperSelector *opers,
    double *weights,
    void *callerState,
    void (*deliver)(MolpherMolecule *, void *),
    int iteration
    ) :
    mNewMols(newMols),
    mSmiles(smiles),
    mParentSmile(parentSmile),
    mOpers(opers),
    mWeights(weights),
    mCallerState(callerState),
    mDeliver(deliver),
    mIteration(iteration)
{
    // no-op
}

void ReturnResults::operator()(const tbb::blocked_range<int> &r) const
{
    for (int i = r.begin(); i != r.end(); ++i) {
        if (mNewMols[i]) {
            MolpherMolecule result(mSmiles[i], mParentSmile,
                mOpers[i], mWeights[i], mIteration);
            mDeliver(&result, mCallerState);
        }
    }
}
