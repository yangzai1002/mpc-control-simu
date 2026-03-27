/* *
 * Class: SensorLidarPacketServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_sensorlidarpacket.h"





namespace ara {
namespace adsfi {

using EventLidarPacketDataType = ara::adsfi::SensorLidarPacket;
using SendEventLidarPacketType = std::shared_ptr<EventLidarPacketDataType>;


class SensorLidarPacketServiceInterfaceServer final {
public:
    SensorLidarPacketServiceInterfaceServer() = delete;

    explicit SensorLidarPacketServiceInterfaceServer(const uint32_t id);

    virtual ~SensorLidarPacketServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventLidarPacketData(const SendEventLidarPacketType& data);
    void ClearEventLidarPacketContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif