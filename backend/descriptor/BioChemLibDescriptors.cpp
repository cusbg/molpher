#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "inout.h"
#include "BioChemLibDescriptors.hpp"
#include "MolpherMolecule.h"

const std::string BioChemLibDescriptors::MOLECULE_DESCRIPTORS = "molecule";

BioChemLibDescriptors::BioChemLibDescriptors(const std::string &scriptPath,
        const std::string &workDirPath,
        const std::vector<std::string> &descriptor,
        bool useFragments)
: scriptPath(scriptPath), workDirPath(workDirPath), descriptor(descriptor),
useFragments(useFragments)
{

}

void BioChemLibDescriptors::add(MolpherMolecule& morph)
{
    values.insert(std::make_pair(morph.smiles,
            std::map<std::string, std::vector<double>>()));
}

void BioChemLibDescriptors::compute()
{
    // Write SCV smiles file.
    std::string smilesPath = workDirPath + "/smiles.smi";
    std::ofstream smiles_stream(smilesPath.c_str());
    for (auto iter : values) {
        smiles_stream << iter.first << std::endl;
    }
    smiles_stream.close();
    // Prepare command.
    std::string outputPath = workDirPath + "/output.json";
    std::string command = "python";
    command += " -i \"" + scriptPath + "\"";
    command += " -o \"" + outputPath + "";
    if (useFragments) {
        command += " --fragments";
    }
    // Call external process.
    SynchCout("Executing command: " + command + " ... ");
    int ret = system(command.c_str());
    assert(ret == 0);
    SynchCout("Executing command: " + command + " ... done");
    // Load output JSON file.
    std::ifstream output_stream(outputPath.c_str());
    boost::property_tree::ptree data;
    boost::property_tree::read_json(output_stream, data);
    output_stream.close();
    // Convert into the results.
    BOOST_FOREACH(boost::property_tree::ptree::value_type& child,
            data.get_child("data"))
    {
        std::string moleculeSmiles = child.second.get<std::string>("smiles");
        DescriptorValues moleculeDescriptors;
        if (useFragments) {

            BOOST_FOREACH(boost::property_tree::ptree::value_type& fragment,
                    child.second.get_child("fragments"))
            {
                std::string fragmetnSmiles =
                        fragment.second.get<std::string>("smiles");
                std::vector<double> descriptors;
                BOOST_FOREACH(boost::property_tree::ptree::value_type& value,
                        fragment.second.get_child("values"))
                {
                    descriptors.push_back(value.second.get_value<double>());
                }
                moleculeDescriptors.insert(
                    std::make_pair(fragmetnSmiles, descriptors));
            }
        } else {
            std::vector<double> descriptors;
            BOOST_FOREACH(boost::property_tree::ptree::value_type& value,
                    child.second.get_child("values"))
            {
                descriptors.push_back(value.second.get_value<double>());
            }
            moleculeDescriptors.insert(
                    std::make_pair(MOLECULE_DESCRIPTORS, descriptors));
        }
        values.insert(std::make_pair(moleculeSmiles, moleculeDescriptors));
    };
}

BioChemLibDescriptors::DescriptorValues BioChemLibDescriptors::get(const MolpherMolecule& morph)
{
    return values.at(morph.smiles);
}