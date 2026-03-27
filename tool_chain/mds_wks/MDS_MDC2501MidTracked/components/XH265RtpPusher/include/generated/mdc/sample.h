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
#include "xh265rtppusher_logger.h"
#include "mdc/xh265_rtp_pusher/xh265_rtp_pusher_swc.h"
#include "mdc/xh265_rtp_pusher/sensorimageserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::xh265rtppusher::Xh265rtppusherLogger;
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
    void SensorImageServiceInterfaceClientRun();
    
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
    static void ReceiveAraAdsfiEventSensorImageDataHandle(const ara::adsfi::EventSensorImageDataType& data);
    
    std::unique_ptr<mdc::xh265_rtp_pusher::Xh265RtpPusherSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    
    std::unique_ptr<std::thread> SensorImageServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif