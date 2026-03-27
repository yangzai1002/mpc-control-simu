/* *
 * CLASS: PerceptionStaticEnvServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/planning_base/perceptionstaticenvserviceinterface_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/planning_base/perceptionstaticenvserviceinterface_client_impl.h"

namespace ara {
namespace adsfi {
class PerceptionStaticEnvServiceInterfaceClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        PerceptionStaticEnvServiceInterfaceClientPtr_ = std::make_unique<PerceptionStaticEnvServiceInterfaceClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<PerceptionStaticEnvServiceInterfaceClientImpl>& GetPerceptionStaticEnvServiceInterfaceClient()
    {
        return PerceptionStaticEnvServiceInterfaceClientPtr_;
    }
private:
    std::unique_ptr<PerceptionStaticEnvServiceInterfaceClientImpl> PerceptionStaticEnvServiceInterfaceClientPtr_;
};

PerceptionStaticEnvServiceInterfaceClient::PerceptionStaticEnvServiceInterfaceClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<PerceptionStaticEnvServiceInterfaceClient::Impl>(instanceId);
}

PerceptionStaticEnvServiceInterfaceClient::~PerceptionStaticEnvServiceInterfaceClient()
{}

bool PerceptionStaticEnvServiceInterfaceClient::Init()
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->Init();
}

void PerceptionStaticEnvServiceInterfaceClient::Stop()
{
    pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->Stop();
}

/* event relative */
void PerceptionStaticEnvServiceInterfaceClient::RegisterEventPerceptionStaticEnvNotifyHandler(const EventPerceptionStaticEnvHandlerType handler)
{
    pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->RegisterEventPerceptionStaticEnvNotifyHandler(handler);
}
void PerceptionStaticEnvServiceInterfaceClient::EventPerceptionStaticEnvNotify()
{
    pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->EventPerceptionStaticEnvNotify();
}
void PerceptionStaticEnvServiceInterfaceClient::EventPerceptionStaticEnvContainerClear()
{
    pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->EventPerceptionStaticEnvContainerClear();
}
bool PerceptionStaticEnvServiceInterfaceClient::EventPerceptionStaticEnvContainerEmpty()
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->EventPerceptionStaticEnvContainerEmpty();
}
RecvEventPerceptionStaticEnvType PerceptionStaticEnvServiceInterfaceClient::GetEventPerceptionStaticEnvOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->GetEventPerceptionStaticEnvOneData(freshDataTime);
}
RecvEventPerceptionStaticEnvType PerceptionStaticEnvServiceInterfaceClient::GetEventPerceptionStaticEnvOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->GetEventPerceptionStaticEnvOneDataBlocking(blockTimeout);
}
std::vector<RecvEventPerceptionStaticEnvType> PerceptionStaticEnvServiceInterfaceClient::GetEventPerceptionStaticEnvNdata(const size_t n)
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->GetEventPerceptionStaticEnvNdata(n);
}

uint32_t PerceptionStaticEnvServiceInterfaceClient::GetRecvQSize() const
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->GetRecvQSize();
}
bool PerceptionStaticEnvServiceInterfaceClient::IsStop() const
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->IsStop();
}
uint32_t PerceptionStaticEnvServiceInterfaceClient::GetInstanceId() const
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceClient()->GetInstanceId();
}



} /* namespace adsfi */
} /* namespace ara */
