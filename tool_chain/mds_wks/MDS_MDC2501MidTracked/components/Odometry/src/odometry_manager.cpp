/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#include "odometry_manager.h"
#include "mdc/odometry_logger.h"

namespace mdc {
namespace odometry {
/**
* @brief 请在此方法中添加组件代码初始化逻辑，若有阻塞操作，建议开启新线程来运行阻塞操作
*
*/
bool OdometryManager::OnInitialize()
{
    samplePtr_ = std::make_unique<mdc::Sample>();
    if (!samplePtr_->Init()) {
        return false;
    }
    return true;
}

/**
* @brief 请在此方法中添加组件代码运行逻辑，若有阻塞操作，建议开启新线程来运行阻塞操作
*
*/
void OdometryManager::Run()
{
    samplePtr_->Run();
    return;
}

/**
* @brief 若组件作为CM通信服务端，请在此处停止服务
*
*/
void OdometryManager::OnTerminate()
{
    samplePtr_->Stop();
    return;
}
} /* namespace odometry */
} /* namespace mdc */