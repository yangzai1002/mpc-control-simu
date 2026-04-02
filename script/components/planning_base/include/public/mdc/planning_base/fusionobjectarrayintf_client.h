/* *
 * Class: FusionObjectArrayIntf client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ADSFI_FUSIONOBJECTARRAYINTFCLIENT
#define ADSFI_FUSIONOBJECTARRAYINTFCLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msghaffusionoutarray.h"





namespace adsfi {

using FusionObjectArrayHandlerType = std::function<void (ara::adsfi::MsgHafFusionOutArray)>;
using FusionObjectArrayDataType = ara::adsfi::MsgHafFusionOutArray;
using RecvFusionObjectArrayType = std::shared_ptr<FusionObjectArrayDataType>;
class FusionObjectArrayIntfClient final {
public:
    FusionObjectArrayIntfClient() = delete;
    explicit FusionObjectArrayIntfClient(const uint32_t instanceId);
    virtual ~FusionObjectArrayIntfClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterFusionObjectArrayNotifyHandler(const FusionObjectArrayHandlerType handler);
    void FusionObjectArrayNotify();
    void FusionObjectArrayContainerClear();
    bool FusionObjectArrayContainerEmpty();
    RecvFusionObjectArrayType GetFusionObjectArrayOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvFusionObjectArrayType GetFusionObjectArrayOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvFusionObjectArrayType> GetFusionObjectArrayNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */

#endif