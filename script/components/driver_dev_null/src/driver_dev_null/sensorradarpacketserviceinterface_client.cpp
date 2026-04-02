/* *
 * CLASS: SensorRadarPacketServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/driver_dev_null/sensorradarpacketserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/driver_dev_null/sensorradarpacketserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorRadarPacketServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorRadarPacketServiceInterfaceClientPtr_ = std::make_unique<SensorRadarPacketServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorRadarPacketServiceInterfaceClientImpl>& GetSensorRadarPacketServiceInterfaceClient()
    {
        return SensorRadarPacketServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorRadarPacketServiceInterfaceClientImpl> SensorRadarPacketServiceInterfaceClientPtr_;
};

SensorRadarPacketServiceInterfaceClient::SensorRadarPacketServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorRadarPacketServiceInterfaceClient::Impl>(instanceId);
}

SensorRadarPacketServiceInterfaceClient::~SensorRadarPacketServiceInterfaceClient()
{}

bool SensorRadarPacketServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->Init();
}

void SensorRadarPacketServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorRadarPacketServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorRadarPacketServiceInterfaceClient::RegisterEventSensorRadarPacketNotifyHandler(const EventSensorRadarPacketHandlerType handler)
{
    pImpl_->GetSensorRadarPacketServiceInterfaceClient()->RegisterEventSensorRadarPacketNotifyHandler(handler);
}
void SensorRadarPacketServiceInterfaceClient::EventSensorRadarPacketNotify()
{
    pImpl_->GetSensorRadarPacketServiceInterfaceClient()->EventSensorRadarPacketNotify();
}
void SensorRadarPacketServiceInterfaceClient::EventSensorRadarPacketContainerClear()
{
    pImpl_->GetSensorRadarPacketServiceInterfaceClient()->EventSensorRadarPacketContainerClear();
}
bool SensorRadarPacketServiceInterfaceClient::EventSensorRadarPacketContainerEmpty()
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->EventSensorRadarPacketContainerEmpty();
}
RecvEventSensorRadarPacketType SensorRadarPacketServiceInterfaceClient::GetEventSensorRadarPacketOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->GetEventSensorRadarPacketOneData(freshDataTime);
}
RecvEventSensorRadarPacketType SensorRadarPacketServiceInterfaceClient::GetEventSensorRadarPacketOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->GetEventSensorRadarPacketOneDataBlocking(blockTimeout);
}
std::vector<RecvEventSensorRadarPacketType> SensorRadarPacketServiceInterfaceClient::GetEventSensorRadarPacketNdata(const size_t n)
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->GetEventSensorRadarPacketNdata(n);
}

uint32_t SensorRadarPacketServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->GetRecvQSize();
}
bool SensorRadarPacketServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->IsStop();
}
uint32_t SensorRadarPacketServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorRadarPacketServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
