#include <fstream>
#include <sstream>

#include "csv.hpp"

std::vector<std::vector<std::string>> csv_load(const std::string& path) {
    std::vector<std::vector<std::string>> result;

    std::ifstream stream;
    stream.open(path.c_str());
    if (!stream.is_open()) {
        throw std::runtime_error("Can't open given file: " + path);
    }

    std::string line;
    while (std::getline(stream, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        result.push_back(row);
    }
    stream.close();

    return result;
}