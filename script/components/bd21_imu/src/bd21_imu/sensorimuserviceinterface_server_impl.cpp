/* *
 * CLASS: SensorImuServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/sensorimuserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorImuServiceInterfaceServerImpl::SensorImuServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorImuServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorImuServiceInterfaceServerImpl::~SensorImuServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorImuServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorImuServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorImuServiceInterfaceServerImpl::EventSensorImuNotify()
{
    sendEventSensorImuCv_.notify_one();
    return;
}

bool SensorImuServiceInterfaceServerImpl::SendEventSensorImuData(const SendEventSensorImuType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventSensorImu.Allocate();
    *val = *data;
    this->EventSensorImu.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorImuServiceInterfaceServerImpl::TransferEventSensorImuData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventSensorImuMtx_);
        std::cv_status status = sendEventSensorImuCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventSensorImuContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventSensorImu.Allocate();
        *val = *data;
        this->EventSensorImu.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorImuServiceInterfaceServerImpl::ClearEventSensorImuContainer()
{
    std::lock_guard<std::mutex> lck(sendEventSensorImuMtx_);
    eventSensorImuContainer_.Clear();
    return;
}

void SensorImuServiceInterfaceServerImpl::CreateEventSensorImuThread()
{
    eventSensorImuThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorImuServiceInterfaceServerImpl::TransferEventSensorImuData, this));
}

} /* namespace adsfi */
} /* namespace ara */
