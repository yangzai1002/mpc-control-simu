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
#include "hwl106lidarfront_logger.h"
#include "mdc/hw_l106_lidar/hw_l106_lidar_swc.h"
#include "mdc/hw_l106_lidar/sensorlidarpointcloudserviceinterface_server.h"
#include "mdc/hw_l106_lidar/sensorlidarpacketserviceinterface_server.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::hwl106lidarfront::Hwl106lidarfrontLogger;
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
    void SensorLidarPointCloudServiceInterfaceServerRun();
    
    /**
     * @brief 服务端发送数据
     *
     */
    void SensorLidarPacketServiceInterfaceServerRun();
    
    
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

    
    std::unique_ptr<mdc::hw_l106_lidar::HwL106LidarSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> SensorLidarPointCloudServiceInterfaceServerThreadPtr_{nullptr};std::unique_ptr<std::thread> SensorLidarPacketServiceInterfaceServerThreadPtr_{nullptr};
    
};
}
#endif