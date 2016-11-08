/*
 * File:   PaDELdesc.h
 * Author: martin
 *
 * Created on 26. prosinec 2014, 12:43
 */

#ifndef PADELDESC_H
#define	PADELDESC_H

#include <string>
#include <vector>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;

namespace PaDELdesc {

    /**
     * Usage:
     *
     * For each iteration:
     * ctor
     * addMol(morph.id, morph.smile);
     * morph.descriptorsFilePath =getOutputFilePath
     * computeDescriptors
     *
     * calculator.getDescValues(morph.id) -> return descriptors
     *
     */
    class PaDELDescriptorCalculator {
        private:
            string PaDELPath;
            string workDirPath;
            string configXMLPath;
            string availableDescsCSVPath;
            string SMILESFilePath;
            string outputFilePath;
            map<string, pair<string,string> > descriptorClassType;
            vector<string> descriptors2compute;
            boost::property_tree::ptree configXML;
            map<string, string> mols;
            vector<string> molNames;
            unsigned int threadsCnt;

            typedef map<string, map<string, double> > DataMap;
            DataMap computedData;

            void LoadAvailableDescriptors(const string &CSV_desc_list_path);
            void SaveDescConfig();

        public:
            PaDELDescriptorCalculator(
                    const string &PaDELPath
                    , const string &workDirPath
                    , const vector<string> &descriptor
                    , const unsigned int threads = 2
                    , const string &descriptorsCSV = "/descriptors.csv"
            );
            void saveDescConfigFile(const string &path);
            void saveDescConfigFile();
            void saveSMILES(const string &path);
            void saveSMILESFile();
            void addMol(const string &id, const string &smiles);
            void computeDescriptors();
            map<string, double>& getDescValues(const string &mol_id);

            string getOutputFilePath();
    };
}

#endif	/* PADELDESC_H */

