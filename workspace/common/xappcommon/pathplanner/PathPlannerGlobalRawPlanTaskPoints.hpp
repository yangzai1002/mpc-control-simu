#pragma once

#include <vector>

#include "GlobalPathGeneration.hpp"
#include "PathPlanner.hpp"
#include "TaskPoint.hpp"
#include "log/log.h"

namespace app_common {
    class PathPlannerGlobalRawPlanTaskPoints : public PathPlanner {
    public:
        PathPlannerGlobalRawPlanTaskPoints(const std::string& osm_file, const std::string& roads_info,
                                           const std::vector<TaskPoint>& task_points)
            : PathPlanner(), _task_points(task_points) {
            auto global_path_generation_ = std::make_shared<GlobalPathGeneration>();
            global_path_generation_->Init(osm_file.c_str());
            _readout_waypoints = global_path_generation_->GetWayPointsByRoadsInfo(roads_info);
            AINFO << "waypoints size: " << _readout_waypoints.size();
            if (_readout_waypoints.size() < _min_way_points) {
                AERROR << "waypoints " << osm_file << " less than " << _min_way_points << ": "
                       << _readout_waypoints.size();
                throw ExceptionTrackPointsTooFew(osm_file, _readout_waypoints.size());
            }
        }

        virtual ~PathPlannerGlobalRawPlanTaskPoints() = default;

        std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> paths() override {
            std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> r;

            auto segments = app_common::GeoUtils::split_waypoints(_readout_waypoints, _task_points);
            for (auto& segment : segments) {
                // 补一个点，以免在转换成 trackpoints
                // 时最后一个点丢失（否则，因为最后一个点缺少计算 heading
                // 的参照点，就不得不丢弃最后一个点）
                app_common::GeoUtils::AddVirtualWaypoint(segment);
                r.emplace_back(app_common::GeoUtils::Waypoints2Trackpoints(segment, 0));
            }

            return r;
        }

    private:
        const std::vector<TaskPoint> _task_points;
    }; // namespace app_common
} // namespace app_common
