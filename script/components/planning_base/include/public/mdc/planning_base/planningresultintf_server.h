/* *
 * Class: PlanningResultIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_PLANNINGRESULTINTFSERVER
#define ADSFI_PLANNINGRESULTINTFSERVER
#include <memory>

#include "ara/adsfi/impl_type_msghafegotrajectory.h"





namespace adsfi {

using PlanningResultDataType = ara::adsfi::MsgHafEgoTrajectory;
using SendPlanningResultType = std::shared_ptr<PlanningResultDataType>;


class PlanningResultIntfServer final {
public:
    PlanningResultIntfServer() = delete;

    explicit PlanningResultIntfServer(const uint32_t id);

    virtual ~PlanningResultIntfServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendPlanningResultData(const SendPlanningResultType& data);
    void ClearPlanningResultContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif