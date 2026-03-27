/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::planning_base::PlanningBaseSwC>()), workFlag_(true)
{}

Sample::~Sample()
{
    if (workFlag_) {
        Stop();
    }
}

bool Sample::InitHandle()
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Some operations during initialization.";
    return true;
}

void Sample::StopHandle()
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Some operations during stopping";
    return;
}

bool Sample::Init()
{
    SPL_LOG_SPACE::InitAppLogging();
    SPL_LOG_SPACE::InitLoggerCtx("SPL", "sample log contex");
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to create the SWC object.";
        return false;
    }
    swcPtr_->SetInitCallback(std::bind(&Sample::InitHandle));
    swcPtr_->SetStopCallback(std::bind(&Sample::StopHandle));
    if (!swcPtr_->Init()) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "SWC initialization failed.";
        return false;
    }
	//LOAD_ALL_CONFIG
	//AdsfiDataManager::Instance()->adsfi_interface.Init();

    plan_result = std::make_shared<adsfi::PlanningResultDataType>();
   	//AdsfiDataManager::Instance()->adsfi_interface.Init();

   	std::ifstream file("/opt/usr/zjy/path.log");  // 路径自己改
   	    if (!file.is_open()) {
   	        printf("open path.log failed");
   	        return false;
   	    }
   	    std::string line;
   	    bool first_point = false;
   	    double first_x = 0;
   	    double first_y = 0;
   	    while (std::getline(file, line)) {
   	        if (line.empty()) continue;
   	        double x, y;
   	        // 按逗号分割
   	        if (sscanf(line.c_str(), "%lf,%lf", &x, &y) == 2)
   	        {
   	            // 创建轨迹点
   	        	if(!first_point)
   	        	{
   	        		first_x = x;
   	        		first_y = y;
   	        		first_point = true;
   	        	}

   	        	ara::adsfi::MsgHafWayPoint point;
   	            point.x = x - first_x;
   	            point.y = y - first_y;

//   	            point.x = x;
//   	            point.y = y;
   	            // 添加到结果
   	            plan_result->wayPoints.push_back(point);
   	        }
   	 }
   	  /*最后一个点是第一个点绝对utm坐标，平移使用*/
   	 ara::adsfi::MsgHafWayPoint point;
   	 point.x = first_x;
   	 point.y = first_y;
   	 plan_result->wayPoints.push_back(point);
    return true;
}

void Sample::Run()
{
    PlanningResultIntfServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::PlanningResultIntfServerRun, this));
    PerceptionStaticEnvServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::PerceptionStaticEnvServiceInterfaceClientRun, this));
    FusionObjectArrayIntfClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::FusionObjectArrayIntfClientRun, this));
    LocationInfoIntfClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationInfoIntfClientRun, this));
    VehicleInfomationServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::VehicleInfomationServiceInterfaceClientRun, this));
    
}

void Sample::PlanningResultIntfServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto planningResultIntfServerPortVec = swcPtr_->GetPlanningResultIntfServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : planningResultIntfServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetPlanningResultIntfServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
	        serverPtr->SendPlanningResultData(plan_result);
			printf("send planing trajectory!!!\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
            //auto data = std::make_shared<adsfi::PlanningResultDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
          //  serverPtr->SendPlanningResultData(data);
          //  std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}
void Sample::PerceptionStaticEnvServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto perceptionStaticEnvServiceInterfaceClientPortVec = swcPtr_->GetPerceptionStaticEnvServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : perceptionStaticEnvServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetPerceptionStaticEnvServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventPerceptionStaticEnvNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventPerceptionStaticEnvDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventPerceptionStaticEnvOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventPerceptionStaticEnvNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventPerceptionStaticEnvOneDataBlocking(1000U);
            
        }
    }
}
void Sample::FusionObjectArrayIntfClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto fusionObjectArrayIntfClientPortVec = swcPtr_->GetFusionObjectArrayIntfClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : fusionObjectArrayIntfClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetFusionObjectArrayIntfClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterFusionObjectArrayNotifyHandler(
                std::bind(&Sample::ReceiveAdsfiFusionObjectArrayDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetFusionObjectArrayOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetFusionObjectArrayNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetFusionObjectArrayOneDataBlocking(1000U);
            
        }
    }
}
void Sample::LocationInfoIntfClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto locationInfoIntfClientPortVec = swcPtr_->GetLocationInfoIntfClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : locationInfoIntfClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetLocationInfoIntfClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterLocationInfoNotifyHandler(
                std::bind(&Sample::ReceiveAdsfiLocationInfoDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetLocationInfoOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetLocationInfoNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetLocationInfoOneDataBlocking(1000U);
            
        }
    }
}
void Sample::VehicleInfomationServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto vehicleInfomationServiceInterfaceClientPortVec = swcPtr_->GetVehicleInfomationServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : vehicleInfomationServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetVehicleInfomationServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventVehicleInfoNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventVehicleInfoOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventVehicleInfoNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventVehicleInfoOneDataBlocking(1000U);
            
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((PlanningResultIntfServerThreadPtr_ != nullptr) &&
        (PlanningResultIntfServerThreadPtr_->joinable())) {
        PlanningResultIntfServerThreadPtr_->join();
    }
    PlanningResultIntfServerThreadPtr_ = nullptr;
    
    if ((PerceptionStaticEnvServiceInterfaceClientThreadPtr_ != nullptr) &&
        (PerceptionStaticEnvServiceInterfaceClientThreadPtr_->joinable())) {
        PerceptionStaticEnvServiceInterfaceClientThreadPtr_->join();
    }
    PerceptionStaticEnvServiceInterfaceClientThreadPtr_ = nullptr;
    if ((FusionObjectArrayIntfClientThreadPtr_ != nullptr) &&
        (FusionObjectArrayIntfClientThreadPtr_->joinable())) {
        FusionObjectArrayIntfClientThreadPtr_->join();
    }
    FusionObjectArrayIntfClientThreadPtr_ = nullptr;
    if ((LocationInfoIntfClientThreadPtr_ != nullptr) &&
        (LocationInfoIntfClientThreadPtr_->joinable())) {
        LocationInfoIntfClientThreadPtr_->join();
    }
    LocationInfoIntfClientThreadPtr_ = nullptr;
    if ((VehicleInfomationServiceInterfaceClientThreadPtr_ != nullptr) &&
        (VehicleInfomationServiceInterfaceClientThreadPtr_->joinable())) {
        VehicleInfomationServiceInterfaceClientThreadPtr_->join();
    }
    VehicleInfomationServiceInterfaceClientThreadPtr_ = nullptr;
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}

void Sample::ReceiveAraAdsfiEventPerceptionStaticEnvDataHandle(const ara::adsfi::EventPerceptionStaticEnvDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventPerceptionStaticEnv data";
}
void Sample::ReceiveAdsfiFusionObjectArrayDataHandle(const adsfi::FusionObjectArrayDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving FusionObjectArray data";
}
void Sample::ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving LocationInfo data";
}
void Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventVehicleInfo data";
}

}
