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
#include "vehiclechassissend_logger.h"
#include "mdc/mid_track_chassis_send/mid_track_chassis_send_swc.h"
#include "mdc/mid_track_chassis_send/faultdataserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::vehiclechassissend::VehiclechassissendLogger;
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
     * @brief 客户端接收数据
     *
     */
    void FaultDataServiceInterfaceClientRun();
    
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
    static void ReceiveAraAdsfiEventFaultDataDataHandle(const ara::adsfi::EventFaultDataDataType& data);
    
    std::unique_ptr<mdc::mid_track_chassis_send::MidTrackChassisSendSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    
    std::unique_ptr<std::thread> FaultDataServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif