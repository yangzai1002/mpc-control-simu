/* *
 * Class: PlanningResultIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_PLANNINGRESULTINTFCLIENT
#define ADSFI_PLANNINGRESULTINTFCLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghafegotrajectory.h"





namespace adsfi {

using PlanningResultHandlerType = std::function<void (ara::adsfi::MsgHafEgoTrajectory)>;
using PlanningResultDataType = ara::adsfi::MsgHafEgoTrajectory;
using RecvPlanningResultType = std::shared_ptr<PlanningResultDataType>;
class PlanningResultIntfClient final {
public:
    PlanningResultIntfClient() = delete;
    explicit PlanningResultIntfClient(const uint32_t instanceId);
    virtual ~PlanningResultIntfClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterPlanningResultNotifyHandler(const PlanningResultHandlerType handler);
    void PlanningResultNotify();
    void PlanningResultContainerClear();
    bool PlanningResultContainerEmpty();
    RecvPlanningResultType GetPlanningResultOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvPlanningResultType GetPlanningResultOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvPlanningResultType> GetPlanningResultNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif