/* *
 * Class: SensorGpsServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORGPSSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORGPSSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/bd21_imu_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorgpsserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorGpsServiceInterfaceSkeleton;
using EventGpsDataType = ara::adsfi::MsgHafGnssInfo;
using SendEventGpsType = std::shared_ptr<EventGpsDataType>;

namespace {
using LOG_SPACE = mdc::bd21_imu::Bd21ImuLogger;
}

class SensorGpsServiceInterfaceServerImpl : public SensorGpsServiceInterfaceSkeleton {
public:
    explicit SensorGpsServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorGpsServiceInterfaceServerImpl();

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
    bool SendEventGpsData(const SendEventGpsType& data);
    void ClearEventGpsContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventGpsThreadPtr_{nullptr};
    void CreateEventGpsThread();
    void EventGpsNotify();
    void TransferEventGpsData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventGpsType> eventGpsContainer_;
    std::mutex sendEventGpsMtx_;
    std::condition_variable sendEventGpsCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif