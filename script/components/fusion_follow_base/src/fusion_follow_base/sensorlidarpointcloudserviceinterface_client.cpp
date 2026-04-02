/* *
 * CLASS: SensorLidarPointCloudServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/sensorlidarpointcloudserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/fusion_follow_base/sensorlidarpointcloudserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class SensorLidarPointCloudServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        SensorLidarPointCloudServiceInterfaceClientPtr_ = std::make_unique<SensorLidarPointCloudServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<SensorLidarPointCloudServiceInterfaceClientImpl>& GetSensorLidarPointCloudServiceInterfaceClient()
    {
        return SensorLidarPointCloudServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<SensorLidarPointCloudServiceInterfaceClientImpl> SensorLidarPointCloudServiceInterfaceClientPtr_;
};

SensorLidarPointCloudServiceInterfaceClient::SensorLidarPointCloudServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<SensorLidarPointCloudServiceInterfaceClient::Impl>(instanceId);
}

SensorLidarPointCloudServiceInterfaceClient::~SensorLidarPointCloudServiceInterfaceClient()
{}

bool SensorLidarPointCloudServiceInterfaceClient::Init()
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->Init();
}

void SensorLidarPointCloudServiceInterfaceClient::Stop()
{
    pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->Stop();
}

/* event relative */
void SensorLidarPointCloudServiceInterfaceClient::RegisterEventPointCloud2NotifyHandler(const EventPointCloud2HandlerType handler)
{
    pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->RegisterEventPointCloud2NotifyHandler(handler);
}
void SensorLidarPointCloudServiceInterfaceClient::EventPointCloud2Notify()
{
    pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->EventPointCloud2Notify();
}
void SensorLidarPointCloudServiceInterfaceClient::EventPointCloud2ContainerClear()
{
    pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->EventPointCloud2ContainerClear();
}
bool SensorLidarPointCloudServiceInterfaceClient::EventPointCloud2ContainerEmpty()
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->EventPointCloud2ContainerEmpty();
}
RecvEventPointCloud2Type SensorLidarPointCloudServiceInterfaceClient::GetEventPointCloud2OneData(const uint32_t freshDataTime)
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->GetEventPointCloud2OneData(freshDataTime);
}
RecvEventPointCloud2Type SensorLidarPointCloudServiceInterfaceClient::GetEventPointCloud2OneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->GetEventPointCloud2OneDataBlocking(blockTimeout);
}
std::vector<RecvEventPointCloud2Type> SensorLidarPointCloudServiceInterfaceClient::GetEventPointCloud2Ndata(const size_t n)
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->GetEventPointCloud2Ndata(n);
}

uint32_t SensorLidarPointCloudServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->GetRecvQSize();
}
bool SensorLidarPointCloudServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->IsStop();
}
uint32_t SensorLidarPointCloudServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetSensorLidarPointCloudServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
