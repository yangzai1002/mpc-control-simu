/* *
 * Class: FaultDataServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_FAULTDATASERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_FAULTDATASERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/xfault_handler_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/faultdataserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::FaultDataServiceInterfaceSkeleton;
using EventFaultDataDataType = ara::adsfi::FaultData;
using SendEventFaultDataType = std::shared_ptr<EventFaultDataDataType>;

namespace {
using LOG_SPACE = mdc::xfault_handler_base::XfaultHandlerBaseLogger;
}

class FaultDataServiceInterfaceServerImpl : public FaultDataServiceInterfaceSkeleton {
public:
    explicit FaultDataServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~FaultDataServiceInterfaceServerImpl();

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
    bool SendEventFaultDataData(const SendEventFaultDataType& data);
    void ClearEventFaultDataContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventFaultDataThreadPtr_{nullptr};
    void CreateEventFaultDataThread();
    void EventFaultDataNotify();
    void TransferEventFaultDataData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventFaultDataType> eventFaultDataContainer_;
    std::mutex sendEventFaultDataMtx_;
    std::condition_variable sendEventFaultDataCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif