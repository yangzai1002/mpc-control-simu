/* *
 * CLASS: Object3dArrayIntf server implementation
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#include "mdc/lidar_det_base/object3darrayintf_server.h"
#include "mdc/lidar_det_base/object3darrayintf_server_impl.h"

namespace adsfi {
class Object3dArrayIntfServer::Impl {
public:
    Impl(const uint32_t id,
        const ara::com::MethodCallProcessingMode& mode = ara::com::MethodCallProcessingMode::kPoll)
    {
        Object3dArrayIntfServerPtr_ = std::make_unique<Object3dArrayIntfServerImpl>(id, mode);
    }
    ~Impl() {}
    const std::unique_ptr<Object3dArrayIntfServerImpl>& GetObject3dArrayIntfServer()
    {
        return Object3dArrayIntfServerPtr_;
    }
private:
    std::unique_ptr<Object3dArrayIntfServerImpl> Object3dArrayIntfServerPtr_;
};

Object3dArrayIntfServer::Object3dArrayIntfServer(const uint32_t id)
{
    pImpl_ = std::make_unique<Object3dArrayIntfServer::Impl>(id);
}

Object3dArrayIntfServer::~Object3dArrayIntfServer()
{}

bool Object3dArrayIntfServer::Init()
{
    return pImpl_->GetObject3dArrayIntfServer()->Init();
}

void Object3dArrayIntfServer::Stop()
{
    pImpl_->GetObject3dArrayIntfServer()->Stop();
}

bool Object3dArrayIntfServer::IsStop() const
{
    return pImpl_->GetObject3dArrayIntfServer()->IsStop();
}

uint32_t Object3dArrayIntfServer::GetInstanceId() const
{
    return pImpl_->GetObject3dArrayIntfServer()->GetInstanceId();
}

/* event relative */
bool Object3dArrayIntfServer::SendEventPerceptionLidarObjectsData(const SendEventPerceptionLidarObjectsType& data)
{
    return pImpl_->GetObject3dArrayIntfServer()->SendEventPerceptionLidarObjectsData(data);
}
void Object3dArrayIntfServer::ClearEventPerceptionLidarObjectsContainer()
{
    pImpl_->GetObject3dArrayIntfServer()->ClearEventPerceptionLidarObjectsContainer();
}

} /* namespace adsfi */
