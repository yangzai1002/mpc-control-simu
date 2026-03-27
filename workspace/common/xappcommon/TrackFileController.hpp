#ifndef TRACK_FILE_CONTROLLER_HPP
#define TRACK_FILE_CONTROLLER_HPP

#include "GpxFileController.hpp"
#include "JsonUtils.hpp"
#include "custom_stack.h"

namespace app_common {
    class TrackFileController : public app_common::GpxFileController {
    public:
        explicit TrackFileController(const std::string &base_dir)
            : app_common::GpxFileController(base_dir, "default_track.txt") {
        }
    };
} // namespace app_common
#endif
