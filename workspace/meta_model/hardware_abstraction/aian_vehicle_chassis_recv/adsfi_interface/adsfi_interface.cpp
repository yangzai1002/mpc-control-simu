#include <fmt/format.h>

#include "adsfi_interface.h"
#include "monitor/faulthandle_api.hpp"

#define ERRORCODE_CONFIG_READ_ERROR "1052001" // 配置参数读取失败

AdsfiInterface::AdsfiInterface() {
    std::string can_name;
    auto        ptr = CustomStack::Instance();
    auto        full_name = std::string("auto/canbus/receive/can_device");
    if (!ptr->GetProjectConfigValue(full_name, can_name) || can_name.empty()) {
        FaultHandle::FaultApi::Instance()->SetFaultCode(ERRORCODE_CONFIG_READ_ERROR, full_name);
        auto error = fmt::format("read project config {} failed", full_name);
        AERROR << error;
        throw std::runtime_error(error);
    }
    AINFO << full_name << ": " << can_name;
    FaultHandle::FaultApi::Instance()->ResetFaultCode(ERRORCODE_CONFIG_READ_ERROR);

    _driver_ptr = std::make_shared<AionCanbusRecvData>();
    _driver_ptr->config.can_name = can_name;
    _driver_ptr->Init();
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 20);
    // avos::common::AvosNode::Init("AionCanbusReceive", true, "/opt/usr/zxz/log/", "/opt/usr/zxz/log/",
    //                              "/opt/usr/zxz/log/");
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    if (__glibc_unlikely(nullptr == msg)) {
        AERROR << "msg is null";
        return -1;
    }
    CanbusRecvDataInteract::Instance()->GetVehicleInformation(*msg);
    return 0;
}
