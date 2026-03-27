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
#include "xvehiclestatusrecorder_logger.h"
#include "mdc/xvehicle_status_recorder_base/xvehicle_status_recorder_base_swc.h"
#include "mdc/xvehicle_status_recorder_base/remotecontrolserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/bodycommandserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/locationinfointf_client.h"
#include "mdc/xvehicle_status_recorder_base/sensorgpsserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/vehicleinfomationserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/faultdataserviceinterface_client.h"
#include "mdc/xvehicle_status_recorder_base/msgimagedatalistserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::xvehiclestatusrecorder::XvehiclestatusrecorderLogger;
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
    void RemoteControlServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void BodyCommandServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void LocationInfoIntfClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void SensorGpsServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void VehicleInfomationServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void FaultDataServiceInterfaceClientRun();
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
    static void ReceiveAraAdsfiEventRCAppRemoteCtlDataHandle(const ara::adsfi::EventRCAppRemoteCtlDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventVehicleActControlDataHandle(const ara::adsfi::EventVehicleActControlDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventGpsDataHandle(const ara::adsfi::EventGpsDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventFaultDataDataHandle(const ara::adsfi::EventFaultDataDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventImageListDataHandle(const ara::adsfi::EventImageListDataType& data);
    
    std::unique_ptr<mdc::xvehicle_status_recorder_base::XvehicleStatusRecorderBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    
    std::unique_ptr<std::thread> RemoteControlServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> BodyCommandServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationInfoIntfClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> SensorGpsServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> VehicleInfomationServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> FaultDataServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> MsgImageDataListServiceInterfaceClientThreadPtr_{nullptr};
    
};
}
#endif