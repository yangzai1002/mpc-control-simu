/* *
 * Class: FusionObjectArrayIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_FUSIONOBJECTARRAYINTFSERVERIMPL
#define ADSFI_FUSIONOBJECTARRAYINTFSERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/fusion_follow_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "adsfi/fusionobjectarrayintf_skeleton.h"

namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using adsfi::skeleton::FusionObjectArrayIntfSkeleton;
using FusionObjectArrayDataType = ara::adsfi::MsgHafFusionOutArray;
using SendFusionObjectArrayType = std::shared_ptr<FusionObjectArrayDataType>;

namespace {
using LOG_SPACE = mdc::fusion_follow_base::FusionFollowBaseLogger;
}

class FusionObjectArrayIntfServerImpl : public FusionObjectArrayIntfSkeleton {
public:
    explicit FusionObjectArrayIntfServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~FusionObjectArrayIntfServerImpl();

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
    bool SendFusionObjectArrayData(const SendFusionObjectArrayType& data);
    void ClearFusionObjectArrayContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> fusionObjectArrayThreadPtr_{nullptr};
    void CreateFusionObjectArrayThread();
    void FusionObjectArrayNotify();
    void TransferFusionObjectArrayData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendFusionObjectArrayType> fusionObjectArrayContainer_;
    std::mutex sendFusionObjectArrayMtx_;
    std::condition_variable sendFusionObjectArrayCv_;
};
} /* namespace adsfi */

#endif