/* *
 * CLASS: SensorCompressedImageListServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_camera_list/sensorcompressedimagelistserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorCompressedImageListServiceInterfaceServerImpl::SensorCompressedImageListServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorCompressedImageListServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorCompressedImageListServiceInterfaceServerImpl::~SensorCompressedImageListServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorCompressedImageListServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorCompressedImageListServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorCompressedImageListServiceInterfaceServerImpl::EventCompressedImageListNotify()
{
    sendEventCompressedImageListCv_.notify_one();
    return;
}

bool SensorCompressedImageListServiceInterfaceServerImpl::SendEventCompressedImageListData(const SendEventCompressedImageListType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventCompressedImageList.Allocate();
    *val = *data;
    this->EventCompressedImageList.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorCompressedImageListServiceInterfaceServerImpl::TransferEventCompressedImageListData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventCompressedImageListMtx_);
        std::cv_status status = sendEventCompressedImageListCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventCompressedImageListContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventCompressedImageList.Allocate();
        *val = *data;
        this->EventCompressedImageList.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorCompressedImageListServiceInterfaceServerImpl::ClearEventCompressedImageListContainer()
{
    std::lock_guard<std::mutex> lck(sendEventCompressedImageListMtx_);
    eventCompressedImageListContainer_.Clear();
    return;
}

void SensorCompressedImageListServiceInterfaceServerImpl::CreateEventCompressedImageListThread()
{
    eventCompressedImageListThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorCompressedImageListServiceInterfaceServerImpl::TransferEventCompressedImageListData, this));
}

} /* namespace adsfi */
} /* namespace ara */
