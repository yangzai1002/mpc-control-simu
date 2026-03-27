/* *
 * Class: SensorWheelSpeedServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorwheelspeedserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/fusion_location_base_logger.h"
namespace ara {
namespace adsfi {
using SensorWheelSpeedServiceInterfaceProxy = ara::adsfi::proxy::SensorWheelSpeedServiceInterfaceProxy;

using EventWheelSpeedHandlerType = std::function<void (ara::adsfi::MsgHafWheelSpeedList)>;
using EventWheelSpeedDataType = ara::adsfi::MsgHafWheelSpeedList;
using RecvEventWheelSpeedType = std::shared_ptr<EventWheelSpeedDataType>;
namespace {
using LOG_SPACE = mdc::fusion_location_base::FusionLocationBaseLogger;
}

class SensorWheelSpeedServiceInterfaceClientImpl {
public:
    explicit SensorWheelSpeedServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorWheelSpeedServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventWheelSpeedNotifyHandler(const EventWheelSpeedHandlerType handler);
    void EventWheelSpeedNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventWheelSpeedMtx_);
        EventWheelSpeedCv_.notify_all();
    }

    void EventWheelSpeedContainerClear()
    {
        EventWheelSpeedContainer_.Clear();
    }

    bool EventWheelSpeedContainerEmpty()
    {
        return EventWheelSpeedContainer_.Empty();
    }

    RecvEventWheelSpeedType GetEventWheelSpeedOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventWheelSpeedType GetEventWheelSpeedOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventWheelSpeedType> GetEventWheelSpeedNdata(const size_t n);
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

    void SensorWheelSpeedServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorWheelSpeedServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorWheelSpeedServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventWheelSpeedType> EventWheelSpeedContainer_;
    std::mutex EventWheelSpeedMtx_;
    std::condition_variable EventWheelSpeedCv_;
    void PushEventWheelSpeedDataToContainer(EventWheelSpeedDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif