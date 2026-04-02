/* *
 * CLASS: PlanningResultIntf client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/control_base/planningresultintf_client.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"
#include "mdc/control_base/planningresultintf_client_impl.h"

namespace adsfi {
class PlanningResultIntfClient::Impl {
public:
    Impl(const uint32_t instanceId)
    {
        PlanningResultIntfClientPtr_ = std::make_unique<PlanningResultIntfClientImpl>(instanceId);
    }
    ~Impl() {}
    const std::unique_ptr<PlanningResultIntfClientImpl>& GetPlanningResultIntfClient()
    {
        return PlanningResultIntfClientPtr_;
    }
private:
    std::unique_ptr<PlanningResultIntfClientImpl> PlanningResultIntfClientPtr_;
};

PlanningResultIntfClient::PlanningResultIntfClient(const uint32_t instanceId)
{
    pImpl_ = std::make_unique<PlanningResultIntfClient::Impl>(instanceId);
}

PlanningResultIntfClient::~PlanningResultIntfClient()
{}

bool PlanningResultIntfClient::Init()
{
    return pImpl_->GetPlanningResultIntfClient()->Init();
}

void PlanningResultIntfClient::Stop()
{
    pImpl_->GetPlanningResultIntfClient()->Stop();
}

/* event relative */
void PlanningResultIntfClient::RegisterPlanningResultNotifyHandler(const PlanningResultHandlerType handler)
{
    pImpl_->GetPlanningResultIntfClient()->RegisterPlanningResultNotifyHandler(handler);
}
void PlanningResultIntfClient::PlanningResultNotify()
{
    pImpl_->GetPlanningResultIntfClient()->PlanningResultNotify();
}
void PlanningResultIntfClient::PlanningResultContainerClear()
{
    pImpl_->GetPlanningResultIntfClient()->PlanningResultContainerClear();
}
bool PlanningResultIntfClient::PlanningResultContainerEmpty()
{
    return pImpl_->GetPlanningResultIntfClient()->PlanningResultContainerEmpty();
}
RecvPlanningResultType PlanningResultIntfClient::GetPlanningResultOneData(const uint32_t freshDataTime)
{
    return pImpl_->GetPlanningResultIntfClient()->GetPlanningResultOneData(freshDataTime);
}
RecvPlanningResultType PlanningResultIntfClient::GetPlanningResultOneDataBlocking(const uint32_t blockTimeout)
{
    return pImpl_->GetPlanningResultIntfClient()->GetPlanningResultOneDataBlocking(blockTimeout);
}
std::vector<RecvPlanningResultType> PlanningResultIntfClient::GetPlanningResultNdata(const size_t n)
{
    return pImpl_->GetPlanningResultIntfClient()->GetPlanningResultNdata(n);
}

uint32_t PlanningResultIntfClient::GetRecvQSize() const
{
    return pImpl_->GetPlanningResultIntfClient()->GetRecvQSize();
}
bool PlanningResultIntfClient::IsStop() const
{
    return pImpl_->GetPlanningResultIntfClient()->IsStop();
}
uint32_t PlanningResultIntfClient::GetInstanceId() const
{
    return pImpl_->GetPlanningResultIntfClient()->GetInstanceId();
}



} /* namespace adsfi */
