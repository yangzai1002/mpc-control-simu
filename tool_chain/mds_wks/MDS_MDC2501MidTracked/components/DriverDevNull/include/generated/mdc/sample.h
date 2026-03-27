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
#include "driverdevnull_logger.h"
#include "mdc/driver_dev_null/driver_dev_null_swc.h"
#include "mdc/driver_dev_null/sensorcompressedimagelistserviceinterface_client.h"
#include "mdc/driver_dev_null/sensorradarpacketserviceinterface_client.h"
#include "mdc/driver_dev_null/sensorlidarpacketserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::driverdevnull::DriverdevnullLogger;
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
    void SensorCompressedImageListServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorRadarPacketServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorLidarPacketServiceInterfaceClientRun();
    
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
    static void ReceiveAraAdsfiEventCompressedImageListDataHandle(const ara::adsfi::EventCompressedImageListDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventSensorRadarPacketDataHandle(const ara::adsfi::EventSensorRadarPacketDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventLidarPacketDataHandle(const ara::adsfi::EventLidarPacketDataType& data);
    
    std::unique_ptr<mdc::driver_dev_null::DriverDevNullSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    
    std::unique_ptr<std::thread> SensorCompressedImageListServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorRadarPacketServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorLidarPacketServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif