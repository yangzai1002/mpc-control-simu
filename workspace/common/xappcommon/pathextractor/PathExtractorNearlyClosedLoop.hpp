#ifndef _PATH_EXECUTOR_NEARLY_CLOSED_LOOP
#define _PATH_EXECUTOR_NEARLY_CLOSED_LOOP

#include "PathExtractor.hpp"
#include "log/log.h"

namespace app_common {
    class PathExtractorNearlyClosedLoop : public PathExtractor {
    public:
        explicit PathExtractorNearlyClosedLoop(std::vector<std::vector<ara::adsfi::IdpBusinessPoint>>&& path)
            : PathExtractor(std::move(path)) {
            if (_current_segment_index < _planned_path_segments.size()) {
                _merged_path = _planned_path_segments[_current_segment_index];
            }
        }

        virtual ~PathExtractorNearlyClosedLoop() = default;

        void done() override {
            _planned_path_segments.clear();
            _current_segment_index = 0;
        }

        ara::adsfi::IdpBusinessPath next(double cur_lat, double cur_lon, double xg, double yg, double yaw, double cur_speed_mps,
                          double expected_speed_mps) override {
            auto cur_gccs = app_common::GeoUtils::get_gccs(cur_lat, cur_lon);
            xg = cur_gccs.xg;
            yg = cur_gccs.yg;
            AINFO << "current location, gps: " << app_common::GeoUtils::str_gps(cur_lat, cur_lon)
                  << ", gccs: " << app_common::GeoUtils::str_gccs(xg, yg);

            auto& current_path = _merged_path;
            auto  r = app_common::GeoUtils::nearest_index(current_path, xg, yg, cur_speed_mps, _look_ahead_time);
            AINFO << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << r << "/"
                  << GeoUtils::str_gccs(current_path[r]);
            if (!_started) {
                _started = true;
                auto distance = app_common::GeoUtils::gccs_distance(current_path[r].xg, current_path[r].yg, xg, yg);
                AINFO << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << r
                      << "/" << GeoUtils::str_gccs(current_path[r]) << ", distance: " << distance;
                if (distance > _max_distance_to_nearest_point) {
                    throw ExceptionTooFarToNearestPoint(distance);
                }
            }
            current_path.erase(current_path.begin(), current_path.begin() + r);
            AINFO << "planned points numer: " << current_path.size();

            auto point_counts = 1000;
            if (expected_speed_mps > (60.0 / 3.6)) {
                point_counts = 2000;
            } else if (expected_speed_mps > (30.0 / 3.6)) {
                point_counts = 1500;
            } else {
            }
            auto  i = 0;
            auto& last = current_path.front();
            auto  track_path = ara::adsfi::IdpBusinessPath{};
            for (auto iter = current_path.begin(); iter != current_path.end() && i < point_counts; ++iter, i++) {
                auto diff_x = iter->xg - xg;
                auto diff_y = iter->yg - yg;
                auto cos_yaw = std::cos(yaw * M_PI / 180);
                auto sin_yaw = std::sin(yaw * M_PI / 180);
                iter->x = diff_x * cos_yaw + diff_y * sin_yaw;
                iter->y = -diff_x * sin_yaw + diff_y * cos_yaw;

                track_path.points.push_back(*iter);
                track_path.length += app_common::GeoUtils::gccs_distance(last, *iter);
            }

            check_and_merge_next(point_counts / 3);

            AINFO << "extracted points num from planned points: " << track_path.points.size();
            return std::move(track_path);
        }

    private:
        void check_and_merge_next(std::int32_t expected_num_points) {
            // 0.1 的分辨率，如果剩下的点小于 15 个点就合并下一个路径段
            if (_merged_path.size() > expected_num_points) {
                return;
            }

            ++_current_segment_index;
            if (_current_segment_index >= _planned_path_segments.size()) {
                _current_segment_index = 0;
            }

            auto index = _merged_path.back().index;
            std::for_each(_planned_path_segments[_current_segment_index].begin(),
                          _planned_path_segments[_current_segment_index].end(), [this, &index](auto& p) {
                              p.index = ++index;
                              _merged_path.push_back(p);
                          });
        }

    private:
        bool                       _started = false;
        std::vector<ara::adsfi::IdpBusinessPoint> _merged_path;
    };
} // namespace app_common
#endif
