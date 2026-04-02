/* *
 * CLASS: SensorWheelSpeedServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/lidar_slam_base/sensorwheelspeedserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/lidar_slam_base/sensorwheelspeedserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorWheelSpeedServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorWheelSpeedServiceInterfaceClientPtr_ = std::make_unique<SensorWheelSpeedServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorWheelSpeedServiceInterfaceClientImpl>& GetSensorWheelSpeedServiceInterfaceClient()
    {
        return SensorWheelSpeedServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorWheelSpeedServiceInterfaceClientImpl> SensorWheelSpeedServiceInterfaceClientPtr_;
};

SensorWheelSpeedServiceInterfaceClient::SensorWheelSpeedServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorWheelSpeedServiceInterfaceClient::Impl>(instanceId);
}

SensorWheelSpeedServiceInterfaceClient::~SensorWheelSpeedServiceInterfaceClient()
{}

bool SensorWheelSpeedServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->Init();
}

void SensorWheelSpeedServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorWheelSpeedServiceInterfaceClient::RegisterEventWheelSpeedNotifyHandler(const EventWheelSpeedHandlerType handler)
{
    pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->RegisterEventWheelSpeedNotifyHandler(handler);
}
void SensorWheelSpeedServiceInterfaceClient::EventWheelSpeedNotify()
{
    pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->EventWheelSpeedNotify();
}
void SensorWheelSpeedServiceInterfaceClient::EventWheelSpeedContainerClear()
{
    pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->EventWheelSpeedContainerClear();
}
bool SensorWheelSpeedServiceInterfaceClient::EventWheelSpeedContainerEmpty()
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->EventWheelSpeedContainerEmpty();
}
RecvEventWheelSpeedType SensorWheelSpeedServiceInterfaceClient::GetEventWheelSpeedOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->GetEventWheelSpeedOneData(freshDataTime);
}
RecvEventWheelSpeedType SensorWheelSpeedServiceInterfaceClient::GetEventWheelSpeedOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->GetEventWheelSpeedOneDataBlocking(blockTimeout);
}
std::vector<RecvEventWheelSpeedType> SensorWheelSpeedServiceInterfaceClient::GetEventWheelSpeedNdata(const size_t n)
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->GetEventWheelSpeedNdata(n);
}

uint32_t SensorWheelSpeedServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->GetRecvQSize();
}
bool SensorWheelSpeedServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->IsStop();
}
uint32_t SensorWheelSpeedServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorWheelSpeedServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
