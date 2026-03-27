/* *
 * Class: RadarPointCloudServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_RADARPOINTCLOUDSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/hw_fr671_radar_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/radarpointcloudserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::RadarPointCloudServiceInterfaceSkeleton;
using EventRadarPointCloudDataType = ara::adsfi::SensorRadarPointCloud;
using SendEventRadarPointCloudType = std::shared_ptr<EventRadarPointCloudDataType>;

namespace {
using LOG_SPACE = mdc::hw_fr671_radar::HwFr671RadarLogger;
}

class RadarPointCloudServiceInterfaceServerImpl : public RadarPointCloudServiceInterfaceSkeleton {
public:
    explicit RadarPointCloudServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~RadarPointCloudServiceInterfaceServerImpl();

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
    bool SendEventRadarPointCloudData(const SendEventRadarPointCloudType& data);
    void ClearEventRadarPointCloudContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventRadarPointCloudThreadPtr_{nullptr};
    void CreateEventRadarPointCloudThread();
    void EventRadarPointCloudNotify();
    void TransferEventRadarPointCloudData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventRadarPointCloudType> eventRadarPointCloudContainer_;
    std::mutex sendEventRadarPointCloudMtx_;
    std::condition_variable sendEventRadarPointCloudCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif