/* *
 * Class: LocationInfoIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_LOCATIONINFOINTFSERVER
#define ADSFI_LOCATIONINFOINTFSERVER
#include <memory>

#include "ara/adsfi/impl_type_msghaflocation.h"





namespace adsfi {

using LocationInfoDataType = ara::adsfi::MsgHafLocation;
using SendLocationInfoType = std::shared_ptr<LocationInfoDataType>;


class LocationInfoIntfServer final {
public:
    LocationInfoIntfServer() = delete;

    explicit LocationInfoIntfServer(const uint32_t id);

    virtual ~LocationInfoIntfServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendLocationInfoData(const SendLocationInfoType& data);
    void ClearLocationInfoContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif