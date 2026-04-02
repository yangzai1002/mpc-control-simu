/* *
 * Class: SensorRadarPacketServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorradarpacketserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/driver_dev_null_logger.h"
namespace ara {
namespace adsfi {
using SensorRadarPacketServiceInterfaceProxy = ara::adsfi::proxy::SensorRadarPacketServiceInterfaceProxy;

using EventSensorRadarPacketHandlerType = std::function<void (ara::adsfi::SensorRadarPacket)>;
using EventSensorRadarPacketDataType = ara::adsfi::SensorRadarPacket;
using RecvEventSensorRadarPacketType = std::shared_ptr<EventSensorRadarPacketDataType>;
namespace {
using LOG_SPACE = mdc::driver_dev_null::DriverDevNullLogger;
}

class SensorRadarPacketServiceInterfaceClientImpl {
public:
    explicit SensorRadarPacketServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorRadarPacketServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventSensorRadarPacketNotifyHandler(const EventSensorRadarPacketHandlerType handler);
    void EventSensorRadarPacketNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventSensorRadarPacketMtx_);
        EventSensorRadarPacketCv_.notify_all();
    }

    void EventSensorRadarPacketContainerClear()
    {
        EventSensorRadarPacketContainer_.Clear();
    }

    bool EventSensorRadarPacketContainerEmpty()
    {
        return EventSensorRadarPacketContainer_.Empty();
    }

    RecvEventSensorRadarPacketType GetEventSensorRadarPacketOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventSensorRadarPacketType GetEventSensorRadarPacketOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventSensorRadarPacketType> GetEventSensorRadarPacketNdata(const size_t n);
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

    void SensorRadarPacketServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorRadarPacketServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorRadarPacketServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventSensorRadarPacketType> EventSensorRadarPacketContainer_;
    std::mutex EventSensorRadarPacketMtx_;
    std::condition_variable EventSensorRadarPacketCv_;
    void PushEventSensorRadarPacketDataToContainer(EventSensorRadarPacketDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif