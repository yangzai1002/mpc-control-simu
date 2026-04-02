/* *
 * CLASS: SensorRadarPacketServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_fr671_radar/sensorradarpacketserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorRadarPacketServiceInterfaceServerImpl::SensorRadarPacketServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorRadarPacketServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorRadarPacketServiceInterfaceServerImpl::~SensorRadarPacketServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorRadarPacketServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorRadarPacketServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorRadarPacketServiceInterfaceServerImpl::EventSensorRadarPacketNotify()
{
    sendEventSensorRadarPacketCv_.notify_one();
    return;
}

bool SensorRadarPacketServiceInterfaceServerImpl::SendEventSensorRadarPacketData(const SendEventSensorRadarPacketType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventSensorRadarPacket.Allocate();
    *val = *data;
    this->EventSensorRadarPacket.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorRadarPacketServiceInterfaceServerImpl::TransferEventSensorRadarPacketData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventSensorRadarPacketMtx_);
        std::cv_status status = sendEventSensorRadarPacketCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventSensorRadarPacketContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventSensorRadarPacket.Allocate();
        *val = *data;
        this->EventSensorRadarPacket.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorRadarPacketServiceInterfaceServerImpl::ClearEventSensorRadarPacketContainer()
{
    std::lock_guard<std::mutex> lck(sendEventSensorRadarPacketMtx_);
    eventSensorRadarPacketContainer_.Clear();
    return;
}

void SensorRadarPacketServiceInterfaceServerImpl::CreateEventSensorRadarPacketThread()
{
    eventSensorRadarPacketThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorRadarPacketServiceInterfaceServerImpl::TransferEventSensorRadarPacketData, this));
}

} /* namespace adsfi */
} /* namespace ara */
