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
#include "lidardetection_logger.h"
#include "mdc/lidar_det_base/lidar_det_base_swc.h"
#include "mdc/lidar_det_base/object3darrayintf_server.h"
#include "mdc/lidar_det_base/sensorlidarpointcloudserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::lidardetection::LidardetectionLogger;
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
    void Object3dArrayIntfServerRun();
    
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorLidarPointCloudServiceInterfaceClientRun();
    
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
    
    std::unique_ptr<mdc::lidar_det_base::LidarDetBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> Object3dArrayIntfServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorLidarPointCloudServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif