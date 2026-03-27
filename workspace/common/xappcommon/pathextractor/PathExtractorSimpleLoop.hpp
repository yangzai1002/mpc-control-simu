#ifndef _PATH_EXECUTOR_SIMPLE_LOOP_HPP_
#define _PATH_EXECUTOR_SIMPLE_LOOP_HPP_

#include "PathExtractorSimple.hpp"

namespace app_common {
    class PathExtractorSimpleLoop : public PathExtractorSimple {
    public:
        explicit PathExtractorSimpleLoop(std::vector<std::vector<ara::adsfi::IdpBusinessPoint>>&& path)
            : PathExtractorSimple(std::move(path)), _planned_path_segments_backup(_planned_path_segments) {
        }

        virtual ~PathExtractorSimpleLoop() = default;

        void reset() override {
            _planned_path_segments.clear();
            _current_segment_index = 0;
            _planned_path_segments_backup.clear();
        }

        void done() override {
            ++_current_segment_index;
            if (_current_segment_index >= _planned_path_segments.size()) {
                _planned_path_segments = _planned_path_segments_backup;
                _current_segment_index = 0;
            }
        }

    private:
        decltype(_planned_path_segments) _planned_path_segments_backup;
    };
} // namespace app_common
#endif