#ifndef _PATH_EXECUTOR_HPP
#define _PATH_EXECUTOR_HPP

#include <limits>
#include <vector>

#include "LocationInfoHandler.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_idpbusinesscommand.h"
#include "ara/adsfi/impl_type_idpplanningstatus.h"
#include "ara/adsfi/impl_type_idptaskexecutestatus.h"
#include "ara/adsfi/impl_type_idpvehicleinformation.h"
#include "pathextractor/PathExtractor.hpp"

namespace app_common {
    class PathExecutor {
    public:
        explicit PathExecutor(std::shared_ptr<PathExtractor> path_extractor)
            : _path_extractor(std::move(path_extractor)) {
            if (nullptr == _path_extractor) {
                throw std::runtime_error("path extractor is null");
            }
        }

        // 执行状态，0-空闲；1-执行中；2-暂停中；3-GPS位置异常；4-到达任务点；5-用户结束；6-正常结束；7-退自主结束；
        enum class ExecuteStatus : std::int32_t {
            IDLE = 0,
            RUNNING = 1,
            PAUSE = 2,
            GPS_LOCATION_ERROR = 3,
            ARRIVE_DESTINATION = 4,
            USER_TERMINATE = 5,
            NORMAL_END = 6,
            TERMINATE_AUTONOMOUS = 7
        };

        virtual ~PathExecutor() = default;

        virtual ara::adsfi::IdpTaskExecuteStatus start(ara::adsfi::IdpBusinessCommand&          business_command,
                                                       const adsfi::LocationInfo&               location,
                                                       const ara::adsfi::IdpVehicleInformation& vi) {
            ara::adsfi::IdpTaskExecuteStatus status{};
            business_command.business_path.points.clear();

            while (!_path_extractor->is_all_done()) {
                if (!app_common::GeoUtils::is_location_ok(location)) {
                    status.status = (std::int32_t)ExecuteStatus::GPS_LOCATION_ERROR;
                    break;
                }

                // business_command.business_path =
                //     _path_extractor->next(location.llh.lat, location.llh.lon, vi.vehicle_act_state.speed,
                //                           business_command.param.max_speed / 3.6);
                auto gccs = app_common::GeoUtils::get_gccs(app_common::LocationInfoHandler::Lat(location),
                                                           app_common::LocationInfoHandler::Lon(location));
                business_command.business_path = _path_extractor->next(
                    app_common::LocationInfoHandler::Lat(location), app_common::LocationInfoHandler::Lon(location),
                    gccs.xg, gccs.yg, app_common::LocationInfoHandler::Heading(location),
                    std::numeric_limits<double>::max() / 100.0, business_command.param.max_speed / 3.6);

                if (!business_command.business_path.points.empty()) {
                    auto gccs = app_common::GeoUtils::get_gccs(app_common::LocationInfoHandler::Lat(location),
                                                               app_common::LocationInfoHandler::Lon(location));
                    // status.distance_to_nearest = app_common::GeoUtils::gccs_distance(
                    //     business_command.business_path.points[0].x, business_command.business_path.points[0].y,
                    //     gccs.xg, gccs.yg);
                } else {
                    // status.distance_to_nearest = 0;
                }

                AINFO << "extracted points number: " << business_command.business_path.points.size();
                auto pcount = std::min((int)business_command.business_path.points.size(), 10);
                for (auto i = 0; i < pcount; ++i) {
                    auto& p = business_command.business_path.points[i];
                    // AINFO << "trackpoints " << i
                    AINFO << "trackpoints " << i << "/" << business_command.business_path.points.size()
                          << ", point: " << app_common::GeoUtils::str_gccs(p) << ", heading: " << p.heading
                          << ", index: " << p.index << ", avoid: " << business_command.param.task_avoid_mode
                          << ", speed: " << business_command.param.max_speed;
                }
                if (business_command.business_path.points.size() <= 5) {
                    AINFO << "extracted points number is less than 5, done this path.";
                    business_command.business_path.points.clear();
                    _path_extractor->done();
                    continue;
                } else {
                    status.status = (std::int32_t)ExecuteStatus::RUNNING;
                    break;
                }
            }

            return status;
        }

        // 使用者需要等到 ara::adsfi::IdpTaskExecuteStatus.status == 0 后才可以销毁 PathExecutor 对象
        virtual ara::adsfi::IdpTaskExecuteStatus resume(ara::adsfi::IdpBusinessCommand&          business_command,
                                                        const adsfi::LocationInfo&               location,
                                                        const ara::adsfi::IdpVehicleInformation& vi,
                                                        const ara::adsfi::IdpPlanningStatus&     planning_status) = 0;

    public:
        std::shared_ptr<PathExtractor> _path_extractor;
    };
} // namespace app_common

#endif
