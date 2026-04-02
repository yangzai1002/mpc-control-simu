/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
#include "adsfi_manager/adsfi_data_manager.h"
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::lidar_det_base::LidarDetBaseSwC>()), workFlag_(true)
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
    	ApLogInterface::Instance()->Init("LidarDetection");
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
    
    return true;
}

void Sample::Run()
{
    Object3dArrayIntfServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::Object3dArrayIntfServerRun, this));
    SensorLidarPointCloudServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorLidarPointCloudServiceInterfaceClientRun, this));
    
}

void Sample::Object3dArrayIntfServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto object3dArrayIntfServerPortVec = swcPtr_->GetObject3dArrayIntfServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : object3dArrayIntfServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetObject3dArrayIntfServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
			PRE_TIMMER_SENTENCE
		auto code_tag_key = portName +"_EventPerceptionLidarObjectsData";

            auto data = std::make_shared<adsfi::EventPerceptionLidarObjectsDataType>();
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			if(AdsfiDataManager::Instance()->adsfi_interface.Process(portName,data) == 0){
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
            serverPtr->SendEventPerceptionLidarObjectsData(data);
		AVOS_MSGPACK_PUBLISH("LidarDetection",portName, "MsgHaf3dDetectionOutArray" , static_cast<void *>(data.get()));

		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			}

			END_TIMMER_SENTENCE
        }
    }
}
void Sample::SensorLidarPointCloudServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto sensorLidarPointCloudServiceInterfaceClientPortVec = swcPtr_->GetSensorLidarPointCloudServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : sensorLidarPointCloudServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetSensorLidarPointCloudServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
			PRE_TIMMER_SENTENCE
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventPointCloud2NotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventPointCloud2DataHandle,
                std::placeholders::_1));
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventPointCloud2OneDataBlocking(1000U);
			AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			if(oneDataBlocking != nullptr)
			{
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			    AdsfiDataManager::Instance()->adsfi_interface.Callback(portName, oneDataBlocking);
			    AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagSubtagId);
			}
            
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((Object3dArrayIntfServerThreadPtr_ != nullptr) &&
        (Object3dArrayIntfServerThreadPtr_->joinable())) {
        Object3dArrayIntfServerThreadPtr_->join();
    }
    Object3dArrayIntfServerThreadPtr_ = nullptr;
    
    if ((SensorLidarPointCloudServiceInterfaceClientThreadPtr_ != nullptr) &&
        (SensorLidarPointCloudServiceInterfaceClientThreadPtr_->joinable())) {
        SensorLidarPointCloudServiceInterfaceClientThreadPtr_->join();
    }
    SensorLidarPointCloudServiceInterfaceClientThreadPtr_ = nullptr;
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}

void Sample::ReceiveAraAdsfiEventPointCloud2DataHandle(const ara::adsfi::EventPointCloud2DataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventPointCloud2 data";
}

}
