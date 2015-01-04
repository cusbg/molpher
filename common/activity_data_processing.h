/* 
 * File:   activity_data_IO.h
 * Author: Krwemil
 *
 * Created on 25. říjen 2014, 18:13
 */

#ifndef ACTIVITY_DATA_IO_H
#define	ACTIVITY_DATA_IO_H

// activity data manipulation and IO

namespace adp {

    void normalizeActivesData(
            std::vector<std::vector<double> > &data,
            double min_value,
            double max_value,
            std::vector<std::pair<double, double> > &normalization_ceofficients,
            std::vector<double> &etalon
            );
    
    void readPropFromSDF(
            const std::string &SDFpath,
            const std::string &property_name,
            std::vector<std::string> &smiles
            );
}

#endif	/* ACTIVITY_DATA_IO_H */

