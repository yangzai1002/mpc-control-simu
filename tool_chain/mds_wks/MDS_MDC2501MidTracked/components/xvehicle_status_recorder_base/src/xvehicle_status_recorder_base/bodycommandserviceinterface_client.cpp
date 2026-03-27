/* *
 * CLASS: BodyCommandServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xvehicle_status_recorder_base/bodycommandserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/xvehicle_status_recorder_base/bodycommandserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class BodyCommandServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        BodyCommandServiceInterfaceClientPtr_ = std::make_unique<BodyCommandServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<BodyCommandServiceInterfaceClientImpl>& GetBodyCommandServiceInterfaceClient()
    {
        return BodyCommandServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<BodyCommandServiceInterfaceClientImpl> BodyCommandServiceInterfaceClientPtr_;
};

BodyCommandServiceInterfaceClient::BodyCommandServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<BodyCommandServiceInterfaceClient::Impl>(instanceId);
}

BodyCommandServiceInterfaceClient::~BodyCommandServiceInterfaceClient()
{}

bool BodyCommandServiceInterfaceClient::Init()
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->Init();
}

void BodyCommandServiceInterfaceClient::Stop()
{
    pImpl_->GetBodyCommandServiceInterfaceClient()->Stop();
}

/* event relative */
void BodyCommandServiceInterfaceClient::RegisterEventVehicleActControlNotifyHandler(const EventVehicleActControlHandlerType handler)
{
    pImpl_->GetBodyCommandServiceInterfaceClient()->RegisterEventVehicleActControlNotifyHandler(handler);
}
void BodyCommandServiceInterfaceClient::EventVehicleActControlNotify()
{
    pImpl_->GetBodyCommandServiceInterfaceClient()->EventVehicleActControlNotify();
}
void BodyCommandServiceInterfaceClient::EventVehicleActControlContainerClear()
{
    pImpl_->GetBodyCommandServiceInterfaceClient()->EventVehicleActControlContainerClear();
}
bool BodyCommandServiceInterfaceClient::EventVehicleActControlContainerEmpty()
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->EventVehicleActControlContainerEmpty();
}
RecvEventVehicleActControlType BodyCommandServiceInterfaceClient::GetEventVehicleActControlOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->GetEventVehicleActControlOneData(freshDataTime);
}
RecvEventVehicleActControlType BodyCommandServiceInterfaceClient::GetEventVehicleActControlOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->GetEventVehicleActControlOneDataBlocking(blockTimeout);
}
std::vector<RecvEventVehicleActControlType> BodyCommandServiceInterfaceClient::GetEventVehicleActControlNdata(const size_t n)
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->GetEventVehicleActControlNdata(n);
}

uint32_t BodyCommandServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->GetRecvQSize();
}
bool BodyCommandServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->IsStop();
}
uint32_t BodyCommandServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetBodyCommandServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
