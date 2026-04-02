/* *
 * Class: FusionObjectArrayIntf server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ADSFI_FUSIONOBJECTARRAYINTFSERVER
#define ADSFI_FUSIONOBJECTARRAYINTFSERVER
#include <memory>

#include "ara/adsfi/impl_type_msghaffusionoutarray.h"





namespace adsfi {

using FusionObjectArrayDataType = ara::adsfi::MsgHafFusionOutArray;
using SendFusionObjectArrayType = std::shared_ptr<FusionObjectArrayDataType>;


class FusionObjectArrayIntfServer final {
public:
    FusionObjectArrayIntfServer() = delete;

    explicit FusionObjectArrayIntfServer(const uint32_t id);

    virtual ~FusionObjectArrayIntfServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendFusionObjectArrayData(const SendFusionObjectArrayType& data);
    void ClearFusionObjectArrayContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif