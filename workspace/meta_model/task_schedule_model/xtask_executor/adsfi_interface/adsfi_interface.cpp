#include <fmt/format.h>

#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"

AdsfiInterface::AdsfiInterface() : _last_business_mode(0) {
    auto ptr = CustomStack::Instance();
    auto full_path = "keyfile_storage_path";
    if (!ptr->GetProjectConfigValue(full_path, _keyfile_storage_path) || _keyfile_storage_path.empty()) {
        auto error = fmt::format("load project config {} failed", full_path);
        ApError("task_executor") << error;
        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error(error);
    }

    // https://landshipai.feishu.cn/wiki/Cr6pwWIcgifuJskBE3UcvWPWnid
    full_path = "planner/extra_faults_to_clear";
    if (!ptr->GetProjectConfigArray(full_path, _extra_faults_to_clear)) {
        auto error = fmt::format("load project config {} failed", full_path);
        ApError("task_executor") << error;
        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error(error);
    }
    for (const auto &fault_code : _extra_faults_to_clear) {
        ApInfo("task_executor") << "extra_faults_to_clear: " << fault_code;
    }
    FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);

    _gpx_file_handler =
        std::make_shared<app_common::GpxFileHandler>(_keyfile_storage_path + "/tracks", "default_track.txt");

    // make sure received state always not null
    _vehicle_info_ptr = std::make_shared<ara::adsfi::VehicleInformation>(ara::adsfi::VehicleInformation{});
    _location_info_ptr = std::make_shared<ara::adsfi::MsgHafLocation>(ara::adsfi::MsgHafLocation{});
    _planning_status_track_ptr = std::make_shared<ara::adsfi::PlanningStatus>(ara::adsfi::PlanningStatus{});
    _planning_status_follow_ptr = std::make_shared<ara::adsfi::PlanningStatus>(ara::adsfi::PlanningStatus{});
    _planning_status_arrive_ptr = std::make_shared<ara::adsfi::PlanningStatus>(ara::adsfi::PlanningStatus{});
    _planning_status_adjust_ptr = std::make_shared<ara::adsfi::PlanningStatus>(ara::adsfi::PlanningStatus{});
    _business_command_ptr = std::make_shared<ara::adsfi::BusinessCommand>(ara::adsfi::BusinessCommand{});
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 100);
    // ApLogInterface::Instance()->Init("XTaskExecutor");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("task_executor") << "msg null, name: " << name;
        return;
    }

    std::lock_guard lg(_in_mtx);
    _vehicle_info_ptr = msg;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("task_executor") << "msg null, name: " << name;
        return;
    }

    std::lock_guard lg(_in_mtx);
    _location_info_ptr = msg;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PlanningStatus> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("task_executor") << "msg null, name: " << name;
        return;
    }

    if (std::string::npos != name.find("tpplanningstatus_track")) {
        std::lock_guard lg(_in_mtx);
        _planning_status_track_ptr = msg;
        return;
    }

    if (std::string::npos != name.find("tpplanningstatus_follow")) {
        std::lock_guard lg(_in_mtx);
        _planning_status_follow_ptr = msg;
        return;
    }

    if (std::string::npos != name.find("tpplanningstatus_arrive")) {
        std::lock_guard lg(_in_mtx);
        _planning_status_arrive_ptr = msg;
        return;
    }

    if (std::string::npos != name.find("tpplanningstatus_adjust")) {
        std::lock_guard lg(_in_mtx);
        _planning_status_adjust_ptr = msg;
        return;
    }

    ApError("task_executor") << "msg, whose? " << name;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("task_executor") << "msg null, name: " << name;
        return;
    }

    std::lock_guard lg(_in_mtx);
    _business_command_ptr = msg;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::BusinessCommand> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("task_executor") << "msg is null";
        return -1;
    }
    // always not null
    ara::adsfi::MsgHafLocation     location;
    ara::adsfi::PlanningStatus     planning_status_track;
    ara::adsfi::PlanningStatus     planning_status_arrive;
    ara::adsfi::PlanningStatus     planning_status_follow;
    ara::adsfi::PlanningStatus     planning_status_adjust;
    ara::adsfi::VehicleInformation vehicle_information;

    {
        std::lock_guard lg(_in_mtx);
        location = *_location_info_ptr;
        planning_status_track = *_planning_status_track_ptr;
        planning_status_arrive = *_planning_status_arrive_ptr;
        planning_status_follow = *_planning_status_follow_ptr;
        planning_status_adjust = *_planning_status_adjust_ptr;
        vehicle_information = *_vehicle_info_ptr;
    }

    {
        _out_business_command = *_business_command_ptr;
        execute(location, planning_status_track, planning_status_arrive, planning_status_follow, planning_status_adjust,
                vehicle_information, _out_business_command, _out_task_execute_status);
        *msg = _out_business_command;
        msg->header.time = app_common::DatetimeUtil::current_sec_nsec<decltype(msg->header.time)>();
    }

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::TaskExecuteStatus> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        ApError("task_executor") << "msg is null";
        return -1;
    }

    *msg = _out_task_execute_status;

    return 0;
}

void AdsfiInterface::execute(const ara::adsfi::MsgHafLocation &location,                // 车辆当前位置
                             const ara::adsfi::PlanningStatus &planning_status_track,   // 规划层当前状态: track
                             const ara::adsfi::PlanningStatus &planning_status_arrive,  // 规划层当前状态: arrive
                             const ara::adsfi::PlanningStatus &planning_status_follow,  // 规划层当前状态: follow
                             const ara::adsfi::PlanningStatus &planning_status_adjust,  // 规划层当前状态: adjust
                             const ara::adsfi::VehicleInformation &vehicle_information, // 车辆当前状态
                             ara::adsfi::BusinessCommand &         business_command,
                             ara::adsfi::TaskExecuteStatus &       task_execute_status) {
    if (_last_business_mode != 0 && 0 == business_command.business_mode) {
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_TRACK_LOAD);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_TRACK_DESTINATION);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_TRACK_POINTS_FEW);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_TRACK_VEHICLE_TOO_FAR);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_TOO_SHORT);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_DISTANCE_FORM_S_TO_E);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_HEADING);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_LOOPTRACK_INCONSISTENCY_HEADING);
        for (const auto &fault_code : _extra_faults_to_clear) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(fault_code);
        }
        // 业务模式从“有任务”切回 0：认为本次任务已退出，需清空 progress 的基线状态。
        // 但若此前已 NORMAL_END 并进入“完成保持”窗口，则 progress/里程仍需保持最后一次输出，
        // 因此仅在非保持状态下才清零百分比与里程缓存。
        ApInfo("task_executor") << "[XTaskExecute][里程] 业务模式切回 0：清空 progress/里程基线。hold_completed="
                                << _hold_completed_progress;
        _has_progress_baseline = false;
        _progress_baseline_remaining_m = 0.0;
        if (!_hold_completed_progress) {
            ApInfo("task_executor")
                << "[XTaskExecute][里程] 业务模式切回 0：非完成保持状态，清零里程缓存与进度百分比。";
            _last_progress_percent = 0;
            _mileage.ClearCache();
        }
    }

    // 目前只管理循迹
    if (app_common::PathExecutor::is_track_or_arrive(business_command.business_mode)) {
        if (0 == _last_business_mode || !_path_executor) {
            auto has_error = true;
            try {
                switch ((PatrolType)business_command.param.patrol_type) {
                    case PatrolType::TurnAround: { // 正着去、正着回来、即要掉头
                        auto planner = std::make_shared<app_common::PathPlannerTrackTwoway>(
                            business_command.param.patrol_name, business_command.param.patrol_dest, *_gpx_file_handler,
                            false);
                        auto extractor = std::make_shared<app_common::PathExtractorSimple>(planner->paths());
                        _path_executor = std::make_shared<app_common::PathExecutorSimple>(extractor);
                    } break;
                    case PatrolType::Reverse: { // 正着去、退着回来，即回来时倒车
                        auto planner = std::make_shared<app_common::PathPlannerTrackTwoway>(
                            business_command.param.patrol_name, business_command.param.patrol_dest, *_gpx_file_handler,
                            true);
                        auto extractor = std::make_shared<app_common::PathExtractorSimple>(planner->paths());
                        _path_executor = std::make_shared<app_common::PathExecutorSimple>(extractor);
                    } break;
                    case PatrolType::Return:
                    case PatrolType::Oneway: { // 单程循迹
                        auto planner = std::make_shared<app_common::PathPlannerTrackOneway>(
                            business_command.param.patrol_name, business_command.param.patrol_dest, *_gpx_file_handler);
                        auto extractor = std::make_shared<app_common::PathExtractorSimple>(planner->paths());
                        _path_executor = std::make_shared<app_common::PathExecutorSimple>(extractor);
                    } break;
                    case PatrolType::ClosedLoop: { // 闭环循迹
                        auto planner = std::make_shared<app_common::PathPlannerLoopTrackClosed>(
                            business_command.param.patrol_name, *_gpx_file_handler);
                        auto extractor = std::make_shared<app_common::PathExtractorClosedLoop>(planner->paths());
                        _path_executor = std::make_shared<app_common::PathExecutorSimple>(extractor);
                    } break;
                    // case PatrolType::GlobalOneway: {
                    //     auto planner = std::make_shared<app_common::PathPlannerGlobalOneway>(*_gpx_file_handler);
                    //     auto extractor = std::make_shared<app_common::PathExtractorSimple>(planner->paths());
                    //     auto task_points = load_task_points(_global_task_points_file);
                    //     _path_executor =
                    //         std::make_shared<app_common::PathExecutorSimpleTaskPoints>(extractor, task_points, 3.0);
                    // } break;
                    // case PatrolType::GlobalRawplan: {
                    //     auto osm_file = _keyfile_storage_path + "/global.osm";
                    //     auto planner = std::make_shared<app_common::PathPlannerGlobalRawPlan>(
                    //         osm_file, business_command.param.patrol_roads);
                    //     auto extractor = std::make_shared<app_common::PathExtractorSimple>(planner->paths());
                    //     auto task_points = load_task_points(_global_task_points_file);
                    //     _path_executor =
                    //         std::make_shared<app_common::PathExecutorSimpleTaskPoints>(extractor, task_points, 3.0);
                    // } break;
                    default:
                        throw std::runtime_error("unknow patrol type: " +
                                                 std::to_string(business_command.param.patrol_type));
                }
                task_execute_status = _path_executor->start(business_command, location, vehicle_information);
                ApInfo("task_executor") << "XTaskExecute: " << task_execute_status.status;
                has_error = false;
            } catch (const app_common::ExceptionLoadPlannerConfig &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, e.config_name);
            } catch (const app_common::ExceptionLoadExtractorConfig &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, e.config_name);
            } catch (const app_common::ExceptionLoadExeutorConfig &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, e.config_name);
            } catch (const app_common::ExceptionLoadTrackFile &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_TRACK_LOAD, e.track_file);
            } catch (const app_common::ExceptionInvalidDestination &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_TRACK_DESTINATION, e.track_file);
            } catch (const app_common::ExceptionTrackPointsTooFew &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_TRACK_POINTS_FEW, e.track_file);
            } catch (const app_common::ExceptionStartTooFewPoints &e) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_TRACK_POINTS_FEW);
            } catch (const std::exception &e) {
                ApError("task_executor") << "error: " << e.what();
            }
            if (has_error) {
                _path_executor.reset();
                _has_progress_baseline = false;
                _progress_baseline_remaining_m = 0.0;
                _last_progress_percent = 0;
                _hold_completed_progress = false;
            }
        } else if (app_common::PathExecutor::is_track(business_command.business_mode)) {
            task_execute_status =
                _path_executor->resume(business_command, location, vehicle_information, planning_status_track);
            ApInfo("task_executor") << "XTaskExecute: " << task_execute_status.status;
            task_execute_status.type = 1;
            task_execute_status.current_limit_speed = planning_status_track.current_limit_speed;
            if (task_execute_status.status == (std::int32_t)app_common::PathExecutor::ExecuteStatus::IDLE) {
                ApInfo("task_executor") << "here";
                _path_executor = nullptr;
            }
        } else {
            task_execute_status =
                _path_executor->resume(business_command, location, vehicle_information, planning_status_arrive);
            ApInfo("task_executor") << "XTaskExecute: " << task_execute_status.status;
            task_execute_status.type = 3;
            task_execute_status.current_limit_speed = planning_status_arrive.current_limit_speed;
            if (task_execute_status.status == (std::int32_t)app_common::PathExecutor::ExecuteStatus::IDLE) {
                ApInfo("task_executor") << "here";
                _path_executor = nullptr;
            }
        }
    } else if (app_common::PathExecutor::is_follow(business_command.business_mode)) { // 跟随
        ApInfo("task_executor") << "here";
        task_execute_status.type = 2;
        task_execute_status.current_limit_speed = planning_status_follow.current_limit_speed;
        // 任务状态，0-空闲；1-执行中；2-GPS位置异常；3-到达任务点；4-用户结束；5-正常结束；6-退自主结束；
        switch (planning_status_follow.command_status) {
            case 1:
                task_execute_status.status = (std::int32_t)app_common::PathExecutor::ExecuteStatus::RUNNING;
                break;
            case 2:
                task_execute_status.status = (std::int32_t)app_common::PathExecutor::ExecuteStatus::USER_TERMINATE;
                business_command.param.command = 0;
                break;
            case 3:
                task_execute_status.status =
                    (std::int32_t)app_common::PathExecutor::ExecuteStatus::TERMINATE_AUTONOMOUS;
                business_command.param.command = 0;
                break;
            case 0:
            default:
                task_execute_status.status = (std::int32_t)app_common::PathExecutor::ExecuteStatus::IDLE;
                break;
        }
    } else if (app_common::PathExecutor::is_adjust(business_command.business_mode)) { // 调姿
        ApInfo("task_executor") << "here";
        task_execute_status.type = 1;
        task_execute_status.current_limit_speed = planning_status_adjust.current_limit_speed;
        // 任务状态，0-空闲；1-执行中；2-GPS位置异常；3-到达任务点；4-用户结束；5-正常结束；6-退自主结束；
        switch (planning_status_adjust.command_status) {
            case 1:
                task_execute_status.status = (std::int32_t)app_common::PathExecutor::ExecuteStatus::RUNNING;
                break;
            case 2:
                task_execute_status.status = (std::int32_t)app_common::PathExecutor::ExecuteStatus::USER_TERMINATE;
                business_command.param.command = 0;
                break;
            case 3:
                task_execute_status.status =
                    (std::int32_t)app_common::PathExecutor::ExecuteStatus::TERMINATE_AUTONOMOUS;
                business_command.param.command = 0;
                break;
            case 0:
            default:
                task_execute_status.status = (std::int32_t)app_common::PathExecutor::ExecuteStatus::IDLE;
                break;
        }
    } else {
        // 如果不是循迹不做处理
        task_execute_status.status = 0;
        task_execute_status.current_limit_speed = 0;
    }

    const std::int32_t input_command = business_command.param.command;
    // 里程对外输出单位统一为 m（米），并与 progress 的“完成保持/清零触发”规则保持一致。
    _mileage.UpdateByRemainingLengthM(business_command, input_command, _last_business_mode, _path_executor.get(),
                                      task_execute_status, _has_progress_baseline, _progress_baseline_remaining_m,
                                      _last_progress_percent, _hold_completed_progress, _last_input_command);

    _last_business_mode = business_command.business_mode;
}
