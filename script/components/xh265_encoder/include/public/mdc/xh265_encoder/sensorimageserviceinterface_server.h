/* *
 * Class: SensorImageServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORIMAGESERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORIMAGESERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msgimagedata.h"





namespace ara {
namespace adsfi {

using EventSensorImageDataType = ara::adsfi::MsgImageData;
using SendEventSensorImageType = std::shared_ptr<EventSensorImageDataType>;


class SensorImageServiceInterfaceServer final {
public:
    SensorImageServiceInterfaceServer() = delete;

    explicit SensorImageServiceInterfaceServer(const uint32_t id);

    virtual ~SensorImageServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventSensorImageData(const SendEventSensorImageType& data);
    void ClearEventSensorImageContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif