
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <memory>
#include <string>

#include "XMidTrackSendByZMQ.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "log/log.h"
#include "time/node_time.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
        timeout_ms = 200;
    }

    ~AdsfiInterface() {
    }

    void Init();

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg);

    void SetInnerActMsg(const std::shared_ptr<ara::adsfi::VehicleActControl> &ptr) {
        std::lock_guard<std::mutex> lock(act_control_mtx_);
        update_time.store(avos::common::NodeTime::Now().ToSecond());
        last_act_msg = *ptr;
    }

    void GetInnerActMsg(ara::adsfi::VehicleActControl &msg) {
        std::lock_guard<std::mutex> lock(act_control_mtx_);
        msg = last_act_msg;
    }

private:
    std::int32_t                     timeout_ms;
    ara::adsfi::VehicleActControl    last_act_msg;
    ara::adsfi::VehicleSensorControl sensor_ctl;
    ara::adsfi::FaultData            fault_data;
    std::atomic<double>              update_time;

    std::mutex act_control_mtx_;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
