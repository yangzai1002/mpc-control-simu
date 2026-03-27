#pragma once

#include "GpxFileHandler.hpp"
#include "PathPlanner.hpp"
#include "ap_log/ap_log_interface.h"

namespace app_common {
    class PathPlannerGlobalOneway : public PathPlanner {
    public:
        explicit PathPlannerGlobalOneway(const GpxFileHandler& gpx_file_handler) : PathPlanner(), _dest("z") {
            std::string file_track("GLOBALOSM");
            try {
                ApInfo("task_executor") << "waypoints " << file_track << " meta data loading ...";
                auto start_end = gpx_file_handler.gpx_file_start_end(file_track);
                _file_track_start = start_end.first;
                _file_track_end = start_end.second;
                ApInfo("task_executor") << "waypoints " << file_track << " meta data loaded: (" << start_end.first << ", "
                      << start_end.second << ")";
            } catch (const std::exception& e) {
                ApError("task_executor") << "waypoints " << file_track << " meta data failed loading: " << e.what();
                throw ExceptionLoadTrackFile(file_track);
            }
            if (_dest != _file_track_start && _dest != _file_track_end) {
                ApError("task_executor") << "waypoints " << file_track << " meta data(" << _file_track_start << ", " << _file_track_end
                       << ") failed matching: " << _dest;
                throw ExceptionInvalidDestination(file_track, _dest);
            }

            ApInfo("task_executor") << "waypoints " << file_track << " loading ...";
            auto tmp = gpx_file_handler.gpx_file_to_waypoints(file_track);
            ApInfo("task_executor") << "waypoints " << file_track << " loaded: " << tmp.first.size();
            if (!tmp.second) {
                ApInfo("task_executor") << "waypoints " << file_track << " interpolating ...";
                app_common::GeoUtils::WaypointsInterpolation(tmp.first, _readout_waypoints);
                ApInfo("task_executor") << "waypoints " << file_track << " interpolated: " << _readout_waypoints.size();
                ApInfo("task_executor") << "waypoints " << file_track << "smoothing ...";
                smooth_waypoints(_readout_waypoints);
                ApInfo("task_executor") << "waypoints " << file_track << "smoothed: " << _readout_waypoints.size();
            } else {
                _readout_waypoints = std::move(tmp.first);
            }

            if (_readout_waypoints.size() < _min_way_points) {
                ApError("task_executor") << "waypoints " << file_track << " less than " << _min_way_points << ": "
                       << _readout_waypoints.size();
                throw ExceptionTrackPointsTooFew(file_track, _readout_waypoints.size());
            }
        }

        virtual ~PathPlannerGlobalOneway() = default;

        std::vector<std::vector<ara::adsfi::BusinessPoint>> paths() override {
            std::vector<std::vector<ara::adsfi::BusinessPoint>> r;

            if (_dest != _file_track_end) {
                std::reverse(_readout_waypoints.begin(), _readout_waypoints.end());
            }
            // 补一个点，以免在转换成 trackpoints
            // 时最后一个点丢失（否则，因为最后一个点缺少计算 heading
            // 的参照点，就不得不丢弃最后一个点）
            app_common::GeoUtils::AddVirtualWaypoint(_readout_waypoints);
            r.emplace_back(app_common::GeoUtils::Waypoints2Trackpoints(_readout_waypoints, 0));

            return r;
        }

    private:
        const std::string _dest;

        std::string _file_track_start;
        std::string _file_track_end;
    };
} // namespace app_common
