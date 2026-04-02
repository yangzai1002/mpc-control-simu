/* *
 * Class: SensorGpsServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORGPSSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORGPSSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorgpsserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/odometry_base_logger.h"
namespace ara {
namespace adsfi {
using SensorGpsServiceInterfaceProxy = ara::adsfi::proxy::SensorGpsServiceInterfaceProxy;

using EventGpsHandlerType = std::function<void (ara::adsfi::MsgHafGnssInfo)>;
using EventGpsDataType = ara::adsfi::MsgHafGnssInfo;
using RecvEventGpsType = std::shared_ptr<EventGpsDataType>;
namespace {
using LOG_SPACE = mdc::odometry_base::OdometryBaseLogger;
}

class SensorGpsServiceInterfaceClientImpl {
public:
    explicit SensorGpsServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorGpsServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventGpsNotifyHandler(const EventGpsHandlerType handler);
    void EventGpsNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventGpsMtx_);
        EventGpsCv_.notify_all();
    }

    void EventGpsContainerClear()
    {
        EventGpsContainer_.Clear();
    }

    bool EventGpsContainerEmpty()
    {
        return EventGpsContainer_.Empty();
    }

    RecvEventGpsType GetEventGpsOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventGpsType GetEventGpsOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventGpsType> GetEventGpsNdata(const size_t n);
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

    void SensorGpsServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorGpsServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorGpsServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventGpsType> EventGpsContainer_;
    std::mutex EventGpsMtx_;
    std::condition_variable EventGpsCv_;
    void PushEventGpsDataToContainer(EventGpsDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif