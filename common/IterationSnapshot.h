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

#include "fingerprint_selectors.h"
#include "simcoeff_selectors.h"
#include "dimred_selectors.h"
#include "chemoper_selectors.h"
#include "scaffold_selectors.hpp"

#include "MolpherParam.h"
#include "MolpherMolecule.h"
#include "activity_data_processing.h"
#include "descriptor/CSV.h"
#include "descriptor/DataConverter.h"

struct IterationSnapshot
{

    IterationSnapshot() : jobId(0), iterIdx(0), elapsedSeconds(0),
    padelBatchSize(1000)
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
        // BOOST_SERIALIZATION_NVP macro enable us to use xml serialisation
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
        ar & BOOST_SERIALIZATION_NVP(prunedDuringThisIter);
        ar & BOOST_SERIALIZATION_NVP(tempSource);
        ar & BOOST_SERIALIZATION_NVP(scaffoldSelector);
        ar & BOOST_SERIALIZATION_NVP(pathMolecules);
        ar & BOOST_SERIALIZATION_NVP(pathScaffoldMolecules);
        ar & BOOST_SERIALIZATION_NVP(candidateScaffoldMolecules);
        ar & BOOST_SERIALIZATION_NVP(relevantDescriptorNames);
        ar & BOOST_SERIALIZATION_NVP(etalonValues);
        ar & BOOST_SERIALIZATION_NVP(padelPath);
    }

    bool IsValid()
    {
        bool decoysValid = true;
        for (auto it = decoys.begin(); it != decoys.end(); ++it) {
            if (!it->IsValid()) {
                decoysValid = false;
                break;
            }
        }
        if (ScaffoldMode()) {
            if (!tempSource.IsValid() && pathMolecules.empty()) {
                return false;
            }
        }
        if (params.activityMorphing) {
            bool activityMorphingValid = params.startMolMaxCount >= 0 &&
                    etalonValues.size() == relevantDescriptorNames.size() &&
                    etalonValues.size() == descWeights.size();
            return (!chemOperSelectors.empty()) &&
                    params.IsValid() &&
                    decoysValid &&
                    activityMorphingValid;
        } else {
            if (source.smile == "") {
                return false;
            }
        }
        return (!chemOperSelectors.empty()) &&
                params.IsValid() &&
                source.IsValid() &&
                target.IsValid() &&
                (source.smile != target.smile) &&
                decoysValid;
    }

    bool ScaffoldMode() const
    {
        return ((int) scaffoldSelector != SF_NONE);
    }

    typedef std::map<std::string, MolpherMolecule> CandidateMap;
    typedef std::map<std::string, boost::uint32_t> MorphDerivationMap;
    typedef std::vector<std::string> PrunedMoleculeVector;
    typedef std::map<std::string, std::string> ScaffoldSmileMap;

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
     * Vector or used morphing operators.
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

    /**
     * Used for MOO, represent the etalon.
     */
    std::vector<double> etalonValues;

    /**
     * Used for MOO, represent names of used descriptors.
     * Must have same size as etalonValues.
     */
    std::vector<std::string> relevantDescriptorNames;

    /**
     * Used for MOO, values to impute if the descriptor computation fail.
     */
    std::vector<double> imputedValues;

    /**
     * Weight of descriptors, must have same size as etalonValues.
     */
    std::vector<double> descWeights;

    /**
     * Used for MOO with PaDEL.
     */
    unsigned int padelBatchSize;

    /**
     * Introduced by MOO, as a source for candidate molecules in a first
     * iteration.
     */
    CandidateMap sourceMols;

    /**
     * Path to PaDEL.
     */
    std::string padelPath;

    /**
     * Values used to normalise computed descriptors. Used by MOO.
     *
     * The first value represent a scale, the second value
     * represent a shift. The application is: value * scale + shift .
     *
     * Values are computed based on the input data.
     */
    std::vector<std::pair<double, double> > normalizationCoefficients;

    MorphDerivationMap morphDerivations;

    /**
     * Molecules pruned in this iteration, used only to store
     * this information into the snapshot file.
     */
    PrunedMoleculeVector prunedDuringThisIter;

    /**
     * Used for scaffold hopping.
     */
    MolpherMolecule tempSource;

    /**
     * Used for scaffold hopping.
     */
    boost::int32_t scaffoldSelector;

    /**
     * Used for scaffold hopping.
     */
    std::vector<MolpherMolecule> pathMolecules;

    /**
     * Used for scaffold hopping.
     */
    ScaffoldSmileMap pathScaffoldMolecules;

    /**
     * Used for scaffold hopping.
     */
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
