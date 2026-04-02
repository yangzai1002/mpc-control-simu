/* *
 * Class: RemoteControlServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_REMOTECONTROLSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_REMOTECONTROLSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/xzmq_rc_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/remotecontrolserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::RemoteControlServiceInterfaceSkeleton;
using EventRCAppRemoteCtlDataType = ara::adsfi::AppRemoteControl;
using SendEventRCAppRemoteCtlType = std::shared_ptr<EventRCAppRemoteCtlDataType>;

namespace {
using LOG_SPACE = mdc::xzmq_rc::XzmqRcLogger;
}

class RemoteControlServiceInterfaceServerImpl : public RemoteControlServiceInterfaceSkeleton {
public:
    explicit RemoteControlServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~RemoteControlServiceInterfaceServerImpl();

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
    bool SendEventRCAppRemoteCtlData(const SendEventRCAppRemoteCtlType& data);
    void ClearEventRCAppRemoteCtlContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventRCAppRemoteCtlThreadPtr_{nullptr};
    void CreateEventRCAppRemoteCtlThread();
    void EventRCAppRemoteCtlNotify();
    void TransferEventRCAppRemoteCtlData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventRCAppRemoteCtlType> eventRCAppRemoteCtlContainer_;
    std::mutex sendEventRCAppRemoteCtlMtx_;
    std::condition_variable sendEventRCAppRemoteCtlCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif