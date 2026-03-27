/* *
 * Class: RemoteControlServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_REMOTECONTROLSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_REMOTECONTROLSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/remotecontrolserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/xvehicle_status_recorder_base_logger.h"
namespace ara {
namespace adsfi {
using RemoteControlServiceInterfaceProxy = ara::adsfi::proxy::RemoteControlServiceInterfaceProxy;

using EventRCAppRemoteCtlHandlerType = std::function<void (ara::adsfi::AppRemoteControl)>;
using EventRCAppRemoteCtlDataType = ara::adsfi::AppRemoteControl;
using RecvEventRCAppRemoteCtlType = std::shared_ptr<EventRCAppRemoteCtlDataType>;
namespace {
using LOG_SPACE = mdc::xvehicle_status_recorder_base::XvehicleStatusRecorderBaseLogger;
}

class RemoteControlServiceInterfaceClientImpl {
public:
    explicit RemoteControlServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~RemoteControlServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventRCAppRemoteCtlNotifyHandler(const EventRCAppRemoteCtlHandlerType handler);
    void EventRCAppRemoteCtlNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventRCAppRemoteCtlMtx_);
        EventRCAppRemoteCtlCv_.notify_all();
    }

    void EventRCAppRemoteCtlContainerClear()
    {
        EventRCAppRemoteCtlContainer_.Clear();
    }

    bool EventRCAppRemoteCtlContainerEmpty()
    {
        return EventRCAppRemoteCtlContainer_.Empty();
    }

    RecvEventRCAppRemoteCtlType GetEventRCAppRemoteCtlOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventRCAppRemoteCtlType GetEventRCAppRemoteCtlOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventRCAppRemoteCtlType> GetEventRCAppRemoteCtlNdata(const size_t n);
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

    void RemoteControlServiceInterfaceCallback(ara::com::ServiceHandleContainer<RemoteControlServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<RemoteControlServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventRCAppRemoteCtlType> EventRCAppRemoteCtlContainer_;
    std::mutex EventRCAppRemoteCtlMtx_;
    std::condition_variable EventRCAppRemoteCtlCv_;
    void PushEventRCAppRemoteCtlDataToContainer(EventRCAppRemoteCtlDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif