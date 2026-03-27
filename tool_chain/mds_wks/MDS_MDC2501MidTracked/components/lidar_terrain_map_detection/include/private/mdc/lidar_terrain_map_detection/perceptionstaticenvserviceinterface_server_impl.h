/* *
 * Class: PerceptionStaticEnvServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/lidar_terrain_map_detection_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/perceptionstaticenvserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::PerceptionStaticEnvServiceInterfaceSkeleton;
using EventPerceptionStaticEnvDataType = ara::adsfi::PerceptionStaticEnv;
using SendEventPerceptionStaticEnvType = std::shared_ptr<EventPerceptionStaticEnvDataType>;

namespace {
using LOG_SPACE = mdc::lidar_terrain_map_detection::LidarTerrainMapDetectionLogger;
}

class PerceptionStaticEnvServiceInterfaceServerImpl : public PerceptionStaticEnvServiceInterfaceSkeleton {
public:
    explicit PerceptionStaticEnvServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~PerceptionStaticEnvServiceInterfaceServerImpl();

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
    bool SendEventPerceptionStaticEnvData(const SendEventPerceptionStaticEnvType& data);
    void ClearEventPerceptionStaticEnvContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventPerceptionStaticEnvThreadPtr_{nullptr};
    void CreateEventPerceptionStaticEnvThread();
    void EventPerceptionStaticEnvNotify();
    void TransferEventPerceptionStaticEnvData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventPerceptionStaticEnvType> eventPerceptionStaticEnvContainer_;
    std::mutex sendEventPerceptionStaticEnvMtx_;
    std::condition_variable sendEventPerceptionStaticEnvCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif