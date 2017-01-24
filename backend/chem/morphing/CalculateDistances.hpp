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

#pragma once


#include <vector>
#include <string>

#include <GraphMol/GraphMol.h>

#include <tbb/atomic.h>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>

#include "chemoper_selectors.h"
#include "chem/fingerprintStrategy/FingerprintStrategy.h"
#include "chem/simCoefStrategy/SimCoefStrategy.h"
#include "MolpherMolecule.h"
#include "MorphingData.h"
#include "chem/morphingStrategy/MorphingStrategy.h"
#include "chem/SimCoefCalculator.hpp"

class CalculateDistances
{
public:
    CalculateDistances(
            RDKit::RWMol **newMols,
            SimCoefCalculator &scCalc,
            Fingerprint *targetFp,
            std::vector<Fingerprint *> &decoysFp,
            double *distToTarget,
            double *distToClosestDecoy,
            int nextDecoy
            );

    void operator()(const tbb::blocked_range<int> &r) const;

private:
    RDKit::RWMol **mNewMols;
    SimCoefCalculator &mScCalc;
    Fingerprint *mTargetFp;
    std::vector<Fingerprint *> &mDecoysFp;

    double *mDistToTarget;
    double *mDistToClosestDecoy;

    /**
     * Determine next decoy to visit.
     */
    int mNextDecoy;
};

/**
 * Use external script to calculate distances for given molecules.
 *
 * The call of external script is:
 * {script_path} -i {input_file} -o {output_file}
 *
 * Where the {input_file} contains one SMILES per line.
 * The {output_file} is created by the script, it must contains
 * same number of lines as the {input_file} on each line must be distance
 * to target for respective molecule.
 */
class CalculateDistancesScript
{
public:
    CalculateDistancesScript(tbb::concurrent_vector<MolpherMolecule>& molecules,
            const std::string& script,
            const std::string& workingDirectory,
            unsigned int iteration,
            bool useDistance);
    void operator()() const;
private:
    tbb::concurrent_vector<MolpherMolecule>& mMolecules;
    const std::string& mScript;
    std::string mWorkingDirectory;
    bool mUseDistance;
};