/* *
 * Class: LocationServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef MDC_LOCATION_LOCATIONSERVICEINTERFACESERVER
#define MDC_LOCATION_LOCATIONSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msghaflocation.h"





namespace mdc {
namespace location {

using LocationEventDataType = ara::adsfi::MsgHafLocation;
using SendLocationEventType = std::shared_ptr<LocationEventDataType>;


class LocationServiceInterfaceServer final {
public:
    LocationServiceInterfaceServer() = delete;

    explicit LocationServiceInterfaceServer(const uint32_t id);

    virtual ~LocationServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendLocationEventData(const SendLocationEventType& data);
    void ClearLocationEventContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace location */
} /* namespace mdc */

#endif