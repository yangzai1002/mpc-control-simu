#pragma once

#include "GlobalPathGeneration.hpp"
#include "PathPlanner.hpp"
#include "log/log.h"

namespace app_common {
    class PathPlannerGlobalRawPlan : public PathPlanner {
    public:
        PathPlannerGlobalRawPlan(const std::string& osm_file, const std::string& roads_info) : PathPlanner() {
            auto global_path_generation = std::make_shared<GlobalPathGeneration>();
            global_path_generation->Init(osm_file.c_str());
            _readout_waypoints = global_path_generation->GetWayPointsByRoadsInfo(roads_info);
            AINFO << "waypoints size: " << _readout_waypoints.size();
            if (_readout_waypoints.size() < _min_way_points) {
                AERROR << "waypoints " << osm_file << " less than " << _min_way_points << ": "
                       << _readout_waypoints.size();
                throw ExceptionTrackPointsTooFew(osm_file, _readout_waypoints.size());
            }
        }

        virtual ~PathPlannerGlobalRawPlan() = default;

        std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> paths() override {
            std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> r;

            // 补一个点，以免在转换成 trackpoints
            // 时最后一个点丢失（否则，因为最后一个点缺少计算 heading
            // 的参照点，就不得不丢弃最后一个点）
            app_common::GeoUtils::AddVirtualWaypoint(_readout_waypoints);
            r.emplace_back(app_common::GeoUtils::Waypoints2Trackpoints(_readout_waypoints, 0));

            return r;
        }
    }; // namespace app_common
} // namespace app_common
