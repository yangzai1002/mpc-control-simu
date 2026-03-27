/* *
 * Class: SensorCompressedImageListServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/hw_camera_list_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorcompressedimagelistserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorCompressedImageListServiceInterfaceSkeleton;
using EventCompressedImageListDataType = ara::adsfi::MsgImageDataList;
using SendEventCompressedImageListType = std::shared_ptr<EventCompressedImageListDataType>;

namespace {
using LOG_SPACE = mdc::hw_camera_list::HwCameraListLogger;
}

class SensorCompressedImageListServiceInterfaceServerImpl : public SensorCompressedImageListServiceInterfaceSkeleton {
public:
    explicit SensorCompressedImageListServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorCompressedImageListServiceInterfaceServerImpl();

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
    bool SendEventCompressedImageListData(const SendEventCompressedImageListType& data);
    void ClearEventCompressedImageListContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventCompressedImageListThreadPtr_{nullptr};
    void CreateEventCompressedImageListThread();
    void EventCompressedImageListNotify();
    void TransferEventCompressedImageListData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventCompressedImageListType> eventCompressedImageListContainer_;
    std::mutex sendEventCompressedImageListMtx_;
    std::condition_variable sendEventCompressedImageListCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif