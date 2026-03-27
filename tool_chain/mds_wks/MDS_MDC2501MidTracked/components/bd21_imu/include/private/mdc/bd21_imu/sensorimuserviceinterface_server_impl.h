/* *
 * Class: SensorImuServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORIMUSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORIMUSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/bd21_imu_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorimuserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorImuServiceInterfaceSkeleton;
using EventSensorImuDataType = ara::adsfi::MsgHafIMU;
using SendEventSensorImuType = std::shared_ptr<EventSensorImuDataType>;

namespace {
using LOG_SPACE = mdc::bd21_imu::Bd21ImuLogger;
}

class SensorImuServiceInterfaceServerImpl : public SensorImuServiceInterfaceSkeleton {
public:
    explicit SensorImuServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorImuServiceInterfaceServerImpl();

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
    bool SendEventSensorImuData(const SendEventSensorImuType& data);
    void ClearEventSensorImuContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventSensorImuThreadPtr_{nullptr};
    void CreateEventSensorImuThread();
    void EventSensorImuNotify();
    void TransferEventSensorImuData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventSensorImuType> eventSensorImuContainer_;
    std::mutex sendEventSensorImuMtx_;
    std::condition_variable sendEventSensorImuCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif