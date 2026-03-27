/* *
 * Class: SensorWheelSpeedServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORWHEELSPEEDSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"





namespace ara {
namespace adsfi {

using EventWheelSpeedDataType = ara::adsfi::MsgHafWheelSpeedList;
using SendEventWheelSpeedType = std::shared_ptr<EventWheelSpeedDataType>;


class SensorWheelSpeedServiceInterfaceServer final {
public:
    SensorWheelSpeedServiceInterfaceServer() = delete;

    explicit SensorWheelSpeedServiceInterfaceServer(const uint32_t id);

    virtual ~SensorWheelSpeedServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventWheelSpeedData(const SendEventWheelSpeedType& data);
    void ClearEventWheelSpeedContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif