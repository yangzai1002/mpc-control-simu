/* *
 * CLASS: SensorLidarPacketServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_l106_lidar/sensorlidarpacketserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorLidarPacketServiceInterfaceServerImpl::SensorLidarPacketServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorLidarPacketServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorLidarPacketServiceInterfaceServerImpl::~SensorLidarPacketServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorLidarPacketServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorLidarPacketServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorLidarPacketServiceInterfaceServerImpl::EventLidarPacketNotify()
{
    sendEventLidarPacketCv_.notify_one();
    return;
}

bool SensorLidarPacketServiceInterfaceServerImpl::SendEventLidarPacketData(const SendEventLidarPacketType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventLidarPacket.Allocate();
    *val = *data;
    this->EventLidarPacket.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorLidarPacketServiceInterfaceServerImpl::TransferEventLidarPacketData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventLidarPacketMtx_);
        std::cv_status status = sendEventLidarPacketCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventLidarPacketContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventLidarPacket.Allocate();
        *val = *data;
        this->EventLidarPacket.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorLidarPacketServiceInterfaceServerImpl::ClearEventLidarPacketContainer()
{
    std::lock_guard<std::mutex> lck(sendEventLidarPacketMtx_);
    eventLidarPacketContainer_.Clear();
    return;
}

void SensorLidarPacketServiceInterfaceServerImpl::CreateEventLidarPacketThread()
{
    eventLidarPacketThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorLidarPacketServiceInterfaceServerImpl::TransferEventLidarPacketData, this));
}

} /* namespace adsfi */
} /* namespace ara */
