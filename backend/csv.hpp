#pragma once

#include <vector>

/**
 * Load and return convent of the CSV file.
 *
 * Use ',' as cell separator and '\n' as row separator.
 *
 * @param path
 * @return
 */
std::vector<std::vector<std::string>> csv_load(const std::string& path);
