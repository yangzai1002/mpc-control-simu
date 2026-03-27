#include <fmt/format.h>

#include "adsfi_interface.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"

#define ERRORCODE_CONFIG_READ_ERROR "1052001" // 配置参数读取失败

AdsfiInterface::AdsfiInterface() {
    std::string can_name;
    auto        ptr = CustomStack::Instance();
    std::string full_path = "auto/canbus/receive/can_device";
    if (!ptr->GetProjectConfigValue(full_path, can_name) || can_name.empty()) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(ERRORCODE_CONFIG_READ_ERROR, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        AERROR << error;
        throw std::runtime_error(error);
    }
    AINFO << full_path << ": " << can_name;

    FaultHandle::FaultApi::Instance()->ResetFaultCode(ERRORCODE_CONFIG_READ_ERROR);

    _driver_ptr = std::make_shared<G318CanbusRecvData>();
    _driver_ptr->config.can_name = can_name;
    _driver_ptr->Init();
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    // avos::common::AvosNode::Init("G318CanbusRecv", true, "/opt/usr/zxz/log/", "/opt/usr/zxz/log/", "/opt/usr/zxz/log/");
    SetScheduleInfo("timmer", 20);
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    // ara::adsfi::SensorWheelSpeed wheel_speed;
    G318CanbusRecvDataInteract::Instance()->GetVehicleInformation(*msg);
    AINFO << " drv_mode: " << (std::int32_t)msg->vehicle_act_state.drive_mode;
    return 0;
}
