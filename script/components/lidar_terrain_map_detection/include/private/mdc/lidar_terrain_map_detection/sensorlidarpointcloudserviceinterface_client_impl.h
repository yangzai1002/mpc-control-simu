/* *
 * Class: SensorLidarPointCloudServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACECLIENT_IMPL
#define ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACECLIENT_IMPL

#include <memory>
#include <vector>
#include <atomic>
#include "ara/adsfi/sensorlidarpointcloudserviceinterface_proxy.h"
#include "ara/exec/execution_client.h"
#include <mutex>
#include <condition_variable>
#include "mdc/utility/thread_safe_stack.h"
#include "mdc/lidar_terrain_map_detection_logger.h"
namespace ara {
namespace adsfi {
using SensorLidarPointCloudServiceInterfaceProxy = ara::adsfi::proxy::SensorLidarPointCloudServiceInterfaceProxy;

using EventPointCloud2HandlerType = std::function<void (ara::adsfi::MsgLidarFrame)>;
using EventPointCloud2DataType = ara::adsfi::MsgLidarFrame;
using RecvEventPointCloud2Type = std::shared_ptr<EventPointCloud2DataType>;
namespace {
using LOG_SPACE = mdc::lidar_terrain_map_detection::LidarTerrainMapDetectionLogger;
}

class SensorLidarPointCloudServiceInterfaceClientImpl {
public:
    explicit SensorLidarPointCloudServiceInterfaceClientImpl(const uint32_t instanceId);
    virtual ~SensorLidarPointCloudServiceInterfaceClientImpl();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventPointCloud2NotifyHandler(const EventPointCloud2HandlerType handler);
    void EventPointCloud2Notify()
    {
        std::lock_guard<std::mutex> recvLk(EventPointCloud2Mtx_);
        EventPointCloud2Cv_.notify_all();
    }

    void EventPointCloud2ContainerClear()
    {
        EventPointCloud2Container_.Clear();
    }

    bool EventPointCloud2ContainerEmpty()
    {
        return EventPointCloud2Container_.Empty();
    }

    RecvEventPointCloud2Type GetEventPointCloud2OneData(const uint32_t freshDataTime = UINT32_MAX);

    RecvEventPointCloud2Type GetEventPointCloud2OneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);

    std::vector<RecvEventPointCloud2Type> GetEventPointCloud2Ndata(const size_t n);
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

    void SensorLidarPointCloudServiceInterfaceCallback(ara::com::ServiceHandleContainer<SensorLidarPointCloudServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle);
    std::unique_ptr<SensorLidarPointCloudServiceInterfaceProxy> proxyPtr_{nullptr};
    uint32_t recvQSize_{15U};
    ara::com::FindServiceHandle serviceHandle_{};
    void EmReportExec();

    /* event relative */
    mdc::ThreadSafeStack<RecvEventPointCloud2Type> EventPointCloud2Container_;
    std::mutex EventPointCloud2Mtx_;
    std::condition_variable EventPointCloud2Cv_;
    void PushEventPointCloud2DataToContainer(EventPointCloud2DataType&& data);
};
} /* namespace adsfi */
} /* namespace ara */
#endif