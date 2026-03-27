/* *
 * Class: SensorLidarPacketServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/hw_l106_lidar_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorlidarpacketserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorLidarPacketServiceInterfaceSkeleton;
using EventLidarPacketDataType = ara::adsfi::SensorLidarPacket;
using SendEventLidarPacketType = std::shared_ptr<EventLidarPacketDataType>;

namespace {
using LOG_SPACE = mdc::hw_l106_lidar::HwL106LidarLogger;
}

class SensorLidarPacketServiceInterfaceServerImpl : public SensorLidarPacketServiceInterfaceSkeleton {
public:
    explicit SensorLidarPacketServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorLidarPacketServiceInterfaceServerImpl();

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
    bool SendEventLidarPacketData(const SendEventLidarPacketType& data);
    void ClearEventLidarPacketContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventLidarPacketThreadPtr_{nullptr};
    void CreateEventLidarPacketThread();
    void EventLidarPacketNotify();
    void TransferEventLidarPacketData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventLidarPacketType> eventLidarPacketContainer_;
    std::mutex sendEventLidarPacketMtx_;
    std::condition_variable sendEventLidarPacketCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif