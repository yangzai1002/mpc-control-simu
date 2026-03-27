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
#include "sensorvisualize_logger.h"
#include "mdc/sensor_viualize_base/sensor_viualize_base_swc.h"
#include "mdc/sensor_viualize_base/sensorlidarpointcloudserviceinterface_client.h"
#include "mdc/sensor_viualize_base/msgimagedatalistserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::sensorvisualize::SensorvisualizeLogger;
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
    void SensorLidarPointCloudServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void MsgImageDataListServiceInterfaceClientRun();
    
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
    static void ReceiveAraAdsfiEventPointCloud2DataHandle(const ara::adsfi::EventPointCloud2DataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventImageListDataHandle(const ara::adsfi::EventImageListDataType& data);
    
    std::unique_ptr<mdc::sensor_viualize_base::SensorViualizeBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    
    std::unique_ptr<std::thread> SensorLidarPointCloudServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> MsgImageDataListServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif