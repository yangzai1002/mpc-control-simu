/* *
 * Class: PerceptionStaticEnvServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/perceptionstaticenvserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/planning_base_logger.h"
namespace ara {
namespace adsfi {
using PerceptionStaticEnvServiceInterfaceProxy = ara::adsfi::proxy::PerceptionStaticEnvServiceInterfaceProxy;

using EventPerceptionStaticEnvHandlerType = std::function<void (ara::adsfi::PerceptionStaticEnv)>;
using EventPerceptionStaticEnvDataType = ara::adsfi::PerceptionStaticEnv;
using RecvEventPerceptionStaticEnvType = std::shared_ptr<EventPerceptionStaticEnvDataType>;
namespace {
using LOG_SPACE = mdc::planning_base::PlanningBaseLogger;
}

class PerceptionStaticEnvServiceInterfaceClientImpl {
public:
    explicit PerceptionStaticEnvServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~PerceptionStaticEnvServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventPerceptionStaticEnvNotifyHandler(const EventPerceptionStaticEnvHandlerType handler);
    void EventPerceptionStaticEnvNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventPerceptionStaticEnvMtx_);
        EventPerceptionStaticEnvCv_.notify_all();
    }

    void EventPerceptionStaticEnvContainerClear()
    {
        EventPerceptionStaticEnvContainer_.Clear();
    }

    bool EventPerceptionStaticEnvContainerEmpty()
    {
        return EventPerceptionStaticEnvContainer_.Empty();
    }

    RecvEventPerceptionStaticEnvType GetEventPerceptionStaticEnvOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventPerceptionStaticEnvType GetEventPerceptionStaticEnvOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventPerceptionStaticEnvType> GetEventPerceptionStaticEnvNdata(const size_t n);
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

    void PerceptionStaticEnvServiceInterfaceCallback(ara::com::ServiceHandleContainer<PerceptionStaticEnvServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<PerceptionStaticEnvServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventPerceptionStaticEnvType> EventPerceptionStaticEnvContainer_;
    std::mutex EventPerceptionStaticEnvMtx_;
    std::condition_variable EventPerceptionStaticEnvCv_;
    void PushEventPerceptionStaticEnvDataToContainer(EventPerceptionStaticEnvDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif