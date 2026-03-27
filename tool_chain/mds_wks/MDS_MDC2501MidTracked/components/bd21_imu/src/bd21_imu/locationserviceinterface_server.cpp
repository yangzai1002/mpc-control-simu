/* *
 * CLASS: LocationServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/locationserviceinterface_server.h"
#include "mdc/bd21_imu/locationserviceinterface_server_impl.h"

namespace mdc {
namespace location {
class LocationServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        LocationServiceInterfaceServerPtr_ = std::make_unique<LocationServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<LocationServiceInterfaceServerImpl>& GetLocationServiceInterfaceServer()
    {
        return LocationServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<LocationServiceInterfaceServerImpl> LocationServiceInterfaceServerPtr_;
};

LocationServiceInterfaceServer::LocationServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<LocationServiceInterfaceServer::Impl>(id);
}

LocationServiceInterfaceServer::~LocationServiceInterfaceServer()
{}

bool LocationServiceInterfaceServer::Init()
{
    return pImpl_->GetLocationServiceInterfaceServer()->Init();
}

void LocationServiceInterfaceServer::Stop()
{
    pImpl_->GetLocationServiceInterfaceServer()->Stop();
}

bool LocationServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetLocationServiceInterfaceServer()->IsStop();
}

uint32_t LocationServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetLocationServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool LocationServiceInterfaceServer::SendLocationEventData(const SendLocationEventType& data)
{
    return pImpl_->GetLocationServiceInterfaceServer()->SendLocationEventData(data);
}
void LocationServiceInterfaceServer::ClearLocationEventContainer()
{
    pImpl_->GetLocationServiceInterfaceServer()->ClearLocationEventContainer();
}

} /* namespace location */
} /* namespace mdc */
