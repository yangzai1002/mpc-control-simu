/* *
 * CLASS: FaultDataServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/mid_track_chassis_send/faultdataserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/mid_track_chassis_send/faultdataserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class FaultDataServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        FaultDataServiceInterfaceClientPtr_ = std::make_unique<FaultDataServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<FaultDataServiceInterfaceClientImpl>& GetFaultDataServiceInterfaceClient()
    {
        return FaultDataServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<FaultDataServiceInterfaceClientImpl> FaultDataServiceInterfaceClientPtr_;
};

FaultDataServiceInterfaceClient::FaultDataServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<FaultDataServiceInterfaceClient::Impl>(instanceId);
}

FaultDataServiceInterfaceClient::~FaultDataServiceInterfaceClient()
{}

bool FaultDataServiceInterfaceClient::Init()
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->Init();
}

void FaultDataServiceInterfaceClient::Stop()
{
    pImpl_->GetFaultDataServiceInterfaceClient()->Stop();
}

/* event relative */
void FaultDataServiceInterfaceClient::RegisterEventFaultDataNotifyHandler(const EventFaultDataHandlerType handler)
{
    pImpl_->GetFaultDataServiceInterfaceClient()->RegisterEventFaultDataNotifyHandler(handler);
}
void FaultDataServiceInterfaceClient::EventFaultDataNotify()
{
    pImpl_->GetFaultDataServiceInterfaceClient()->EventFaultDataNotify();
}
void FaultDataServiceInterfaceClient::EventFaultDataContainerClear()
{
    pImpl_->GetFaultDataServiceInterfaceClient()->EventFaultDataContainerClear();
}
bool FaultDataServiceInterfaceClient::EventFaultDataContainerEmpty()
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->EventFaultDataContainerEmpty();
}
RecvEventFaultDataType FaultDataServiceInterfaceClient::GetEventFaultDataOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->GetEventFaultDataOneData(freshDataTime);
}
RecvEventFaultDataType FaultDataServiceInterfaceClient::GetEventFaultDataOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->GetEventFaultDataOneDataBlocking(blockTimeout);
}
std::vector<RecvEventFaultDataType> FaultDataServiceInterfaceClient::GetEventFaultDataNdata(const size_t n)
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->GetEventFaultDataNdata(n);
}

uint32_t FaultDataServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->GetRecvQSize();
}
bool FaultDataServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->IsStop();
}
uint32_t FaultDataServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetFaultDataServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
