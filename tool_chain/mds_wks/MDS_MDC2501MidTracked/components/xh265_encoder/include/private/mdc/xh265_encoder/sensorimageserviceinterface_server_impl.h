/* *
 * Class: SensorImageServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORIMAGESERVICEINTERFACESERVERIMPL
#define ARA_ADSFI_SENSORIMAGESERVICEINTERFACESERVERIMPL

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "mdc/xh265_encoder_logger.h"
#include "ara/core/future.h"
#include "ara/com/types.h"
#include "mdc/utility/thread_safe_stack.h"

#include "ara/adsfi/sensorimageserviceinterface_skeleton.h"

namespace ara {
namespace adsfi {

using ara::com::InstanceIdentifier;
using ara::com::MethodCallProcessingMode;
using ara::adsfi::skeleton::SensorImageServiceInterfaceSkeleton;
using EventSensorImageDataType = ara::adsfi::MsgImageData;
using SendEventSensorImageType = std::shared_ptr<EventSensorImageDataType>;

namespace {
using LOG_SPACE = mdc::xh265_encoder::Xh265EncoderLogger;
}

class SensorImageServiceInterfaceServerImpl : public SensorImageServiceInterfaceSkeleton {
public:
    explicit SensorImageServiceInterfaceServerImpl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll);

    virtual ~SensorImageServiceInterfaceServerImpl();

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
    bool SendEventSensorImageData(const SendEventSensorImageType& data);
    void ClearEventSensorImageContainer();
    
    

private:
    /* event relative */
    std::unique_ptr<std::thread> eventSensorImageThreadPtr_{nullptr};
    void CreateEventSensorImageThread();
    void EventSensorImageNotify();
    void TransferEventSensorImageData();

    /* 服务实例ID */
    uint32_t instanceId_;

    /* 服务标识 */
    std::atomic<bool> workFlag_;

    /* 提供服务标识 */
    std::atomic<bool> offerServFlag_;

    /* event relative */
    mdc::ThreadSafeStack<SendEventSensorImageType> eventSensorImageContainer_;
    std::mutex sendEventSensorImageMtx_;
    std::condition_variable sendEventSensorImageCv_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif