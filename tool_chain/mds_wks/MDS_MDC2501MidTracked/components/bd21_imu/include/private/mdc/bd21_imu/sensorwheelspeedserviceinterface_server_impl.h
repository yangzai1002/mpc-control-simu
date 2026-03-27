/* *
 * Class: SensorWheelSpeedServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/bd21_imu_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorwheelspeedserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorWheelSpeedServiceInterfaceSkeleton;
using EventWheelSpeedDataType = ara::adsfi::MsgHafWheelSpeedList;
using SendEventWheelSpeedType = std::shared_ptr<EventWheelSpeedDataType>;

namespace {
using LOG_SPACE = mdc::bd21_imu::Bd21ImuLogger;
}

class SensorWheelSpeedServiceInterfaceServerImpl : public SensorWheelSpeedServiceInterfaceSkeleton {
public:
    explicit SensorWheelSpeedServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorWheelSpeedServiceInterfaceServerImpl();

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
    bool SendEventWheelSpeedData(const SendEventWheelSpeedType& data);
    void ClearEventWheelSpeedContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventWheelSpeedThreadPtr_{nullptr};
    void CreateEventWheelSpeedThread();
    void EventWheelSpeedNotify();
    void TransferEventWheelSpeedData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventWheelSpeedType> eventWheelSpeedContainer_;
    std::mutex sendEventWheelSpeedMtx_;
    std::condition_variable sendEventWheelSpeedCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif