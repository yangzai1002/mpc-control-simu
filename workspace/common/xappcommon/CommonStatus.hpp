#pragma once

#include <cstdint>
#include <string>

namespace app_common {
    enum class CommonStatus : std::int32_t { IDLE, RUNNING, PAUSED, ERROR };

    inline std::string common_status_str(CommonStatus s) {
        std::string x;

        if (CommonStatus::IDLE == s) {
            x = "IDLE";
        } else if (CommonStatus::RUNNING == s) {
            x = "RUNNING";
        } else if (CommonStatus::PAUSED == s) {
            x = "PAUSED";
        } else if (CommonStatus::ERROR == s) {
            x = "ERROR";
        }

        return x;
    }
} // namespace app_common