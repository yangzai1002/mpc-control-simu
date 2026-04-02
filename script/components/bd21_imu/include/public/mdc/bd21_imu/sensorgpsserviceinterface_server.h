/* *
 * Class: SensorGpsServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORGPSSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORGPSSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msghafgnssinfo.h"





namespace ara {
namespace adsfi {

using EventGpsDataType = ara::adsfi::MsgHafGnssInfo;
using SendEventGpsType = std::shared_ptr<EventGpsDataType>;


class SensorGpsServiceInterfaceServer final {
public:
    SensorGpsServiceInterfaceServer() = delete;

    explicit SensorGpsServiceInterfaceServer(const uint32_t id);

    virtual ~SensorGpsServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventGpsData(const SendEventGpsType& data);
    void ClearEventGpsContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif