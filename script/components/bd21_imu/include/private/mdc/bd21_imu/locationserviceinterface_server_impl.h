/* *
 * Class: LocationServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef MDC_LOCATION_LOCATIONSERVICEINTERFACESERVERIMPL
#define MDC_LOCATION_LOCATIONSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/bd21_imu_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "mdc/location/locationserviceinterface_skeleton.h"

namespace mdc {
namespace location {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using mdc::location::skeleton::LocationServiceInterfaceSkeleton;
using LocationEventDataType = ara::adsfi::MsgHafLocation;
using SendLocationEventType = std::shared_ptr<LocationEventDataType>;

namespace {
using LOG_SPACE = mdc::bd21_imu::Bd21ImuLogger;
}

class LocationServiceInterfaceServerImpl : public LocationServiceInterfaceSkeleton {
public:
    explicit LocationServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~LocationServiceInterfaceServerImpl();

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
    bool SendLocationEventData(const SendLocationEventType& data);
    void ClearLocationEventContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> locationEventThreadPtr_{nullptr};
    void CreateLocationEventThread();
    void LocationEventNotify();
    void TransferLocationEventData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendLocationEventType> locationEventContainer_;
    std::mutex sendLocationEventMtx_;
    std::condition_variable sendLocationEventCv_;
};
} /* namespace location */
} /* namespace mdc */

#endif