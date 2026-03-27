#ifndef _PATH_EXECUTOR_SIMPLE_TASKPOINTS_HPP_
#define _PATH_EXECUTOR_SIMPLE_TASKPOINTS_HPP_

#include <vector>

#include "../pathextractor/PathExtractor.hpp"
#include "GeoUtils.hpp"
#include "PathExecutor.hpp"
#include "TaskPoint.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_idpplanningstatus.h"
#include "fmt/format.h"

namespace app_common {
    class PathExecutorSimpleTaskPoints : public PathExecutor {
    public:
        PathExecutorSimpleTaskPoints(const std::shared_ptr<PathExtractor>& path_extractor,
                                     const std::vector<TaskPoint>& task_points, double threshold_distance)
            : PathExecutor(path_extractor)
            , _task_points(task_points)
            , _threshold_distance(threshold_distance)
            , _last_planning_status{}
            , _last_business_command{}
            , _last_is_all_done(false) {
        }

        virtual ~PathExecutorSimpleTaskPoints() = default;

        ara::adsfi::IdpTaskExecuteStatus resume(ara::adsfi::IdpBusinessCommand&          business_command,
                                                const adsfi::LocationInfo&               location,
                                                const ara::adsfi::IdpVehicleInformation& vi,
                                                const ara::adsfi::IdpPlanningStatus&     planning_status) override {
            // 执行状态，0-空闲；1-执行中；2-暂停中；3-GPS位置异常；4-到达任务点；5-用户结束；6-正常结束；7-退自主结束；
            ara::adsfi::IdpTaskExecuteStatus status{};

            if (1 != business_command.business_mode) { // 未在循迹
                if (1 == _last_business_command.business_mode) {
                    status.status = (std::int32_t)ExecuteStatus::USER_TERMINATE;
                } else {
                    status.status = (std::int32_t)ExecuteStatus::IDLE;
                }
            } else {                                       // 循迹中
                if (0 == business_command.param.command) { // 任务停止状态
                    _path_extractor->reset();
                }
                // 0: 空闲  1: 执行中  2: 主动结束任务  3: 退出自主
                switch (planning_status.command_status) {
                    case 0: { // 0: 空闲
                        if (2 == _last_planning_status.command_status || 1 == _last_planning_status.command_status) {
                            status.status = (std::int32_t)ExecuteStatus::RUNNING;
                            _path_extractor->done();

                            std::for_each(_task_points.begin(), _task_points.end(),
                                          [this, &status, location](const TaskPoint& tp) {
                                              if (app_common::GeoUtils::gps_real_distance(
                                                      app_common::LocationInfoHandler::Lat(location),
                                                      app_common::LocationInfoHandler::Lon(location), tp.lat,
                                                      tp.lon) < _threshold_distance) {
                                                  ara::adsfi::IdpBusinessTaskPoint p{};
                                                  p.lat = tp.lat;
                                                  p.lon = tp.lon;
                                                  status.task_points.task_points.push_back(p);
                                              }
                                          });
                            if (!status.task_points.task_points.empty()) {
                                status.status = (std::int32_t)ExecuteStatus::ARRIVE_DESTINATION;
                                AINFO << "here";
                            } else {
                                AINFO << "here";
                                if (_path_extractor->is_all_done()) {
                                    if (_last_is_all_done) {
                                        status.status = (std::int32_t)ExecuteStatus::IDLE;
                                        AINFO << "here";
                                    } else {
                                        AINFO << "here";
                                        status.status = (std::int32_t)ExecuteStatus::NORMAL_END;
                                    }
                                } else {
                                    AINFO << "here";
                                    status.status = (std::int32_t)ExecuteStatus::RUNNING;
                                }
                            }
                            AINFO << "here";
                        } else if (3 == _last_planning_status.command_status) {
                            status.status = (std::int32_t)ExecuteStatus::TERMINATE_AUTONOMOUS;
                            _path_extractor->reset();
                            AINFO << "here";
                        } else if (0 == _last_planning_status.command_status) {
                            if (_path_extractor->is_all_done()) {
                                if (_last_is_all_done) {
                                    AINFO << "here";
                                    status.status = (std::int32_t)ExecuteStatus::IDLE;
                                } else {
                                    AINFO << "here";
                                    status.status = (std::int32_t)ExecuteStatus::NORMAL_END;
                                }
                            } else {
                                AINFO << "here";
                                status.status = (std::int32_t)ExecuteStatus::RUNNING;
                            }
                        }
                    } break;

                    case 1: { // 1: 执行中
                        status.status = (std::int32_t)ExecuteStatus::RUNNING;
                    } break;

                    case 2: { // 2: 主动结束任务
                        status.status = (std::int32_t)ExecuteStatus::RUNNING;
                    } break;
                    case 3: { // 3: 退出自主
                        status.status = (std::int32_t)ExecuteStatus::TERMINATE_AUTONOMOUS;
                    } break;
                }
            }

            if ((std::int32_t)ExecuteStatus::RUNNING == status.status) {
                // 20250822: 正常提路。因为导控所的定位会出现突然跳变到 2 的情况，导致车辆急停，所以先屏蔽
                // if (!app_common::GeoUtils::is_location_ok(location)) {
                //     status.status = (std::int32_t)ExecuteStatus::GPS_LOCATION_ERROR;
                // } else {
                auto gccs = app_common::GeoUtils::get_gccs(app_common::LocationInfoHandler::Lat(location),
                                                           app_common::LocationInfoHandler::Lon(location));
                business_command.business_path = _path_extractor->next(
                    app_common::LocationInfoHandler::Lat(location), app_common::LocationInfoHandler::Lon(location),
                    gccs.xg, gccs.yg, app_common::LocationInfoHandler::Heading(location), vi.vehicle_act_state.speed,
                    business_command.param.max_speed / 3.6);
                // }

                AINFO << "extracted points number: " << business_command.business_path.points.size();
                auto pcount = std::min((int)business_command.business_path.points.size(), 10);
                for (auto i = 0; i < pcount; ++i) {
                    auto& p = business_command.business_path.points[i];
                    // AINFO << "trackpoints " << i
                    AINFO << "trackpoints " << i << "/" << business_command.business_path.points.size()
                          << ", point: " << app_common::GeoUtils::str_gccs(p) << ", heading: " << p.heading
                          << ", relative: " << app_common::GeoUtils::str_relative(p) << ", index: " << p.index
                          << ", avoid: " << business_command.param.task_avoid_mode
                          << ", speed: " << business_command.param.max_speed;
                }
                if (2 == planning_status.command_status) {
                    business_command.param.command = 0;
                }
                if (2 == business_command.param.command) { // 暂停
                    status.status = (std::int32_t)ExecuteStatus::PAUSE;
                }
            } else if ((std::int32_t)ExecuteStatus::TERMINATE_AUTONOMOUS == status.status) {
                business_command.param.command = 0;
            }

            AINFO << fmt::format(
                "business_mode: (this={}, last={}), planning_status: (this={}, last={}), is_all_done: (this={}, "
                "last={}), out_execute_status: {}, out_business_command: {}",
                business_command.business_mode, _last_business_command.business_mode, planning_status.command_status,
                _last_planning_status.command_status, _path_extractor->is_all_done(), _last_is_all_done, status.status,
                business_command.param.command);

            _last_planning_status = planning_status;
            _last_business_command = business_command;
            _last_is_all_done = _path_extractor->is_all_done();

            return status;
        }

    private:
        const std::vector<TaskPoint> _task_points;
        const double                 _threshold_distance;

        // 过程变量
        ara::adsfi::IdpPlanningStatus  _last_planning_status;
        ara::adsfi::IdpBusinessCommand _last_business_command;
        bool                           _last_is_all_done;
    };
} // namespace app_common

#endif
