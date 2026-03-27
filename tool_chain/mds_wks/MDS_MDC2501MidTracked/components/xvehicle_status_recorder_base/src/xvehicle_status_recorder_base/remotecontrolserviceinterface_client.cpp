/* *
 * CLASS: RemoteControlServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xvehicle_status_recorder_base/remotecontrolserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/xvehicle_status_recorder_base/remotecontrolserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class RemoteControlServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        RemoteControlServiceInterfaceClientPtr_ = std::make_unique<RemoteControlServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<RemoteControlServiceInterfaceClientImpl>& GetRemoteControlServiceInterfaceClient()
    {
        return RemoteControlServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<RemoteControlServiceInterfaceClientImpl> RemoteControlServiceInterfaceClientPtr_;
};

RemoteControlServiceInterfaceClient::RemoteControlServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<RemoteControlServiceInterfaceClient::Impl>(instanceId);
}

RemoteControlServiceInterfaceClient::~RemoteControlServiceInterfaceClient()
{}

bool RemoteControlServiceInterfaceClient::Init()
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->Init();
}

void RemoteControlServiceInterfaceClient::Stop()
{
    pImpl_->GetRemoteControlServiceInterfaceClient()->Stop();
}

/* event relative */
void RemoteControlServiceInterfaceClient::RegisterEventRCAppRemoteCtlNotifyHandler(const EventRCAppRemoteCtlHandlerType handler)
{
    pImpl_->GetRemoteControlServiceInterfaceClient()->RegisterEventRCAppRemoteCtlNotifyHandler(handler);
}
void RemoteControlServiceInterfaceClient::EventRCAppRemoteCtlNotify()
{
    pImpl_->GetRemoteControlServiceInterfaceClient()->EventRCAppRemoteCtlNotify();
}
void RemoteControlServiceInterfaceClient::EventRCAppRemoteCtlContainerClear()
{
    pImpl_->GetRemoteControlServiceInterfaceClient()->EventRCAppRemoteCtlContainerClear();
}
bool RemoteControlServiceInterfaceClient::EventRCAppRemoteCtlContainerEmpty()
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->EventRCAppRemoteCtlContainerEmpty();
}
RecvEventRCAppRemoteCtlType RemoteControlServiceInterfaceClient::GetEventRCAppRemoteCtlOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->GetEventRCAppRemoteCtlOneData(freshDataTime);
}
RecvEventRCAppRemoteCtlType RemoteControlServiceInterfaceClient::GetEventRCAppRemoteCtlOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->GetEventRCAppRemoteCtlOneDataBlocking(blockTimeout);
}
std::vector<RecvEventRCAppRemoteCtlType> RemoteControlServiceInterfaceClient::GetEventRCAppRemoteCtlNdata(const size_t n)
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->GetEventRCAppRemoteCtlNdata(n);
}

uint32_t RemoteControlServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->GetRecvQSize();
}
bool RemoteControlServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->IsStop();
}
uint32_t RemoteControlServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetRemoteControlServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
