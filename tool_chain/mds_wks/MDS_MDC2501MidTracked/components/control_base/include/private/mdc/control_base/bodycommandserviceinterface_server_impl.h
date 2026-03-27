/* *
 * Class: BodyCommandServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_BODYCOMMANDSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_BODYCOMMANDSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/control_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/bodycommandserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::BodyCommandServiceInterfaceSkeleton;
using EventVehicleActControlDataType = ara::adsfi::VehicleActControl;
using SendEventVehicleActControlType = std::shared_ptr<EventVehicleActControlDataType>;

namespace {
using LOG_SPACE = mdc::control_base::ControlBaseLogger;
}

class BodyCommandServiceInterfaceServerImpl : public BodyCommandServiceInterfaceSkeleton {
public:
    explicit BodyCommandServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~BodyCommandServiceInterfaceServerImpl();

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
    bool SendEventVehicleActControlData(const SendEventVehicleActControlType& data);
    void ClearEventVehicleActControlContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventVehicleActControlThreadPtr_{nullptr};
    void CreateEventVehicleActControlThread();
    void EventVehicleActControlNotify();
    void TransferEventVehicleActControlData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventVehicleActControlType> eventVehicleActControlContainer_;
    std::mutex sendEventVehicleActControlMtx_;
    std::condition_variable sendEventVehicleActControlCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif