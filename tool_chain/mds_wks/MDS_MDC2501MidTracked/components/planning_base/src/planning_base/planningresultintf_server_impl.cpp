/* *
 * CLASS: PlanningResultIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/planning_base/planningresultintf_server_impl.h"

namespace adsfi {
PlanningResultIntfServerImpl::PlanningResultIntfServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : PlanningResultIntfSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

PlanningResultIntfServerImpl::~PlanningResultIntfServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool PlanningResultIntfServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void PlanningResultIntfServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void PlanningResultIntfServerImpl::PlanningResultNotify()
{
    sendPlanningResultCv_.notify_one();
    return;
}

bool PlanningResultIntfServerImpl::SendPlanningResultData(const SendPlanningResultType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->PlanningResult.Allocate();
    *val = *data;
    this->PlanningResult.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void PlanningResultIntfServerImpl::TransferPlanningResultData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendPlanningResultMtx_);
        std::cv_status status = sendPlanningResultCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *planningResultContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->PlanningResult.Allocate();
        *val = *data;
        this->PlanningResult.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void PlanningResultIntfServerImpl::ClearPlanningResultContainer()
{
    std::lock_guard<std::mutex> lck(sendPlanningResultMtx_);
    planningResultContainer_.Clear();
    return;
}

void PlanningResultIntfServerImpl::CreatePlanningResultThread()
{
    planningResultThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&PlanningResultIntfServerImpl::TransferPlanningResultData, this));
}

} /* namespace adsfi */
