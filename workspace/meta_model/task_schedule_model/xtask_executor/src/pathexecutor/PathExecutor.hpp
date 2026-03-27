#ifndef _PATH_EXECUTOR_HPP
#define _PATH_EXECUTOR_HPP

#include <limits>
#include <vector>

#include "../Ec408.hpp"
#include "../pathextractor/PathExtractor.hpp"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "custom_stack.h"

namespace app_common {
    class ExceptionLoadExeutorConfig : public std::exception {
    public:
        const std::string config_name;
        explicit ExceptionLoadExeutorConfig(const std::string& config_name_)
            : std::exception(), config_name(config_name_) {
        }
    };

    class PathExecutor {
    public:
        static bool is_track(std::int32_t mode) {
            return (1 == mode || 11 == mode);
        }

        static bool is_follow(std::int32_t mode) {
            return (3 == mode || 31 == mode);
        }

        static bool is_arrive(std::int32_t mode) {
            return (2 == mode);
        }

        static bool is_adjust(std::int32_t mode) {
            return (4 == mode);
        }

        static bool is_track_or_arrive(std::int32_t mode) {
            return (is_track(mode) || is_arrive(mode));
        }

    public:
        explicit PathExecutor(std::shared_ptr<PathExtractor> path_extractor)
            : _path_extractor(std::move(path_extractor)) {
            if (nullptr == _path_extractor) {
                throw std::runtime_error("path extractor is null");
            }

            auto full_path = "planner/track/match_global_nearest_point_when_start";
            if (!CustomStack::Instance()->GetProjectConfigValue(full_path, _match_global_nearest_point_when_start)) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExeutorConfig(full_path);
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

        virtual double remaining_length_m() const {
            if (!_path_extractor) {
                return 0.0;
            }
            return _path_extractor->remaining_length_m();
        }

        virtual ara::adsfi::TaskExecuteStatus start(ara::adsfi::BusinessCommand&          business_command,
                                                    const ara::adsfi::MsgHafLocation&     location,
                                                    const ara::adsfi::VehicleInformation& vi) {
            ara::adsfi::TaskExecuteStatus status{};
            business_command.business_path.points.clear();

            while (!_path_extractor->is_all_done()) {
                if (!app_common::GeoUtils::is_location_ok(location)) {
                    status.status = (std::int32_t)ExecuteStatus::GPS_LOCATION_ERROR;
                    break;
                }

                auto current_speed_mps = _match_global_nearest_point_when_start
                                             ? std::numeric_limits<double>::max() / 100.0
                                             : vi.vehicle_act_state.speed;
                business_command.business_path = _path_extractor->next(
                    location.llh.y, location.llh.x, location.pose.pose.position.x, location.pose.pose.position.y,
                    location.pose.pose.orientation.z, current_speed_mps, business_command.param.max_speed / 3.6);

                if (!business_command.business_path.points.empty()) {
                    auto gccs = app_common::GeoUtils::get_gccs(location.llh.y, location.llh.x);
                    status.distance_to_nearest = app_common::GeoUtils::gccs_distance(
                        business_command.business_path.points[0].xg, business_command.business_path.points[0].yg,
                        gccs.xg, gccs.yg);
                } else {
                    status.distance_to_nearest = 0;
                }

                ApInfo("task_executor") << "extracted points number: " << business_command.business_path.points.size();
                auto pcount = std::min((int)business_command.business_path.points.size(), 10);
                for (auto i = 0; i < pcount; ++i) {
                    auto& p = business_command.business_path.points[i];
                    // ApInfo("task_executor") << "trackpoints " << i
                    ApInfo("task_executor") << "trackpoints " << i << "/" << business_command.business_path.points.size()
                          << ", point: " << app_common::GeoUtils::str_gccs(p) << ", heading: " << p.heading
                          << ", index: " << p.index << ", avoid: " << business_command.param.task_avoid_mode
                          << ", speed: " << business_command.param.max_speed;
                }
                if (business_command.business_path.points.size() <= 5) {
                    ApInfo("task_executor") << "extracted points number is less than 5, done this path.";
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

        // 使用者需要等到 TaskExecuteStatus.status == 0 后才可以销毁 PathExecutor 对象
        virtual ara::adsfi::TaskExecuteStatus resume(ara::adsfi::BusinessCommand&          business_command,
                                                     const ara::adsfi::MsgHafLocation&     location,
                                                     const ara::adsfi::VehicleInformation& vi,
                                                     const ara::adsfi::PlanningStatus&     planning_status) = 0;

    public:
        std::shared_ptr<PathExtractor> _path_extractor;
        bool                           _match_global_nearest_point_when_start;
    };
} // namespace app_common

#endif
