/* *
 * Class: SensorLidarPointCloudServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORLIDARPOINTCLOUDSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/hw_l106_lidar_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorlidarpointcloudserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorLidarPointCloudServiceInterfaceSkeleton;
using EventPointCloud2DataType = ara::adsfi::MsgLidarFrame;
using SendEventPointCloud2Type = std::shared_ptr<EventPointCloud2DataType>;

namespace {
using LOG_SPACE = mdc::hw_l106_lidar::HwL106LidarLogger;
}

class SensorLidarPointCloudServiceInterfaceServerImpl : public SensorLidarPointCloudServiceInterfaceSkeleton {
public:
    explicit SensorLidarPointCloudServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorLidarPointCloudServiceInterfaceServerImpl();

    bool Init();

    void Stop();

    inline bool IsStop() const
    {
        return !workFlag_;
    }

    inline uint32_t GetInstanceId() const
    {
        return instanceId_;
    }
    /* event relative */
    bool SendEventPointCloud2Data(const SendEventPointCloud2Type& data);
    void ClearEventPointCloud2Container();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventPointCloud2ThreadPtr_{nullptr};
    void CreateEventPointCloud2Thread();
    void EventPointCloud2Notify();
    void TransferEventPointCloud2Data();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventPointCloud2Type> eventPointCloud2Container_;
    std::mutex sendEventPointCloud2Mtx_;
    std::condition_variable sendEventPointCloud2Cv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif