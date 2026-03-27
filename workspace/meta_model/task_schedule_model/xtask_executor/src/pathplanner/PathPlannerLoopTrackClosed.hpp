#pragma once

#include "../Ec408.hpp"
#include "GpxFileHandler.hpp"
#include "PathPlanner.hpp"
#include "custom_stack.h"
#include "ap_log/ap_log_interface.h"

namespace app_common {
    class PathPlannerLoopTrackClosed : public PathPlanner {
    public:
        PathPlannerLoopTrackClosed(const std::string &file_track, GpxFileHandler &gpx_file_handler)
            : PathPlanner(), _file_track(file_track) {
            std::string full_path = "planner/closedloop/constraints/min_total_distance";
            try {
                _min_total_distance = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << "failed";
                throw ExceptionLoadPlannerConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _min_total_distance;

            full_path = "planner/closedloop/constraints/max_distance_between_start_end";
            try {
                _max_distance_between_start_end = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << "failed";
                throw ExceptionLoadPlannerConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _max_distance_between_start_end;

            load_track_file_as_waypoints(gpx_file_handler);
        }

        virtual ~PathPlannerLoopTrackClosed() = default;

        std::vector<std::vector<ara::adsfi::BusinessPoint>> paths() override {
            return _prepared_businesspoints;
        }

    private:
        void load_track_file_as_waypoints(GpxFileHandler &gpx_file_handler) {
            // 1. 读取轨迹文件，转换为路点
            std::vector<Waypoint> tmp_waypoints;
            try {
                ApInfo("task_executor") << "waypoints " << _file_track << " loading ...";
                auto tmp = gpx_file_handler.gpx_file_to_waypoints(_file_track);
                ApInfo("task_executor") << "waypoints " << _file_track << " loaded, size: " << tmp.first.size();
                tmp_waypoints = std::move(tmp.first);
            } catch (const std::exception &e) {
                ApError("task_executor") << "waypoints " << _file_track << " load error: " << e.what();
                throw ExceptionLoadTrackFile(_file_track);
            }

            // 2. 检查起始点距离终点是否太远
            auto &b = tmp_waypoints.front();
            auto &e = tmp_waypoints.back();
            auto  dist = app_common::GeoUtils::gps_distance(b.lat, b.lon, e.lat, e.lon);
            ApInfo("task_executor") << "waypoints " << _file_track << " distance between first " << GeoUtils::str_gps(b) << " and last "
                  << GeoUtils::str_gps(e) << " is: " << dist;
            if (dist > _max_distance_between_start_end) {
                ApError("task_executor") << "waypoints " << _file_track << " too far away from start and end: " << dist;
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_LOOPTRACK_DISTANCE_FORM_S_TO_E,
                                                                std::to_string(dist));
            } else {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_DISTANCE_FORM_S_TO_E);
            }

            // 3. 插值，并检查路点数是否足够
            ApInfo("task_executor") << "waypoints " << _file_track << " interpolating ...";
            GeoUtils::WaypointsInterpolation(tmp_waypoints, _readout_waypoints);
            ApInfo("task_executor") << "waypoints " << _file_track << " interpolated, size: " << _readout_waypoints.size();
            if (_readout_waypoints.size() < _min_way_points) {
                ApError("task_executor") << "waypoints " << _file_track << " less than " << _min_way_points << ": "
                       << _readout_waypoints.size();
                throw ExceptionTrackPointsTooFew(_file_track, _readout_waypoints.size());
            }
            smooth_waypoints(_readout_waypoints);

            // [begin, end]
            _prepared_businesspoints.emplace_back(app_common::GeoUtils::Waypoints2Trackpoints(_readout_waypoints));

            std::vector<Waypoint> end_to_begin;
            std::vector<Waypoint> end_to_begin_interpolated;
            end_to_begin.push_back(e);
            end_to_begin.push_back(b);
            GeoUtils::WaypointsInterpolation(end_to_begin, end_to_begin_interpolated);

            // 4. 检查轨迹总长度是否足够
            ApInfo("task_executor") << "waypoints " << _file_track << " distance computing ...";
            auto distance_begin_to_end = GeoUtils::gps_distance(_readout_waypoints);
            ApInfo("task_executor") << "waypoints " << _file_track << " distance(begin_to_end): " << distance_begin_to_end;
            auto distance_end_to_begin = GeoUtils::gps_distance(end_to_begin_interpolated);
            ApInfo("task_executor") << "waypoints " << _file_track << " distance(end_to_begin): " << distance_end_to_begin;
            auto distance = distance_begin_to_end + distance_end_to_begin;
            ApInfo("task_executor") << "waypoints " << _file_track << " distance: " << distance;
            if (distance < _min_total_distance) {
                ApError("task_executor") << "waypoints " << _file_track << " too short, less than " << _min_total_distance << ": "
                       << distance;
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_LOOPTRACK_TOO_SHORT,
                                                                std::to_string(distance));
            } else {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_TOO_SHORT);
            }
            smooth_waypoints(end_to_begin_interpolated);

            // [end, begin]
            _prepared_businesspoints.emplace_back(
                app_common::GeoUtils::Waypoints2Trackpoints(end_to_begin_interpolated));
        }

    private:
        double _min_total_distance = 0;               // # 绕圈循迹时，路径最小总长度（单位：米）
        double _max_distance_between_start_end = 3.0; // 绕圈循迹时，起始点位置距离最小距离差（单位：米）

        const std::string                                   _file_track;
        std::vector<std::vector<ara::adsfi::BusinessPoint>> _prepared_businesspoints;
    }; // namespace app_common
} // namespace app_common
