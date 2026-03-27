#ifndef _PATH_EXECUTOR_CLOSED_LOOP
#define _PATH_EXECUTOR_CLOSED_LOOP

#include "PathExtractor.hpp"
#include "geometry_tools.hpp"
#include "log/log.h"

namespace app_common {
    class PathExtractorClosedLoop : public PathExtractor {
    public:
        explicit PathExtractorClosedLoop(std::vector<std::vector<ara::adsfi::IdpBusinessPoint>>&& path)
            : PathExtractor(std::move(path)) {
            // 基本保护: [0]-->[begin, end], [1]-->[end, begin]
            if (_planned_path_segments.size() != 2 || _planned_path_segments[0].empty() ||
                _planned_path_segments[1].empty()) {
                throw std::runtime_error("error path segments");
            }

            // 对于绕圈循迹来说，起始位置处的 heading 应该是差不多的，否则就会出现折返路线
            auto begin_part = points_extraction(true, _planned_path_segments[0], 0, _heading_fit_points);
            auto end_part = points_extraction(false, _planned_path_segments[0], _planned_path_segments.size() - 1,
                                              _heading_fit_points);
            auto end_to_begin = _planned_path_segments[1];
            auto begin_part_heading = estimate_north_heading(begin_part);
            auto end_part_heading = estimate_north_heading(end_part);
            auto end_to_begin_heading = estimate_north_heading(end_to_begin);
            if (begin_part_heading.has_value() && end_part_heading.has_value() && end_to_begin_heading.has_value()) {
                AINFO << "begin part heading: " << begin_part_heading.value();
                AINFO << "end part heading: " << end_part_heading.value();
                AINFO << "end to begin heading: " << end_to_begin_heading.value();
                auto diff1 = heading_diff_deg(begin_part_heading.value(), end_to_begin_heading.value());
                auto diff2 = heading_diff_deg(end_part_heading.value(), end_to_begin_heading.value());
                AINFO << "heading diff1: " << diff1;
                AINFO << "heading diff2: " << diff2;
                if (diff1 > _max_heading_diff || diff2 > _max_heading_diff) {
                    AERROR << "heading diff too large";
                    throw ExceptionPathHeadingInconsistent();
                }
            } else {
                AERROR << "heading estimation failed";
                throw ExceptionPathHeadingInconsistent();
            }

            _merged_path = _planned_path_segments[0];
            _merged_path.insert(_merged_path.end(), _planned_path_segments[1].begin() + 1,
                                _planned_path_segments[1].end());
        }

        virtual ~PathExtractorClosedLoop() = default;

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

            auto point_counts = 1000;
            if (expected_speed_mps > (60.0 / 3.6)) {
                point_counts = 2000;
            } else if (expected_speed_mps > (30.0 / 3.6)) {
                point_counts = 1500;
            }

            if (!_started) {
                try {
                    auto result = init_vehicle_index_and_direction(_merged_path, xg, yg, yaw, _heading_fit_points,
                                                                   _max_heading_diff);
                    _rearranged_path = rearrange_path_points(_merged_path, result.first, result.second);
                    _merged_path = _rearranged_path;
                } catch (const std::exception& e) {
                    AERROR << "init heading failed: " << e.what();
                    throw ExceptionHeadingDecision(-1);
                }
                AINFO << "rearranged path points: " << _rearranged_path.size();
                auto distance = std::hypot(_merged_path[0].xg - xg, _merged_path[0].yg - yg);
                AINFO << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is "
                      << GeoUtils::str_gccs(_merged_path[0]) << ", distance: " << distance;
                if (distance > _max_distance_to_nearest_point) {
                    throw ExceptionTooFarToNearestPoint(distance);
                }
                _started = true;
            } else {
                erase_passed_points(xg, yg, cur_speed_mps, point_counts);
                _last_speed_mps = cur_speed_mps;
            }

            auto track_path = next_path(xg, yg, yaw, point_counts);
            return std::move(track_path);
        }

    private:
        static std::pair<bool, std::size_t> init_vehicle_index_and_direction(const std::vector<ara::adsfi::IdpBusinessPoint>& points,
                                                                             double xg, double yg, double self_heading,
                                                                             std::size_t heading_fit_points,
                                                                             double      max_heading_diff) {
            auto start_point = app_common::GeoUtils::nearest_index(points, xg, yg);
            AINFO << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << start_point
                  << "/" << GeoUtils::str_gccs(points[start_point]);

            auto forward_points = points_extraction(true, points, start_point, heading_fit_points);
            auto backward_points = points_extraction(false, points, start_point, heading_fit_points);
            auto forward_heading = estimate_north_heading(forward_points);
            auto backward_heading = estimate_north_heading(backward_points);

            double self_north_heading = 90 - self_heading;
            if (self_north_heading < 0) {
                self_north_heading += 360;
            }

            if (forward_heading.has_value()) {
                AINFO << "forward heading: " << forward_heading.value();
                double heading = forward_heading.value();
                double heading_diff = heading_diff_deg(heading, self_north_heading);
                AINFO << "forward heading: " << heading << ", self heading: " << self_north_heading
                      << ", diff: " << heading_diff;
                if (heading_diff < max_heading_diff) {
                    return {true, start_point};
                }
            }

            if (backward_heading.has_value()) {
                AINFO << "backward heading: " << backward_heading.value();
                double heading = backward_heading.value(); // 转换为正东为基准的航向角
                double heading_diff = heading_diff_deg(heading, self_north_heading);
                AINFO << "backward heading: " << heading << ", self heading: " << self_north_heading
                      << ", diff: " << heading_diff;
                if (heading_diff < max_heading_diff) {
                    return {false, start_point};
                }
            }

            throw std::runtime_error("cannot estimate heading");
        }

        // 重新排列路径点，使得车辆当前位置为第一个点，方便后续判断是否发生了翻转
        static std::vector<ara::adsfi::IdpBusinessPoint> rearrange_path_points(const std::vector<ara::adsfi::IdpBusinessPoint>& points, bool forward,
                                                                std::size_t start_point) {
            if (points.empty()) {
                AERROR << "path is empty";
                throw std::runtime_error("path is empty");
            }
            if (start_point >= points.size()) {
                AERROR << "last index out of range";
                throw std::runtime_error("last index out of range");
            }
            if (points.size() > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
                AERROR << "too many points in path";
                throw std::runtime_error("too many points in path");
            }

            std::vector<ara::adsfi::IdpBusinessPoint> rearranged_points;
            rearranged_points.reserve(points.size());

            if (forward) {
                for (size_t i = start_point; i < points.size(); ++i) {
                    rearranged_points.push_back(points[i]);
                }
                for (size_t i = 0; i < start_point; ++i) {
                    rearranged_points.push_back(points[i]);
                }
            } else {
                for (std::int32_t i = static_cast<std::int32_t>(start_point); i >= 0; --i) {
                    rearranged_points.push_back(points[i]);
                }
                for (std::int32_t i = static_cast<std::int32_t>(points.size()) - 1; i > start_point; --i) {
                    rearranged_points.push_back(points[i]);
                }
            }

            std::int32_t i = 0;
            std::for_each(rearranged_points.begin(), rearranged_points.end(),
                          [&i](ara::adsfi::IdpBusinessPoint& p) { p.index = i++; });

            return rearranged_points;
        }

        void erase_passed_points(double v_xg, double v_yg, double v_speed_mps, std::size_t expected_min_points) {
            if (_merged_path.empty()) {
                AERROR << "path is empty"; // 不会发生，但还是做个保护
                return;
            }
            const double smoothed_speed = (v_speed_mps + _last_speed_mps) / 2.0;
            const double max_search_length = (smoothed_speed > 0)
                                                 ? std::max(_min_look_ahead_distance, smoothed_speed * _look_ahead_time)
                                                 : _min_look_ahead_distance;
            AINFO << "last speed: " << _last_speed_mps << ", current speed: " << v_speed_mps
                  << ", smoothed speed: " << smoothed_speed << ", max search length: " << max_search_length;

            auto min_dist_iter = _merged_path.begin();
            auto min_dist = std::hypot(min_dist_iter->xg - v_xg, min_dist_iter->yg - v_yg);
            auto accumulated_length = 0.0;
            for (auto iter = _merged_path.begin(), prev_iter = iter;
                 iter != _merged_path.end() && accumulated_length < max_search_length; ++iter) {
                auto d = std::hypot(iter->xg - v_xg, iter->yg - v_yg);
                accumulated_length += std::hypot(iter->xg - prev_iter->xg, iter->yg - prev_iter->yg);
                prev_iter = iter;
                if (d < min_dist) {
                    min_dist = d;
                    min_dist_iter = iter;
                }
            }

            auto back_index = _merged_path.back().index;
            AINFO << "neareast to " << app_common::GeoUtils::str_gccs(v_xg, v_yg) << " in planned points is "
                  << GeoUtils::str_gccs(*min_dist_iter) << ", distance: " << min_dist;
            _merged_path.erase(_merged_path.begin(), min_dist_iter);

            if (_merged_path.size() < expected_min_points) {
                std::for_each(_rearranged_path.begin(), _rearranged_path.end(), [&](const auto& p) {
                    auto iter = _merged_path.insert(_merged_path.end(), p);
                    iter->index = ++back_index;
                });
            }
        }

        ara::adsfi::IdpBusinessPath next_path(double v_xg, double v_yg, double yaw, std::size_t expected_point_counts) {
            ara::adsfi::IdpBusinessPath track_path{};
            if (_merged_path.empty()) {
                AERROR << "path is empty"; // 不会发生，但还是做个保护
                return track_path;
            }
            AINFO << "extracting points from 0/" << GeoUtils::str_gccs(_merged_path[0]);

            std::size_t count = 0;
            auto        old_idx = _merged_path.back().index + 1;
            for (auto iter = _merged_path.begin(), prev_iter = _merged_path.begin();
                 iter != _merged_path.end() && count < expected_point_counts; ++iter, ++count) {
                auto diff_x = iter->xg - v_xg;
                auto diff_y = iter->yg - v_yg;
                auto cos_yaw = std::cos(yaw * M_PI / 180);
                auto sin_yaw = std::sin(yaw * M_PI / 180);
                iter->x = diff_x * cos_yaw + diff_y * sin_yaw;
                iter->y = -diff_x * sin_yaw + diff_y * cos_yaw;

                track_path.points.push_back(*iter);
                track_path.length += app_common::GeoUtils::gccs_distance(*prev_iter, *iter);
                prev_iter = iter;
            }
            AINFO << "extracted points num from planned points: " << track_path.points.size();

            return track_path;
        }

    private:
        std::vector<ara::adsfi::IdpBusinessPoint> _merged_path;
        std::vector<ara::adsfi::IdpBusinessPoint> _rearranged_path;

        bool   _started = false;
        double _last_speed_mps = 0.0;
    };
} // namespace app_common
#endif
