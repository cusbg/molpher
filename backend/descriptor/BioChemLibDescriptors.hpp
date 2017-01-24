#pragma once

#include <string>
#include <vector>
#include <map>

#include "MolpherMolecule.h"

class BioChemLibDescriptors
{
public:
    typedef std::map<std::string, std::vector<double>> DescriptorValues;
    /**
     * Used as a key to descriptors in non fragments mode.
     */
    static const std::string MOLECULE_DESCRIPTORS;
public:
    BioChemLibDescriptors(const std::string &scriptPath,
            const std::string &workDirPath,
            const std::vector<std::string> &descriptor,
            bool useFragments);
    void add(MolpherMolecule& morph);
    void compute();
    DescriptorValues get(const MolpherMolecule& morph);
private:
    /**
     * Path to script from BioChem tools.
     */
    const std::string &scriptPath;
    /**
     * Path to working directory.
     */
    const std::string &workDirPath;
    /**
     * List of descriptors to compute.
     */
    const std::vector<std::string> &descriptor;
    /**
     * If true for each molecule fragments with descriptors are generated,
     * else only descriptors for given molecule are used.
     */
    bool useFragments;
    /**
     * For each SMILES store vector of values.
     */
    std::map<std::string, DescriptorValues > values;
};