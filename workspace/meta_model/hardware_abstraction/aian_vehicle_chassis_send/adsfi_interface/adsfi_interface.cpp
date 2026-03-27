#include <fmt/format.h>

#include "DatetimeUtil.hpp"
#include "adsfi_interface.h"
#include "monitor/faulthandle_api.hpp"

#define ERRORCODE_CONFIG_READ_ERROR "1062001" // 配置参数读取失败

AdsfiInterface::AdsfiInterface()
    : _expiry_threshold_control_ms(100), _expiry_threshold_fault_ms(100), _driver_ptr(nullptr) {
    std::string full_path;
    std::string can_name;

    auto ptr = CustomStack::Instance();
    full_path = "auto/canbus/send/can_device";
    if (!ptr->GetProjectConfigValue(full_path, can_name) || can_name.empty()) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(ERRORCODE_CONFIG_READ_ERROR, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        AERROR << error;
        throw std::runtime_error(error);
    }
    AINFO << full_path << ": " << can_name;

    full_path = "auto/canbus/send/expiry_threshold_control";
    if (!ptr->GetProjectConfigValue(full_path, _expiry_threshold_control_ms) || _expiry_threshold_control_ms < 0) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(ERRORCODE_CONFIG_READ_ERROR, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        AERROR << error;
        throw std::runtime_error(error);
    }
    AINFO << full_path << ": " << _expiry_threshold_control_ms;

    full_path = "auto/canbus/send/expiry_threshold_fault";
    if (!ptr->GetProjectConfigValue(full_path, _expiry_threshold_fault_ms) || _expiry_threshold_fault_ms < 0) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(ERRORCODE_CONFIG_READ_ERROR, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        AERROR << error;
        throw std::runtime_error(error);
    }
    AINFO << full_path << ": " << _expiry_threshold_fault_ms;

    FaultHandle::FaultApi::Instance()->ResetFaultCode(ERRORCODE_CONFIG_READ_ERROR);

    _driver_ptr = std::make_shared<AionCanbusSendData>();
    _driver_ptr->config.can_name = can_name;
    _driver_ptr->Init();
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 20);
    // avos::common::AvosNode::Init("AionCanbusSend", true, "/opt/usr/zxz/log/", "/opt/usr/zxz/log/",
    // "/opt/usr/zxz/log/");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        AERROR << "msg is null";
        return;
    }

    ara::adsfi::VehicleSensorControl sensor_ctl;
    ara::adsfi::FaultData            fault_data;
    if (_expiry_threshold_control_ms > 0) { // 会丢失第一帧
        auto double_time_last = app_common::DatetimeUtil::double_time_from_sec_nsec(_last_control_data_time);
        auto double_time_this = app_common::DatetimeUtil::double_time_from_sec_nsec(msg->header.time);
        if (fabs(double_time_this - double_time_last) * 1e3 > _expiry_threshold_control_ms * 2) {
            AINFO << "Control data expired, disable canbus.";
            _driver_ptr->disable();
        } else {
            AINFO << "Control data not expired, enable canbus.";
            _driver_ptr->enable();
        }
    }
    _last_control_data_time = msg->header.time;

    AionCanbusDataInteract::Instance()->SetVehicleActControl(*msg, _estop);
    AionCanbusDataInteract::Instance()->SetVehicleSensorControl(sensor_ctl);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        AERROR << "msg is null";
        return;
    }

    if (_expiry_threshold_fault_ms > 0) { // 会丢失第一帧
        auto double_time_last = app_common::DatetimeUtil::double_time_from_sec_nsec(_last_fault_data_time);
        auto double_time_this = app_common::DatetimeUtil::double_time_from_sec_nsec(msg->header.time);
        if (fabs(double_time_this - double_time_this) * 1e3 > _expiry_threshold_fault_ms * 2) {
            AINFO << "Fault data expired, disable canbus.";
            _driver_ptr->disable();
        } else {
            AINFO << "Fault data not expired, enable canbus.";
            _driver_ptr->enable();
        }
    }
    _last_fault_data_time = msg->header.time;

    if (msg->fault_level >= 3) {
        _estop = 1;
    } else {
        _estop = 0;
    }
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleSensorControl> &msg) {
}
