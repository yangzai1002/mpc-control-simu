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
#include "fusionfollow_logger.h"
#include "mdc/fusion_follow_base/fusion_follow_base_swc.h"
#include "mdc/fusion_follow_base/fusionobjectarrayintf_server.h"
#include "mdc/fusion_follow_base/sensorlidarpointcloudserviceinterface_client.h"
#include "mdc/fusion_follow_base/radarpointcloudserviceinterface_client.h"
#include "mdc/fusion_follow_base/locationinfointf_client.h"
#include "mdc/fusion_follow_base/object3darrayintf_client.h"
#include "mdc/fusion_follow_base/visionsegmentserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::fusionfollow::FusionfollowLogger;
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
    void FusionObjectArrayIntfServerRun();
    
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorLidarPointCloudServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void RadarPointCloudServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void LocationInfoIntfClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void Object3dArrayIntfClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void VisionSegmentServiceInterfaceClientRun();
    
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
    static void ReceiveAraAdsfiEventRadarPointCloudDataHandle(const ara::adsfi::EventRadarPointCloudDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAdsfiEventPerceptionLidarObjectsDataHandle(const adsfi::EventPerceptionLidarObjectsDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventVisionSegmentDataHandle(const ara::adsfi::EventVisionSegmentDataType& data);
    
    std::unique_ptr<mdc::fusion_follow_base::FusionFollowBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> FusionObjectArrayIntfServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorLidarPointCloudServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> RadarPointCloudServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationInfoIntfClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> Object3dArrayIntfClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> VisionSegmentServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif