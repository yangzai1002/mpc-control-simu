/* *
 * Class: VisionSegmentServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/multy_camera_det_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/visionsegmentserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::VisionSegmentServiceInterfaceSkeleton;
using EventVisionSegmentDataType = ara::adsfi::MsgCameraVPResult;
using SendEventVisionSegmentType = std::shared_ptr<EventVisionSegmentDataType>;

namespace {
using LOG_SPACE = mdc::multy_camera_det_base::MultyCameraDetBaseLogger;
}

class VisionSegmentServiceInterfaceServerImpl : public VisionSegmentServiceInterfaceSkeleton {
public:
    explicit VisionSegmentServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~VisionSegmentServiceInterfaceServerImpl();

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
    bool SendEventVisionSegmentData(const SendEventVisionSegmentType& data);
    void ClearEventVisionSegmentContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventVisionSegmentThreadPtr_{nullptr};
    void CreateEventVisionSegmentThread();
    void EventVisionSegmentNotify();
    void TransferEventVisionSegmentData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventVisionSegmentType> eventVisionSegmentContainer_;
    std::mutex sendEventVisionSegmentMtx_;
    std::condition_variable sendEventVisionSegmentCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif