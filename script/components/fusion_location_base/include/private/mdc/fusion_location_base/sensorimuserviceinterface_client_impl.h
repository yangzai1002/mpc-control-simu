/* *
 * Class: SensorImuServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORIMUSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORIMUSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorimuserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/fusion_location_base_logger.h"
namespace ara {
namespace adsfi {
using SensorImuServiceInterfaceProxy = ara::adsfi::proxy::SensorImuServiceInterfaceProxy;

using EventSensorImuHandlerType = std::function<void (ara::adsfi::MsgHafIMU)>;
using EventSensorImuDataType = ara::adsfi::MsgHafIMU;
using RecvEventSensorImuType = std::shared_ptr<EventSensorImuDataType>;
namespace {
using LOG_SPACE = mdc::fusion_location_base::FusionLocationBaseLogger;
}

class SensorImuServiceInterfaceClientImpl {
public:
    explicit SensorImuServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorImuServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventSensorImuNotifyHandler(const EventSensorImuHandlerType handler);
    void EventSensorImuNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventSensorImuMtx_);
        EventSensorImuCv_.notify_all();
    }

    void EventSensorImuContainerClear()
    {
        EventSensorImuContainer_.Clear();
    }

    bool EventSensorImuContainerEmpty()
    {
        return EventSensorImuContainer_.Empty();
    }

    RecvEventSensorImuType GetEventSensorImuOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventSensorImuType GetEventSensorImuOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventSensorImuType> GetEventSensorImuNdata(const size_t n);
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

    void SensorImuServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorImuServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorImuServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventSensorImuType> EventSensorImuContainer_;
    std::mutex EventSensorImuMtx_;
    std::condition_variable EventSensorImuCv_;
    void PushEventSensorImuDataToContainer(EventSensorImuDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif