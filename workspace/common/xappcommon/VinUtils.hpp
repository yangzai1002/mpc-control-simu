//
// Created by yixingwang on 2023/11/22.
//

#ifndef COMMON_UTILS_VIN_UTILS_HPP
#define COMMON_UTILS_VIN_UTILS_HPP

#include <unistd.h>

#include <fstream>
#include <list>
#include <string>

#include "custom_stack.h"

namespace app_common {
    class VinUtils {
    public:
        static VinUtils& Instance() {
            static VinUtils inst;
            return inst;
        }

        VinUtils(const VinUtils&) = delete;

        static std::string GetVin() {
            return Instance()._vin;
        }

    private:
        VinUtils() {
            GetVin(_vin);
        }

        static std::int32_t GetVin(std::string& vin) {
            std::list<std::string> paths;
            auto                   vin_path = CustomStack::Instance()->GetVehicleVinPath();
            if (!vin_path.empty()) {
                paths.push_back(vin_path);
            }
            if (paths.empty()) {
                paths.push_back("/avos/vin");
                paths.push_back("/opt/vin");
                paths.push_back("/opt/usr/vin");
            }

            for (auto& path : paths) {
                if (access(path.c_str(), R_OK) == 0) {
                    vin_path = path;
                    break;
                }
            }
            if (vin_path.empty()) {
                vin = "M";
                return -1;
            }

            std::ifstream in(vin_path);
            if (!in.is_open()) {
                vin = "M";
                return -1;
            }
            std::getline(in, vin);
            vin += "M";
            return 0;
        }

    private:
        std::string _vin;
    }; // namespace app_common
};     // namespace app_common

#endif
