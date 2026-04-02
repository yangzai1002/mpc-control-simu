/* *
 * CLASS: SensorGpsServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/odometry_base/sensorgpsserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/odometry_base/sensorgpsserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorGpsServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorGpsServiceInterfaceClientPtr_ = std::make_unique<SensorGpsServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorGpsServiceInterfaceClientImpl>& GetSensorGpsServiceInterfaceClient()
    {
        return SensorGpsServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorGpsServiceInterfaceClientImpl> SensorGpsServiceInterfaceClientPtr_;
};

SensorGpsServiceInterfaceClient::SensorGpsServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorGpsServiceInterfaceClient::Impl>(instanceId);
}

SensorGpsServiceInterfaceClient::~SensorGpsServiceInterfaceClient()
{}

bool SensorGpsServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->Init();
}

void SensorGpsServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorGpsServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorGpsServiceInterfaceClient::RegisterEventGpsNotifyHandler(const EventGpsHandlerType handler)
{
    pImpl_->GetSensorGpsServiceInterfaceClient()->RegisterEventGpsNotifyHandler(handler);
}
void SensorGpsServiceInterfaceClient::EventGpsNotify()
{
    pImpl_->GetSensorGpsServiceInterfaceClient()->EventGpsNotify();
}
void SensorGpsServiceInterfaceClient::EventGpsContainerClear()
{
    pImpl_->GetSensorGpsServiceInterfaceClient()->EventGpsContainerClear();
}
bool SensorGpsServiceInterfaceClient::EventGpsContainerEmpty()
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->EventGpsContainerEmpty();
}
RecvEventGpsType SensorGpsServiceInterfaceClient::GetEventGpsOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->GetEventGpsOneData(freshDataTime);
}
RecvEventGpsType SensorGpsServiceInterfaceClient::GetEventGpsOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->GetEventGpsOneDataBlocking(blockTimeout);
}
std::vector<RecvEventGpsType> SensorGpsServiceInterfaceClient::GetEventGpsNdata(const size_t n)
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->GetEventGpsNdata(n);
}

uint32_t SensorGpsServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->GetRecvQSize();
}
bool SensorGpsServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->IsStop();
}
uint32_t SensorGpsServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorGpsServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
