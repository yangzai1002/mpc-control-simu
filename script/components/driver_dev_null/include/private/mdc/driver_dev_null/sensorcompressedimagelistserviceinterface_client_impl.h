/* *
 * Class: SensorCompressedImageListServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorcompressedimagelistserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/driver_dev_null_logger.h"
namespace ara {
namespace adsfi {
using SensorCompressedImageListServiceInterfaceProxy = ara::adsfi::proxy::SensorCompressedImageListServiceInterfaceProxy;

using EventCompressedImageListHandlerType = std::function<void (ara::adsfi::MsgImageDataList)>;
using EventCompressedImageListDataType = ara::adsfi::MsgImageDataList;
using RecvEventCompressedImageListType = std::shared_ptr<EventCompressedImageListDataType>;
namespace {
using LOG_SPACE = mdc::driver_dev_null::DriverDevNullLogger;
}

class SensorCompressedImageListServiceInterfaceClientImpl {
public:
    explicit SensorCompressedImageListServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorCompressedImageListServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventCompressedImageListNotifyHandler(const EventCompressedImageListHandlerType handler);
    void EventCompressedImageListNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventCompressedImageListMtx_);
        EventCompressedImageListCv_.notify_all();
    }

    void EventCompressedImageListContainerClear()
    {
        EventCompressedImageListContainer_.Clear();
    }

    bool EventCompressedImageListContainerEmpty()
    {
        return EventCompressedImageListContainer_.Empty();
    }

    RecvEventCompressedImageListType GetEventCompressedImageListOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventCompressedImageListType GetEventCompressedImageListOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventCompressedImageListType> GetEventCompressedImageListNdata(const size_t n);
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

    void SensorCompressedImageListServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorCompressedImageListServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorCompressedImageListServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventCompressedImageListType> EventCompressedImageListContainer_;
    std::mutex EventCompressedImageListMtx_;
    std::condition_variable EventCompressedImageListCv_;
    void PushEventCompressedImageListDataToContainer(EventCompressedImageListDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif