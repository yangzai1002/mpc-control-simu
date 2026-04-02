/* *
 * Class: BagStatusServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_BAGSTATUSSERVICEINTERFACESERVER
#define ARA_ADSFI_BAGSTATUSSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_faultdata.h"





namespace ara {
namespace adsfi {

using EventBagStatusDataType = ara::adsfi::FaultData;
using SendEventBagStatusType = std::shared_ptr<EventBagStatusDataType>;


class BagStatusServiceInterfaceServer final {
public:
    BagStatusServiceInterfaceServer() = delete;

    explicit BagStatusServiceInterfaceServer(const uint32_t id);

    virtual ~BagStatusServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventBagStatusData(const SendEventBagStatusType& data);
    void ClearEventBagStatusContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif