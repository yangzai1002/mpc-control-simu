/* *
 * Class: LocationInfoIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_LOCATIONINFOINTFSERVERIMPL
#define ADSFI_LOCATIONINFOINTFSERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/fusion_location_base_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "adsfi/locationinfointf_skeleton.h"

namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using adsfi::skeleton::LocationInfoIntfSkeleton;
using LocationInfoDataType = ara::adsfi::MsgHafLocation;
using SendLocationInfoType = std::shared_ptr<LocationInfoDataType>;

namespace {
using LOG_SPACE = mdc::fusion_location_base::FusionLocationBaseLogger;
}

class LocationInfoIntfServerImpl : public LocationInfoIntfSkeleton {
public:
    explicit LocationInfoIntfServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~LocationInfoIntfServerImpl();

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
    bool SendLocationInfoData(const SendLocationInfoType& data);
    void ClearLocationInfoContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> locationInfoThreadPtr_{nullptr};
    void CreateLocationInfoThread();
    void LocationInfoNotify();
    void TransferLocationInfoData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendLocationInfoType> locationInfoContainer_;
    std::mutex sendLocationInfoMtx_;
    std::condition_variable sendLocationInfoCv_;
};
} /* namespace adsfi */

#endif