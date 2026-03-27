/* *
 * CLASS: PerceptionStaticEnvServiceInterface server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/lidar_terrain_map_detection/perceptionstaticenvserviceinterface_server.h"
#include "mdc/lidar_terrain_map_detection/perceptionstaticenvserviceinterface_server_impl.h"

namespace ara {
namespace adsfi {
class PerceptionStaticEnvServiceInterfaceServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        PerceptionStaticEnvServiceInterfaceServerPtr_ = std::make_unique<PerceptionStaticEnvServiceInterfaceServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<PerceptionStaticEnvServiceInterfaceServerImpl>& GetPerceptionStaticEnvServiceInterfaceServer()
    {
        return PerceptionStaticEnvServiceInterfaceServerPtr_;
    }
private:
    std::unique_ptr<PerceptionStaticEnvServiceInterfaceServerImpl> PerceptionStaticEnvServiceInterfaceServerPtr_;
};

PerceptionStaticEnvServiceInterfaceServer::PerceptionStaticEnvServiceInterfaceServer(const uint32_t id)
{
    pImpl_ = std::make_unique<PerceptionStaticEnvServiceInterfaceServer::Impl>(id);
}

PerceptionStaticEnvServiceInterfaceServer::~PerceptionStaticEnvServiceInterfaceServer()
{}

bool PerceptionStaticEnvServiceInterfaceServer::Init()
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceServer()->Init();
}

void PerceptionStaticEnvServiceInterfaceServer::Stop()
{
    pImpl_->GetPerceptionStaticEnvServiceInterfaceServer()->Stop();
}

bool PerceptionStaticEnvServiceInterfaceServer::IsStop() const
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceServer()->IsStop();
}

uint32_t PerceptionStaticEnvServiceInterfaceServer::GetInstanceId() const
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceServer()->GetInstanceId();
}

/* event relative */
bool PerceptionStaticEnvServiceInterfaceServer::SendEventPerceptionStaticEnvData(const SendEventPerceptionStaticEnvType& data)
{
    return pImpl_->GetPerceptionStaticEnvServiceInterfaceServer()->SendEventPerceptionStaticEnvData(data);
}
void PerceptionStaticEnvServiceInterfaceServer::ClearEventPerceptionStaticEnvContainer()
{
    pImpl_->GetPerceptionStaticEnvServiceInterfaceServer()->ClearEventPerceptionStaticEnvContainer();
}

} /* namespace adsfi */
} /* namespace ara */
