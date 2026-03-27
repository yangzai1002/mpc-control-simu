
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "../src/XTaskExecuteMileage.hpp"
#include "../src/pathexecutor/PathExecutorSimple.hpp"
#include "../src/pathextractor/PathExtractorClosedLoop.hpp"
#include "../src/pathextractor/PathExtractorSimple.hpp"
#include "../src/pathplanner/PathPlannerLoopTrackClosed.hpp"
#include "../src/pathplanner/PathPlannerTrackOneway.hpp"
#include "../src/pathplanner/PathPlannerTrackTwoway.hpp"
#include "DatetimeUtil.hpp"
#include "GpxFileHandler.hpp"
#include "PatrolType.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "config_loader/custom_stack.h"
#include "monitor/faulthandle_api.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PlanningStatus> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::BusinessCommand> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::TaskExecuteStatus> &msg);

private:
    void execute(const ara::adsfi::MsgHafLocation &    location,               // 车辆当前位置
                 const ara::adsfi::PlanningStatus &    planning_status_track,  // 规划层当前状态: track
                 const ara::adsfi::PlanningStatus &    planning_status_arrive, // 规划层当前状态: arrive
                 const ara::adsfi::PlanningStatus &    planning_status_follow, // 规划层当前状态: follow
                 const ara::adsfi::PlanningStatus &    planning_status_adjust, // 规划层当前状态: adjust
                 const ara::adsfi::VehicleInformation &vehicle_information,    // 车辆当前状态
                 ara::adsfi::BusinessCommand &business_command, ara::adsfi::TaskExecuteStatus &task_execute_status);

private:
    // configurations
    std::string                                 _keyfile_storage_path;
    std::vector<std::string>                    _extra_faults_to_clear;
    std::shared_ptr<app_common::GpxFileHandler> _gpx_file_handler;

    // received state
    std::mutex                                      _in_mtx;
    std::shared_ptr<ara::adsfi::VehicleInformation> _vehicle_info_ptr;
    std::shared_ptr<ara::adsfi::MsgHafLocation>     _location_info_ptr;
    std::shared_ptr<ara::adsfi::PlanningStatus>     _planning_status_track_ptr;
    std::shared_ptr<ara::adsfi::PlanningStatus>     _planning_status_follow_ptr;
    std::shared_ptr<ara::adsfi::PlanningStatus>     _planning_status_arrive_ptr;
    std::shared_ptr<ara::adsfi::PlanningStatus>     _planning_status_adjust_ptr;
    std::shared_ptr<ara::adsfi::BusinessCommand>    _business_command_ptr;

    // runtime state
    std::int32_t                              _last_business_mode{};
    std::shared_ptr<app_common::PathExecutor> _path_executor;
    ara::adsfi::TaskExecuteStatus             task_execute_status;
    // progress/里程计算的内部状态（跨周期保留）：
    // - _has_progress_baseline/_progress_baseline_remaining_m：
    //   首次进入“可计算分支”时建立的 remaining 基线（单位：m，米），用于 progress 计算；
    //   同时也作为 total_mileage 的口径来源（总里程 = 建立基线时的 remaining）。
    bool   _has_progress_baseline{false};
    double _progress_baseline_remaining_m{0.0};
    // - _last_progress_percent/_hold_completed_progress：
    //   用于保证 progress 单调不减，以及在 NORMAL_END 后将 progress 锁定为
    //   100%，直到出现清零触发（start/用户结束/退自主等）。
    int  _last_progress_percent{0};
    bool _hold_completed_progress{false};
    // - _mileage：
    //   progress 计算 + 里程缓存/发布/日志节流（跨周期保留）。
    std::int32_t _last_input_command{0};

    XTaskExecuteMileage _mileage{};

    // output
    ara::adsfi::BusinessCommand   _out_business_command;
    ara::adsfi::TaskExecuteStatus _out_task_execute_status;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
