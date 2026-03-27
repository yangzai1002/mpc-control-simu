#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "adsfi_manager/base_adsfi_interface.h"
#include "ap_log/ap_log_interface.h"

// 引入所有需要的业务数据类型头文件
#include "PerceptionFinalObjects.h"
#include "PerceptionStaticEnv.h"
#include "PerceptionTargetObj.h"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_videoencodingperf.h"

void Xzmq_notification3(
    const ara::adsfi::MsgHafLocation& imu, const ara::adsfi::MsgHafGnssInfo& location,
    const ara::adsfi::FaultData& fault_data, const ara::adsfi::VehicleInformation& vehicle_information,
    const ara::adsfi::VehicleActControl& control, const ara::adsfi::PerceptionStaticEnv& semantic_map,
    const ara::adsfi::MsgHafFusionOutArray& detected_objects, const ara::adsfi::MsgHafFusionOutArray& target_obj_out,
    const ara::adsfi::BusinessCommand& business_command, const ara::adsfi::TaskManageStatus& task_manage_status,
    const ara::adsfi::AppVideoProfile& video_profile, const ara::adsfi::AppRemoteControl& remote_control,
    const ara::adsfi::VideoEncodingPerf& encoding_perf);

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    // 回调函数
    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::AppVideoProfile>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::TaskManageStatus>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::BusinessCommand>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafLocation>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgHafGnssInfo>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::FaultData>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VehicleInformation>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VideoEncodingPerf>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VehicleActControl>& msg);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::AppRemoteControl>& msg);

private:
    void WorkerLoop();

    void GetCurrentDataSnapshotUnsafe(
        ara::adsfi::MsgHafLocation& imu, ara::adsfi::MsgHafGnssInfo& location, ara::adsfi::FaultData& fault_data,
        ara::adsfi::VehicleInformation& vehicle_info, ara::adsfi::VehicleActControl& control,
        ara::adsfi::PerceptionStaticEnv& semantic_map, ara::adsfi::MsgHafFusionOutArray& detected_objects,
        ara::adsfi::MsgHafFusionOutArray& target_obj, ara::adsfi::BusinessCommand& business_cmd,
        ara::adsfi::TaskManageStatus& task_status, ara::adsfi::AppVideoProfile& video_profile,
        ara::adsfi::AppRemoteControl& remote_control, ara::adsfi::VideoEncodingPerf& encoding_perf);

private:
    // 本地缓存数据
    mutable std::mutex               data_mutex_;
    ara::adsfi::AppVideoProfile      latest_video_profile_;
    ara::adsfi::TaskManageStatus     latest_task_manage_status_;
    ara::adsfi::BusinessCommand      latest_business_command_;
    ara::adsfi::MsgHafFusionOutArray latest_detected_objects_;
    ara::adsfi::MsgHafFusionOutArray latest_target_obj_out_;
    ara::adsfi::MsgHafLocation       latest_imu_;
    ara::adsfi::MsgHafGnssInfo       latest_location_;
    ara::adsfi::PerceptionStaticEnv  latest_semantic_map_;
    ara::adsfi::FaultData            latest_fault_data_;
    ara::adsfi::VehicleInformation   latest_vehicle_information_;
    ara::adsfi::VideoEncodingPerf    latest_encoding_perf_;
    ara::adsfi::VehicleActControl    latest_control_;
    ara::adsfi::AppRemoteControl     latest_remote_control_;

    std::atomic<bool> running_{false};

public:
    BASE_TEMPLATE_FUNCION
};

#endif