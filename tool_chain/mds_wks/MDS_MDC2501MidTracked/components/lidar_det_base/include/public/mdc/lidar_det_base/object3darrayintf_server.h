/* *
 * Class: Object3dArrayIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_OBJECT3DARRAYINTFSERVER
#define ADSFI_OBJECT3DARRAYINTFSERVER
#include <memory>

#include "ara/adsfi/impl_type_msghaf3ddetectionoutarray.h"





namespace adsfi {

using EventPerceptionLidarObjectsDataType = ara::adsfi::MsgHaf3dDetectionOutArray;
using SendEventPerceptionLidarObjectsType = std::shared_ptr<EventPerceptionLidarObjectsDataType>;


class Object3dArrayIntfServer final {
public:
    Object3dArrayIntfServer() = delete;

    explicit Object3dArrayIntfServer(const uint32_t id);

    virtual ~Object3dArrayIntfServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventPerceptionLidarObjectsData(const SendEventPerceptionLidarObjectsType& data);
    void ClearEventPerceptionLidarObjectsContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif