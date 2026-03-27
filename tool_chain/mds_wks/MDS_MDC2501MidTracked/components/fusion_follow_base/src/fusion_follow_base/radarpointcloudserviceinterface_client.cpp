/* *
 * CLASS: RadarPointCloudServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/radarpointcloudserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/fusion_follow_base/radarpointcloudserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class RadarPointCloudServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        RadarPointCloudServiceInterfaceClientPtr_ = std::make_unique<RadarPointCloudServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<RadarPointCloudServiceInterfaceClientImpl>& GetRadarPointCloudServiceInterfaceClient()
    {
        return RadarPointCloudServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<RadarPointCloudServiceInterfaceClientImpl> RadarPointCloudServiceInterfaceClientPtr_;
};

RadarPointCloudServiceInterfaceClient::RadarPointCloudServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<RadarPointCloudServiceInterfaceClient::Impl>(instanceId);
}

RadarPointCloudServiceInterfaceClient::~RadarPointCloudServiceInterfaceClient()
{}

bool RadarPointCloudServiceInterfaceClient::Init()
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->Init();
}

void RadarPointCloudServiceInterfaceClient::Stop()
{
    pImpl_->GetRadarPointCloudServiceInterfaceClient()->Stop();
}

/* event relative */
void RadarPointCloudServiceInterfaceClient::RegisterEventRadarPointCloudNotifyHandler(const EventRadarPointCloudHandlerType handler)
{
    pImpl_->GetRadarPointCloudServiceInterfaceClient()->RegisterEventRadarPointCloudNotifyHandler(handler);
}
void RadarPointCloudServiceInterfaceClient::EventRadarPointCloudNotify()
{
    pImpl_->GetRadarPointCloudServiceInterfaceClient()->EventRadarPointCloudNotify();
}
void RadarPointCloudServiceInterfaceClient::EventRadarPointCloudContainerClear()
{
    pImpl_->GetRadarPointCloudServiceInterfaceClient()->EventRadarPointCloudContainerClear();
}
bool RadarPointCloudServiceInterfaceClient::EventRadarPointCloudContainerEmpty()
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->EventRadarPointCloudContainerEmpty();
}
RecvEventRadarPointCloudType RadarPointCloudServiceInterfaceClient::GetEventRadarPointCloudOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->GetEventRadarPointCloudOneData(freshDataTime);
}
RecvEventRadarPointCloudType RadarPointCloudServiceInterfaceClient::GetEventRadarPointCloudOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->GetEventRadarPointCloudOneDataBlocking(blockTimeout);
}
std::vector<RecvEventRadarPointCloudType> RadarPointCloudServiceInterfaceClient::GetEventRadarPointCloudNdata(const size_t n)
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->GetEventRadarPointCloudNdata(n);
}

uint32_t RadarPointCloudServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->GetRecvQSize();
}
bool RadarPointCloudServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->IsStop();
}
uint32_t RadarPointCloudServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetRadarPointCloudServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
