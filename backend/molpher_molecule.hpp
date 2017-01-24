#pragma once

#include <iterator>
#include <fstream>

class MolpherMolecule;

/**
 * Write SMILES of given molecules to file. Each SMILES on new line.
 *
 * @param path
 * @param begin
 * @param end
 */
void molecule_write_smiles(const std::string& path,
        std::vector<MolpherMolecule*>::iterator begin,
        std::vector<MolpherMolecule*>::iterator end);

void molecule_read_score(const std::string& path,
        std::vector<MolpherMolecule*>& molecules);

void molecule_read_descriptors(const std::string& path,
        std::vector<MolpherMolecule*>& molecules);
