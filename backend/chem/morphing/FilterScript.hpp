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

/**
 * Use external script to filter candidates.
 *
 * The call of external script is:
 * {script_path} -i {input_file} -o {output_file}
 *
 * Where the {input_file} contains one SMILES per line.
 * The {output_file} is created by the script, it must contains
 * same number of lines as the {input_file} on each line must be the
 * SMILES and filter value.
 *
 * Where 1 means that the molecule will survive.
 */
class FilterScript
{
public:
    FilterScript(tbb::concurrent_vector<MolpherMolecule>& molecules,
            const std::string& script,
            const std::string& workingDirectory,
            unsigned int iteration,
            std::vector<bool> &survivors);
    void operator()() const;
private:
    tbb::concurrent_vector<MolpherMolecule>& mMolecules;
    const std::string& mScript;
    std::string mWorkingDirectory;
    std::vector<bool> &mSurvivors;
};