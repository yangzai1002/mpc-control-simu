/* *
 * Class: RemoteControlServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_REMOTECONTROLSERVICEINTERFACESERVER
#define ARA_ADSFI_REMOTECONTROLSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_appremotecontrol.h"





namespace ara {
namespace adsfi {

using EventRCAppRemoteCtlDataType = ara::adsfi::AppRemoteControl;
using SendEventRCAppRemoteCtlType = std::shared_ptr<EventRCAppRemoteCtlDataType>;


class RemoteControlServiceInterfaceServer final {
public:
    RemoteControlServiceInterfaceServer() = delete;

    explicit RemoteControlServiceInterfaceServer(const uint32_t id);

    virtual ~RemoteControlServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventRCAppRemoteCtlData(const SendEventRCAppRemoteCtlType& data);
    void ClearEventRCAppRemoteCtlContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif