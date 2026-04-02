/* *
 * CLASS: SensorLidarPacketServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/driver_dev_null/sensorlidarpacketserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/driver_dev_null/sensorlidarpacketserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorLidarPacketServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorLidarPacketServiceInterfaceClientPtr_ = std::make_unique<SensorLidarPacketServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorLidarPacketServiceInterfaceClientImpl>& GetSensorLidarPacketServiceInterfaceClient()
    {
        return SensorLidarPacketServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorLidarPacketServiceInterfaceClientImpl> SensorLidarPacketServiceInterfaceClientPtr_;
};

SensorLidarPacketServiceInterfaceClient::SensorLidarPacketServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorLidarPacketServiceInterfaceClient::Impl>(instanceId);
}

SensorLidarPacketServiceInterfaceClient::~SensorLidarPacketServiceInterfaceClient()
{}

bool SensorLidarPacketServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->Init();
}

void SensorLidarPacketServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorLidarPacketServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorLidarPacketServiceInterfaceClient::RegisterEventLidarPacketNotifyHandler(const EventLidarPacketHandlerType handler)
{
    pImpl_->GetSensorLidarPacketServiceInterfaceClient()->RegisterEventLidarPacketNotifyHandler(handler);
}
void SensorLidarPacketServiceInterfaceClient::EventLidarPacketNotify()
{
    pImpl_->GetSensorLidarPacketServiceInterfaceClient()->EventLidarPacketNotify();
}
void SensorLidarPacketServiceInterfaceClient::EventLidarPacketContainerClear()
{
    pImpl_->GetSensorLidarPacketServiceInterfaceClient()->EventLidarPacketContainerClear();
}
bool SensorLidarPacketServiceInterfaceClient::EventLidarPacketContainerEmpty()
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->EventLidarPacketContainerEmpty();
}
RecvEventLidarPacketType SensorLidarPacketServiceInterfaceClient::GetEventLidarPacketOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->GetEventLidarPacketOneData(freshDataTime);
}
RecvEventLidarPacketType SensorLidarPacketServiceInterfaceClient::GetEventLidarPacketOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->GetEventLidarPacketOneDataBlocking(blockTimeout);
}
std::vector<RecvEventLidarPacketType> SensorLidarPacketServiceInterfaceClient::GetEventLidarPacketNdata(const size_t n)
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->GetEventLidarPacketNdata(n);
}

uint32_t SensorLidarPacketServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->GetRecvQSize();
}
bool SensorLidarPacketServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->IsStop();
}
uint32_t SensorLidarPacketServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorLidarPacketServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
