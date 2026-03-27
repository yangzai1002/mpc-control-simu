/* *
 * CLASS: SensorImuServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_location_base/sensorimuserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/fusion_location_base/sensorimuserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorImuServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorImuServiceInterfaceClientPtr_ = std::make_unique<SensorImuServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorImuServiceInterfaceClientImpl>& GetSensorImuServiceInterfaceClient()
    {
        return SensorImuServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorImuServiceInterfaceClientImpl> SensorImuServiceInterfaceClientPtr_;
};

SensorImuServiceInterfaceClient::SensorImuServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorImuServiceInterfaceClient::Impl>(instanceId);
}

SensorImuServiceInterfaceClient::~SensorImuServiceInterfaceClient()
{}

bool SensorImuServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->Init();
}

void SensorImuServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorImuServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorImuServiceInterfaceClient::RegisterEventSensorImuNotifyHandler(const EventSensorImuHandlerType handler)
{
    pImpl_->GetSensorImuServiceInterfaceClient()->RegisterEventSensorImuNotifyHandler(handler);
}
void SensorImuServiceInterfaceClient::EventSensorImuNotify()
{
    pImpl_->GetSensorImuServiceInterfaceClient()->EventSensorImuNotify();
}
void SensorImuServiceInterfaceClient::EventSensorImuContainerClear()
{
    pImpl_->GetSensorImuServiceInterfaceClient()->EventSensorImuContainerClear();
}
bool SensorImuServiceInterfaceClient::EventSensorImuContainerEmpty()
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->EventSensorImuContainerEmpty();
}
RecvEventSensorImuType SensorImuServiceInterfaceClient::GetEventSensorImuOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->GetEventSensorImuOneData(freshDataTime);
}
RecvEventSensorImuType SensorImuServiceInterfaceClient::GetEventSensorImuOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->GetEventSensorImuOneDataBlocking(blockTimeout);
}
std::vector<RecvEventSensorImuType> SensorImuServiceInterfaceClient::GetEventSensorImuNdata(const size_t n)
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->GetEventSensorImuNdata(n);
}

uint32_t SensorImuServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->GetRecvQSize();
}
bool SensorImuServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->IsStop();
}
uint32_t SensorImuServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorImuServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
