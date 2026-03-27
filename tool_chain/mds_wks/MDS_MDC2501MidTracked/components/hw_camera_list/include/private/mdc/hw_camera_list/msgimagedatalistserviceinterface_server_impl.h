/* *
 * Class: MsgImageDataListServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/hw_camera_list_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/msgimagedatalistserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::MsgImageDataListServiceInterfaceSkeleton;
using EventImageListDataType = ara::adsfi::MsgImageDataList;
using SendEventImageListType = std::shared_ptr<EventImageListDataType>;

namespace {
using LOG_SPACE = mdc::hw_camera_list::HwCameraListLogger;
}

class MsgImageDataListServiceInterfaceServerImpl : public MsgImageDataListServiceInterfaceSkeleton {
public:
    explicit MsgImageDataListServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~MsgImageDataListServiceInterfaceServerImpl();

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
    bool SendEventImageListData(const SendEventImageListType& data);
    void ClearEventImageListContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventImageListThreadPtr_{nullptr};
    void CreateEventImageListThread();
    void EventImageListNotify();
    void TransferEventImageListData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventImageListType> eventImageListContainer_;
    std::mutex sendEventImageListMtx_;
    std::condition_variable sendEventImageListCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif