/* *
 * Class: Object3dArrayIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_OBJECT3DARRAYINTFCLIENT_IMPL
#define ADSFI_OBJECT3DARRAYINTFCLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "adsfi/object3darrayintf_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/fusion_follow_base_logger.h"
namespace adsfi {
using Object3dArrayIntfProxy = adsfi::proxy::Object3dArrayIntfProxy;

using EventPerceptionLidarObjectsHandlerType = std::function<void (ara::adsfi::MsgHaf3dDetectionOutArray)>;
using EventPerceptionLidarObjectsDataType = ara::adsfi::MsgHaf3dDetectionOutArray;
using RecvEventPerceptionLidarObjectsType = std::shared_ptr<EventPerceptionLidarObjectsDataType>;
namespace {
using LOG_SPACE = mdc::fusion_follow_base::FusionFollowBaseLogger;
}

class Object3dArrayIntfClientImpl {
public:
    explicit Object3dArrayIntfClientImpl(const uint32_t instanceId);
    virtual ~Object3dArrayIntfClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventPerceptionLidarObjectsNotifyHandler(const EventPerceptionLidarObjectsHandlerType handler);
    void EventPerceptionLidarObjectsNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventPerceptionLidarObjectsMtx_);
        EventPerceptionLidarObjectsCv_.notify_all();
    }

    void EventPerceptionLidarObjectsContainerClear()
    {
        EventPerceptionLidarObjectsContainer_.Clear();
    }

    bool EventPerceptionLidarObjectsContainerEmpty()
    {
        return EventPerceptionLidarObjectsContainer_.Empty();
    }

    RecvEventPerceptionLidarObjectsType GetEventPerceptionLidarObjectsOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventPerceptionLidarObjectsType GetEventPerceptionLidarObjectsOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventPerceptionLidarObjectsType> GetEventPerceptionLidarObjectsNdata(const size_t n);
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

    void Object3dArrayIntfCallback(ara::com::ServiceHandleContainer<Object3dArrayIntfProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<Object3dArrayIntfProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventPerceptionLidarObjectsType> EventPerceptionLidarObjectsContainer_;
    std::mutex EventPerceptionLidarObjectsMtx_;
    std::condition_variable EventPerceptionLidarObjectsCv_;
    void PushEventPerceptionLidarObjectsDataToContainer(EventPerceptionLidarObjectsDataType&& data);
};
} /* namespace adsfi */
#endif