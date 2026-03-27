/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
#include "adsfi_manager/adsfi_data_manager.h"
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::control_base::ControlBaseSwC>()), workFlag_(true)
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
	AdsfiDataManager::Instance()->adsfi_interface.DeInit();
    return;
}

bool Sample::Init()
{
    	ApLogInterface::Instance()->Init("Control");
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
	AdsfiDataManager::Instance()->adsfi_interface.Init();
    
    return true;
}

void Sample::Run()
{
    BodyCommandServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::BodyCommandServiceInterfaceServerRun, this));
    VehicleInfomationServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::VehicleInfomationServiceInterfaceClientRun, this));
    FaultDataServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::FaultDataServiceInterfaceClientRun, this));
    LocationInfoIntfClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationInfoIntfClientRun, this));
    PlanningResultIntfClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::PlanningResultIntfClientRun, this));
    
}

void Sample::BodyCommandServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto bodyCommandServiceInterfaceServerPortVec = swcPtr_->GetBodyCommandServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : bodyCommandServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetBodyCommandServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
			PRE_TIMMER_SENTENCE
		auto code_tag_key = portName +"_EventVehicleActControlData";

            auto data = std::make_shared<ara::adsfi::EventVehicleActControlDataType>();
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			if(AdsfiDataManager::Instance()->adsfi_interface.Process(portName,data) == 0){
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
            serverPtr->SendEventVehicleActControlData(data);
		AVOS_MSGPACK_PUBLISH("Control",portName, "VehicleActControl" , static_cast<void *>(data.get()));

		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			}

			END_TIMMER_SENTENCE
        }
    }
}
void Sample::VehicleInfomationServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
	std::map<std::string , std::shared_ptr<control_base::VehicleInfomationServiceInterfaceClient>> client_map;
    auto ClientPortVec = swcPtr_->GetVehicleInfomationServiceInterfaceClientVec();
    size_t port_name_size = ClientPortVec.size();
    pthread_setname_np(pthread_self(), ClientPortVec.back().c_str());
    if(port_name_size> 1){
        for(size_t i = 0; i < port_name_size-1; ++i){
            std::thread([this, ClientPortVec, i](){
                pthread_setname_np(pthread_self(), ClientPortVec[i].c_str());
                std::map<std::string , std::shared_ptr<control_base::VehicleInfomationServiceInterfaceClient>> client_map;
                while(workFlag_){
                    auto portName = ClientPortVec[i];
                    
                    
                    auto code_tag_key = portName+"_VehicleInfomationServiceInterface";
                                        auto it = client_map.find(portName);
                    if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        client_map[portName] = swcPtr_->GetVehicleInfomationServiceInterfaceClient(portName);

                    }
                    auto clientPtr=client_map[portName];

                    /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
                    if (!clientPtr) {
                        client_map[portName] = swcPtr_->GetVehicleInfomationServiceInterfaceClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        continue;
                    }


			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventVehicleInfoNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventVehicleInfoOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}

				}
			}).detach();
		}
	}

	while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
            auto portName = ClientPortVec.back();
            auto code_tag_key = portName+"_VehicleInfomationServiceInterface";           
            auto it = client_map.find(portName);
            if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                client_map[portName] = swcPtr_->GetVehicleInfomationServiceInterfaceClient(portName);

            }
            auto clientPtr=client_map[portName];

            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            if (!clientPtr) {
                client_map[portName] = swcPtr_->GetVehicleInfomationServiceInterfaceClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                 SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                 continue;
            }



			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventVehicleInfoNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventVehicleInfoOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}


		}
}

void Sample::FaultDataServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
	std::map<std::string , std::shared_ptr<control_base::FaultDataServiceInterfaceClient>> client_map;
    auto ClientPortVec = swcPtr_->GetFaultDataServiceInterfaceClientVec();
    size_t port_name_size = ClientPortVec.size();
    pthread_setname_np(pthread_self(), ClientPortVec.back().c_str());
    if(port_name_size> 1){
        for(size_t i = 0; i < port_name_size-1; ++i){
            std::thread([this, ClientPortVec, i](){
                pthread_setname_np(pthread_self(), ClientPortVec[i].c_str());
                std::map<std::string , std::shared_ptr<control_base::FaultDataServiceInterfaceClient>> client_map;
                while(workFlag_){
                    auto portName = ClientPortVec[i];
                    
                    
                    auto code_tag_key = portName+"_FaultDataServiceInterface";
                                        auto it = client_map.find(portName);
                    if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        client_map[portName] = swcPtr_->GetFaultDataServiceInterfaceClient(portName);

                    }
                    auto clientPtr=client_map[portName];

                    /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
                    if (!clientPtr) {
                        client_map[portName] = swcPtr_->GetFaultDataServiceInterfaceClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        continue;
                    }


			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventFaultDataNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventFaultDataDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventFaultDataOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}

				}
			}).detach();
		}
	}

	while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
            auto portName = ClientPortVec.back();
            auto code_tag_key = portName+"_FaultDataServiceInterface";           
            auto it = client_map.find(portName);
            if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                client_map[portName] = swcPtr_->GetFaultDataServiceInterfaceClient(portName);

            }
            auto clientPtr=client_map[portName];

            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            if (!clientPtr) {
                client_map[portName] = swcPtr_->GetFaultDataServiceInterfaceClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                 SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                 continue;
            }



			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventFaultDataNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventFaultDataDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventFaultDataOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}


		}
}
void Sample::PlanningResultIntfClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
	std::map<std::string , std::shared_ptr<control_base::PlanningResultIntfClient>> client_map;
    auto ClientPortVec = swcPtr_->GetPlanningResultIntfClientVec();
    size_t port_name_size = ClientPortVec.size();
    pthread_setname_np(pthread_self(), ClientPortVec.back().c_str());
    if(port_name_size> 1){
        for(size_t i = 0; i < port_name_size-1; ++i){
            std::thread([this, ClientPortVec, i](){
                pthread_setname_np(pthread_self(), ClientPortVec[i].c_str());
                std::map<std::string , std::shared_ptr<control_base::PlanningResultIntfClient>> client_map;
                while(workFlag_){
                    auto portName = ClientPortVec[i];

                    auto code_tag_key = portName+"_PlanningTrajectoryServiceInterface";
                                        auto it = client_map.find(portName);
                    if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        client_map[portName] = swcPtr_->GetPlanningResultIntfClient(portName);

                    }
                    auto clientPtr=client_map[portName];

                    /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
                    if (!clientPtr) {
                        client_map[portName] = swcPtr_->GetPlanningResultIntfClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        continue;
                    }


			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterPlanningResultNotifyHandler(
                std::bind(&Sample::ReceiveAdsfiPlanningResultDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetPlanningResultOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}

				}
			}).detach();
		}
	}

	while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
            auto portName = ClientPortVec.back();
            auto code_tag_key = portName+"_PlanningTrajectoryServiceInterface";           
            auto it = client_map.find(portName);
            if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                client_map[portName] = swcPtr_->GetPlanningResultIntfClient(portName);

            }
            auto clientPtr=client_map[portName];

            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            if (!clientPtr) {
                client_map[portName] = swcPtr_->GetPlanningResultIntfClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                 SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                 continue;
            }



			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterPlanningResultNotifyHandler(
                std::bind(&Sample::ReceiveAdsfiPlanningResultDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetPlanningResultOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}


		}
}
void Sample::LocationInfoIntfClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
	std::map<std::string , std::shared_ptr<control_base::LocationInfoIntfClient>> client_map;
    auto ClientPortVec = swcPtr_->GetLocationInfoIntfClientVec();
    size_t port_name_size = ClientPortVec.size();
    pthread_setname_np(pthread_self(), ClientPortVec.back().c_str());
    if(port_name_size> 1){
        for(size_t i = 0; i < port_name_size-1; ++i){

            std::thread([this, ClientPortVec, i](){
                pthread_setname_np(pthread_self(), ClientPortVec[i].c_str());
                std::map<std::string , std::shared_ptr<control_base::LocationInfoIntfClient>> client_map;
                while(workFlag_){
                    auto portName = ClientPortVec[i];
                    std::cout <<"portName:"<<portName<<std::endl;
                    
                    auto code_tag_key = portName+"_LocationInfoIntf";
                                        auto it = client_map.find(portName);
                    if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        client_map[portName] = swcPtr_->GetLocationInfoIntfClient(portName);

                    }
                    auto clientPtr=client_map[portName];

                    /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
                    if (!clientPtr) {
                        client_map[portName] = swcPtr_->GetLocationInfoIntfClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        continue;
                    }


			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterLocationInfoNotifyHandler(
                std::bind(&Sample::ReceiveAdsfiLocationInfoDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetLocationInfoOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}

				}
			}).detach();
		}
	}

	while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
            auto portName = ClientPortVec.back();
            auto code_tag_key = portName+"_LocationInfoIntf";           
            auto it = client_map.find(portName);
            if (it == client_map.end() ) {
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                client_map[portName] = swcPtr_->GetLocationInfoIntfClient(portName);

            }
            auto clientPtr=client_map[portName];

            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            if (!clientPtr) {
                client_map[portName] = swcPtr_->GetLocationInfoIntfClient(portName);
                        AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
                 SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                 continue;
            }



			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterLocationInfoNotifyHandler(
                std::bind(&Sample::ReceiveAdsfiLocationInfoDataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetLocationInfoOneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}


		}
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((BodyCommandServiceInterfaceServerThreadPtr_ != nullptr) &&
        (BodyCommandServiceInterfaceServerThreadPtr_->joinable())) {
        BodyCommandServiceInterfaceServerThreadPtr_->join();
    }
    BodyCommandServiceInterfaceServerThreadPtr_ = nullptr;
    
    if ((VehicleInfomationServiceInterfaceClientThreadPtr_ != nullptr) &&
        (VehicleInfomationServiceInterfaceClientThreadPtr_->joinable())) {
        VehicleInfomationServiceInterfaceClientThreadPtr_->join();
    }
    VehicleInfomationServiceInterfaceClientThreadPtr_ = nullptr;
    if ((FaultDataServiceInterfaceClientThreadPtr_ != nullptr) &&
        (FaultDataServiceInterfaceClientThreadPtr_->joinable())) {
        FaultDataServiceInterfaceClientThreadPtr_->join();
    }
    FaultDataServiceInterfaceClientThreadPtr_ = nullptr;
    if ((LocationInfoIntfClientThreadPtr_ != nullptr) &&
        (LocationInfoIntfClientThreadPtr_->joinable())) {
        LocationInfoIntfClientThreadPtr_->join();
    }
    LocationInfoIntfClientThreadPtr_ = nullptr;
    if ((PlanningResultIntfClientThreadPtr_ != nullptr) &&
        (PlanningResultIntfClientThreadPtr_->joinable())) {
        PlanningResultIntfClientThreadPtr_->join();
    }
    PlanningResultIntfClientThreadPtr_ = nullptr;
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}

void Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventVehicleInfo data";
}
void Sample::ReceiveAraAdsfiEventFaultDataDataHandle(const ara::adsfi::EventFaultDataDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventFaultData data";
}
void Sample::ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving LocationInfo data";
}
void Sample::ReceiveAdsfiPlanningResultDataHandle(const adsfi::PlanningResultDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving PlanningResult data";
}

}
