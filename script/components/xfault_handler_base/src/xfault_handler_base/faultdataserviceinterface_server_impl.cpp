/* *
 * CLASS: FaultDataServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/xfault_handler_base/faultdataserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
FaultDataServiceInterfaceServerImpl::FaultDataServiceInterfaceServerImpl(const uint32_t id,
    const ara::com::MethodCallProcessingMode& mode)
    : FaultDataServiceInterfaceSkeleton(InstanceIdentifier(ara::core::StringView(std::to_string(id).c_str())),
          mode),
      instanceId_(id),
      workFlag_(true),
      offerServFlag_(false)
{}

FaultDataServiceInterfaceServerImpl::~FaultDataServiceInterfaceServerImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool FaultDataServiceInterfaceServerImpl::Init()
{
    if (offerServFlag_) {
        return true;
    }

    this->OfferService();
    offerServFlag_ = true;
    return true;
}

void FaultDataServiceInterfaceServerImpl::Stop()
{
    workFlag_ = false;
    if (offerServFlag_) {
        this->StopOfferService();
    }
    offerServFlag_ = false;
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Data send baes stops offer service. idx: " << instanceId_;
}


void FaultDataServiceInterfaceServerImpl::EventFaultDataNotify()
{
    sendEventFaultDataCv_.notify_one();
    return;
}

bool FaultDataServiceInterfaceServerImpl::SendEventFaultDataData(const SendEventFaultDataType& data)
{
    if (!data) {
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "The input data is empty";
        return false;
    }
    auto val = this->EventFaultData.Allocate();
    *val = *data;
    this->EventFaultData.Send(std::move(val));
    LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                                               << GetInstanceId();
    return true;
}

void FaultDataServiceInterfaceServerImpl::TransferEventFaultDataData()
{
    while (workFlag_) {
        std::unique_lock<std::mutex> lck(sendEventFaultDataMtx_);
        std::cv_status status = sendEventFaultDataCv_.wait_for(lck, std::chrono::seconds(1U));
        if (!workFlag_) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Got StopFlag! Sending function return";
            return;
        }
        if (status == std::cv_status::timeout) {
            LOG_SPACE::GetLoggerIns("SERV")->LogWarn() << "Timeout while waiting for condition variable";
            continue;
        }

        auto data = *eventFaultDataContainer_.Pop();
        if (!data) {
            LOG_SPACE::GetLoggerIns("SERV")->LogError() << "The data to be sent is empty";
            continue;
        }
        lck.unlock();

        auto val = this->EventFaultData.Allocate();
        *val = *data;
        this->EventFaultData.Send(std::move(val));
        LOG_SPACE::GetLoggerIns("SERV")->LogInfo() << "Skeleton send object success! Instance id: "
                  << GetInstanceId();
    }
}

void FaultDataServiceInterfaceServerImpl::ClearEventFaultDataContainer()
{
    std::lock_guard<std::mutex> lck(sendEventFaultDataMtx_);
    eventFaultDataContainer_.Clear();
    return;
}

void FaultDataServiceInterfaceServerImpl::CreateEventFaultDataThread()
{
    eventFaultDataThreadPtr_ = std::make_unique<std::thread>(
        std::bind(&FaultDataServiceInterfaceServerImpl::TransferEventFaultDataData, this));
}

} /* namespace adsfi */
} /* namespace ara */
