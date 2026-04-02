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
#include "hwfr671radarfront_logger.h"
#include "mdc/hw_fr671_radar/hw_fr671_radar_swc.h"
#include "mdc/hw_fr671_radar/radarpointcloudserviceinterface_server.h"
#include "mdc/hw_fr671_radar/sensorradarpacketserviceinterface_server.h"
#include "mdc/hw_fr671_radar/locationserviceinterface_client.h"
#include "mdc/hw_fr671_radar/vehicleinfomationserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::hwfr671radarfront::Hwfr671radarfrontLogger;
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
    void RadarPointCloudServiceInterfaceServerRun();
    
    /**
     * @brief 服务端发送数据
     *
     */
    void SensorRadarPacketServiceInterfaceServerRun();
    
    /**
     * @brief 客户端接收数据
     *
     */
    void LocationServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void VehicleInfomationServiceInterfaceClientRun();
    
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
    static void ReceiveMdcLocationLocationEventDataHandle(const mdc::location::LocationEventDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data);
    
    std::unique_ptr<mdc::hw_fr671_radar::HwFr671RadarSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> RadarPointCloudServiceInterfaceServerThreadPtr_{nullptr};std::unique_ptr<std::thread> SensorRadarPacketServiceInterfaceServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> VehicleInfomationServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif