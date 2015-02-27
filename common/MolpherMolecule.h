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
        parentChemOper(0),
        scaffoldLevelCreation(0),
        distToTarget(DBL_MAX),
        distToEtalon(DBL_MAX),
        distToClosestDecoy(0),
        molecularWeight(0.0),
        sascore(0.0),
        itersWithoutDistImprovement(0),
        posX(0),
        posY(0)
    {
    }

    MolpherMolecule(std::string &smile) :
        smile(smile),
        parentChemOper(0),
        scaffoldLevelCreation(0),
        distToTarget(DBL_MAX),
        distToEtalon(DBL_MAX),
        distToClosestDecoy(0),
        molecularWeight(0.0),
        sascore(0.0),
        itersWithoutDistImprovement(0),
        posX(0),
        posY(0)
    {
    }
    
    MolpherMolecule(const std::string &smile, const std::string &id) :
        smile(smile),
        id(id),
        parentChemOper(0),
        scaffoldLevelCreation(0),
        distToTarget(DBL_MAX),
        distToEtalon(DBL_MAX),
        distToClosestDecoy(0),
        molecularWeight(0.0),
        sascore(0.0),
        itersWithoutDistImprovement(0),
        posX(0),
        posY(0)
    {
    }
    
    MolpherMolecule(const std::string &smile,
        const std::string &id,
        const std::vector<double> &descriptors,
        const std::string &descriptorsFilePath,
        const std::vector<std::string> &relevantDescriptorNames
    ) :
        smile(smile),
        id(id),
        descriptorValues(descriptors),
        descriptorsFilePath(descriptorsFilePath),
//        relevantDescriptorIndices(relevantDescriptorIndices),
        relevantDescriptorNames(relevantDescriptorNames),
        parentChemOper(0),
        scaffoldLevelCreation(0),
        distToTarget(DBL_MAX),
        distToEtalon(DBL_MAX),
        distToClosestDecoy(0),
        molecularWeight(0.0),
        sascore(0.0),
        itersWithoutDistImprovement(0),
        posX(0),
        posY(0)
    {
    }

    MolpherMolecule(std::string &smile, std::string &formula) :
        smile(smile),
        formula(formula),
        parentChemOper(0),
        scaffoldLevelCreation(0),
        distToTarget(DBL_MAX),
        distToEtalon(DBL_MAX),
        distToClosestDecoy(0),
        molecularWeight(0.0),
        sascore(0.0),
        itersWithoutDistImprovement(0),
        posX(0),
        posY(0)
    {
    }

    MolpherMolecule(std::string &smile, std::string &formula,
        std::string &parentSmile, boost::int32_t parentChemOper,
        std::string &scaffoldSmile, boost::int32_t scaffoldLevelCreation,
        double distToTarget, double distToClosestDecoy, double molecularWeight,
        double sascore
        ) :
        smile(smile),
        formula(formula),
        parentChemOper(parentChemOper),
        parentSmile(parentSmile),
        scaffoldSmile(scaffoldSmile),
        scaffoldLevelCreation(scaffoldLevelCreation),
        distToTarget(distToTarget),
        distToEtalon(DBL_MAX),
        distToClosestDecoy(distToClosestDecoy),
        molecularWeight(molecularWeight),
        sascore(sascore),
        itersWithoutDistImprovement(0),
        posX(0),
        posY(0)
    {
    }

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // usage of BOOST_SERIALIZATION_NVP enable us to use xml serialisation
        ar & BOOST_SERIALIZATION_NVP(smile) &
            BOOST_SERIALIZATION_NVP(formula) &
            BOOST_SERIALIZATION_NVP(parentChemOper) &
            BOOST_SERIALIZATION_NVP(parentSmile) &
            BOOST_SERIALIZATION_NVP(descendants) &
            BOOST_SERIALIZATION_NVP(historicDescendants) &
            BOOST_SERIALIZATION_NVP(scaffoldSmile) &
            BOOST_SERIALIZATION_NVP(scaffoldLevelCreation) &
            BOOST_SERIALIZATION_NVP(distToTarget) &
            BOOST_SERIALIZATION_NVP(distToEtalon) &
            BOOST_SERIALIZATION_NVP(distToClosestDecoy) &
            BOOST_SERIALIZATION_NVP(molecularWeight) &
            BOOST_SERIALIZATION_NVP(itersWithoutDistImprovement) &
            BOOST_SERIALIZATION_NVP(posX) &
            BOOST_SERIALIZATION_NVP(etalonDistances) &
//            BOOST_SERIALIZATION_NVP(relevantDescriptorIndices) &
            BOOST_SERIALIZATION_NVP(relevantDescriptorNames) &
            BOOST_SERIALIZATION_NVP(descriptorValues) &
            BOOST_SERIALIZATION_NVP(descriptorsFilePath) &
            BOOST_SERIALIZATION_NVP(id) &
            BOOST_SERIALIZATION_NVP(posY);
    }

    bool IsValid()
    {
        return (!smile.empty());
    }
    
    void SaveDescriptors(std::map<std::string, double> &descriptors) {
        for (std::vector<std::string>::iterator it = relevantDescriptorNames.begin(); it != relevantDescriptorNames.end(); it++ ) {
            descriptorValues.push_back(descriptors[*it]);
        }
    }
    
    void normalizeDescriptors(std::vector<std::pair<double, double> > &norm_coefs) {
        assert(descriptorValues.size() == norm_coefs.size());
        std::vector<double>::iterator it;
        unsigned int idx = 0;
        for (it = descriptorValues.begin(); it != descriptorValues.end(); it++, idx++) {
            double A = norm_coefs[idx].first;
            double B = norm_coefs[idx].second;
            if ( ((boost::math::isnan)(A)) || ((boost::math::isnan)(B)) ) {
                continue;
            }
            *it = A * (*it) + B;
        }
    }
    
    void ComputeEtalonDistances(std::vector<double> &etalon, double NA_penalty = 1000) {
        assert(descriptorValues.size() == etalon.size());
        double sum_dist_squared = 0;
        std::vector<double>::iterator it;
        unsigned int idx = 0;
        for (it = etalon.begin(); it != etalon.end(); it++, idx++) {
            double morph_value = descriptorValues[idx];
            if (((boost::math::isnan)(morph_value))) {
                etalonDistances.push_back(NA_penalty);
                sum_dist_squared += std::pow(NA_penalty, 2);
            } else {
                double etalon_value = *it;
                double squared_distance = 0;
                if (!((boost::math::isnan)(etalon_value))) {
                    squared_distance = std::pow(etalon_value - morph_value, 2);
                    assert(!((boost::math::isnan)(squared_distance)));
                    etalonDistances.push_back(std::sqrt(squared_distance));
                } else {
                    assert(false);
                }
                sum_dist_squared += squared_distance;
            }

        }
        distToEtalon = std::sqrt(sum_dist_squared);
        assert(!((boost::math::isnan)(distToEtalon)));
        assert(descriptorValues.size() == etalonDistances.size());
    }

    std::string smile;
    std::string formula;
    boost::int32_t parentChemOper;
    std::string parentSmile;
    std::set<std::string> descendants;
    std::set<std::string> historicDescendants;

    std::string scaffoldSmile;
    boost::int32_t scaffoldLevelCreation;

    double distToTarget;
    double distToEtalon;
    std::vector<double> etalonDistances;
    std::vector<std::string> relevantDescriptorNames;
    std::vector<double> descriptorValues;
    std::string descriptorsFilePath;
    std::string id;

    /**
     * Hold distance to the nextDecoy. Value -1 say that
     * there is decoy that we should visit.
     */
    double distToClosestDecoy;
    double molecularWeight;

    /**
     * Keep sascore for filter, computed in generate morphs,
     * constructors updated. The value is not serialised.
     */
    double sascore;

    boost::uint32_t itersWithoutDistImprovement;

    double posX;
    double posY;

    /**
     * Store index of next decoy that should be visited by
     * this molecule. If greater then number of decoys
     * then molecule can go straight for the target.
     *
     * The value is not serialised.
     */
    // int nextDecoy;
};

// turn off versioning
BOOST_CLASS_IMPLEMENTATION(MolpherMolecule, object_serializable)
// turn off tracking
BOOST_CLASS_TRACKING(MolpherMolecule, track_never)
