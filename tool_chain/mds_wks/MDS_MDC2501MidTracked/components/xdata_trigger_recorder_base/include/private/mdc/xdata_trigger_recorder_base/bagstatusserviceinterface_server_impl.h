/* *
 * Class: BagStatusServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_BAGSTATUSSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_BAGSTATUSSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/xdata_trigger_recorder_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/bagstatusserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::BagStatusServiceInterfaceSkeleton;
using EventBagStatusDataType = ara::adsfi::FaultData;
using SendEventBagStatusType = std::shared_ptr<EventBagStatusDataType>;

namespace {
using LOG_SPACE = mdc::xdata_trigger_recorder_base::XdataTriggerRecorderBaseLogger;
}

class BagStatusServiceInterfaceServerImpl : public BagStatusServiceInterfaceSkeleton {
public:
    explicit BagStatusServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~BagStatusServiceInterfaceServerImpl();

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
    bool SendEventBagStatusData(const SendEventBagStatusType& data);
    void ClearEventBagStatusContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventBagStatusThreadPtr_{nullptr};
    void CreateEventBagStatusThread();
    void EventBagStatusNotify();
    void TransferEventBagStatusData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventBagStatusType> eventBagStatusContainer_;
    std::mutex sendEventBagStatusMtx_;
    std::condition_variable sendEventBagStatusCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif