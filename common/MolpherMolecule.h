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
#include <set>
#include <map>
#include <vector>
#include <cfloat>
#include <cmath>

#include <boost/cstdint.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/level.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include <iostream>

struct MolpherMolecule
{

    MolpherMolecule() :
    score(0),
    parentChemOper(0),
    molecularWeight(0.0),
    itersWithoutDistImprovement(0),
    iteration(0)
    {
    }

    MolpherMolecule(std::string &smile, boost::int32_t iteration) :
    score(0),
    smiles(smile),
    parentChemOper(0),
    molecularWeight(0.0),
    itersWithoutDistImprovement(0),
    iteration(iteration)
    {
    }

    MolpherMolecule(std::string &smile, std::string &parentSmile,
            boost::int32_t parentChemOper, double molecularWeight,
            boost::int32_t iteration) :
    score(0),
    smiles(smile),
    parentChemOper(parentChemOper),
    parentSmile(parentSmile),
    molecularWeight(molecularWeight),
    itersWithoutDistImprovement(0),
    iteration(iteration)
    {
    }

    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(smiles);
        ar & BOOST_SERIALIZATION_NVP(parentChemOper);
        ar & BOOST_SERIALIZATION_NVP(parentSmile);
        ar & BOOST_SERIALIZATION_NVP(descendants);
        ar & BOOST_SERIALIZATION_NVP(historicDescendants);
        ar & BOOST_SERIALIZATION_NVP(score);
        ar & BOOST_SERIALIZATION_NVP(descriptors);
        ar & BOOST_SERIALIZATION_NVP(itersWithoutDistImprovement);
        //
        ar & BOOST_SERIALIZATION_NVP(molecularWeight);
    }

    bool IsValid()
    {
        return (!smiles.empty());
    }

    std::string smiles;

    boost::int32_t parentChemOper;

    std::string parentSmile;

    std::set<std::string> descendants;

    std::set<std::string> historicDescendants;

    /**
     * Represent a score (similarity, distance) for a molecule.
     *
     * @TODO Revise usage!
     */
    double score;

    /**
     * Values of molecule properties.
     */
    std::vector<double> descriptors;

    /**
     * Number of iterations without score improvement for this molecule
     * and it's subtree.
     */
    boost::uint32_t itersWithoutDistImprovement;

    /**
     * Number of iteration in which this molecule has been created.
     */
    boost::uint32_t iteration;

    /**
     * Used for filtering.
     * TODO Move to filters.
     */
    double molecularWeight;

};

// turn off versioning
BOOST_CLASS_IMPLEMENTATION(MolpherMolecule, object_serializable)
// turn off tracking
BOOST_CLASS_TRACKING(MolpherMolecule, track_never)
