/* *
 * CLASS: SensorLidarPacketServiceInterface client implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/driver_dev_null/sensorlidarpacketserviceinterface_client_impl.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <cstdint>
#include "ara/core/string.h"

namespace ara {
namespace adsfi {
SensorLidarPacketServiceInterfaceClientImpl::SensorLidarPacketServiceInterfaceClientImpl(const uint32_t instanceId)
    : instanceIdx_(instanceId),
      workFlag_(true),
      findServFlag_(false),
      registerFlag_(false),
      emReport_(false)
{}

SensorLidarPacketServiceInterfaceClientImpl::~SensorLidarPacketServiceInterfaceClientImpl()
{
    if (workFlag_) {
        Stop();
    }
}

bool SensorLidarPacketServiceInterfaceClientImpl::Init()
{
    if (findServFlag_) {
        return true;
    }
    ara::core::String insIdStr = std::to_string(instanceIdx_);
    serviceHandle_ = SensorLidarPacketServiceInterfaceProxy::StartFindService([this](
        ara::com::ServiceHandleContainer<SensorLidarPacketServiceInterfaceProxy::HandleType> handles,
        const ara::com::FindServiceHandle findServiceHandle) {
            SensorLidarPacketServiceInterfaceCallback(std::move(handles), findServiceHandle);
            },
        ara::com::InstanceIdentifier(ara::core::StringView(insIdStr)));
    (void)serviceHandle_;
    const uint32_t maxTimeoutCount = 10U;
    for (uint32_t timeoutCount = 0U; timeoutCount < maxTimeoutCount; timeoutCount++) {
        if (proxyPtr_ != nullptr) {
            findServFlag_ = true;
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100U));
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Failed to find the service. instance id:" << insIdStr;
    SensorLidarPacketServiceInterfaceProxy::StopFindService(serviceHandle_);
    return false;
}

void SensorLidarPacketServiceInterfaceClientImpl::SensorLidarPacketServiceInterfaceCallback(
    ara::com::ServiceHandleContainer<SensorLidarPacketServiceInterfaceProxy::HandleType> handles,
    const ara::com::FindServiceHandle findServiceHandle)
{
    (void)findServiceHandle;
    if (proxyPtr_ != nullptr) {
        return;
    }

    for (auto singleHandle : handles) {
        ara::core::String insIdStr = std::to_string(instanceIdx_);
        if (singleHandle.GetInstanceId().ToString() == insIdStr) {
            /* 查找到指定服务实例后, 创建服务的Proxy实例 */
            std::call_once(callFlag_, [&singleHandle, this]() {
                this->proxyPtr_ = std::make_unique<SensorLidarPacketServiceInterfaceProxy>(singleHandle);
            });
            break;
        }
    }
    return;
}

void SensorLidarPacketServiceInterfaceClientImpl::Stop()
{
    workFlag_ = false;
    if (findServFlag_) {
        SensorLidarPacketServiceInterfaceProxy::StopFindService(serviceHandle_);
    }
    if (proxyPtr_ != nullptr) {
        if (proxyPtr_->EventLidarPacket.IsSubscribed()) {
            proxyPtr_->EventLidarPacket.Unsubscribe();
            proxyPtr_->EventLidarPacket.UnsetReceiveHandler();
        }
        proxyPtr_ = nullptr;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Data receive baes stoped. idx: " << instanceIdx_;
}

void SensorLidarPacketServiceInterfaceClientImpl::EmReportExec()
{
    const auto sleepInterval = std::chrono::milliseconds(5U);
    const uint8_t EM_REPORT_TIME_MAX = 3U;
    int count = 0;
    ara::core::Result<void> res;
    while (count < EM_REPORT_TIME_MAX) {
        res = execClient_.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        if (res.HasValue()) {
            break;
        } else {
            std::this_thread::sleep_for(sleepInterval);
            count++;
        }
    }
    if (count == EM_REPORT_TIME_MAX) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "EM report kRunning failed.";
    }
    return;
}

void SensorLidarPacketServiceInterfaceClientImpl::RegisterEventLidarPacketNotifyHandler(const EventLidarPacketHandlerType handler)
{
    if (proxyPtr_ == nullptr) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Failed to find the service instance.";
        return;
    }
    if (!handler) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Can't register a null handler. Otherwise, data will not be received.";
        return;
    }
    if (registerFlag_) {
        return;
    }
    auto NotifyCallback = [this, handler]() {
        (void)proxyPtr_->EventLidarPacket.GetNewSamples([this, handler](
            const ara::com::SamplePtr<EventLidarPacketDataType const> ptr) {
            const auto& notifyData = ptr.Get();
            auto data = *notifyData;
            handler(data);
            PushEventLidarPacketDataToContainer(std::move(data));
            LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "EventLidarPacket: data received. instanceId: " << instanceIdx_;
        });
    };
    proxyPtr_->EventLidarPacket.SetReceiveHandler([NotifyCallback]() { NotifyCallback(); });
    proxyPtr_->EventLidarPacket.Subscribe(recvQSize_);
    EmReportExec();
    registerFlag_ = true;
    return;
}

void SensorLidarPacketServiceInterfaceClientImpl::PushEventLidarPacketDataToContainer(EventLidarPacketDataType&& data)
{
    RecvEventLidarPacketType ptr = std::make_shared<EventLidarPacketDataType>(std::forward<EventLidarPacketDataType>(data));
    EventLidarPacketContainer_.Push(std::move(ptr));
    EventLidarPacketCv_.notify_all();
    return;
}

RecvEventLidarPacketType SensorLidarPacketServiceInterfaceClientImpl::GetEventLidarPacketOneData(const uint32_t freshDataTime)
{
    if (!workFlag_) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Data receive got stopFlag signal.";
        return nullptr;
    }
    auto dataPtr = EventLidarPacketContainer_.GetOneData(freshDataTime);
    if (dataPtr == nullptr) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Get data is nullptr.";
        return nullptr;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Get one data successfully.";
    return *dataPtr;
}

RecvEventLidarPacketType SensorLidarPacketServiceInterfaceClientImpl::GetEventLidarPacketOneDataBlocking(const uint32_t blockTimeout)
{
    std::unique_lock<std::mutex> recvLk(EventLidarPacketMtx_);
    if (EventLidarPacketCv_.wait_for(recvLk, std::chrono::milliseconds(blockTimeout)) == std::cv_status::timeout) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "No new data arrived since this api called. timeout(ms): " <<
            blockTimeout;
        return nullptr;
    }
    if (!workFlag_) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogWarn() << "Data receive got stopFlag.";
        return nullptr;
    }
    auto dataPtr = EventLidarPacketContainer_.GetOneData();
    recvLk.unlock();
    if (dataPtr == nullptr) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogError() << "Get one data in block mode is nullptr.";
        return nullptr;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Get one data in block mode successfully.";
    return *dataPtr;
}

std::vector<RecvEventLidarPacketType> SensorLidarPacketServiceInterfaceClientImpl::GetEventLidarPacketNdata(const size_t n)
{
    if (!workFlag_) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogWarn() << "Data receive got stopFlag";
        return {};
    }

    auto result = EventLidarPacketContainer_.GetNdata(n);

    std::vector<RecvEventLidarPacketType> output;
    output.reserve(result.size());

    for (auto& it : result) {
        if (it != nullptr) {
            output.push_back(*it);
        }
    }
    if (output.size() < n) {
        LOG_SPACE::GetLoggerIns("CLNT")->LogWarn()
            << "Failed to obtain " << n << " data records, because the length of the received data is "<< output.size();
        return output;
    }
    LOG_SPACE::GetLoggerIns("CLNT")->LogInfo() << "Get " << n << " data successfully.";
    return output;
}
} /* namespace adsfi */
} /* namespace ara */
