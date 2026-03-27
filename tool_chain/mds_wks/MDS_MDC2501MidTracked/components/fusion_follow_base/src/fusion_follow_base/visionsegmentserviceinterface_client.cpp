/* *
 * CLASS: VisionSegmentServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/fusion_follow_base/visionsegmentserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/fusion_follow_base/visionsegmentserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class VisionSegmentServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        VisionSegmentServiceInterfaceClientPtr_ = std::make_unique<VisionSegmentServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<VisionSegmentServiceInterfaceClientImpl>& GetVisionSegmentServiceInterfaceClient()
    {
        return VisionSegmentServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<VisionSegmentServiceInterfaceClientImpl> VisionSegmentServiceInterfaceClientPtr_;
};

VisionSegmentServiceInterfaceClient::VisionSegmentServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<VisionSegmentServiceInterfaceClient::Impl>(instanceId);
}

VisionSegmentServiceInterfaceClient::~VisionSegmentServiceInterfaceClient()
{}

bool VisionSegmentServiceInterfaceClient::Init()
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->Init();
}

void VisionSegmentServiceInterfaceClient::Stop()
{
    pImpl_->GetVisionSegmentServiceInterfaceClient()->Stop();
}

/* event relative */
void VisionSegmentServiceInterfaceClient::RegisterEventVisionSegmentNotifyHandler(const EventVisionSegmentHandlerType handler)
{
    pImpl_->GetVisionSegmentServiceInterfaceClient()->RegisterEventVisionSegmentNotifyHandler(handler);
}
void VisionSegmentServiceInterfaceClient::EventVisionSegmentNotify()
{
    pImpl_->GetVisionSegmentServiceInterfaceClient()->EventVisionSegmentNotify();
}
void VisionSegmentServiceInterfaceClient::EventVisionSegmentContainerClear()
{
    pImpl_->GetVisionSegmentServiceInterfaceClient()->EventVisionSegmentContainerClear();
}
bool VisionSegmentServiceInterfaceClient::EventVisionSegmentContainerEmpty()
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->EventVisionSegmentContainerEmpty();
}
RecvEventVisionSegmentType VisionSegmentServiceInterfaceClient::GetEventVisionSegmentOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->GetEventVisionSegmentOneData(freshDataTime);
}
RecvEventVisionSegmentType VisionSegmentServiceInterfaceClient::GetEventVisionSegmentOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->GetEventVisionSegmentOneDataBlocking(blockTimeout);
}
std::vector<RecvEventVisionSegmentType> VisionSegmentServiceInterfaceClient::GetEventVisionSegmentNdata(const size_t n)
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->GetEventVisionSegmentNdata(n);
}

uint32_t VisionSegmentServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->GetRecvQSize();
}
bool VisionSegmentServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->IsStop();
}
uint32_t VisionSegmentServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetVisionSegmentServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
