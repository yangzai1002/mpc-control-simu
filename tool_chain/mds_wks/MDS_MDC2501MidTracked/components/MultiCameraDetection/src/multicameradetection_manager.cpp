/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#include "multicameradetection_manager.h"
#include "mdc/multicameradetection_logger.h"

namespace mdc {
namespace multicameradetection {
/**
* @brief 请在此方法中添加组件代码初始化逻辑，若有阻塞操作，建议开启新线程来运行阻塞操作
*
*/
bool MulticameradetectionManager::OnInitialize()
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
void MulticameradetectionManager::Run()
{
    samplePtr_->Run();
    return;
}

/**
* @brief 若组件作为CM通信服务端，请在此处停止服务
*
*/
void MulticameradetectionManager::OnTerminate()
{
    samplePtr_->Stop();
    return;
}
} /* namespace multicameradetection */
} /* namespace mdc */