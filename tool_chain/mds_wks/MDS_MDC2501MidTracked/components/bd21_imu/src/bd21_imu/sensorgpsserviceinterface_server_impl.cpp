/* *
 * CLASS: SensorGpsServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/sensorgpsserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorGpsServiceInterfaceServerImpl::SensorGpsServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorGpsServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorGpsServiceInterfaceServerImpl::~SensorGpsServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorGpsServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorGpsServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorGpsServiceInterfaceServerImpl::EventGpsNotify()
{
    sendEventGpsCv_.notify_one();
    return;
}

bool SensorGpsServiceInterfaceServerImpl::SendEventGpsData(const SendEventGpsType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventGps.Allocate();
    *val = *data;
    this->EventGps.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorGpsServiceInterfaceServerImpl::TransferEventGpsData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventGpsMtx_);
        std::cv_status status = sendEventGpsCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventGpsContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventGps.Allocate();
        *val = *data;
        this->EventGps.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorGpsServiceInterfaceServerImpl::ClearEventGpsContainer()
{
    std::lock_guard<std::mutex> lck(sendEventGpsMtx_);
    eventGpsContainer_.Clear();
    return;
}

void SensorGpsServiceInterfaceServerImpl::CreateEventGpsThread()
{
    eventGpsThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorGpsServiceInterfaceServerImpl::TransferEventGpsData, this));
}

} /* namespace adsfi */
} /* namespace ara */
