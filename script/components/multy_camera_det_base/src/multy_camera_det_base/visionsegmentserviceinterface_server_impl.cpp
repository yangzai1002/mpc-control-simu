/* *
 * CLASS: VisionSegmentServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/multy_camera_det_base/visionsegmentserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
VisionSegmentServiceInterfaceServerImpl::VisionSegmentServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : VisionSegmentServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

VisionSegmentServiceInterfaceServerImpl::~VisionSegmentServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool VisionSegmentServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void VisionSegmentServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void VisionSegmentServiceInterfaceServerImpl::EventVisionSegmentNotify()
{
    sendEventVisionSegmentCv_.notify_one();
    return;
}

bool VisionSegmentServiceInterfaceServerImpl::SendEventVisionSegmentData(const SendEventVisionSegmentType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventVisionSegment.Allocate();
    *val = *data;
    this->EventVisionSegment.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void VisionSegmentServiceInterfaceServerImpl::TransferEventVisionSegmentData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventVisionSegmentMtx_);
        std::cv_status status = sendEventVisionSegmentCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventVisionSegmentContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventVisionSegment.Allocate();
        *val = *data;
        this->EventVisionSegment.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void VisionSegmentServiceInterfaceServerImpl::ClearEventVisionSegmentContainer()
{
    std::lock_guard<std::mutex> lck(sendEventVisionSegmentMtx_);
    eventVisionSegmentContainer_.Clear();
    return;
}

void VisionSegmentServiceInterfaceServerImpl::CreateEventVisionSegmentThread()
{
    eventVisionSegmentThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&VisionSegmentServiceInterfaceServerImpl::TransferEventVisionSegmentData, this));
}

} /* namespace adsfi */
} /* namespace ara */
