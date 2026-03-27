/* *
 * Class: SensorRadarPacketServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_sensorradarpacket.h"





namespace ara {
namespace adsfi {

using EventSensorRadarPacketHandlerType = std::function<void (ara::adsfi::SensorRadarPacket)>;
using EventSensorRadarPacketDataType = ara::adsfi::SensorRadarPacket;
using RecvEventSensorRadarPacketType = std::shared_ptr<EventSensorRadarPacketDataType>;
class SensorRadarPacketServiceInterfaceClient final {
public:
    SensorRadarPacketServiceInterfaceClient() = delete;
    explicit SensorRadarPacketServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorRadarPacketServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventSensorRadarPacketNotifyHandler(const EventSensorRadarPacketHandlerType handler);
    void EventSensorRadarPacketNotify();
    void EventSensorRadarPacketContainerClear();
    bool EventSensorRadarPacketContainerEmpty();
    RecvEventSensorRadarPacketType GetEventSensorRadarPacketOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventSensorRadarPacketType GetEventSensorRadarPacketOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventSensorRadarPacketType> GetEventSensorRadarPacketNdata(const size_t n);

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