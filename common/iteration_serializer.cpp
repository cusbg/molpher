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
#include "boost/optional/optional.hpp"

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

        molecule.put("smiles", iter->second.smiles);
        molecule.put("parenSmiles", iter->second.parentSmile);
        molecule.put("score", iter->second.score);
        molecule.put("iteration", iter->second.iteration);
        boost::property_tree::ptree descriptors;
        for (auto desc : iter->second.descriptors) {
            boost::property_tree::ptree value;
            value.put("", desc);
            descriptors.push_back(std::make_pair("", value));
        }
        if (!descriptors.empty()) {
            molecule.push_back(std::make_pair("descriptors", descriptors));
        }
        candidates.push_back(std::make_pair("", molecule));
    }
    boost::property_tree::ptree pt;
    pt.put("job", snp.jobId);
    pt.put("iteration", snp.iterIdx);
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

/**
 * Create molecule from given smile. The smile may change as it's
 * RDKit smile Set the iteration to zero.
 *
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
    return MolpherMolecule(smile, 0);
}

void loadPropertyTree(const boost::property_tree::ptree& pt, MolpherParam& params)
{

    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, pt)
    {
        if (v.first == "acceptMax") {
            params.cntCandidatesToKeepMax = std::stoi(v.second.data());
        } else if (v.first == "farProduce") {
            params.cntMorphs = std::stoi(v.second.data());
        } else if (v.first == "closeProduce") {
            params.cntMorphsInDepth = std::stoi(v.second.data());
        } else if (v.first == "farCloseThreashold") {
            params.scoreToTargetDepthSwitch = std::stod(v.second.data());
        } else if (v.first == "iterationThreshold") {
            params.iterationThreshold = std::stoi(v.second.data());
        } else if (v.first == "maxTimeMinutes") {
            params.timeMaxSeconds = std::stoi(v.second.data()) * 60;
        } else if (v.first == "weightMin") {
            params.minAcceptableMolecularWeight = std::stod(v.second.data());
        } else if (v.first == "weightMax") {
            params.maxAcceptableMolecularWeight = std::stod(v.second.data());
        } else if (v.first == "moopPruns") {
            params.mooopPruns = std::stoi(v.second.data());
        } else if (v.first == "filterScriptCommand") {
            params.filterScriptCommand = v.second.data();
        } else if (v.first == "scoreScriptCommand") {
            params.scoreScriptCommand = v.second.data();
        } else if (v.first == "descriptorsScriptCommand") {
            params.descriptorsScriptCommand = v.second.data();
        } else if (v.first == "scoreIsDistance") {
            params.scoreIsDistance =
                    v.second.data() == "1" || v.second.data() == "true";
        }
    }
}

void loadPropertyTree(boost::property_tree::ptree& pt, IterationSnapshot &snp)
{

    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            pt.get_child("data"))
    {
        if (v.first == "id") {
            snp.jobId = v.second.data();
        } else if (v.first == "sources") {

            BOOST_FOREACH(
                    boost::property_tree::ptree::value_type const& v, v.second)
            {
                std::string smiles = v.second.get<std::string>("smiles");
                MolpherMolecule mol = createMoleculeFromSmile(smiles);
                snp.sourceMols.insert(std::make_pair(mol.smiles, mol));
            }
        } else if (v.first == "param") {
            loadPropertyTree(v.second, snp.params);
        } else if (v.first == "storagePath") {
            snp.storagePath = v.second.data();
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
