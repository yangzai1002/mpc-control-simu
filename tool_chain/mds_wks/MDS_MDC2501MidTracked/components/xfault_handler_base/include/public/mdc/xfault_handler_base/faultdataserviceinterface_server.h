/* *
 * Class: FaultDataServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_FAULTDATASERVICEINTERFACESERVER
#define ARA_ADSFI_FAULTDATASERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_faultdata.h"





namespace ara {
namespace adsfi {

using EventFaultDataDataType = ara::adsfi::FaultData;
using SendEventFaultDataType = std::shared_ptr<EventFaultDataDataType>;


class FaultDataServiceInterfaceServer final {
public:
    FaultDataServiceInterfaceServer() = delete;

    explicit FaultDataServiceInterfaceServer(const uint32_t id);

    virtual ~FaultDataServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventFaultDataData(const SendEventFaultDataType& data);
    void ClearEventFaultDataContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif