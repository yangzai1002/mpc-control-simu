#ifndef _PATH_EXECUTOR_SIMPLE_HPP
#define _PATH_EXECUTOR_SIMPLE_HPP

#include "PathExtractor.hpp"
#include "log/log.h"

namespace app_common {
    class PathExtractorSimple : public PathExtractor {
    public:
        explicit PathExtractorSimple(std::vector<std::vector<ara::adsfi::IdpBusinessPoint>>&& path)
            : PathExtractor(std::move(path)) {
        }

        virtual ~PathExtractorSimple() = default;

        void done() override {
            ++_current_segment_index;
            if (_current_segment_index >= _planned_path_segments.size()) {
                _planned_path_segments.clear();
                _current_segment_index = 0;
            }
        }

        ara::adsfi::IdpBusinessPath next(double cur_lat, double cur_lon, double xg, double yg, double yaw,
                                         double cur_speed_mps, double expected_speed_mps) override {
            if (_current_segment_index >= _planned_path_segments.size() || _planned_path_segments.empty()) {
                return ara::adsfi::IdpBusinessPath{};
            }

            auto cur_gccs = app_common::GeoUtils::get_gccs(cur_lat, cur_lon);
            xg = cur_gccs.xg;
            yg = cur_gccs.yg;
            AINFO << "current location, gps: " << app_common::GeoUtils::str_gps(cur_lat, cur_lon)
                  << ", gccs: " << app_common::GeoUtils::str_gccs(xg, yg);

            auto& current_path = _planned_path_segments[_current_segment_index];
            auto  r = app_common::GeoUtils::nearest_index(current_path, xg, yg, cur_speed_mps, _look_ahead_time);
            AINFO << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << r << "/"
                  << GeoUtils::str_gccs(current_path[r]);
            current_path.erase(current_path.begin(), current_path.begin() + r);
            AINFO << "planned points numer: " << current_path.size();

            if (!_started) {
                // 启动时，检查启动时路径点是否足够
                if (current_path.size() < _min_way_points) {
                    AERROR << "when start, way points too few to dest, current_path.size(): " << current_path.size()
                           << ", _min_way_points: " << _min_way_points;
                    throw ExceptionStartTooFewPoints(current_path.size());
                }

                // 启动时，检查自车位置距离最近点是否太远
                auto distance = app_common::GeoUtils::gccs_distance(current_path[0].xg, current_path[0].yg, xg, yg);
                AINFO << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << r
                      << "/" << GeoUtils::str_gccs(current_path[0]) << ", distance: " << distance;
                if (distance > _max_distance_to_nearest_point) {
                    AINFO << "when start, too far to nearest point";
                    throw ExceptionTooFarToNearestPoint(distance);
                }

                // 启动时，检查自车航向与循迹方向航向，对于循迹来说，不能调头
                auto begin_part = points_extraction(true, current_path, 0, _heading_fit_points);
                auto begin_part_heading = estimate_north_heading(begin_part);
                if (!begin_part_heading.has_value()) {
                    AERROR << "heading estimation failed";
                    throw ExceptionPathHeadingInconsistent();
                }
                AINFO << "begin part heading: " << begin_part_heading.value();
                double self_north_heading = 90 - yaw;
                if (self_north_heading < 0) {
                    self_north_heading += 360;
                }
                AINFO << "self heading: " << self_north_heading;
                auto diff1 = heading_diff_deg(begin_part_heading.value(), self_north_heading);
                AINFO << "heading diff1: " << diff1;
                if (diff1 > _max_heading_diff) {
                    AERROR << "heading diff too large";
                    throw ExceptionPathHeadingInconsistent();
                }
                _started = true;
            }

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
            AINFO << "extracted points num from planned points: " << track_path.points.size();
            return std::move(track_path);
        }

    private:
        bool _started = false;
    };
} // namespace app_common
#endif
