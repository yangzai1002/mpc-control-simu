/* *
 * Class: RemoteControlServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_REMOTECONTROLSERVICEINTERFACECLIENT
#define ARA_ADSFI_REMOTECONTROLSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_appremotecontrol.h"





namespace ara {
namespace adsfi {

using EventRCAppRemoteCtlHandlerType = std::function<void (ara::adsfi::AppRemoteControl)>;
using EventRCAppRemoteCtlDataType = ara::adsfi::AppRemoteControl;
using RecvEventRCAppRemoteCtlType = std::shared_ptr<EventRCAppRemoteCtlDataType>;
class RemoteControlServiceInterfaceClient final {
public:
    RemoteControlServiceInterfaceClient() = delete;
    explicit RemoteControlServiceInterfaceClient(const uint32_t instanceId);
    virtual ~RemoteControlServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventRCAppRemoteCtlNotifyHandler(const EventRCAppRemoteCtlHandlerType handler);
    void EventRCAppRemoteCtlNotify();
    void EventRCAppRemoteCtlContainerClear();
    bool EventRCAppRemoteCtlContainerEmpty();
    RecvEventRCAppRemoteCtlType GetEventRCAppRemoteCtlOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventRCAppRemoteCtlType GetEventRCAppRemoteCtlOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventRCAppRemoteCtlType> GetEventRCAppRemoteCtlNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif