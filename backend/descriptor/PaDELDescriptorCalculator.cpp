
#include "PaDELDescriptorCalculator.h"
#include "CSV.h"
#include "DataConverter.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <set>
#include <cstdlib>

using namespace std;

// private methods

void PaDELdesc::PaDELDescriptorCalculator::LoadAvailableDescriptors(const string &CSV_desc_list_path) {
    CSVparse::CSV available_descriptors(CSV_desc_list_path, ";", "");
    const vector<string>& classes = available_descriptors.getStringData("Descriptor Java Class");
    const vector<string>& names = available_descriptors.getStringData("Descriptor");
    const vector<string>& types = available_descriptors.getStringData("Class");

    string curent_class;
    vector<string>::size_type counter(0);
    for (vector<string>::const_iterator it = classes.begin(); it != classes.end(); it++) {
        string name = names[counter];
        string type = types[counter];
        string classname = *it;
        if (classname.compare(available_descriptors.getEmptyValue()) != 0) {
            descriptorClassType[name].first = classname;
            curent_class = classname;
        } else {
            descriptorClassType[name].first = curent_class;
        }
        descriptorClassType[name].second = type;
        ++counter;
    }
        }

void PaDELdesc::PaDELDescriptorCalculator::SaveDescConfig() {
    using boost::property_tree::ptree;

    ptree rootNode;

    ptree group2D;
    group2D.put("<xmlattr>.name", "2D");
    ptree group3D;
    group3D.put("<xmlattr>.name", "3D");

    set<string> added_classes;
    for (vector<string>::iterator it = descriptors2compute.begin(); it != descriptors2compute.end(); it++ ) {
        if (descriptorClassType.find(*it) != descriptorClassType.end()) {
            string classname(descriptorClassType[*it].first);
            if (added_classes.find(classname) != added_classes.end()) {
                continue;
            }
            string type(descriptorClassType[*it].second);
            ptree desc;
            desc.put("<xmlattr>.name", classname);
            desc.put("<xmlattr>.value", "true");
            added_classes.insert(classname);
            if (type.compare("2D") == 0) {
                group2D.add_child("Descriptor", desc);
            } else if (type.compare("3D") == 0) {
                group3D.add_child("Descriptor", desc);
            } else {
                assert(false);
            }
        } else {
            cout << "Could not locate key: " << *it << endl;
            assert(false);
        }
    }

    rootNode.add_child("Group", group2D);
    rootNode.add_child("Group", group3D);
    configXML.add_child("Root", rootNode);
}

// public methods

void PaDELdesc::PaDELDescriptorCalculator::addMol(const string &id, const string &smiles) {
    mols[id] = smiles;
    molNames.push_back(id);
}

void PaDELdesc::PaDELDescriptorCalculator::computeDescriptors() {
    SaveDescConfig();
    saveDescConfigFile();
    saveSMILESFile();
    string command( "java -jar \"" + PaDELPath + "PaDEL-Descriptor.jar\" ");
    string options("-dir \"" + SMILESFilePath + "\" -convert3d -descriptortypes \"" + configXMLPath + "\" -file \"" + outputFilePath + "\" -retainorder -log -2d -3d -maxruntime 10000 -threads " + CSVparse::DataConverter::ValToString(threadsCnt));
    string call(command + options);
    cout << call << endl;
    int ret = system(call.c_str());
    assert(ret == 0);

    CSVparse::CSV descriptors(outputFilePath, ",", "");
    const vector<string> &names = descriptors.getStringData("Name");
    computedData.clear();
    unsigned int mol_idx(0);
    for (vector<string>::iterator mol_name = molNames.begin(); mol_name != molNames.end(); mol_name++) {
        assert(mol_name->compare(names[mol_idx]) == 0);
        computedData[*mol_name] = map<string, double>();
        for (vector<string>::iterator desc_name = descriptors2compute.begin(); desc_name != descriptors2compute.end(); desc_name++) {
            computedData[*mol_name][*desc_name] = descriptors.getFloatData(*desc_name)[mol_idx];
        }
        ++mol_idx;
    }
}

void PaDELdesc::PaDELDescriptorCalculator::saveSMILES(const string &path) {
    ofstream smiles_file(path.c_str());
    for (vector<string>::iterator mol_name = molNames.begin(); mol_name != molNames.end(); mol_name++) {
        smiles_file << mols[*mol_name] << "\t" << *mol_name << endl;
    }
    smiles_file.close();
}

void PaDELdesc::PaDELDescriptorCalculator::saveSMILESFile() {
    saveSMILES(SMILESFilePath);
}

void PaDELdesc::PaDELDescriptorCalculator::saveDescConfigFile(const string &path) {
    boost::property_tree::write_xml(path, configXML, std::locale(),
            boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
}

void PaDELdesc::PaDELDescriptorCalculator::saveDescConfigFile() {
    saveDescConfigFile(configXMLPath);
}

map<string, double>& PaDELdesc::PaDELDescriptorCalculator::getDescValues(const string &mol_id) {
    assert(computedData.find(mol_id) != computedData.end());
    return computedData[mol_id];
}

string PaDELdesc::PaDELDescriptorCalculator::getOutputFilePath() {
    return outputFilePath;
}

// constructors

PaDELdesc::PaDELDescriptorCalculator::PaDELDescriptorCalculator(
    const string &PaDELPath
    , const string &workDirPath
    , const vector<string> &descriptors
    , const unsigned int threads
    , const string &descriptorsCSV
) :
    PaDELPath(PaDELPath + "/")
    , workDirPath(workDirPath + "/")
    , threadsCnt(threads)
    , configXMLPath(workDirPath + "/descriptors.xml")
    , availableDescsCSVPath(PaDELPath + descriptorsCSV)
    , SMILESFilePath(workDirPath + "/mols.smi")
    , outputFilePath(workDirPath + "/results.csv")
    , descriptors2compute(descriptors)
{
    LoadAvailableDescriptors(availableDescsCSVPath);
}
