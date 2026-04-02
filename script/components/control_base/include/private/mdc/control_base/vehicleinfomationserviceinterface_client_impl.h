/* *
 * Class: VehicleInfomationServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/vehicleinfomationserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/control_base_logger.h"
namespace ara {
namespace adsfi {
using VehicleInfomationServiceInterfaceProxy = ara::adsfi::proxy::VehicleInfomationServiceInterfaceProxy;

using EventVehicleInfoHandlerType = std::function<void (ara::adsfi::VehicleInformation)>;
using EventVehicleInfoDataType = ara::adsfi::VehicleInformation;
using RecvEventVehicleInfoType = std::shared_ptr<EventVehicleInfoDataType>;
namespace {
using LOG_SPACE = mdc::control_base::ControlBaseLogger;
}

class VehicleInfomationServiceInterfaceClientImpl {
public:
    explicit VehicleInfomationServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~VehicleInfomationServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventVehicleInfoNotifyHandler(const EventVehicleInfoHandlerType handler);
    void EventVehicleInfoNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventVehicleInfoMtx_);
        EventVehicleInfoCv_.notify_all();
    }

    void EventVehicleInfoContainerClear()
    {
        EventVehicleInfoContainer_.Clear();
    }

    bool EventVehicleInfoContainerEmpty()
    {
        return EventVehicleInfoContainer_.Empty();
    }

    RecvEventVehicleInfoType GetEventVehicleInfoOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventVehicleInfoType GetEventVehicleInfoOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventVehicleInfoType> GetEventVehicleInfoNdata(const size_t n);
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

    void VehicleInfomationServiceInterfaceCallback(ara::com::ServiceHandleContainer<VehicleInfomationServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<VehicleInfomationServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventVehicleInfoType> EventVehicleInfoContainer_;
    std::mutex EventVehicleInfoMtx_;
    std::condition_variable EventVehicleInfoCv_;
    void PushEventVehicleInfoDataToContainer(EventVehicleInfoDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif