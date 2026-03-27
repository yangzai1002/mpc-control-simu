/* *
 * Class: PlanningResultIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_PLANNINGRESULTINTFCLIENT_IMPL
#define ADSFI_PLANNINGRESULTINTFCLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "adsfi/planningresultintf_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/control_base_logger.h"
namespace adsfi {
using PlanningResultIntfProxy = adsfi::proxy::PlanningResultIntfProxy;

using PlanningResultHandlerType = std::function<void (ara::adsfi::MsgHafEgoTrajectory)>;
using PlanningResultDataType = ara::adsfi::MsgHafEgoTrajectory;
using RecvPlanningResultType = std::shared_ptr<PlanningResultDataType>;
namespace {
using LOG_SPACE = mdc::control_base::ControlBaseLogger;
}

class PlanningResultIntfClientImpl {
public:
    explicit PlanningResultIntfClientImpl(const uint32_t instanceId);
    virtual ~PlanningResultIntfClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterPlanningResultNotifyHandler(const PlanningResultHandlerType handler);
    void PlanningResultNotify()
    {
        std::lock_guard<std::mutex> recvLk(PlanningResultMtx_);
        PlanningResultCv_.notify_all();
    }

    void PlanningResultContainerClear()
    {
        PlanningResultContainer_.Clear();
    }

    bool PlanningResultContainerEmpty()
    {
        return PlanningResultContainer_.Empty();
    }

    RecvPlanningResultType GetPlanningResultOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvPlanningResultType GetPlanningResultOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvPlanningResultType> GetPlanningResultNdata(const size_t n);
    uint32_t GetRecvQSize() const
    {
        return recvQSize_;
    }

    bool IsStop() const
    {
        return !workFlag_;
    }

    uint32_t GetInstanceId() const
    {
        return instanceIdx_;
    }
    
private:
    /* 服务实例ID */
    uint32_t instanceIdx_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 寻找服务标识 */
    std::atomic<bool> findServFlag_;

    /* 注册标识 */
    std::atomic<bool> registerFlag_;

    /* EM模块 */
    ara::exec::ExecutionClient execClient_ {};

    /* EM 上报标识 */
    std::atomic<bool> emReport_;

    /* 服务发现回调 避免多线程同时执行标识 */
    std::once_flag callFlag_{};

    void PlanningResultIntfCallback(ara::com::ServiceHandleContainer<PlanningResultIntfProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<PlanningResultIntfProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvPlanningResultType> PlanningResultContainer_;
    std::mutex PlanningResultMtx_;
    std::condition_variable PlanningResultCv_;
    void PushPlanningResultDataToContainer(PlanningResultDataType&& data);
};
} /* namespace adsfi */
#endif