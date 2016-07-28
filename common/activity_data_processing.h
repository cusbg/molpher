/* 
 * File:   activity_data_IO.h
 * Author: Krwemil
 *
 * Created on 25. říjen 2014, 18:13
 */

#ifndef ACTIVITY_DATA_IO_H
#define	ACTIVITY_DATA_IO_H

#include "CSV.h"

// activity data manipulation and IO

namespace adp {

    void normalizeData(
            std::vector<std::vector<double> > &data,
            double min_value,
            double max_value,
            std::vector<std::pair<double, double> > &normalization_ceofficients,
            std::vector<double> &imputed_values
            );

    void normalizeData(
            std::vector<std::vector<double> > &data
            , std::vector<double> &imputed_values
            , std::vector<std::pair<double, double> > &normalization_ceofficients
            );

    void computeEtalon(
            std::vector<std::vector<double> > &data,
            std::vector<double> &etalon,
            std::string etalon_type = "mean"
            );

    void readPropFromSDF(
            const std::string &SDFpath,
            const std::string &property_name,
            std::vector<std::string> &smiles
            );

    void readRelevantData(
            CSVparse::CSV& input_data_CSV
            , const std::vector<std::string>& relevant_descs_names
            , std::vector<std::vector<double> >& output_data
            , CSVparse::CSV& output_data_CSV
            );

    void readRelevantData(
            CSVparse::CSV& input_data_CSV
            , const std::vector<std::string>& relevant_descs_names
            , std::vector<std::vector<double> >& output_data
            );
}

#endif	/* ACTIVITY_DATA_IO_H */

