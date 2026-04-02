/* *
 * Class: PerceptionStaticEnvServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACESERVER
#define ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_perceptionstaticenv.h"





namespace ara {
namespace adsfi {

using EventPerceptionStaticEnvDataType = ara::adsfi::PerceptionStaticEnv;
using SendEventPerceptionStaticEnvType = std::shared_ptr<EventPerceptionStaticEnvDataType>;


class PerceptionStaticEnvServiceInterfaceServer final {
public:
    PerceptionStaticEnvServiceInterfaceServer() = delete;

    explicit PerceptionStaticEnvServiceInterfaceServer(const uint32_t id);

    virtual ~PerceptionStaticEnvServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventPerceptionStaticEnvData(const SendEventPerceptionStaticEnvType& data);
    void ClearEventPerceptionStaticEnvContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif