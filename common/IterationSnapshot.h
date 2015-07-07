/*
 Copyright (c) 2012 Petr Koupy

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <boost/version.hpp>
#include <boost/cstdint.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#include "fingerprint_selectors.h"
#include "simcoeff_selectors.h"
#include "dimred_selectors.h"
#include "chemoper_selectors.h"
#include "scaffold_selectors.hpp"

#include "MolpherParam.h"
#include "MolpherMolecule.h"
#include "activity_data_processing.h"
#include "CSV.h"
#include "CSV_parser/include/DataConverter.h"

struct IterationSnapshot
{
    IterationSnapshot() :
        jobId(0),
        iterIdx(0),
        elapsedSeconds(0),
        inputActivityDataDir(""),
        saveDataAsCSVs(true),
        saveOnlyMorphData(true),
        activityMorphingInitialized(false)
    {
        fingerprintSelector = DEFAULT_FP;
        simCoeffSelector = DEFAULT_SC;
        dimRedSelector = DEFAULT_DR;
        scaffoldSelector = SF_NONE;
    }

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // usage of BOOST_SERIALIZATION_NVP macro enable us to use xml serialisation
        ar & BOOST_SERIALIZATION_NVP(jobId);
        ar & BOOST_SERIALIZATION_NVP(iterIdx); 
        ar & BOOST_SERIALIZATION_NVP(elapsedSeconds);
        ar & BOOST_SERIALIZATION_NVP(fingerprintSelector);
        ar & BOOST_SERIALIZATION_NVP(simCoeffSelector);
        ar & BOOST_SERIALIZATION_NVP(dimRedSelector);
        ar & BOOST_SERIALIZATION_NVP(chemOperSelectors);
        ar & BOOST_SERIALIZATION_NVP(params);
        ar & BOOST_SERIALIZATION_NVP(source);
        ar & BOOST_SERIALIZATION_NVP(target);
        ar & BOOST_SERIALIZATION_NVP(decoys);
        ar & BOOST_SERIALIZATION_NVP(candidates);
        ar & BOOST_SERIALIZATION_NVP(morphDerivations);
        ar & BOOST_SERIALIZATION_NVP(prunedDuringThisIter);
        ar & BOOST_SERIALIZATION_NVP(tempSource);
        ar & BOOST_SERIALIZATION_NVP(scaffoldSelector);
        ar & BOOST_SERIALIZATION_NVP(pathMolecules);
        ar & BOOST_SERIALIZATION_NVP(pathScaffoldMolecules);
        ar & BOOST_SERIALIZATION_NVP(candidateScaffoldMolecules);
        ar & BOOST_SERIALIZATION_NVP(inputActivityDataDir);
        ar & BOOST_SERIALIZATION_NVP(proteinTargetName);
        ar & BOOST_SERIALIZATION_NVP(activesDescriptorsFile);
        ar & BOOST_SERIALIZATION_NVP(activityMorphingInitialized);
        ar & BOOST_SERIALIZATION_NVP(relevantDescriptorNames);
        ar & BOOST_SERIALIZATION_NVP(actives);
        ar & BOOST_SERIALIZATION_NVP(activesIDs);
        ar & BOOST_SERIALIZATION_NVP(etalonValues);
        ar & BOOST_SERIALIZATION_NVP(normalizationCoefficients);
    }

    bool IsValid()
    {
        bool decoysValid = true;
        std::vector<MolpherMolecule>::iterator it;
        for (it = decoys.begin(); it != decoys.end(); ++it) {
            if (!it->IsValid()) {
                decoysValid = false;
                break;
            }
        }

        bool scaffoldsValid = !ScaffoldMode() ||
            (tempSource.IsValid() && !pathMolecules.empty());
        
        if (params.activityMorphing) {
            bool activityMorphingValid = true;
            activityMorphingValid = params.startMolMaxCount >= 0;
            fs::path data_dir(inputActivityDataDir);
            if (activityMorphingValid && fs::exists(data_dir)) {
                fs::path actives(allActivesSMILESFile);
                fs::path train_actives_descs(activesDescriptorsFile);
                fs::path test_actives_descs(testActivesDescriptorsFile);
                fs::path inactives_desc(inactivesDescriptorsFile);
                fs::path weights(descriptorWeightsFile);
                fs::path source_mols(sourceMolsDescriptorsFile);
                
                activityMorphingValid = fs::exists(actives) 
                        && fs::exists(train_actives_descs)
                        && fs::exists(test_actives_descs)
                        && fs::exists(inactives_desc)
                        && fs::exists(source_mols)
                        && fs::exists(weights);
                
                if (etalonFile.size() != 0) {
                    fs::path et_file(etalonFile);
                    activityMorphingValid = fs::exists(et_file);
                }
                
            } else {
                activityMorphingValid = false;
            }
            
            return (!chemOperSelectors.empty()) &&
            params.IsValid() &&
            decoysValid &&
            activityMorphingValid;
        }

        return (!chemOperSelectors.empty()) &&
            params.IsValid() &&
            source.IsValid() &&
            target.IsValid() &&
            (source.smile != target.smile) &&
            decoysValid &&
            scaffoldsValid;
    }

    bool ScaffoldMode() const
    {
        return ((int)scaffoldSelector != SF_NONE);
    }
    
    bool IsActivityMorphingOn() const {
        return params.activityMorphing && !activityMorphingInitialized;
    }
  
    typedef std::map<std::string, MolpherMolecule> CandidateMap;
    typedef std::map<std::string, boost::uint32_t> MorphDerivationMap;
    typedef std::vector<std::string> PrunedMoleculeVector;
    typedef std::map<std::string, std::string> ScaffoldSmileMap;

    void saveEtalonDistancesCSV(CSVparse::CSV& descriptors_CSV, const std::string& out_path) {
        CSVparse::CSV output;
        std::vector<std::string> ids;
        std::vector<double> etal_dists;
        for (unsigned int idx = 0; idx < descriptors_CSV.getRowCount(); idx++) {
            std::string id = CSVparse::DataConverter::ValToString(idx+1);
            ids.push_back(id);
            MolpherMolecule mm("", id);
            mm.descriptorsFilePath = "";

            for (std::vector<std::string>::iterator it = relevantDescriptorNames.begin(); it != relevantDescriptorNames.end(); it++) {
                mm.descriptorValues.push_back(descriptors_CSV.getFloatData(*it)[idx]);
            }

            mm.normalizeDescriptors(normalizationCoefficients, imputedValues);
            mm.ComputeEtalonDistances(etalonValues, descWeights);

            etal_dists.push_back(mm.distToEtalon);
        }
        output.addStringData("Number", ids);
        output.addFloatData("DistToEtalon", etal_dists);
        output.write(out_path);
    }
       
    void PrepareActivityData() {        
        // read the selected features
//        CSVparse::CSV analysis_results_CSV(inputActivityDataDir + proteinTargetName + analysisResultsSuffix, ";", "NA", true, true);
//        const std::vector<string> &desc_names = analysis_results_CSV.getHeader();
//        unsigned int rejected_row_idx = analysis_results_CSV.getRowIdx("rejected");
//        for (std::vector<string>::const_iterator it = desc_names.begin(); it != desc_names.end(); it++) {
//            if (analysis_results_CSV.getFloatData(*it)[rejected_row_idx] == 1) {
//                relevantDescriptorNames.push_back(*it);
//            }
//        }
        
        CSVparse::CSV weights(descriptorWeightsFile, ",", "NA");
        const std::vector<string> &desc_names = weights.getHeader();
        for (std::vector<string>::const_iterator it = desc_names.begin(); it != desc_names.end(); it++) {
            relevantDescriptorNames.push_back(*it);
            descWeights.push_back(weights.getFloatData(*it)[0]);
        }
        
        // load data about actives
        CSVparse::CSV actives_descs_CSV(activesDescriptorsFile, ",", "");
        activesIDs = actives_descs_CSV.getStringData("Name");
        for (std::vector<string>::const_iterator id = activesIDs.begin(); id != activesIDs.end(); id++) {
            activesIDsSet.insert(*id);
        }
        
        // load data about source mols
        CSVparse::CSV sources_descs_CSV(sourceMolsDescriptorsFile, ",", "");
        sourceIDs = sources_descs_CSV.getStringData("Name");
        for (std::vector<string>::const_iterator id = sourceIDs.begin(); id != sourceIDs.end(); id++) {
            sourceIDsSet.insert(*id);
        }
        std::vector<std::vector<double> > source_mols;
        adp::readRelevantData(sources_descs_CSV, relevantDescriptorNames, source_mols);
        
        // load data about test mols
        CSVparse::CSV tests_descs_CSV(testActivesDescriptorsFile, ",", "");
        testIDs = tests_descs_CSV.getStringData("Name");
        for (std::vector<string>::const_iterator id = testIDs.begin(); id != testIDs.end(); id++) {
            testIDsSet.insert(*id);
        }
        std::vector<std::vector<double> > test_mols;
        adp::readRelevantData(tests_descs_CSV, relevantDescriptorNames, test_mols);
        
        // load data about decoys
        CSVparse::CSV decoys_descs_CSV(inactivesDescriptorsFile, ",", "");
        
        // extract only the data for selected features
        std::vector<std::vector<double> > all_mols;
        std::vector<std::vector<double> > actives;
        adp::readRelevantData(actives_descs_CSV, relevantDescriptorNames, all_mols);
        if (saveDataAsCSVs) {
            // write relevant data to disk
//            CSVparse::CSV actives_CSV;
            adp::readRelevantData(actives_descs_CSV, relevantDescriptorNames, actives);
//            actives_CSV.write(inputActivityDataDir + proteinTargetName + "_active_mols_selected_feats.csv");
//            CSVparse::CSV decoys_CSV;
            adp::readRelevantData(decoys_descs_CSV, relevantDescriptorNames, all_mols);
//            decoys_CSV.write(inputActivityDataDir + proteinTargetName + "_decoy_mols_selected_feats.csv");
        } else {
            adp::readRelevantData(actives_descs_CSV, relevantDescriptorNames, actives);
            adp::readRelevantData(decoys_descs_CSV, relevantDescriptorNames, all_mols);
        }
      
        // compute normalization (feature scaling) coefficients and normalize all data
        adp::normalizeData(all_mols, 0, 1, normalizationCoefficients, imputedValues);
        adp::normalizeData(actives, imputedValues, normalizationCoefficients);
        
        if (etalonFile.size() != 0) {
            CSVparse::CSV etalon_csv(etalonFile, ",", "");
            assert(etalon_csv.getColumnCount() == normalizationCoefficients.size());
            for (unsigned int desc_idx = 0; desc_idx != etalon_csv.getColumnCount(); desc_idx++ ) {
                double A = normalizationCoefficients[desc_idx].first;
                double B = normalizationCoefficients[desc_idx].second;
                double val = A * etalon_csv.getFloatData(desc_idx)[0] + B;
                etalonValues.push_back(val);
            }
        } else {
            // use the scaled data to compute etalon values
            adp::computeEtalon(actives, etalonValues, params.etalonType);
        }
        
        // read structures of active, test and source molecules
        CSVparse::CSV all_actives(allActivesSMILESFile, "\t", "", false, false);
        const std::vector<std::string> &ids = all_actives.getStringData(1);
        const std::vector<std::string> &smiles = all_actives.getStringData(0);
        std::map<std::string, std::string> idSmileMap;
        for (unsigned int idx = 0; idx < smiles.size(); idx++) {
            idSmileMap[ids[idx]] = smiles[idx];
        }
        std::vector<std::string> actives_smiles;
        for (unsigned int idx = 0; idx < activesIDs.size(); idx++) {
            actives_smiles.push_back(idSmileMap[activesIDs[idx]]);
        }
//        adp::readPropFromSDF(inputActivityDataDir + activesSDFFile, "PUBCHEM_OPENEYE_CAN_SMILES", actives_smiles);
        
        // create MolpherMolecule from each active
        std::vector<std::string>::size_type idx = 0;
        for (std::vector<std::string>::iterator it = actives_smiles.begin(); it != actives_smiles.end(); it++, idx++) {
            MolpherMolecule mm(*it, activesIDs[idx], actives[idx], activesDescriptorsFile, relevantDescriptorNames);
            mm.ComputeEtalonDistances(etalonValues, descWeights);
            this->actives.insert(std::make_pair<std::string, MolpherMolecule>(*it, mm));
        }

        // create MolpherMolecule from sources and tests
        unsigned int test_counter = 0;
        unsigned int source_counter = 0;
        for (unsigned int idx = 0; idx < ids.size(); idx++) {
            if (testIDsSet.find(ids[idx]) != testIDsSet.end()) {
                MolpherMolecule mm(idSmileMap[ids[idx]], ids[idx]);
                mm.descriptorsFilePath = activesDescriptorsFile;
                
                mm.descriptorValues = test_mols[test_counter];
                
                mm.normalizeDescriptors(normalizationCoefficients, imputedValues);
                mm.ComputeEtalonDistances(etalonValues, descWeights);
                
                testActives.insert(std::make_pair<std::string, MolpherMolecule>(mm.smile, mm));
                test_counter++;
            }
            if (sourceIDsSet.find(ids[idx]) != sourceIDsSet.end()) {
                MolpherMolecule mm(idSmileMap[ids[idx]], ids[idx]);
                mm.descriptorsFilePath = sourceMolsDescriptorsFile;
                
                mm.descriptorValues = source_mols[source_counter];
                
                mm.normalizeDescriptors(normalizationCoefficients, imputedValues);
                mm.ComputeEtalonDistances(etalonValues, descWeights);
                
                sourceMols.insert(std::make_pair<std::string, MolpherMolecule>(mm.smile, mm));
                source_counter++;
            }
        }

        // save etalon distances for actives and decoys
        if (saveDataAsCSVs) {
            saveEtalonDistancesCSV(decoys_descs_CSV, inputActivityDataDir + proteinTargetName + "_decoy_mols_dists.csv");
            saveEtalonDistancesCSV(actives_descs_CSV, inputActivityDataDir + proteinTargetName + "_active_mols_dists.csv");
            saveEtalonDistancesCSV(sources_descs_CSV, inputActivityDataDir + proteinTargetName + "_source_mols_dists.csv");
            saveEtalonDistancesCSV(tests_descs_CSV, inputActivityDataDir + proteinTargetName + "_test_mols_dists.csv");
        }
        
//        exit(0);
        
        activityMorphingInitialized = true;
    }

    /**
     * Job id.
     */
    boost::uint32_t jobId;

    /**
     * Iteration id.
     */
    boost::uint32_t iterIdx;

    /**
     * Total time spend by working on this instance of snapshot.
     */
    boost::uint32_t elapsedSeconds;

    /**
     * Fingerprint used in algorithm.
     * @see FingerprintSelector
     */
    boost::int32_t fingerprintSelector;

    /**
     * Similarity coef used in algorithm.
     * @see SimCoeffSelector
     */
    boost::int32_t simCoeffSelector;

    /**
     * Id of used location computing algorithm.
     * @see DimRedSelector
     */
    boost::int32_t dimRedSelector;

    /**
     * Vector or used morphing operator. Determine
     * possible morphing operation applied during generating
     * new morphs.
     * @see ChemOperSelector
     */
    std::vector<boost::int32_t> chemOperSelectors;

    /**
     * Parameters for morphing algorithm.
     */
    MolpherParam params;

    /**
     * Source molecule.
     */
    MolpherMolecule source;

    /**
     * Target molecule.
     */
    MolpherMolecule target;

    /**
     * Decoys used during exploration of chemical space.
     */
    std::vector<MolpherMolecule> decoys;

    /**
     * Candidate molecules ie. molecule storage.
     */
    CandidateMap candidates;
    CandidateMap actives;
    CandidateMap testActives;
    CandidateMap sourceMols;
    std::vector<std::string> activesIDs;
    std::set<std::string> activesIDsSet;
    std::vector<std::string> sourceIDs;
    std::set<std::string> sourceIDsSet;
    std::vector<std::string> testIDs;
    std::set<std::string> testIDsSet;
    std::vector<double> etalonValues;
//    std::vector<std::vector<double> > actives;
    std::vector<std::pair<double, double> > normalizationCoefficients;
    std::vector<double> imputedValues;
    std::vector<std::string> relevantDescriptorNames;
    std::vector<double> descWeights;
    bool saveDataAsCSVs;
    bool saveOnlyMorphData;
    bool activityMorphingInitialized;
    
    /**
     * activity data files information
     */
    std::string inputActivityDataDir;
    std::string proteinTargetName;
    std::string allActivesSMILESFile;
    std::string activesDescriptorsFile;
    std::string sourceMolsDescriptorsFile;
    std::string testActivesDescriptorsFile;
    std::string inactivesDescriptorsFile;
    std::string descriptorWeightsFile;
    std::string etalonFile;

    MorphDerivationMap morphDerivations;

    PrunedMoleculeVector prunedDuringThisIter;

    /// variables used for scaffold hopping
    MolpherMolecule tempSource;
    boost::int32_t scaffoldSelector;
    std::vector<MolpherMolecule> pathMolecules;
    ScaffoldSmileMap pathScaffoldMolecules;
    ScaffoldSmileMap candidateScaffoldMolecules;
};

// add information about version to archive
//BOOST_CLASS_IMPLEMENTATION(IterationSnapshot, object_class_info)
// turn off versioning
BOOST_CLASS_IMPLEMENTATION(IterationSnapshot, object_serializable)
// turn off tracking
BOOST_CLASS_TRACKING(IterationSnapshot, track_never)
// specify version
//BOOST_CLASS_VERSION(IterationSnapshot, 1)

struct IterationSnapshotProxy
{
    IterationSnapshotProxy() :
        jobId(0),
        iterIdx(0)
    {
    }

    IterationSnapshotProxy(std::string &storage,
        boost::uint32_t jobId, boost::uint32_t iterIdx
        ) :
        storage(storage),
        jobId(jobId),
        iterIdx(iterIdx)
    {
    }

    std::string storage;
    boost::uint32_t jobId;
    boost::uint32_t iterIdx;
};
