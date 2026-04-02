/* *
 * CLASS: BodyCommandServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/control_base/bodycommandserviceinterface_server.h"
#include "mdc/control_base/bodycommandserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class BodyCommandServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        BodyCommandServiceInterfaceServerPtr_ = std::make_unique<BodyCommandServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<BodyCommandServiceInterfaceServerImpl>& GetBodyCommandServiceInterfaceServer()
    {
        return BodyCommandServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<BodyCommandServiceInterfaceServerImpl> BodyCommandServiceInterfaceServerPtr_;
};

BodyCommandServiceInterfaceServer::BodyCommandServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<BodyCommandServiceInterfaceServer::Impl>(id);
}

BodyCommandServiceInterfaceServer::~BodyCommandServiceInterfaceServer()
{}

bool BodyCommandServiceInterfaceServer::Init()
{
    return pImpl_->GetBodyCommandServiceInterfaceServer()->Init();
}

void BodyCommandServiceInterfaceServer::Stop()
{
    pImpl_->GetBodyCommandServiceInterfaceServer()->Stop();
}

bool BodyCommandServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetBodyCommandServiceInterfaceServer()->IsStop();
}

uint32_t BodyCommandServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetBodyCommandServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool BodyCommandServiceInterfaceServer::SendEventVehicleActControlData(const SendEventVehicleActControlType& data)
{
    return pImpl_->GetBodyCommandServiceInterfaceServer()->SendEventVehicleActControlData(data);
}
void BodyCommandServiceInterfaceServer::ClearEventVehicleActControlContainer()
{
    pImpl_->GetBodyCommandServiceInterfaceServer()->ClearEventVehicleActControlContainer();
}

} /* namespace adsfi */
} /* namespace ara */
