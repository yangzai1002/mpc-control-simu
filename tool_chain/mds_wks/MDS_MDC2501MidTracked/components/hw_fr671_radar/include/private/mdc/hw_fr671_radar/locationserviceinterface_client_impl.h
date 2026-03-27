/* *
 * Class: LocationServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef MDC_LOCATION_LOCATIONSERVICEINTERFACECLIENT_IMPL
#define MDC_LOCATION_LOCATIONSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "mdc/location/locationserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/hw_fr671_radar_logger.h"
namespace mdc {
namespace location {
using LocationServiceInterfaceProxy = mdc::location::proxy::LocationServiceInterfaceProxy;

using LocationEventHandlerType = std::function<void (ara::adsfi::MsgHafLocation)>;
using LocationEventDataType = ara::adsfi::MsgHafLocation;
using RecvLocationEventType = std::shared_ptr<LocationEventDataType>;
namespace {
using LOG_SPACE = mdc::hw_fr671_radar::HwFr671RadarLogger;
}

class LocationServiceInterfaceClientImpl {
public:
    explicit LocationServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~LocationServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterLocationEventNotifyHandler(const LocationEventHandlerType handler);
    void LocationEventNotify()
    {
        std::lock_guard<std::mutex> recvLk(LocationEventMtx_);
        LocationEventCv_.notify_all();
    }

    void LocationEventContainerClear()
    {
        LocationEventContainer_.Clear();
    }

    bool LocationEventContainerEmpty()
    {
        return LocationEventContainer_.Empty();
    }

    RecvLocationEventType GetLocationEventOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvLocationEventType GetLocationEventOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvLocationEventType> GetLocationEventNdata(const size_t n);
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

    void LocationServiceInterfaceCallback(ara::com::ServiceHandleContainer<LocationServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<LocationServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvLocationEventType> LocationEventContainer_;
    std::mutex LocationEventMtx_;
    std::condition_variable LocationEventCv_;
    void PushLocationEventDataToContainer(LocationEventDataType&& data);
};
} /* namespace location */
} /* namespace mdc */
#endif