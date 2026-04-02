/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#ifndef MDC_SAMPLE_H
#define MDC_SAMPLE_H
#include <memory>
#include <iostream>
#include <atomic>
#include <thread>
#include "xfaulthandler_logger.h"
#include "mdc/xfault_handler_base/xfault_handler_base_swc.h"
#include "mdc/xfault_handler_base/faultdataserviceinterface_server.h"
#include "mdc/xfault_handler_base/remotecontrolserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::xfaulthandler::XfaulthandlerLogger;
}
class Sample {
public:
    Sample();
    ~Sample();

    /**
     * @brief 初始化swc、服务端和客户端
     *
     */
    bool Init();
    /**
     * @brief 运行服务端和客户端
     *
     */
    void Run();
    /**
     * @brief 停止接收/发送数据
     *
     */
    void Stop();
private:
    
    /**
     * @brief 服务端发送数据
     *
     */
    void FaultDataServiceInterfaceServerRun();
    
    /**
     * @brief 客户端接收数据
     *
     */
    void RemoteControlServiceInterfaceClientRun();
    
    /**
     * @brief 用户在初始化过程中自定义操作
     *
     */
    static bool InitHandle();

    /**
     * @brief 用户在停止服务时自定义操作
     *
     */
    static void StopHandle();

    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventRCAppRemoteCtlDataHandle(const ara::adsfi::EventRCAppRemoteCtlDataType& data);
    
    std::unique_ptr<mdc::xfault_handler_base::XfaultHandlerBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> FaultDataServiceInterfaceServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> RemoteControlServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif