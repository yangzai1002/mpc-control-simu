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
#include "multicameradetection_logger.h"
#include "mdc/multy_camera_det_base/multy_camera_det_base_swc.h"
#include "mdc/multy_camera_det_base/visionsegmentserviceinterface_server.h"
#include "mdc/multy_camera_det_base/msgimagedatalistserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::multicameradetection::MulticameradetectionLogger;
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
    void VisionSegmentServiceInterfaceServerRun();
    
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
    static void ReceiveAraAdsfiEventImageListDataHandle(const ara::adsfi::EventImageListDataType& data);
    
    std::unique_ptr<mdc::multy_camera_det_base::MultyCameraDetBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> VisionSegmentServiceInterfaceServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> MsgImageDataListServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif