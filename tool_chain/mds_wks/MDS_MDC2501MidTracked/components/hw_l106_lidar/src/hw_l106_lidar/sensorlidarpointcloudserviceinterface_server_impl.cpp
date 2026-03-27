/* *
 * CLASS: SensorLidarPointCloudServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/hw_l106_lidar/sensorlidarpointcloudserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorLidarPointCloudServiceInterfaceServerImpl::SensorLidarPointCloudServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorLidarPointCloudServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorLidarPointCloudServiceInterfaceServerImpl::~SensorLidarPointCloudServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorLidarPointCloudServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorLidarPointCloudServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorLidarPointCloudServiceInterfaceServerImpl::EventPointCloud2Notify()
{
    sendEventPointCloud2Cv_.notify_one();
    return;
}

bool SensorLidarPointCloudServiceInterfaceServerImpl::SendEventPointCloud2Data(const SendEventPointCloud2Type& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventPointCloud2.Allocate();
    *val = *data;
    this->EventPointCloud2.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorLidarPointCloudServiceInterfaceServerImpl::TransferEventPointCloud2Data()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventPointCloud2Mtx_);
        std::cv_status status = sendEventPointCloud2Cv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventPointCloud2Container_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventPointCloud2.Allocate();
        *val = *data;
        this->EventPointCloud2.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorLidarPointCloudServiceInterfaceServerImpl::ClearEventPointCloud2Container()
{
    std::lock_guard<std::mutex> lck(sendEventPointCloud2Mtx_);
    eventPointCloud2Container_.Clear();
    return;
}

void SensorLidarPointCloudServiceInterfaceServerImpl::CreateEventPointCloud2Thread()
{
    eventPointCloud2ThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorLidarPointCloudServiceInterfaceServerImpl::TransferEventPointCloud2Data, this));
}

} /* namespace adsfi */
} /* namespace ara */
