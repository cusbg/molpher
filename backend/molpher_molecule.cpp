#include <fstream>
#include <string>

#include "inout.h"
#include "csv.hpp"
#include "molpher_molecule.hpp"

#include "MolpherMolecule.h"

void molecule_read_score_csv(const std::string& path,
        std::vector<MolpherMolecule*>& molecules)
{
    auto iterator = molecules.begin();
    std::vector<std::vector < std::string>> data = csv_load(path);
    for (const auto& row : data) {
        std::string smiles = row[0];
        // Check molecule SMILES and save data.
        MolpherMolecule *molecule = *iterator;
        ++iterator;
        if (molecule->smiles == smiles) {
            molecule->score = std::stof(row[1]);
        } else {
            SynchCout("Invalid SMILES: " + smiles +
                    " expected: " + molecule->smiles);
            throw std::runtime_error("Missing SMILES.");
        }
    }
}

void molecule_read_descriptors_csv(const std::string& path,
        std::vector<MolpherMolecule*>& molecules)
{
    auto iterator = molecules.begin();
    std::vector<std::vector < std::string>> data = csv_load(path);
    for (const auto& row : data) {
        std::string smiles = row[0];
        // Get molecule.
        MolpherMolecule *molecule = *iterator;
        ++iterator;
        if (molecule->smiles != smiles) {
            SynchCout("Invalid SMILES: " + smiles +
                    " expected: " + molecule->smiles);
            throw std::runtime_error("Missing SMILES.");
        }
        // Save data.
        for (auto iter = row.begin() + 1; iter != row.end(); ++iter) {
            molecule->descriptors.push_back(std::stof((*iter)));
        }
    }
}

void molecule_write_smiles(const std::string& path,
        std::vector<MolpherMolecule*>::iterator begin,
        std::vector<MolpherMolecule*>::iterator end)
{
    std::ofstream stream(path.c_str());
    for (; begin != end; ++begin) {
        stream << (*begin)->smiles << std::endl;
    }
    stream.close();
};

void molecule_read_score(const std::string& path,
        std::vector<MolpherMolecule*>& molecules)
{
    molecule_read_score_csv(path, molecules);
}

void molecule_read_descriptors(const std::string& path,
        std::vector<MolpherMolecule*>& molecules)
{
    molecule_read_descriptors_csv(path, molecules);
}