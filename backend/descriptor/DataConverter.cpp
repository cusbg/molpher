
#include "CSV.h"
#include "DataConverter.h"

using namespace std;

int CSVparse::DataConverter::ValToInt(const string &value) {
    stringstream ss;
    ss << value;
    int result;
    ss >> result;
    return result;
}

double CSVparse::DataConverter::ValToDouble(const string &value) {
    stringstream ss;
    ss << value;
    double result;
    ss >> result;
    return result;
}

bool CSVparse::DataConverter::IsNumeric(const string& value, const char decimal_char, const char stringSep) {
    string::const_iterator it;
    bool decimal_mark_found = false;
    bool minus_found = false;
    bool E_found = false;
    bool mark_after_E_found = false;
    if (value.find(stringSep) == 0 && value.rfind(stringSep) == value.size() - 1) {
        return false;
    }
    for (it = value.begin(); it != value.end(); it++) {
        if (std::isdigit(*it)) {
            continue;
        } else if (*it == '-' && it == value.begin()) {
            minus_found = true;
            continue;
        } else if ( (*it == 'E' || *it == 'e') && !E_found) {
            E_found = true;
            continue;
        } else if (*it == decimal_char) {
            if (decimal_mark_found) return false;
            decimal_mark_found = true;
            continue;
        } else if (E_found && (*it == '-' || *it == '+') && !mark_after_E_found) {
            mark_after_E_found = true;
            continue;
        } else {
            return false;
        }
    }
    return !value.empty() && it == value.end();
}

std::string & CSVparse::DataConverter::trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::string & CSVparse::DataConverter::trim(std::string &s, char removed_char) {
        string::size_type start = s.find(removed_char);
        string::size_type end = s.rfind(removed_char);
        if (start != s.npos && end != s.npos) {
            s.erase(start, 1);
            s.erase(end - 1, 1);
        }
        return s;
}

std::string & CSVparse::DataConverter::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string & CSVparse::DataConverter::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}