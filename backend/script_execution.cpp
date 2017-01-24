#include <assert.h>
#include <sstream>
#include <vector>
#include <boost/filesystem.hpp>

#include "inout.h"
#include "script_execution.hpp"
#include "molpher_molecule.hpp"
#include "IterationSnapshot.h"

void compute_descriptors(tbb::concurrent_vector<MolpherMolecule>& morphs,
        std::vector<bool>& survivors,
        const std::string& scriptCommand,
        const std::string& storageDirectory,
        int iteration)
{
    assert(morphs.size() == survivors.size());
    std::vector<MolpherMolecule*> activesMorphs;
    for (int i = 0; i < survivors.size(); ++i) {
        if (survivors[i]) {
            activesMorphs.push_back(&morphs[i]);
        }
    }
    //
    std::stringstream workingDirectory;
    workingDirectory << storageDirectory << "/" << iteration << "_descriptors/";
    // Prepare script input.
    try {
        boost::filesystem::create_directories(workingDirectory.str());
    } catch (boost::filesystem::filesystem_error &exc) {
        SynchCout(exc.what());
    }
    std::string inputPath = workingDirectory.str() + "input.smi";
    molecule_write_smiles(inputPath,
            activesMorphs.begin(), activesMorphs.end());
    // Execute script.
    std::string outputPath = workingDirectory.str() + "output.csv";
    std::string call = scriptCommand + " -i \"" + inputPath + "\" -o \"" +
            outputPath + "\"";
    int result = system(call.c_str());
    assert(result == 0);
    molecule_read_descriptors(outputPath, activesMorphs);
}