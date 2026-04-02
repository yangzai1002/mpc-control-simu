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
#include "odometry_logger.h"
#include "mdc/odometry_base/odometry_base_swc.h"
#include "mdc/odometry_base/locationinfointf_server.h"
#include "mdc/odometry_base/sensorwheelspeedserviceinterface_client.h"
#include "mdc/odometry_base/sensorimuserviceinterface_client.h"
#include "mdc/odometry_base/locationserviceinterface_client.h"
#include "mdc/odometry_base/sensorgpsserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::odometry::OdometryLogger;
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
    void LocationInfoIntfServerRun();
    
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorWheelSpeedServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorImuServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void LocationServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorGpsServiceInterfaceClientRun();
    
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
    static void ReceiveAraAdsfiEventWheelSpeedDataHandle(const ara::adsfi::EventWheelSpeedDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventSensorImuDataHandle(const ara::adsfi::EventSensorImuDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveMdcLocationLocationEventDataHandle(const mdc::location::LocationEventDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventGpsDataHandle(const ara::adsfi::EventGpsDataType& data);
    
    std::unique_ptr<mdc::odometry_base::OdometryBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> LocationInfoIntfServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorWheelSpeedServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorImuServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorGpsServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif