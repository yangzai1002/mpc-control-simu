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
#include "bd21imu_logger.h"
#include "mdc/bd21_imu/bd21_imu_swc.h"
#include "mdc/bd21_imu/locationserviceinterface_server.h"
#include "mdc/bd21_imu/sensorwheelspeedserviceinterface_server.h"
#include "mdc/bd21_imu/sensorgpsserviceinterface_server.h"
#include "mdc/bd21_imu/sensorimuserviceinterface_server.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::bd21imu::Bd21imuLogger;
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
    void LocationServiceInterfaceServerRun();
    
    /**
     * @brief 服务端发送数据
     *
     */
    void SensorWheelSpeedServiceInterfaceServerRun();
    
    /**
     * @brief 服务端发送数据
     *
     */
    void SensorGpsServiceInterfaceServerRun();
    
    /**
     * @brief 服务端发送数据
     *
     */
    void SensorImuServiceInterfaceServerRun();
    
    
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

    
    std::unique_ptr<mdc::bd21_imu::Bd21ImuSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> LocationServiceInterfaceServerThreadPtr_{nullptr};std::unique_ptr<std::thread> SensorWheelSpeedServiceInterfaceServerThreadPtr_{nullptr};std::unique_ptr<std::thread> SensorGpsServiceInterfaceServerThreadPtr_{nullptr};std::unique_ptr<std::thread> SensorImuServiceInterfaceServerThreadPtr_{nullptr};
    
};
}
#endif