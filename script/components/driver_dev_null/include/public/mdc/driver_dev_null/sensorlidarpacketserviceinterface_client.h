/* *
 * Class: SensorLidarPacketServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORLIDARPACKETSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_sensorlidarpacket.h"





namespace ara {
namespace adsfi {

using EventLidarPacketHandlerType = std::function<void (ara::adsfi::SensorLidarPacket)>;
using EventLidarPacketDataType = ara::adsfi::SensorLidarPacket;
using RecvEventLidarPacketType = std::shared_ptr<EventLidarPacketDataType>;
class SensorLidarPacketServiceInterfaceClient final {
public:
    SensorLidarPacketServiceInterfaceClient() = delete;
    explicit SensorLidarPacketServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorLidarPacketServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventLidarPacketNotifyHandler(const EventLidarPacketHandlerType handler);
    void EventLidarPacketNotify();
    void EventLidarPacketContainerClear();
    bool EventLidarPacketContainerEmpty();
    RecvEventLidarPacketType GetEventLidarPacketOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventLidarPacketType GetEventLidarPacketOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventLidarPacketType> GetEventLidarPacketNdata(const size_t n);

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