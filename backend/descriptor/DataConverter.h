/*
 * File:   DataConverter.h
 * Author: martin
 *
 * Created on 17. prosinec 2014, 17:07
 */

#ifndef DATACONVERTER_H
#define	DATACONVERTER_H

#include <string>
#include <sstream>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace CSVparse {

    struct DataConverter {
        static int ValToInt(const std::string &value);
        static double ValToDouble(const std::string &value);
        static bool IsNumeric(const std::string &value, const char decimal_char = '.', const char stringSep = '"');

        template<typename Number>
        static std::string ValToString(Number num) {
            std::stringstream s;
            s << num;
            return s.str();
        }

        // trim from start
        static std::string &ltrim(std::string &s);

        // trim from end
        static std::string &rtrim(std::string &s);

        // trim from both ends
        static std::string &trim(std::string &s);
        static std::string &trim(std::string &s, char removed_char);
    };
}



#endif	/* DATACONVERTER_H */

