#include <fstream>
#include <GraphMol/SmilesParse/SmilesWrite.h>

#include "inout.h"
#include "FilterScript.hpp"


FilterScript::FilterScript(
        tbb::concurrent_vector<MolpherMolecule>& molecules,
        const std::string& script,
        const std::string& workingDirectory,
        unsigned int iteration,
        std::vector<bool> &survivors)
: mMolecules(molecules),
mScript(script),
mSurvivors(survivors)
{
    std::stringstream ss;
    ss << workingDirectory << "/" << iteration << "_filter/";
    mWorkingDirectory = ss.str();
    // Create working directory.
    try {
        boost::filesystem::create_directories(mWorkingDirectory);
    } catch (boost::filesystem::filesystem_error &exc) {
        SynchCout(exc.what());
    }
}

void FilterScript::operator()() const
{
    // Create file with smiles.
    // We use only molecules that are survivors as there is no need
    // to check dead molecules for filter.
    std::string smiles_file_path = mWorkingDirectory + "/smiles.smi";
    std::ofstream smiles_file(smiles_file_path);
    int counter = 0;
    for (auto iter = mMolecules.begin(); iter != mMolecules.end(); ++iter) {
        if (!mSurvivors[counter]) {
            // If dead then move to next one.
            counter++;
            continue;
        }else {
            counter++;
            smiles_file << iter->smiles << std::endl;
        }
    }
    smiles_file.close();
    // Run external script.
    std::string distances_file_path = mWorkingDirectory + "/filter";
    std::string call = mScript + " -i \"" + smiles_file_path + "\""
            " -o \"" + distances_file_path + "\"";
    int ret = system(call.c_str());
    assert(ret == 0);
    // Read results (CSV).
    std::ifstream distances_file(distances_file_path.c_str());

    int filter;
    std::string smiles = "";

    distances_file >> smiles >> filter;
    counter = 0;
    int filteredOut = 0;
    for (auto iter = mMolecules.begin(); iter != mMolecules.end(); ++iter) {
        if (!mSurvivors[counter]) {
            // If dead then move to next one.
            counter++;
            continue;
        }
        if (iter->smiles != smiles) {
            SynchCout("[ERROR] Missing record for: " + iter->smiles +
                    " current: " + smiles);
            throw std::exception();
        }
        mSurvivors[counter] = filter == 1;
        if (filter != 1) {
            filteredOut++;
        }
        distances_file >> smiles >> filter;
        counter++;
    }
    distances_file.close();
    // Print some info.
    std::stringstream ss;
    ss << "\t" << "Filtered out: " << filteredOut;
    SynchCout(ss.str());
}