/* *
 * Class: FusionObjectArrayIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_FUSIONOBJECTARRAYINTFCLIENT_IMPL
#define ADSFI_FUSIONOBJECTARRAYINTFCLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "adsfi/fusionobjectarrayintf_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/planning_base_logger.h"
namespace adsfi {
using FusionObjectArrayIntfProxy = adsfi::proxy::FusionObjectArrayIntfProxy;

using FusionObjectArrayHandlerType = std::function<void (ara::adsfi::MsgHafFusionOutArray)>;
using FusionObjectArrayDataType = ara::adsfi::MsgHafFusionOutArray;
using RecvFusionObjectArrayType = std::shared_ptr<FusionObjectArrayDataType>;
namespace {
using LOG_SPACE = mdc::planning_base::PlanningBaseLogger;
}

class FusionObjectArrayIntfClientImpl {
public:
    explicit FusionObjectArrayIntfClientImpl(const uint32_t instanceId);
    virtual ~FusionObjectArrayIntfClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterFusionObjectArrayNotifyHandler(const FusionObjectArrayHandlerType handler);
    void FusionObjectArrayNotify()
    {
        std::lock_guard<std::mutex> recvLk(FusionObjectArrayMtx_);
        FusionObjectArrayCv_.notify_all();
    }

    void FusionObjectArrayContainerClear()
    {
        FusionObjectArrayContainer_.Clear();
    }

    bool FusionObjectArrayContainerEmpty()
    {
        return FusionObjectArrayContainer_.Empty();
    }

    RecvFusionObjectArrayType GetFusionObjectArrayOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvFusionObjectArrayType GetFusionObjectArrayOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvFusionObjectArrayType> GetFusionObjectArrayNdata(const size_t n);
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

    void FusionObjectArrayIntfCallback(ara::com::ServiceHandleContainer<FusionObjectArrayIntfProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<FusionObjectArrayIntfProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvFusionObjectArrayType> FusionObjectArrayContainer_;
    std::mutex FusionObjectArrayMtx_;
    std::condition_variable FusionObjectArrayCv_;
    void PushFusionObjectArrayDataToContainer(FusionObjectArrayDataType&& data);
};
} /* namespace adsfi */
#endif