/* *
 * Class: PerceptionStaticEnvServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACECLIENT
#define ARA_ADSFI_PERCEPTIONSTATICENVSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_perceptionstaticenv.h"





namespace ara {
namespace adsfi {

using EventPerceptionStaticEnvHandlerType = std::function<void (ara::adsfi::PerceptionStaticEnv)>;
using EventPerceptionStaticEnvDataType = ara::adsfi::PerceptionStaticEnv;
using RecvEventPerceptionStaticEnvType = std::shared_ptr<EventPerceptionStaticEnvDataType>;
class PerceptionStaticEnvServiceInterfaceClient final {
public:
    PerceptionStaticEnvServiceInterfaceClient() = delete;
    explicit PerceptionStaticEnvServiceInterfaceClient(const uint32_t instanceId);
    virtual ~PerceptionStaticEnvServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventPerceptionStaticEnvNotifyHandler(const EventPerceptionStaticEnvHandlerType handler);
    void EventPerceptionStaticEnvNotify();
    void EventPerceptionStaticEnvContainerClear();
    bool EventPerceptionStaticEnvContainerEmpty();
    RecvEventPerceptionStaticEnvType GetEventPerceptionStaticEnvOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventPerceptionStaticEnvType GetEventPerceptionStaticEnvOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventPerceptionStaticEnvType> GetEventPerceptionStaticEnvNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif