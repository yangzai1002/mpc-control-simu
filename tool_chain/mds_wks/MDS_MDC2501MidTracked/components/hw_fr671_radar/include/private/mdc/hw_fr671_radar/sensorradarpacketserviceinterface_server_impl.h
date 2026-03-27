/* *
 * Class: SensorRadarPacketServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/hw_fr671_radar_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorradarpacketserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorRadarPacketServiceInterfaceSkeleton;
using EventSensorRadarPacketDataType = ara::adsfi::SensorRadarPacket;
using SendEventSensorRadarPacketType = std::shared_ptr<EventSensorRadarPacketDataType>;

namespace {
using LOG_SPACE = mdc::hw_fr671_radar::HwFr671RadarLogger;
}

class SensorRadarPacketServiceInterfaceServerImpl : public SensorRadarPacketServiceInterfaceSkeleton {
public:
    explicit SensorRadarPacketServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorRadarPacketServiceInterfaceServerImpl();

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
    bool SendEventSensorRadarPacketData(const SendEventSensorRadarPacketType& data);
    void ClearEventSensorRadarPacketContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventSensorRadarPacketThreadPtr_{nullptr};
    void CreateEventSensorRadarPacketThread();
    void EventSensorRadarPacketNotify();
    void TransferEventSensorRadarPacketData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventSensorRadarPacketType> eventSensorRadarPacketContainer_;
    std::mutex sendEventSensorRadarPacketMtx_;
    std::condition_variable sendEventSensorRadarPacketCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif