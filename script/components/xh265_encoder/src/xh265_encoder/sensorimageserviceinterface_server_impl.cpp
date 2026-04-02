/* *
 * CLASS: SensorImageServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xh265_encoder/sensorimageserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorImageServiceInterfaceServerImpl::SensorImageServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorImageServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorImageServiceInterfaceServerImpl::~SensorImageServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorImageServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorImageServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorImageServiceInterfaceServerImpl::EventSensorImageNotify()
{
    sendEventSensorImageCv_.notify_one();
    return;
}

bool SensorImageServiceInterfaceServerImpl::SendEventSensorImageData(const SendEventSensorImageType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventSensorImage.Allocate();
    *val = *data;
    this->EventSensorImage.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorImageServiceInterfaceServerImpl::TransferEventSensorImageData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventSensorImageMtx_);
        std::cv_status status = sendEventSensorImageCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventSensorImageContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventSensorImage.Allocate();
        *val = *data;
        this->EventSensorImage.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorImageServiceInterfaceServerImpl::ClearEventSensorImageContainer()
{
    std::lock_guard<std::mutex> lck(sendEventSensorImageMtx_);
    eventSensorImageContainer_.Clear();
    return;
}

void SensorImageServiceInterfaceServerImpl::CreateEventSensorImageThread()
{
    eventSensorImageThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorImageServiceInterfaceServerImpl::TransferEventSensorImageData, this));
}

} /* namespace adsfi */
} /* namespace ara */
