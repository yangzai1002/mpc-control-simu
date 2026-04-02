/* *
 * CLASS: BodyCommandServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/control_base/bodycommandserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
BodyCommandServiceInterfaceServerImpl::BodyCommandServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : BodyCommandServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

BodyCommandServiceInterfaceServerImpl::~BodyCommandServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool BodyCommandServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void BodyCommandServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void BodyCommandServiceInterfaceServerImpl::EventVehicleActControlNotify()
{
    sendEventVehicleActControlCv_.notify_one();
    return;
}

bool BodyCommandServiceInterfaceServerImpl::SendEventVehicleActControlData(const SendEventVehicleActControlType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventVehicleActControl.Allocate();
    *val = *data;
    this->EventVehicleActControl.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void BodyCommandServiceInterfaceServerImpl::TransferEventVehicleActControlData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventVehicleActControlMtx_);
        std::cv_status status = sendEventVehicleActControlCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventVehicleActControlContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventVehicleActControl.Allocate();
        *val = *data;
        this->EventVehicleActControl.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void BodyCommandServiceInterfaceServerImpl::ClearEventVehicleActControlContainer()
{
    std::lock_guard<std::mutex> lck(sendEventVehicleActControlMtx_);
    eventVehicleActControlContainer_.Clear();
    return;
}

void BodyCommandServiceInterfaceServerImpl::CreateEventVehicleActControlThread()
{
    eventVehicleActControlThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&BodyCommandServiceInterfaceServerImpl::TransferEventVehicleActControlData, this));
}

} /* namespace adsfi */
} /* namespace ara */
