/* *
 * CLASS: LocationInfoIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/odometry_base/locationinfointf_server.h"
#include "mdc/odometry_base/locationinfointf_server_impl.h"

namespace adsfi {
class LocationInfoIntfServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        LocationInfoIntfServerPtr_ = std::make_unique<LocationInfoIntfServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<LocationInfoIntfServerImpl>& GetLocationInfoIntfServer()
    {
        return LocationInfoIntfServerPtr_;
    }
private:
    std::unique_ptr<LocationInfoIntfServerImpl> LocationInfoIntfServerPtr_;
};

LocationInfoIntfServer::LocationInfoIntfServer(const uint32_t id)
{
    pImpl_ = std::make_unique<LocationInfoIntfServer::Impl>(id);
}

LocationInfoIntfServer::~LocationInfoIntfServer()
{}

bool LocationInfoIntfServer::Init()
{
    return pImpl_->GetLocationInfoIntfServer()->Init();
}

void LocationInfoIntfServer::Stop()
{
    pImpl_->GetLocationInfoIntfServer()->Stop();
}

bool LocationInfoIntfServer::IsStop() const
{
    return pImpl_->GetLocationInfoIntfServer()->IsStop();
}

uint32_t LocationInfoIntfServer::GetInstanceId() const
{
    return pImpl_->GetLocationInfoIntfServer()->GetInstanceId();
}

/* event relative */
bool LocationInfoIntfServer::SendLocationInfoData(const SendLocationInfoType& data)
{
    return pImpl_->GetLocationInfoIntfServer()->SendLocationInfoData(data);
}
void LocationInfoIntfServer::ClearLocationInfoContainer()
{
    pImpl_->GetLocationInfoIntfServer()->ClearLocationInfoContainer();
}

} /* namespace adsfi */
