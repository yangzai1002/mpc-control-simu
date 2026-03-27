/* *
 * Class: SensorLidarPacketServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorlidarpacketserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/driver_dev_null_logger.h"
namespace ara {
namespace adsfi {
using SensorLidarPacketServiceInterfaceProxy = ara::adsfi::proxy::SensorLidarPacketServiceInterfaceProxy;

using EventLidarPacketHandlerType = std::function<void (ara::adsfi::SensorLidarPacket)>;
using EventLidarPacketDataType = ara::adsfi::SensorLidarPacket;
using RecvEventLidarPacketType = std::shared_ptr<EventLidarPacketDataType>;
namespace {
using LOG_SPACE = mdc::driver_dev_null::DriverDevNullLogger;
}

class SensorLidarPacketServiceInterfaceClientImpl {
public:
    explicit SensorLidarPacketServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorLidarPacketServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventLidarPacketNotifyHandler(const EventLidarPacketHandlerType handler);
    void EventLidarPacketNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventLidarPacketMtx_);
        EventLidarPacketCv_.notify_all();
    }

    void EventLidarPacketContainerClear()
    {
        EventLidarPacketContainer_.Clear();
    }

    bool EventLidarPacketContainerEmpty()
    {
        return EventLidarPacketContainer_.Empty();
    }

    RecvEventLidarPacketType GetEventLidarPacketOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventLidarPacketType GetEventLidarPacketOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventLidarPacketType> GetEventLidarPacketNdata(const size_t n);
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

    void SensorLidarPacketServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorLidarPacketServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorLidarPacketServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventLidarPacketType> EventLidarPacketContainer_;
    std::mutex EventLidarPacketMtx_;
    std::condition_variable EventLidarPacketCv_;
    void PushEventLidarPacketDataToContainer(EventLidarPacketDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif