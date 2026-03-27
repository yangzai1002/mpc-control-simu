/* *
 * Class: FaultDataServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_FAULTDATASERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_FAULTDATASERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/faultdataserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/xdata_trigger_recorder_base_logger.h"
namespace ara {
namespace adsfi {
using FaultDataServiceInterfaceProxy = ara::adsfi::proxy::FaultDataServiceInterfaceProxy;

using EventFaultDataHandlerType = std::function<void (ara::adsfi::FaultData)>;
using EventFaultDataDataType = ara::adsfi::FaultData;
using RecvEventFaultDataType = std::shared_ptr<EventFaultDataDataType>;
namespace {
using LOG_SPACE = mdc::xdata_trigger_recorder_base::XdataTriggerRecorderBaseLogger;
}

class FaultDataServiceInterfaceClientImpl {
public:
    explicit FaultDataServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~FaultDataServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventFaultDataNotifyHandler(const EventFaultDataHandlerType handler);
    void EventFaultDataNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventFaultDataMtx_);
        EventFaultDataCv_.notify_all();
    }

    void EventFaultDataContainerClear()
    {
        EventFaultDataContainer_.Clear();
    }

    bool EventFaultDataContainerEmpty()
    {
        return EventFaultDataContainer_.Empty();
    }

    RecvEventFaultDataType GetEventFaultDataOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventFaultDataType GetEventFaultDataOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventFaultDataType> GetEventFaultDataNdata(const size_t n);
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

    void FaultDataServiceInterfaceCallback(ara::com::ServiceHandleContainer<FaultDataServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<FaultDataServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventFaultDataType> EventFaultDataContainer_;
    std::mutex EventFaultDataMtx_;
    std::condition_variable EventFaultDataCv_;
    void PushEventFaultDataDataToContainer(EventFaultDataDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif