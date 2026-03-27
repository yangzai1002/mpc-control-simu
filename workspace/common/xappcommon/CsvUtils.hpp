#ifndef CSV_UTILS_HPP
#define CSV_UTILS_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "StringUtils.hpp"
#include "log/log.h"

namespace app_common {

    class CSVUtils {
    public:
        CSVUtils() = delete;

        static std::vector<std::vector<std::string>> read(const std::string &filename, char delimiter = ',',
                                                          std::int32_t skip = 0) {
            std::vector<std::vector<std::string>> data;
            std::string                           line;

            std::ifstream file(filename);
            file.exceptions(std::ifstream::badbit);
            while (std::getline(file, line)) {
                if (skip > 0) {
                    --skip;
                    continue;
                }
                StringUtils::trim(line);
                if (line.empty()) {
                    continue;
                }
                std::vector<std::string> row;
                std::stringstream        ss(line);
                std::string              value;
                while (std::getline(ss, value, delimiter)) {
                    StringUtils::trim(value);
                    row.push_back(std::move(value));
                }

                data.push_back(std::move(row));
            }

            return data;
        }
    };
} // namespace app_common

#endif
