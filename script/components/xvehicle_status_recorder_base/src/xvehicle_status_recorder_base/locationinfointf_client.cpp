/* *
 * CLASS: LocationInfoIntf client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xvehicle_status_recorder_base/locationinfointf_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/xvehicle_status_recorder_base/locationinfointf_client_impl.h"

namespace adsfi {
class LocationInfoIntfClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        LocationInfoIntfClientPtr_ = std::make_unique<LocationInfoIntfClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<LocationInfoIntfClientImpl>& GetLocationInfoIntfClient()
    {
        return LocationInfoIntfClientPtr_;
    }
private:
    std::unique_ptr<LocationInfoIntfClientImpl> LocationInfoIntfClientPtr_;
};

LocationInfoIntfClient::LocationInfoIntfClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<LocationInfoIntfClient::Impl>(instanceId);
}

LocationInfoIntfClient::~LocationInfoIntfClient()
{}

bool LocationInfoIntfClient::Init()
{
    return pImpl_->GetLocationInfoIntfClient()->Init();
}

void LocationInfoIntfClient::Stop()
{
    pImpl_->GetLocationInfoIntfClient()->Stop();
}

/* event relative */
void LocationInfoIntfClient::RegisterLocationInfoNotifyHandler(const LocationInfoHandlerType handler)
{
    pImpl_->GetLocationInfoIntfClient()->RegisterLocationInfoNotifyHandler(handler);
}
void LocationInfoIntfClient::LocationInfoNotify()
{
    pImpl_->GetLocationInfoIntfClient()->LocationInfoNotify();
}
void LocationInfoIntfClient::LocationInfoContainerClear()
{
    pImpl_->GetLocationInfoIntfClient()->LocationInfoContainerClear();
}
bool LocationInfoIntfClient::LocationInfoContainerEmpty()
{
    return pImpl_->GetLocationInfoIntfClient()->LocationInfoContainerEmpty();
}
RecvLocationInfoType LocationInfoIntfClient::GetLocationInfoOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetLocationInfoIntfClient()->GetLocationInfoOneData(freshDataTime);
}
RecvLocationInfoType LocationInfoIntfClient::GetLocationInfoOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetLocationInfoIntfClient()->GetLocationInfoOneDataBlocking(blockTimeout);
}
std::vector<RecvLocationInfoType> LocationInfoIntfClient::GetLocationInfoNdata(const size_t n)
{
    return pImpl_->GetLocationInfoIntfClient()->GetLocationInfoNdata(n);
}

uint32_t LocationInfoIntfClient::GetRecvQSize() const
{
    return pImpl_->GetLocationInfoIntfClient()->GetRecvQSize();
}
bool LocationInfoIntfClient::IsStop() const
{
    return pImpl_->GetLocationInfoIntfClient()->IsStop();
}
uint32_t LocationInfoIntfClient::GetInstanceId() const
{
    return pImpl_->GetLocationInfoIntfClient()->GetInstanceId();
}



} /* namespace adsfi */
