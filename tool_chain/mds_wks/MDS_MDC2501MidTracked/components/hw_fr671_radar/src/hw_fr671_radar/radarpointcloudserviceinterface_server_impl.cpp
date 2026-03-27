/* *
 * CLASS: RadarPointCloudServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_fr671_radar/radarpointcloudserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
RadarPointCloudServiceInterfaceServerImpl::RadarPointCloudServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : RadarPointCloudServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

RadarPointCloudServiceInterfaceServerImpl::~RadarPointCloudServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool RadarPointCloudServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void RadarPointCloudServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void RadarPointCloudServiceInterfaceServerImpl::EventRadarPointCloudNotify()
{
    sendEventRadarPointCloudCv_.notify_one();
    return;
}

bool RadarPointCloudServiceInterfaceServerImpl::SendEventRadarPointCloudData(const SendEventRadarPointCloudType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventRadarPointCloud.Allocate();
    *val = *data;
    this->EventRadarPointCloud.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void RadarPointCloudServiceInterfaceServerImpl::TransferEventRadarPointCloudData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventRadarPointCloudMtx_);
        std::cv_status status = sendEventRadarPointCloudCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventRadarPointCloudContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventRadarPointCloud.Allocate();
        *val = *data;
        this->EventRadarPointCloud.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void RadarPointCloudServiceInterfaceServerImpl::ClearEventRadarPointCloudContainer()
{
    std::lock_guard<std::mutex> lck(sendEventRadarPointCloudMtx_);
    eventRadarPointCloudContainer_.Clear();
    return;
}

void RadarPointCloudServiceInterfaceServerImpl::CreateEventRadarPointCloudThread()
{
    eventRadarPointCloudThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&RadarPointCloudServiceInterfaceServerImpl::TransferEventRadarPointCloudData, this));
}

} /* namespace adsfi */
} /* namespace ara */
