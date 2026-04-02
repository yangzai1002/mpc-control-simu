/* *
 * CLASS: RemoteControlServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xzmq_rc/remotecontrolserviceinterface_server.h"
#include "mdc/xzmq_rc/remotecontrolserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class RemoteControlServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        RemoteControlServiceInterfaceServerPtr_ = std::make_unique<RemoteControlServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<RemoteControlServiceInterfaceServerImpl>& GetRemoteControlServiceInterfaceServer()
    {
        return RemoteControlServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<RemoteControlServiceInterfaceServerImpl> RemoteControlServiceInterfaceServerPtr_;
};

RemoteControlServiceInterfaceServer::RemoteControlServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<RemoteControlServiceInterfaceServer::Impl>(id);
}

RemoteControlServiceInterfaceServer::~RemoteControlServiceInterfaceServer()
{}

bool RemoteControlServiceInterfaceServer::Init()
{
    return pImpl_->GetRemoteControlServiceInterfaceServer()->Init();
}

void RemoteControlServiceInterfaceServer::Stop()
{
    pImpl_->GetRemoteControlServiceInterfaceServer()->Stop();
}

bool RemoteControlServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetRemoteControlServiceInterfaceServer()->IsStop();
}

uint32_t RemoteControlServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetRemoteControlServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool RemoteControlServiceInterfaceServer::SendEventRCAppRemoteCtlData(const SendEventRCAppRemoteCtlType& data)
{
    return pImpl_->GetRemoteControlServiceInterfaceServer()->SendEventRCAppRemoteCtlData(data);
}
void RemoteControlServiceInterfaceServer::ClearEventRCAppRemoteCtlContainer()
{
    pImpl_->GetRemoteControlServiceInterfaceServer()->ClearEventRCAppRemoteCtlContainer();
}

} /* namespace adsfi */
} /* namespace ara */
