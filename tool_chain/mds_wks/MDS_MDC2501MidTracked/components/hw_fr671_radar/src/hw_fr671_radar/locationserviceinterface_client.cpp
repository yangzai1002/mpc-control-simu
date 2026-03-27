/* *
 * CLASS: LocationServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_fr671_radar/locationserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/hw_fr671_radar/locationserviceinterface_client_impl.h"

namespace mdc {
namespace location {
class LocationServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        LocationServiceInterfaceClientPtr_ = std::make_unique<LocationServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<LocationServiceInterfaceClientImpl>& GetLocationServiceInterfaceClient()
    {
        return LocationServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<LocationServiceInterfaceClientImpl> LocationServiceInterfaceClientPtr_;
};

LocationServiceInterfaceClient::LocationServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<LocationServiceInterfaceClient::Impl>(instanceId);
}

LocationServiceInterfaceClient::~LocationServiceInterfaceClient()
{}

bool LocationServiceInterfaceClient::Init()
{
    return pImpl_->GetLocationServiceInterfaceClient()->Init();
}

void LocationServiceInterfaceClient::Stop()
{
    pImpl_->GetLocationServiceInterfaceClient()->Stop();
}

/* event relative */
void LocationServiceInterfaceClient::RegisterLocationEventNotifyHandler(const LocationEventHandlerType handler)
{
    pImpl_->GetLocationServiceInterfaceClient()->RegisterLocationEventNotifyHandler(handler);
}
void LocationServiceInterfaceClient::LocationEventNotify()
{
    pImpl_->GetLocationServiceInterfaceClient()->LocationEventNotify();
}
void LocationServiceInterfaceClient::LocationEventContainerClear()
{
    pImpl_->GetLocationServiceInterfaceClient()->LocationEventContainerClear();
}
bool LocationServiceInterfaceClient::LocationEventContainerEmpty()
{
    return pImpl_->GetLocationServiceInterfaceClient()->LocationEventContainerEmpty();
}
RecvLocationEventType LocationServiceInterfaceClient::GetLocationEventOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetLocationServiceInterfaceClient()->GetLocationEventOneData(freshDataTime);
}
RecvLocationEventType LocationServiceInterfaceClient::GetLocationEventOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetLocationServiceInterfaceClient()->GetLocationEventOneDataBlocking(blockTimeout);
}
std::vector<RecvLocationEventType> LocationServiceInterfaceClient::GetLocationEventNdata(const size_t n)
{
    return pImpl_->GetLocationServiceInterfaceClient()->GetLocationEventNdata(n);
}

uint32_t LocationServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetLocationServiceInterfaceClient()->GetRecvQSize();
}
bool LocationServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetLocationServiceInterfaceClient()->IsStop();
}
uint32_t LocationServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetLocationServiceInterfaceClient()->GetInstanceId();
}



} /* namespace location */
} /* namespace mdc */
