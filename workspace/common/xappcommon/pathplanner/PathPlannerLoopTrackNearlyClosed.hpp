#pragma once

#include "GpxFileHandler.hpp"
#include "PathPlanner.hpp"
#include "custom_stack.h"
#include "log/log.h"

namespace app_common {
    class PathPlannerLoopTrackNearlyClosed : public PathPlanner {
    public:
        PathPlannerLoopTrackNearlyClosed(const std::string &file_track, const std::string &dest,
                                         const GpxFileHandler &gpx_file_handler)
            : PathPlanner(), _dest(dest) {
            try {
                AINFO << "waypoints " << file_track << " meta data loading ...";
                auto start_end = gpx_file_handler.gpx_file_start_end(file_track);
                _file_track_start = start_end.first;
                _file_track_end = start_end.second;
                AINFO << "waypoints " << file_track << " meta data loaded: (" << start_end.first << ", "
                      << start_end.second << ")";
            } catch (const std::exception &e) {
                AERROR << "waypoints " << file_track << " meta data failed loading: " << e.what();
                throw ExceptionLoadTrackFile(file_track);
            }
            if (_dest != _file_track_start && _dest != _file_track_end) {
                AERROR << "waypoints " << file_track << " meta data(" << _file_track_start << ", " << _file_track_end
                       << ") failed matching: " << _dest;
                throw ExceptionInvalidDestination(file_track, _dest);
            }

            AINFO << "waypoints " << file_track << " loading ...";
            auto tmp = gpx_file_handler.gpx_file_to_waypoints(file_track);
            AINFO << "waypoints " << file_track << " loaded: " << tmp.first.size();
            if (!tmp.second) {
                AINFO << "waypoints " << file_track << " interpolating ...";
                app_common::GeoUtils::WaypointsInterpolation(tmp.first, _readout_waypoints);
                AINFO << "waypoints " << file_track << " interpolated: " << _readout_waypoints.size();
                AINFO << "waypoints " << file_track << "smoothing ...";
                smooth_waypoints(_readout_waypoints);
                AINFO << "waypoints " << file_track << "smoothed: " << _readout_waypoints.size();
            } else {
                _readout_waypoints = std::move(tmp.first);
            }

            if (_readout_waypoints.size() < _min_way_points) {
                AERROR << "waypoints " << file_track << " less than " << _min_way_points << ": "
                       << _readout_waypoints.size();
                throw ExceptionTrackPointsTooFew(file_track, _readout_waypoints.size());
            }

            auto &b = _readout_waypoints.front();
            auto &e = _readout_waypoints.back();
            auto  dist = app_common::GeoUtils::gps_distance(b.lat, b.lon, e.lat, e.lon);
            auto  dist_threshold = 3.0;
            try {
                dist_threshold = CustomStack::Instance()->GetProjectConfigValue<double>(
                    "planner/closedloop/constraints/max_distance_between_start_end");
            } catch (const std::exception &exp) {
            }
            if (dist > dist_threshold) {
                AERROR << "load track error, track: " << file_track << ", distance: " << dist
                       << ", threshold: " << dist_threshold;
                throw ExceptionTooFarawayFromBeginToEnd(file_track, dist);
            }
        }

        virtual ~PathPlannerLoopTrackNearlyClosed() = default;

        std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> paths() override {
            std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> r;

            if (_dest != _file_track_end) {
                std::reverse(_readout_waypoints.begin(), _readout_waypoints.end());
            }

            // 补一个点，以免在转换成 trackpoints
            // 时最后一个点丢失（否则，因为最后一个点缺少计算 heading
            // 的参照点，就不得不丢弃最后一个点）
            // app_common::GeoUtils::AddVirtualWaypoint(_readout_waypoints);
            r.emplace_back(app_common::GeoUtils::Waypoints2Trackpoints(_readout_waypoints, 0));

            auto &                b = _readout_waypoints.front();
            auto &                e = _readout_waypoints.back();
            std::vector<Waypoint> tmp_waypoints;
            tmp_waypoints.push_back(e);
            tmp_waypoints.push_back(b);

            std::vector<Waypoint> tmp_waypoints2;
            app_common::GeoUtils::WaypointsInterpolation(tmp_waypoints, tmp_waypoints2);
            // smooth_waypoints(tmp_waypoints2);
            r.emplace_back(app_common::GeoUtils::Waypoints2Trackpoints(tmp_waypoints2, 0));

            return r;
        }

    private:
        const std::string _dest;

        std::string _file_track_start;
        std::string _file_track_end;
    }; // namespace app_common
} // namespace app_common
