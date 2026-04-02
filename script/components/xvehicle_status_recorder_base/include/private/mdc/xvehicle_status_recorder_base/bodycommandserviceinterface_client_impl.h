/* *
 * Class: BodyCommandServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_BODYCOMMANDSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_BODYCOMMANDSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/bodycommandserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/xvehicle_status_recorder_base_logger.h"
namespace ara {
namespace adsfi {
using BodyCommandServiceInterfaceProxy = ara::adsfi::proxy::BodyCommandServiceInterfaceProxy;

using EventVehicleActControlHandlerType = std::function<void (ara::adsfi::VehicleActControl)>;
using EventVehicleActControlDataType = ara::adsfi::VehicleActControl;
using RecvEventVehicleActControlType = std::shared_ptr<EventVehicleActControlDataType>;
namespace {
using LOG_SPACE = mdc::xvehicle_status_recorder_base::XvehicleStatusRecorderBaseLogger;
}

class BodyCommandServiceInterfaceClientImpl {
public:
    explicit BodyCommandServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~BodyCommandServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventVehicleActControlNotifyHandler(const EventVehicleActControlHandlerType handler);
    void EventVehicleActControlNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventVehicleActControlMtx_);
        EventVehicleActControlCv_.notify_all();
    }

    void EventVehicleActControlContainerClear()
    {
        EventVehicleActControlContainer_.Clear();
    }

    bool EventVehicleActControlContainerEmpty()
    {
        return EventVehicleActControlContainer_.Empty();
    }

    RecvEventVehicleActControlType GetEventVehicleActControlOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventVehicleActControlType GetEventVehicleActControlOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventVehicleActControlType> GetEventVehicleActControlNdata(const size_t n);
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

    void BodyCommandServiceInterfaceCallback(ara::com::ServiceHandleContainer<BodyCommandServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<BodyCommandServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventVehicleActControlType> EventVehicleActControlContainer_;
    std::mutex EventVehicleActControlMtx_;
    std::condition_variable EventVehicleActControlCv_;
    void PushEventVehicleActControlDataToContainer(EventVehicleActControlDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif