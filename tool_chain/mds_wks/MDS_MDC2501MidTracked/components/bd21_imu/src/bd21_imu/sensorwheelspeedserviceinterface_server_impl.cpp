/* *
 * CLASS: SensorWheelSpeedServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/bd21_imu/sensorwheelspeedserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
SensorWheelSpeedServiceInterfaceServerImpl::SensorWheelSpeedServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : SensorWheelSpeedServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

SensorWheelSpeedServiceInterfaceServerImpl::~SensorWheelSpeedServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorWheelSpeedServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void SensorWheelSpeedServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void SensorWheelSpeedServiceInterfaceServerImpl::EventWheelSpeedNotify()
{
    sendEventWheelSpeedCv_.notify_one();
    return;
}

bool SensorWheelSpeedServiceInterfaceServerImpl::SendEventWheelSpeedData(const SendEventWheelSpeedType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventWheelSpeed.Allocate();
    *val = *data;
    this->EventWheelSpeed.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void SensorWheelSpeedServiceInterfaceServerImpl::TransferEventWheelSpeedData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventWheelSpeedMtx_);
        std::cv_status status = sendEventWheelSpeedCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventWheelSpeedContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventWheelSpeed.Allocate();
        *val = *data;
        this->EventWheelSpeed.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void SensorWheelSpeedServiceInterfaceServerImpl::ClearEventWheelSpeedContainer()
{
    std::lock_guard<std::mutex> lck(sendEventWheelSpeedMtx_);
    eventWheelSpeedContainer_.Clear();
    return;
}

void SensorWheelSpeedServiceInterfaceServerImpl::CreateEventWheelSpeedThread()
{
    eventWheelSpeedThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&SensorWheelSpeedServiceInterfaceServerImpl::TransferEventWheelSpeedData, this));
}

} /* namespace adsfi */
} /* namespace ara */
