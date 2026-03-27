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
#include "planning_logger.h"
#include "mdc/planning_base/planning_base_swc.h"
#include "mdc/planning_base/planningresultintf_server.h"
#include "mdc/planning_base/perceptionstaticenvserviceinterface_client.h"
#include "mdc/planning_base/fusionobjectarrayintf_client.h"
#include "mdc/planning_base/locationinfointf_client.h"
#include "mdc/planning_base/vehicleinfomationserviceinterface_client.h"

namespace mdc {
namespace {
using SPL_LOG_SPACE = mdc::planning::PlanningLogger;
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
    void PlanningResultIntfServerRun();
    
    /**
     * @brief 客户端接收数据
     *
     */
    void PerceptionStaticEnvServiceInterfaceClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void FusionObjectArrayIntfClientRun();
    /**
     * @brief 客户端接收数据
     *
     */
    void LocationInfoIntfClientRun();
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
    static void ReceiveAraAdsfiEventPerceptionStaticEnvDataHandle(const ara::adsfi::EventPerceptionStaticEnvDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAdsfiFusionObjectArrayDataHandle(const adsfi::FusionObjectArrayDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data);
    /**
     * @brief 用户在接收到数据时的自定义操作
     *
     */
    static void ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data);
    
    std::unique_ptr<mdc::planning_base::PlanningBaseSwC> swcPtr_;
    std::atomic<bool> workFlag_;
    std::unique_ptr<std::thread> PlanningResultIntfServerThreadPtr_{nullptr};
    std::unique_ptr<std::thread> PerceptionStaticEnvServiceInterfaceClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> FusionObjectArrayIntfClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> LocationInfoIntfClientThreadPtr_{nullptr};
    std::unique_ptr<std::thread> VehicleInfomationServiceInterfaceClientThreadPtr_{nullptr};
    std::shared_ptr<adsfi::PlanningResultDataType> plan_result;
};
}
#endif
