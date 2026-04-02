/* *
 * Class: SensorImuServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORIMUSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORIMUSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msghafimu.h"





namespace ara {
namespace adsfi {

using EventSensorImuDataType = ara::adsfi::MsgHafIMU;
using SendEventSensorImuType = std::shared_ptr<EventSensorImuDataType>;


class SensorImuServiceInterfaceServer final {
public:
    SensorImuServiceInterfaceServer() = delete;

    explicit SensorImuServiceInterfaceServer(const uint32_t id);

    virtual ~SensorImuServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventSensorImuData(const SendEventSensorImuType& data);
    void ClearEventSensorImuContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif