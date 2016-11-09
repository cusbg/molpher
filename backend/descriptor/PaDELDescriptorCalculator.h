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

namespace PaDELdesc {
    class PaDELDescriptorCalculator {
        private:
            std::string PaDELPath;
            std::string workDirPath;
            std::string configXMLPath;
            std::string availableDescsCSVPath;
            std::string SMILESFilePath;
            std::string outputFilePath;
            std::map<std::string, std::pair<std::string,std::string> > descriptorClassType;
            std::vector<std::string> descriptors2compute;
            boost::property_tree::ptree configXML;
            std::map<std::string, std::string> mols;
            std::vector<std::string> molNames;
            unsigned int threadsCnt;

            typedef std::map<std::string, std::map<std::string, double> > DataMap;
            DataMap computedData;

            void LoadAvailableDescriptors(const std::string &CSV_desc_list_path);
            void SaveDescConfig();

        public:
            PaDELDescriptorCalculator(
                    const std::string &PaDELPath
                    , const std::string &workDirPath
                    , const std::vector<std::string> &descriptor
                    , const unsigned int threads = 2
                    , const std::string &descriptorsCSV = "/descriptors.csv"
            );
            void saveDescConfigFile(const std::string &path);
            void saveDescConfigFile();
            void saveSMILES(const std::string &path);
            void saveSMILESFile();
            void addMol(const std::string &id, const std::string &smiles);
            void computeDescriptors();
            std::map<std::string, double>& getDescValues(const std::string &mol_id);

            std::string getOutputFilePath();
    };
}

#endif	/* PADELDESC_H */

