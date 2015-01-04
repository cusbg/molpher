#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "math.h"

#include "activity_data_processing.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/accumulators/statistics/median.hpp>

namespace ba = boost::accumulators;

#include <GraphMol/FileParsers/MolSupplier.h>
#include <GraphMol/ROMol.h>
#include <RDBoost/Exceptions.h>

void adp::normalizeActivesData(
    std::vector<std::vector<double> > &data,
    double min_value,
    double max_value,
    std::vector<std::pair<double, double> > &normalization_ceofficients,
    std::vector<double> &etalon    
) {
    // Normalizes each data column in data to the interval (min_value, max_value).
    // It writes the A and B normalization coefficients to the normalization_ceofficients vector. 
    // Also computes etalon values for each data column.
    // If there is only one value in the column, the original value is kept and NAN 
    // is saved into the etalon and normalization_ceofficients vectors.
    std::vector<std::vector<double> >::size_type row_count = data.size();
    std::vector<double>::size_type column_count = data[0].size();
    ba::extractor< ba::tag::min > min_;
    ba::extractor< ba::tag::max > max_;
    ba::extractor< ba::tag::median > median_;
    for (int desc_idx = 0; desc_idx < column_count; desc_idx++) {
        ba::accumulator_set< double, ba::features< ba::tag::min, ba::tag::max, ba::tag::median > > acc;
        for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
            double value = data[mol_idx][desc_idx];
            if ( !((boost::math::isnan)(value)) ) { 
                acc(value);
            }
        }
        double minimum = min_( acc );
        double maximum = max_( acc );
        double median = median_( acc );
        if (minimum != maximum) {
            double A = (max_value - min_value) / (maximum - minimum);
            double B = min_value - A * minimum;
            for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
                data[mol_idx][desc_idx] = A * data[mol_idx][desc_idx] + B;
            }
            normalization_ceofficients.push_back(std::pair<double, double>(A, B));
            //std::cout << A * median + B << std::endl;
            etalon.push_back(A * median + B);
        } else {
            normalization_ceofficients.push_back(std::pair<double, double>(NAN, NAN));
            etalon.push_back(NAN);
        }
    }
}

void adp::readPropFromSDF(const std::string &SDFpath, const std::string &property_name, std::vector<std::string> &smiles) {
    RDKit::SDMolSupplier suppl(SDFpath);
    RDKit::ROMol* mol = 0;
    
    unsigned int mol_count = suppl.length();
    for (unsigned int idx = 0; idx < mol_count; idx++) {
        mol = suppl[idx];
        std::string smiles_string;
        if (mol->hasProp(property_name)) {
            mol->getProp<std::string>(property_name, smiles_string);
        } else {
            KeyErrorException exp = KeyErrorException(property_name);
            std::cerr << "Property not found: " << exp.key() << std::endl;
            throw exp;
        }
        smiles.push_back(smiles_string);
        std::cout.flush();
    }
}