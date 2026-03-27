#ifndef _PATH_EXTRACTOR_SIMPLE_HPP
#define _PATH_EXTRACTOR_SIMPLE_HPP

#include "../Ec408.hpp"
#include "PathExtractor.hpp"
#include "ap_log/ap_log_interface.h"

namespace app_common {
    class PathExtractorSimple : public PathExtractor {
    public:
        explicit PathExtractorSimple(std::vector<std::vector<ara::adsfi::BusinessPoint>>&& path)
            : PathExtractor(std::move(path)) {
        }

        virtual ~PathExtractorSimple() = default;

        void done() override {
            // 规划层认为当前段执行完成，此处会丢弃当前段剩余路径点。
            // remaining_length_m 口径：将该段剩余长度计入“已完成”，避免进度卡在 <100。
            if (_current_segment_index < _planned_path_segments.size()) {
                _remaining_length_m -=
                    app_common::GeoUtils::gccs_distance(_planned_path_segments[_current_segment_index]);
                if (_remaining_length_m < 0.0) {
                    _remaining_length_m = 0.0;
                }
            }

            ++_current_segment_index;
            if (_current_segment_index >= _planned_path_segments.size()) {
                _planned_path_segments.clear();
                _current_segment_index = 0;
                _remaining_length_m = 0.0;
            }
        }

        ara::adsfi::BusinessPath next(double cur_lat, double cur_lon, double xg, double yg, double yaw,
                                      double cur_speed_mps, double expected_speed_mps) override {
            if (_current_segment_index >= _planned_path_segments.size() || _planned_path_segments.empty()) {
                return ara::adsfi::BusinessPath{};
            }

            auto cur_gccs = app_common::GeoUtils::get_gccs(cur_lat, cur_lon);
            xg = cur_gccs.xg;
            yg = cur_gccs.yg;
            ApInfo("task_executor") << "current location, gps: " << app_common::GeoUtils::str_gps(cur_lat, cur_lon)
                  << ", gccs: " << app_common::GeoUtils::str_gccs(xg, yg);

            auto& current_path = _planned_path_segments[_current_segment_index];
            auto  r = app_common::GeoUtils::nearest_index(current_path, xg, yg, cur_speed_mps, _look_ahead_time);
            ApInfo("task_executor") << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << r << "/"
                  << GeoUtils::str_gccs(current_path[r]);

            // 进度 remaining_length_m 更新：裁剪掉前缀 [0, r) 相当于车辆沿轨迹前进到 r 点。
            // 这里不改变原有提路逻辑，仅在 erase 前扣减被“跳过/走过”的路径长度。
            if (r > 0 && r < current_path.size()) {
                double removed_length = 0.0;
                for (std::size_t i = 1; i <= r && i < current_path.size(); ++i) {
                    removed_length += app_common::GeoUtils::gccs_distance(current_path[i - 1], current_path[i]);
                }
                _remaining_length_m -= removed_length;
                if (_remaining_length_m < 0.0) {
                    _remaining_length_m = 0.0;
                }
            }

            current_path.erase(current_path.begin(), current_path.begin() + r);
            ApInfo("task_executor") << "planned points numer: " << current_path.size();
            if (!_started) {
                // 启动时，检查启动时路径点是否足够
                if (current_path.size() < _min_way_points) {
                    ApError("task_executor") << "when start, way points too few to dest, current_path.size(): " << current_path.size()
                           << ", _min_way_points: " << _min_way_points;
                    throw ExceptionStartTooFewPoints(current_path.size());
                }

                _started = true;
            }

            // 检查自车位置距离最近点是否太远
            auto distance = app_common::GeoUtils::gccs_distance(current_path[0].xg, current_path[0].yg, xg, yg);
            ApInfo("task_executor") << "neareast to " << app_common::GeoUtils::str_gccs(xg, yg) << " in planned points is " << r << "/"
                  << GeoUtils::str_gccs(current_path[0]) << ", distance: " << distance;
            if (distance > _max_distance_to_nearest_point) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_TRACK_VEHICLE_TOO_FAR,
                                                                std::to_string(distance));
            } else {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_TRACK_VEHICLE_TOO_FAR);
            }

            // 检查自车航向与循迹方向航向
            try {
                // extract beginning part points for heading estimation, points number: _heading_fit_points
                auto begin_part = points_extraction(true, current_path, 0, _heading_fit_points);
                auto begin_part_heading = estimate_north_heading(begin_part);
                if (!begin_part_heading.has_value()) {
                    ApError("task_executor") << "heading estimation failed";
                    FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_LOOPTRACK_INCONSISTENCY_HEADING);
                } else {
                    ApInfo("task_executor") << "begin part heading: " << begin_part_heading.value();
                    double self_north_heading = 90 - yaw;
                    if (self_north_heading < 0) {
                        self_north_heading += 360;
                    }
                    ApInfo("task_executor") << "self heading: " << self_north_heading;
                    auto diff1 = heading_diff_deg(begin_part_heading.value(), self_north_heading);
                    ApInfo("task_executor") << "heading diff1: " << diff1;
                    if (diff1 > _max_heading_diff) {
                        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_LOOPTRACK_INCONSISTENCY_HEADING);
                    } else {
                        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_INCONSISTENCY_HEADING);
                    }
                }
            } catch (const std::exception& e) {
                ApError("task_executor") << "heading estimation failed: " << e.what();
            }

            auto exptected_length = expected_local_path_length(expected_speed_mps * 3.6);
            ApInfo("task_executor") << "expected local path length for expected speed " << expected_speed_mps * 3.6 << " km/h is "
                  << exptected_length << " m";
            auto last = current_path.front();
            auto track_path = ara::adsfi::BusinessPath{};
            for (auto iter = current_path.begin(); iter != current_path.end() && track_path.length < exptected_length;
                 ++iter) {
                auto diff_x = iter->xg - xg;
                auto diff_y = iter->yg - yg;
                auto cos_yaw = std::cos(yaw * M_PI / 180);
                auto sin_yaw = std::sin(yaw * M_PI / 180);
                iter->x = diff_x * cos_yaw + diff_y * sin_yaw;
                iter->y = -diff_x * sin_yaw + diff_y * cos_yaw;

                track_path.points.push_back(*iter);
                track_path.length += app_common::GeoUtils::gccs_distance(last, *iter);
                last = *iter;
            }
            ApInfo("task_executor") << "extracted points num from planned points: " << track_path.points.size();
            return std::move(track_path);
        }

    private:
        bool _started = false;
    }; // namespace app_common
} // namespace app_common
#endif
