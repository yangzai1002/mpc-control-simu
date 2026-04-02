/* *
 * Class: SensorImageServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORIMAGESERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORIMAGESERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msgimagedata.h"





namespace ara {
namespace adsfi {

using EventSensorImageHandlerType = std::function<void (ara::adsfi::MsgImageData)>;
using EventSensorImageDataType = ara::adsfi::MsgImageData;
using RecvEventSensorImageType = std::shared_ptr<EventSensorImageDataType>;
class SensorImageServiceInterfaceClient final {
public:
    SensorImageServiceInterfaceClient() = delete;
    explicit SensorImageServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorImageServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventSensorImageNotifyHandler(const EventSensorImageHandlerType handler);
    void EventSensorImageNotify();
    void EventSensorImageContainerClear();
    bool EventSensorImageContainerEmpty();
    RecvEventSensorImageType GetEventSensorImageOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventSensorImageType GetEventSensorImageOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventSensorImageType> GetEventSensorImageNdata(const size_t n);

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