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
#include "control_logger.h"
#include "mdc/control_base/control_base_swc.h"
#include "mdc/control_base/bodycommandserviceinterface_server.h"
#include "mdc/control_base/vehicleinfomationserviceinterface_client.h"
#include "mdc/control_base/faultdataserviceinterface_client.h"
#include "mdc/control_base/locationinfointf_client.h"
#include "mdc/control_base/planningresultintf_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::control::ControlLogger;
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
    void BodyCommandServiceInterfaceServerRun();
    
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
    void LocationInfoIntfClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void PlanningResultIntfClientRun();
    
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
    static void ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAdsfiPlanningResultDataHandle(const adsfi::PlanningResultDataType& data);
    
    std::unique_ptr<mdc::control_base::ControlBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> BodyCommandServiceInterfaceServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> VehicleInfomationServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> FaultDataServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationInfoIntfClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> PlanningResultIntfClientThreadPtr_{nullptr};
    
};
}
#endif