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

#include <cfloat>

#include <boost/version.hpp>
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/nvp.hpp>

struct MolpherParam
{
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(cntCandidatesToKeepMax);
        ar & BOOST_SERIALIZATION_NVP(cntMorphs);
        ar & BOOST_SERIALIZATION_NVP(cntMorphsInDepth);
        ar & BOOST_SERIALIZATION_NVP(scoreToTargetDepthSwitch);
        ar & BOOST_SERIALIZATION_NVP(iterationThreshold);
        ar & BOOST_SERIALIZATION_NVP(cntIterations);
        ar & BOOST_SERIALIZATION_NVP(timeMaxSeconds);
        ar & BOOST_SERIALIZATION_NVP(minAcceptableMolecularWeight);
        ar & BOOST_SERIALIZATION_NVP(maxAcceptableMolecularWeight);

        ar & BOOST_SERIALIZATION_NVP(mooopPruns);
        ar & BOOST_SERIALIZATION_NVP(chemOperSelectors);
        ar & BOOST_SERIALIZATION_NVP(filterScriptCommand);
        ar & BOOST_SERIALIZATION_NVP(scoreScriptCommand);
        ar & BOOST_SERIALIZATION_NVP(descriptorsScriptCommand);
        ar & BOOST_SERIALIZATION_NVP(scoreIsDistance);

    }

    MolpherParam() {
        chemOperSelectors.push_back(OP_ADD_ATOM);
        chemOperSelectors.push_back(OP_REMOVE_ATOM);
        chemOperSelectors.push_back(OP_ADD_BOND);
        chemOperSelectors.push_back(OP_REMOVE_BOND);
        chemOperSelectors.push_back(OP_MUTATE_ATOM);
        chemOperSelectors.push_back(OP_INTERLAY_ATOM);
        chemOperSelectors.push_back(OP_BOND_REROUTE);
        chemOperSelectors.push_back(OP_BOND_CONTRACTION);
    }

    bool isValid()
    {
        return (cntMorphs > 0) &&
                (cntMorphsInDepth > 0) &&
                (iterationThreshold > 0) &&
                (cntIterations > 0) &&
                (timeMaxSeconds > 0) &&
                (minAcceptableMolecularWeight >= 0.0) &&
                (minAcceptableMolecularWeight <= maxAcceptableMolecularWeight) &&
                (maxAcceptableMolecularWeight > 0.0);
    }

    /**
     * Maximum number of candidates to accept in a single iteration.
     * Use -1 to accept all.
     */
    boost::uint32_t cntCandidatesToKeepMax = 150;

    /**
     * Number of morphs to generate from a single leave.
     */
    boost::uint32_t cntMorphs = 90;

    /**
     * Number of morphs to generate from a single leave in depth.
     * The depth is defined by scoreToTargetDepthSwitch.
     */
    boost::uint32_t cntMorphsInDepth = 200;

    /**
     * Where to switch from cntMorphs to cntMorphsInDepth.
     */
    double scoreToTargetDepthSwitch = 0.1;

    /**
     * Number of iterations after which the molecule is removed from tree.
     * Use -1 to disable this feature.
     *
     * Can be used to limit the in-memory exploration tree depth.
     */
    boost::uint32_t iterationThreshold = -1;

    /**
     * Maximum number of iterations.
     */
    boost::uint32_t cntIterations = 100;

    /**
     * Time limit of execution.
     */
    boost::uint32_t timeMaxSeconds = -1;

    /**
     * Definition of filter for molecular filtering.
     */
    double minAcceptableMolecularWeight;

    double maxAcceptableMolecularWeight;

    /**
     * Number of MOOP runs to execute. Use -1 to disable MOOP filter.
     * The molecule.descriptors are used in the filter.
     */
    boost::uint32_t mooopPruns = -1;

    /**
     * Vector or used mophing operators.
     */
    std::vector<boost::int32_t> chemOperSelectors;

    /**
     * Path to the script that compute filter value for molecules.
     *
     * The value 1 means that the molecule pass the filter.
     *
     * CSV-like file is used to transfer the data.
     */
    std::string filterScriptCommand = "";

    /**
     * Path to script that computes molecule score.
     *
     * CSV-like file is used to transfer the data.
     */
    std::string scoreScriptCommand = "";

    /**
     * Path to script that computes molecule descriptors.
     *
     * CSV-like file is used to transfer the data.
     */
    std::string descriptorsScriptCommand = "";

    /**
     * If true molecule.score is a distance (default value) - ie. the smaller
     * the closer. This is the default behaviour.
     *
     * If it is false the distToTarget represent a similarity and thus the
     * greater the value the close to the target.
     */
    bool scoreIsDistance = true;

};

BOOST_CLASS_IMPLEMENTATION(MolpherParam, object_serializable)
BOOST_CLASS_TRACKING(MolpherParam, track_never)