/* *
 * Class: PlanningResultIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_PLANNINGRESULTINTFSERVERIMPL
#define ADSFI_PLANNINGRESULTINTFSERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/planning_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "adsfi/planningresultintf_skeleton.h"

namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using adsfi::skeleton::PlanningResultIntfSkeleton;
using PlanningResultDataType = ara::adsfi::MsgHafEgoTrajectory;
using SendPlanningResultType = std::shared_ptr<PlanningResultDataType>;

namespace {
using LOG_SPACE = mdc::planning_base::PlanningBaseLogger;
}

class PlanningResultIntfServerImpl : public PlanningResultIntfSkeleton {
public:
    explicit PlanningResultIntfServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~PlanningResultIntfServerImpl();

    bool Init();

    void Stop();

    inline bool IsStop() const
    {
        return !workFlag_;
    }

    inline uint32_t GetInstanceId() const
    {
        return instanceId_;
    }
    /* event relative */
    bool SendPlanningResultData(const SendPlanningResultType& data);
    void ClearPlanningResultContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> planningResultThreadPtr_{nullptr};
    void CreatePlanningResultThread();
    void PlanningResultNotify();
    void TransferPlanningResultData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendPlanningResultType> planningResultContainer_;
    std::mutex sendPlanningResultMtx_;
    std::condition_variable sendPlanningResultCv_;
};
} /* namespace adsfi */

#endif