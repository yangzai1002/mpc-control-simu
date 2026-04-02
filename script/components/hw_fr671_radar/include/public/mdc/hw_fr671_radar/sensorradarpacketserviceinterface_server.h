/* *
 * Class: SensorRadarPacketServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_sensorradarpacket.h"





namespace ara {
namespace adsfi {

using EventSensorRadarPacketDataType = ara::adsfi::SensorRadarPacket;
using SendEventSensorRadarPacketType = std::shared_ptr<EventSensorRadarPacketDataType>;


class SensorRadarPacketServiceInterfaceServer final {
public:
    SensorRadarPacketServiceInterfaceServer() = delete;

    explicit SensorRadarPacketServiceInterfaceServer(const uint32_t id);

    virtual ~SensorRadarPacketServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventSensorRadarPacketData(const SendEventSensorRadarPacketType& data);
    void ClearEventSensorRadarPacketContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif