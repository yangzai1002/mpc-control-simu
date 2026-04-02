/* *
 * CLASS: VehicleInfomationServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/mid_track_chassis_recv/vehicleinfomationserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
VehicleInfomationServiceInterfaceServerImpl::VehicleInfomationServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : VehicleInfomationServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

VehicleInfomationServiceInterfaceServerImpl::~VehicleInfomationServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool VehicleInfomationServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void VehicleInfomationServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void VehicleInfomationServiceInterfaceServerImpl::EventVehicleInfoNotify()
{
    sendEventVehicleInfoCv_.notify_one();
    return;
}

bool VehicleInfomationServiceInterfaceServerImpl::SendEventVehicleInfoData(const SendEventVehicleInfoType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventVehicleInfo.Allocate();
    *val = *data;
    this->EventVehicleInfo.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void VehicleInfomationServiceInterfaceServerImpl::TransferEventVehicleInfoData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventVehicleInfoMtx_);
        std::cv_status status = sendEventVehicleInfoCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventVehicleInfoContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventVehicleInfo.Allocate();
        *val = *data;
        this->EventVehicleInfo.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void VehicleInfomationServiceInterfaceServerImpl::ClearEventVehicleInfoContainer()
{
    std::lock_guard<std::mutex> lck(sendEventVehicleInfoMtx_);
    eventVehicleInfoContainer_.Clear();
    return;
}

void VehicleInfomationServiceInterfaceServerImpl::CreateEventVehicleInfoThread()
{
    eventVehicleInfoThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&VehicleInfomationServiceInterfaceServerImpl::TransferEventVehicleInfoData, this));
}

} /* namespace adsfi */
} /* namespace ara */
