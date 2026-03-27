/* *
 * Class: Object3dArrayIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_OBJECT3DARRAYINTFSERVERIMPL
#define ADSFI_OBJECT3DARRAYINTFSERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/lidar_det_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "adsfi/object3darrayintf_skeleton.h"

namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using adsfi::skeleton::Object3dArrayIntfSkeleton;
using EventPerceptionLidarObjectsDataType = ara::adsfi::MsgHaf3dDetectionOutArray;
using SendEventPerceptionLidarObjectsType = std::shared_ptr<EventPerceptionLidarObjectsDataType>;

namespace {
using LOG_SPACE = mdc::lidar_det_base::LidarDetBaseLogger;
}

class Object3dArrayIntfServerImpl : public Object3dArrayIntfSkeleton {
public:
    explicit Object3dArrayIntfServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~Object3dArrayIntfServerImpl();

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
    bool SendEventPerceptionLidarObjectsData(const SendEventPerceptionLidarObjectsType& data);
    void ClearEventPerceptionLidarObjectsContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventPerceptionLidarObjectsThreadPtr_{nullptr};
    void CreateEventPerceptionLidarObjectsThread();
    void EventPerceptionLidarObjectsNotify();
    void TransferEventPerceptionLidarObjectsData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventPerceptionLidarObjectsType> eventPerceptionLidarObjectsContainer_;
    std::mutex sendEventPerceptionLidarObjectsMtx_;
    std::condition_variable sendEventPerceptionLidarObjectsCv_;
};
} /* namespace adsfi */

#endif