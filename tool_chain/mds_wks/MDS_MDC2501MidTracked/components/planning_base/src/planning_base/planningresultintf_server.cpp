/* *
 * CLASS: PlanningResultIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/planning_base/planningresultintf_server.h"
#include "mdc/planning_base/planningresultintf_server_impl.h"

namespace adsfi {
class PlanningResultIntfServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        PlanningResultIntfServerPtr_ = std::make_unique<PlanningResultIntfServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<PlanningResultIntfServerImpl>& GetPlanningResultIntfServer()
    {
        return PlanningResultIntfServerPtr_;
    }
private:
    std::unique_ptr<PlanningResultIntfServerImpl> PlanningResultIntfServerPtr_;
};

PlanningResultIntfServer::PlanningResultIntfServer(const uint32_t id)
{
    pImpl_ = std::make_unique<PlanningResultIntfServer::Impl>(id);
}

PlanningResultIntfServer::~PlanningResultIntfServer()
{}

bool PlanningResultIntfServer::Init()
{
    return pImpl_->GetPlanningResultIntfServer()->Init();
}

void PlanningResultIntfServer::Stop()
{
    pImpl_->GetPlanningResultIntfServer()->Stop();
}

bool PlanningResultIntfServer::IsStop() const
{
    return pImpl_->GetPlanningResultIntfServer()->IsStop();
}

uint32_t PlanningResultIntfServer::GetInstanceId() const
{
    return pImpl_->GetPlanningResultIntfServer()->GetInstanceId();
}

/* event relative */
bool PlanningResultIntfServer::SendPlanningResultData(const SendPlanningResultType& data)
{
    return pImpl_->GetPlanningResultIntfServer()->SendPlanningResultData(data);
}
void PlanningResultIntfServer::ClearPlanningResultContainer()
{
    pImpl_->GetPlanningResultIntfServer()->ClearPlanningResultContainer();
}

} /* namespace adsfi */
