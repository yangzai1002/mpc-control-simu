/* *
 * Class: VisionSegmentServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACECLIENT
#define ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msgcameravpresult.h"





namespace ara {
namespace adsfi {

using EventVisionSegmentHandlerType = std::function<void (ara::adsfi::MsgCameraVPResult)>;
using EventVisionSegmentDataType = ara::adsfi::MsgCameraVPResult;
using RecvEventVisionSegmentType = std::shared_ptr<EventVisionSegmentDataType>;
class VisionSegmentServiceInterfaceClient final {
public:
    VisionSegmentServiceInterfaceClient() = delete;
    explicit VisionSegmentServiceInterfaceClient(const uint32_t instanceId);
    virtual ~VisionSegmentServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventVisionSegmentNotifyHandler(const EventVisionSegmentHandlerType handler);
    void EventVisionSegmentNotify();
    void EventVisionSegmentContainerClear();
    bool EventVisionSegmentContainerEmpty();
    RecvEventVisionSegmentType GetEventVisionSegmentOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventVisionSegmentType GetEventVisionSegmentOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventVisionSegmentType> GetEventVisionSegmentNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif