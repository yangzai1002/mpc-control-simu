/* *
 * Class: RadarPointCloudServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/radarpointcloudserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/fusion_follow_base_logger.h"
namespace ara {
namespace adsfi {
using RadarPointCloudServiceInterfaceProxy = ara::adsfi::proxy::RadarPointCloudServiceInterfaceProxy;

using EventRadarPointCloudHandlerType = std::function<void (ara::adsfi::SensorRadarPointCloud)>;
using EventRadarPointCloudDataType = ara::adsfi::SensorRadarPointCloud;
using RecvEventRadarPointCloudType = std::shared_ptr<EventRadarPointCloudDataType>;
namespace {
using LOG_SPACE = mdc::fusion_follow_base::FusionFollowBaseLogger;
}

class RadarPointCloudServiceInterfaceClientImpl {
public:
    explicit RadarPointCloudServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~RadarPointCloudServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventRadarPointCloudNotifyHandler(const EventRadarPointCloudHandlerType handler);
    void EventRadarPointCloudNotify()
    {
        std::lock_guard<std::mutex> recvLk(EventRadarPointCloudMtx_);
        EventRadarPointCloudCv_.notify_all();
    }

    void EventRadarPointCloudContainerClear()
    {
        EventRadarPointCloudContainer_.Clear();
    }

    bool EventRadarPointCloudContainerEmpty()
    {
        return EventRadarPointCloudContainer_.Empty();
    }

    RecvEventRadarPointCloudType GetEventRadarPointCloudOneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventRadarPointCloudType GetEventRadarPointCloudOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventRadarPointCloudType> GetEventRadarPointCloudNdata(const size_t n);
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

    void RadarPointCloudServiceInterfaceCallback(ara::com::ServiceHandleContainer<RadarPointCloudServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<RadarPointCloudServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventRadarPointCloudType> EventRadarPointCloudContainer_;
    std::mutex EventRadarPointCloudMtx_;
    std::condition_variable EventRadarPointCloudCv_;
    void PushEventRadarPointCloudDataToContainer(EventRadarPointCloudDataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif