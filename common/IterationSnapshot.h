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
#include "chemoper_selectors.h"

#include "MolpherParam.h"
#include "MolpherMolecule.h"
#include "activity_data_processing.h"
#include "descriptor/CSV.h"
#include "descriptor/DataConverter.h"

void SynchCout(const std::string &s);

struct IterationSnapshot
{
    typedef std::map<std::string, MolpherMolecule> CandidateMap;

    typedef std::map<std::string, boost::uint32_t> MorphDerivationMap;

    typedef std::vector<std::string> PrunedMoleculeVector;

    typedef std::map<std::string, std::string> ScaffoldSmileMap;

    friend class boost::serialization::access;

    IterationSnapshot() : jobId(""), iterIdx(0), elapsedSeconds(0)
    {
    }

    template<typename Archive> void serialize(
            Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(jobId);
        ar & BOOST_SERIALIZATION_NVP(iterIdx);
        ar & BOOST_SERIALIZATION_NVP(elapsedSeconds);
        ar & BOOST_SERIALIZATION_NVP(candidates);
        ar & BOOST_SERIALIZATION_NVP(sourceMols);
        ar & BOOST_SERIALIZATION_NVP(morphDerivations);
        ar & BOOST_SERIALIZATION_NVP(prunedDuringThisIter);
        ar & BOOST_SERIALIZATION_NVP(params);
        ar & BOOST_SERIALIZATION_NVP(storagePath);
    }

    bool isValid()
    {
        if (!params.isValid()) {
            SynchCout("\tInvalid parameters.");
            return false;
        }
        for (auto it = sourceMols.begin(); it != sourceMols.end(); ++it) {
            if (!it->second.IsValid()) {
                SynchCout("\tInvalid source molecule.");
                return false;
            }
        }
        return true;
    }

    /**
     * User given job ID.
     */
    std::string jobId;

    /**
     * Iteration number.
     */
    boost::uint32_t iterIdx;

    /**
     * Total time spend by working on this instance of snapshot.
     */
    boost::uint32_t elapsedSeconds;

    /**
     * Candidate molecules ie. molecule storage.
     */
    CandidateMap candidates;

    /**
     * List of starting molecules.
     */
    CandidateMap sourceMols;

    /**
     *
     */
    MorphDerivationMap morphDerivations;

    /**
     * Molecules pruned in this iteration, used only to store
     * this information into the snapshot file.
     */
    PrunedMoleculeVector prunedDuringThisIter;

    /**
     * Parameters for exploration.
     */
    MolpherParam params;

    /**
     * Storage path for the result.
     */
    std::string storagePath;

};

BOOST_CLASS_IMPLEMENTATION(IterationSnapshot, object_serializable)
BOOST_CLASS_TRACKING(IterationSnapshot, track_never)
