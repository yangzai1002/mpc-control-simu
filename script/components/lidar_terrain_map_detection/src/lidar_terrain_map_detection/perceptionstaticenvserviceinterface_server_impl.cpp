/* *
 * CLASS: PerceptionStaticEnvServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/lidar_terrain_map_detection/perceptionstaticenvserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
PerceptionStaticEnvServiceInterfaceServerImpl::PerceptionStaticEnvServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : PerceptionStaticEnvServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

PerceptionStaticEnvServiceInterfaceServerImpl::~PerceptionStaticEnvServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool PerceptionStaticEnvServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void PerceptionStaticEnvServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void PerceptionStaticEnvServiceInterfaceServerImpl::EventPerceptionStaticEnvNotify()
{
    sendEventPerceptionStaticEnvCv_.notify_one();
    return;
}

bool PerceptionStaticEnvServiceInterfaceServerImpl::SendEventPerceptionStaticEnvData(const SendEventPerceptionStaticEnvType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventPerceptionStaticEnv.Allocate();
    *val = *data;
    this->EventPerceptionStaticEnv.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void PerceptionStaticEnvServiceInterfaceServerImpl::TransferEventPerceptionStaticEnvData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventPerceptionStaticEnvMtx_);
        std::cv_status status = sendEventPerceptionStaticEnvCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventPerceptionStaticEnvContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventPerceptionStaticEnv.Allocate();
        *val = *data;
        this->EventPerceptionStaticEnv.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void PerceptionStaticEnvServiceInterfaceServerImpl::ClearEventPerceptionStaticEnvContainer()
{
    std::lock_guard<std::mutex> lck(sendEventPerceptionStaticEnvMtx_);
    eventPerceptionStaticEnvContainer_.Clear();
    return;
}

void PerceptionStaticEnvServiceInterfaceServerImpl::CreateEventPerceptionStaticEnvThread()
{
    eventPerceptionStaticEnvThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&PerceptionStaticEnvServiceInterfaceServerImpl::TransferEventPerceptionStaticEnvData, this));
}

} /* namespace adsfi */
} /* namespace ara */
