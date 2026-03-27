/* *
 * Class: SensorImageServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORIMAGESERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORIMAGESERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorimageserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/rtspserverswc_logger.h"
namespace ara {
namespace adsfi {
using SensorImageServiceInterfaceProxy = ara::adsfi::proxy::SensorImageServiceInterfaceProxy;

using EventSensorImageHandlerType = std::function<void (ara::adsfi::MsgImageData)>;
using EventSensorImageDataType = ara::adsfi::MsgImageData;
using RecvEventSensorImageType = std::shared_ptr<EventSensorImageDataType>;
namespace {
using LOG_SPACE = mdc::rtspserverswc::RtspserverswcLogger;
}

class SensorImageServiceInterfaceClientImpl {
public:
    explicit SensorImageServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorImageServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventSensorImageNotifyHandler(const EventSensorImageHandlerType handler);
    void EventSensorImageNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventSensorImageMtx_);
        EventSensorImageCv_.notify_all();
    }

    void EventSensorImageContainerClear()
    {
        EventSensorImageContainer_.Clear();
    }

    bool EventSensorImageContainerEmpty()
    {
        return EventSensorImageContainer_.Empty();
    }

    RecvEventSensorImageType GetEventSensorImageOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventSensorImageType GetEventSensorImageOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventSensorImageType> GetEventSensorImageNdata(const size_t n);
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

    void SensorImageServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorImageServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorImageServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventSensorImageType> EventSensorImageContainer_;
    std::mutex EventSensorImageMtx_;
    std::condition_variable EventSensorImageCv_;
    void PushEventSensorImageDataToContainer(EventSensorImageDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif