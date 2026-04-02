/* *
 * Class: VehicleInfomationServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_VEHICLEINFOMATIONSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/mid_track_chassis_recv_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/vehicleinfomationserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::VehicleInfomationServiceInterfaceSkeleton;
using EventVehicleInfoDataType = ara::adsfi::VehicleInformation;
using SendEventVehicleInfoType = std::shared_ptr<EventVehicleInfoDataType>;

namespace {
using LOG_SPACE = mdc::mid_track_chassis_recv::MidTrackChassisRecvLogger;
}

class VehicleInfomationServiceInterfaceServerImpl : public VehicleInfomationServiceInterfaceSkeleton {
public:
    explicit VehicleInfomationServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~VehicleInfomationServiceInterfaceServerImpl();

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
    bool SendEventVehicleInfoData(const SendEventVehicleInfoType& data);
    void ClearEventVehicleInfoContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventVehicleInfoThreadPtr_{nullptr};
    void CreateEventVehicleInfoThread();
    void EventVehicleInfoNotify();
    void TransferEventVehicleInfoData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventVehicleInfoType> eventVehicleInfoContainer_;
    std::mutex sendEventVehicleInfoMtx_;
    std::condition_variable sendEventVehicleInfoCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif