#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "PerceptionFinalObjects.h"
#include "PerceptionTargetObj.h"
#include "StringUtils.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
// #include "ara/adsfi/impl_type_bagstatus.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "ara/adsfi/impl_type_videortpparameter.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"
#define ZMQ_CPP11
#include <zmq.h>

#include "ZmqApiHandler3.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
    }
    ~AdsfiInterface() {
    }

    void Init();

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &msg);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::TaskExecuteStatus> &msg);

    // void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BagStatus> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::BusinessCommand> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleSensorControl> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::AppVideoProfile> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VideoRTPParameter> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::TaskManageStatus> &msg);

private:
    std::shared_ptr<ZmqApiHandler3> _handler;

    std::mutex _data_mtx;

    ara::adsfi::MsgHafLocation       _gps_cache{};
    ara::adsfi::TaskExecuteStatus    _task_exe_status_cache{};
    ara::adsfi::VehicleInformation   _vehicle_information_cache{};
    ara::adsfi::MsgHafFusionOutArray _detected_objects_cache{};
    ara::adsfi::MsgHafFusionOutArray _target_obj_cache{};
    // ara::adsfi::BagStatus          _bag_status_cache{};

public:
    BASE_TEMPLATE_FUNCION
};
#endif