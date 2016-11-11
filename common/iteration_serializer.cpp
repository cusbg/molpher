/*
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Petyr
 * Created on 18.10.2013
 */

#include <string.h>
#include <iostream>
#include <fstream>

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "boost/archive/archive_exception.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/algorithm/string/predicate.hpp"

#include <GraphMol/FileParsers/MolSupplier.h>
#include <GraphMol/FileParsers/MolWriters.h>
#include <GraphMol/Descriptors/MolDescriptors.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/MolOps.h>
#include <RDGeneral/BadFileException.h>

#include "inout.h"
#include "iteration_serializer.hpp"
#include "fingerprint_selectors.h"
#include "simcoeff_selectors.h"

namespace molpher {
namespace iteration {

enum FileType
{
    XML,
    SNP,
    JSON,
    UNKNOWN
};

/**
 * Save snapshot in snp file format.
 *
 * @param file
 * @param snp
 * @return
 */
bool saveSnp(const std::string &file, const IterationSnapshot &snp)
{
    std::ofstream outStream;
    outStream.open(file.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!outStream.good()) {
        return false;
    }

    bool failed = false;
    try {
        boost::archive::text_oarchive oArchive(outStream);
        oArchive << snp;
    } catch (boost::archive::archive_exception &exc) {
        failed = true;
        SynchCout(std::string(exc.what()));
    }
    outStream.close();
    return !failed;
}

bool saveSnpAsJson(const std::string &file, const IterationSnapshot &snp)
{
    boost::property_tree::ptree candidates;
    for (auto iter = snp.candidates.begin(); iter != snp.candidates.end(); ++iter) {
        boost::property_tree::ptree molecule;

        molecule.put("smiles", iter->second.smile);
        molecule.put("parenSmiles", iter->second.parentSmile);
        if (snp.params.activityMorphing) {
            molecule.put("distToEtalon", iter->second.distToEtalon);
        } else {
            molecule.put("distToTarget", iter->second.distToTarget);
        }
        boost::property_tree::ptree descriptors;
        for (auto desc : iter->second.descriptorValues) {
            boost::property_tree::ptree value;
            value.put("", desc);
            descriptors.push_back(std::make_pair("", value));
        }
        molecule.push_back(std::make_pair("descriptors", descriptors));
        candidates.push_back(std::make_pair("", molecule));
    }
    boost::property_tree::ptree pt;
    pt.put("job", snp.jobId);
    pt.put("elapsedSeconds", snp.elapsedSeconds);
    pt.put_child("molecules", candidates);
    //
    boost::property_tree::ptree root;
    root.put_child("data", pt);
    boost::property_tree::write_json(file, pt);
    return true;
}


/**
 * Load snapshot from snp file.
 *
 * @param file
 * @param snp
 * @return
 */
bool loadSnp(const std::string &file, IterationSnapshot &snp)
{
    std::ifstream inStream;
    inStream.open(file.c_str());
    if (!inStream.good()) {
        return false;
    }

    try {
        boost::archive::text_iarchive iArchive(inStream);
        iArchive >> snp;
    } catch (boost::archive::archive_exception &exc) {
        SynchCout(std::string(exc.what()));
        inStream.close();
        return false;
    }

    inStream.close();
    return true;
}

int toInt(const std::string& str)
{
    std::stringstream ss;
    ss << str;
    int result;
    ss >> result;
    return result;
}

/**
 * Create molecule from given smile. The smile may change as it's
 * RDKit smile
 * @param inSmile
 * @return
 */
MolpherMolecule createMoleculeFromSmile(const std::string& inSmile)
{
    RDKit::RWMol* mol = RDKit::SmilesToMol(inSmile);
    try {
        RDKit::MolOps::Kekulize(*mol);
    } catch (const ValueErrorException &exc) {
        SynchCout("Cannot kekulize input molecule.");
    }
    std::string smile(RDKit::MolToSmiles(*mol));
    std::string formula(RDKit::Descriptors::calcMolFormula(*mol));
    return MolpherMolecule(smile, formula);
}

void loadPropertyTree(const boost::property_tree::ptree& pt, MolpherParam& params)
{
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, pt)
    {
        if (v.first == "syntetizedFeasibility") {
            params.useSyntetizedFeasibility =
                    v.second.data() == "1" || v.second.data() == "true";
        } else if (v.first == "useVisualisation") {
            params.useVisualisation =
                    v.second.data() == "1" || v.second.data() == "true";
        } else if (v.first == "acceptMin") {
            params.cntCandidatesToKeep = toInt(v.second.data());
        } else if (v.first == "acceptMax") {
            params.cntCandidatesToKeepMax = toInt(v.second.data());
        } else if (v.first == "farProduce") {
            params.cntMorphs = toInt(v.second.data());
        } else if (v.first == "closeProduce") {
            params.cntMorphsInDepth = toInt(v.second.data());
        } else if (v.first == "farCloseThreashold") {
            std::stringstream ss;
            ss << v.second.data();
            ss >> params.distToTargetDepthSwitch;
        } else if (v.first == "maxMorhpsTotal") {
            params.cntMaxMorphs = toInt(v.second.data());
        } else if (v.first == "nonProducingSurvive") {
            params.itThreshold = toInt(v.second.data());
        } else if (v.first == "itersUntilDecay") {
            params.decayThreshold = toInt(v.second.data());
        } else if (v.first == "iterMax") {
            params.cntIterations = toInt(v.second.data());
        } else if (v.first == "maxTimeMinutes") {
            params.timeMaxSeconds = toInt(v.second.data()) * 60;
        } else if (v.first == "weightMin") {
            params.minAcceptableMolecularWeight = toInt(v.second.data());
        } else if (v.first == "weightMax") {
            params.maxAcceptableMolecularWeight = toInt(v.second.data());
        } else if (v.first == "startMolMaxCount") {
            params.startMolMaxCount = toInt(v.second.data());
        } else if (v.first == "maxAcceptableEtalonDistance") {
            params.maxAcceptableEtalonDistance = toInt(v.second.data());
        } else if (v.first == "maxMOOPruns") {
            params.maxMOOPruns = toInt(v.second.data());
        } else if (v.first == "activityMorphing") {
            params.activityMorphing =
                    v.second.data() == "1" || v.second.data() == "true";
        }
    }
}

void loadPropertyTree(boost::property_tree::ptree& pt, IterationSnapshot &snp)
{
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            pt.get_child("data"))
    {
        if (v.first == "etalon") {
            BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
                    v.second) {
                std::string name = v.second.get<std::string>("name");
                double value = v.second.get<double>("value");
                double imputed = v.second.get<double>("imputed");
                double weight = v.second.get<double>("weight");
                //
                snp.relevantDescriptorNames.push_back(name);
                snp.etalonValues.push_back(value);
                snp.imputedValues.push_back(imputed);
                snp.descWeights.push_back(weight);
                //
                double scale = v.second.get<double>("normalization.scale");
                double shift = v.second.get<double>("normalization.shift");
                //
                snp.normalizationCoefficients.push_back(
                    std::make_pair(scale, shift));
            }
        } else if (v.first == "sources") {
            BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
                    v.second) {
                std::string id = v.second.get<std::string>("id");
                std::string smiles = v.second.get<std::string>("smiles");
                MolpherMolecule mol = createMoleculeFromSmile(smiles);
                mol.id = id;
                snp.sourceMols.insert(std::make_pair(mol.smile, mol));
            }
        } else if (v.first == "padelBatchSize") {
            snp.padelBatchSize = toInt(v.second.data());
        } else if (v.first == "source") {
            snp.source = createMoleculeFromSmile(v.second.get<std::string>("smile"));
        } else if (v.first == "target") {
            snp.target = createMoleculeFromSmile(v.second.get<std::string>("smile"));
        } else if (v.first == "fingerprint") {
            snp.fingerprintSelector = FingerprintParse(v.second.data());
        } else if (v.first == "similarity") {
            snp.simCoeffSelector = SimCoeffParse(v.second.data());
        } else if (v.first == "padelPath") {
            snp.padelPath = v.second.data();
        } else if (v.first == "param") {
            loadPropertyTree(v.second, snp.params);
        } else {
            // unexpected token
        }
    }
}

bool loadXml(const std::string &file, IterationSnapshot &snp)
{
    std::ifstream inStream;
    inStream.open(file.c_str(), std::ios_base::in);
    if (!inStream.good()) {
        return false;
    }
    boost::property_tree::ptree pt;
    boost::property_tree::read_xml(inStream, pt);
    loadPropertyTree(pt, snp);
    inStream.close();
    return true;
}

bool loadJson(const std::string &file, IterationSnapshot &snp)
{
    std::ifstream inStream;
    inStream.open(file.c_str(), std::ios_base::in);
    if (!inStream.good()) {
        return false;
    }
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(inStream, pt);
    loadPropertyTree(pt, snp);
    inStream.close();
    return true;
}


/**
 * Return type of file based on extension.
 * @param file
 * @return File type.
 */
FileType fileType(const std::string &file)
{
    if (boost::algorithm::ends_with(file, ".snp")) {
        return SNP;
    } else if (boost::algorithm::ends_with(file, ".xml")) {
        return XML;
    } else if (boost::algorithm::ends_with(file, ".json")) {
        return JSON;
    } else {
        return UNKNOWN;
    }
}

bool IterationSerializer::save(const std::string &file, const IterationSnapshot &snp) const
{
    switch (fileType(file)) {
    default:
    case SNP:
        return saveSnp(file, snp);
    case JSON:
        return saveSnpAsJson(file, snp);
    }
}

bool IterationSerializer::load(const std::string &file, IterationSnapshot &snp) const
{

    switch (fileType(file)) {
    default:
    case SNP:
        return loadSnp(file, snp);
    case XML:
        return loadXml(file, snp);
    case JSON:
        return loadJson(file, snp);
        return false;
    }
}

}
}