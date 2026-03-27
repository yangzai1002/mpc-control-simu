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
#include "fusionsemanticmap_logger.h"
#include "mdc/fusion_semantic_map_base/fusion_semantic_map_base_swc.h"
#include "mdc/fusion_semantic_map_base/sensorlidarpointcloudserviceinterface_client.h"
#include "mdc/fusion_semantic_map_base/locationinfointf_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::fusionsemanticmap::FusionsemanticmapLogger;
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
    void LocationInfoIntfClientRun();
    
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
    static void ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data);
    
    std::unique_ptr<mdc::fusion_semantic_map_base::FusionSemanticMapBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    
    std::unique_ptr<std::thread> SensorLidarPointCloudServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationInfoIntfClientThreadPtr_{nullptr};
    
};
}
#endif