#pragma once

#include <tbb/concurrent_vector.h>

class MolpherMolecule;

/**
 * Execute given script and load the result as molecules descriptors.
 *
 * @param morphs
 * @param survivors
 * @param scriptCommand
 * @param storageDirectory
 * @param iteration
 */
void compute_descriptors(tbb::concurrent_vector<MolpherMolecule>& morphs,
        std::vector<bool>& survivors,
        const std::string& scriptCommand,
        const std::string& workingDirectory,
        int iteration);