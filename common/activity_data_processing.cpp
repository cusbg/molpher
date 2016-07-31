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
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>

namespace ba = boost::accumulators;

#include <GraphMol/FileParsers/MolSupplier.h>
#include <GraphMol/ROMol.h>

void adp::normalizeData(
    std::vector<std::vector<double> > &data,
    double min_value,
    double max_value,
    std::vector<std::pair<double, double> > &normalization_ceofficients,
    std::vector<double> &imputed_values
) {
    // Normalizes each data column in data to the interval (min_value, max_value).
    std::vector<std::vector<double> >::size_type row_count = data.size();
    std::vector<double>::size_type column_count = data[0].size();
    ba::extractor< ba::tag::min > min_;
    ba::extractor< ba::tag::max > max_;
    ba::extractor< ba::tag::median > median_;
    for (int desc_idx = 0; desc_idx < column_count; desc_idx++) {
        ba::accumulator_set< double, ba::features<ba::tag::min, ba::tag::max, ba::tag::median> > acc;
        std::vector<int> nan_indices;
        
        // save nan indices for imputation and prepare accumulator
        for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
            double value = data[mol_idx][desc_idx];
            if ( !((boost::math::isnan)(value)) ) { 
                acc(value);
            } else {
                nan_indices.push_back(mol_idx);
            }
        }
        
        // impute missing values
        double median = median_( acc );
        imputed_values.push_back(median);
        for (std::vector<int>::iterator it = nan_indices.begin(); it != nan_indices.end(); it++) {
            data[*it][desc_idx] = median;
        }
        
        // normalize data
        double minimum = min_( acc );
        double maximum = max_( acc );
        if (minimum != maximum) {
//            double A = (max_value - min_value) / (maximum - minimum);
//            double B = min_value - A * minimum;
//            for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
//                data[mol_idx][desc_idx] = A * data[mol_idx][desc_idx] + B;
//            }
//            normalization_ceofficients.push_back(std::pair<double, double>(A, B));
//            //std::cout << A * median + B << std::endl;
//            etalon.push_back(A * median + B);
        } else {
            maximum = maximum + 1E-6;
            minimum = minimum - 1E-6;
//            normalization_ceofficients.push_back(std::pair<double, double>(NAN, NAN));
//            etalon.push_back(NAN);
        }
        double A = (max_value - min_value) / (maximum - minimum);
        double B = min_value - A * minimum;
        for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
            data[mol_idx][desc_idx] = A * data[mol_idx][desc_idx] + B;
        }
        normalization_ceofficients.push_back(std::pair<double, double>(A, B));
    }
}

void adp::normalizeData(
        std::vector<std::vector<double> > &data
        , std::vector<double> &imputed_values
        , std::vector<std::pair<double, double> > &normalization_ceofficients
        ) {
    std::vector<std::vector<double> >::size_type row_count = data.size();
    std::vector<double>::size_type column_count = data[0].size();
    for (int desc_idx = 0; desc_idx < column_count; desc_idx++) {
        double A = normalization_ceofficients[desc_idx].first;
        double B = normalization_ceofficients[desc_idx].second;
        for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
            double val = data[mol_idx][desc_idx];
            if (((boost::math::isnan)(val))) {
                val = imputed_values[desc_idx];
            }
            data[mol_idx][desc_idx] = A * val + B;
        }
    }
}

void adp::computeEtalon(
    std::vector<std::vector<double> > &data,
    std::vector<double> &etalon,
    std::string type
) {
    std::vector<std::vector<double> >::size_type row_count = data.size();
    std::vector<double>::size_type column_count = data[0].size();
    ba::extractor< ba::tag::mean > mean_;
    ba::extractor< ba::tag::mean > median_;
    for (int desc_idx = 0; desc_idx < column_count; desc_idx++) {
        ba::accumulator_set< double, ba::features< ba::tag::mean, ba::tag::median > > acc;
        for (int mol_idx = 0; mol_idx < row_count; mol_idx++) {
            double value = data[mol_idx][desc_idx];
            if ( !((boost::math::isnan)(value)) ) { 
                acc(value);
            }
        }
        if (type.compare("mean") == 0) {
            etalon.push_back(mean_( acc ));
        } else if (type.compare("median") == 0) {
            etalon.push_back(median_( acc ));
        } else {
            assert(false);
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

void adp::readRelevantData(
    CSVparse::CSV& input_data_CSV
    , const std::vector<std::string>& relevant_descs_names
    , std::vector<std::vector<double> >& output_data
    , CSVparse::CSV& output_data_CSV
) {
    bool written = false;
    for (unsigned int idx = 0; idx != input_data_CSV.getRowCount(); idx++) {
        std::vector<double> mol_descs_values;
        for (std::vector<std::string>::const_iterator it = relevant_descs_names.begin(); it != relevant_descs_names.end(); it++) {
            if (!written) {
                output_data_CSV.addFloatData(*it, input_data_CSV.getFloatData(*it));
            }
            double val = input_data_CSV.getFloatData(*it)[idx];
            mol_descs_values.push_back(val);
        }
        written = true;
        output_data.push_back(mol_descs_values);
    }
}

void adp::readRelevantData(
    CSVparse::CSV& input_data_CSV
    , const std::vector<std::string>& relevant_descs_names
    , std::vector<std::vector<double> >& output_data
) {
    for (unsigned int idx = 0; idx != input_data_CSV.getRowCount(); idx++) {
        std::vector<double> mol_descs_values;
        for (std::vector<std::string>::const_iterator it = relevant_descs_names.begin(); it != relevant_descs_names.end(); it++) {
            double val = input_data_CSV.getFloatData(*it)[idx];
            mol_descs_values.push_back(val);
        }
        output_data.push_back(mol_descs_values);
    }
}