/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
#include "adsfi_manager/adsfi_data_manager.h"
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::hw_l106_lidar::HwL106LidarSwC>()), workFlag_(true)
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
    	ApLogInterface::Instance()->Init("HwL106LidarFront");
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
    SensorLidarPointCloudServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorLidarPointCloudServiceInterfaceServerRun, this));SensorLidarPacketServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorLidarPacketServiceInterfaceServerRun, this));
    
}

void Sample::SensorLidarPointCloudServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorLidarPointCloudServiceInterfaceServerPortVec = swcPtr_->GetSensorLidarPointCloudServiceInterfaceServerVec();
		pthread_setname_np(pthread_self(),"Process:SensorLidarPointCloud");

    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorLidarPointCloudServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorLidarPointCloudServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
			PRE_TIMMER_SENTENCE
		auto code_tag_key = portName +"_EventPointCloud2Data";

            auto data = std::make_shared<ara::adsfi::EventPointCloud2DataType>();
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			if(AdsfiDataManager::Instance()->adsfi_interface.Process(portName,data) == 0){
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
            serverPtr->SendEventPointCloud2Data(data);
		AVOS_MSGPACK_PUBLISH("HwL106LidarFront",portName, "MsgLidarFrame" , static_cast<void *>(data.get()));

		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			}

			END_TIMMER_SENTENCE
        }
    }
}void Sample::SensorLidarPacketServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorLidarPacketServiceInterfaceServerPortVec = swcPtr_->GetSensorLidarPacketServiceInterfaceServerVec();
		pthread_setname_np(pthread_self(),"Process:SensorLidarPacket");

    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorLidarPacketServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorLidarPacketServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
			PRE_TIMMER_SENTENCE
		auto code_tag_key = portName +"_EventLidarPacketData";

            auto data = std::make_shared<ara::adsfi::EventLidarPacketDataType>();
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			if(AdsfiDataManager::Instance()->adsfi_interface.Process(portName,data) == 0){
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
            serverPtr->SendEventLidarPacketData(data);
		AVOS_MSGPACK_PUBLISH("HwL106LidarFront",portName, "SensorLidarPacket" , static_cast<void *>(data.get()));

		AVOS_CODETAG_ADV1(code_tag_key.c_str(), 0, eAvosCodetagPubtagId);
			}

			END_TIMMER_SENTENCE
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((SensorLidarPointCloudServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorLidarPointCloudServiceInterfaceServerThreadPtr_->joinable())) {
        SensorLidarPointCloudServiceInterfaceServerThreadPtr_->join();
    }
    SensorLidarPointCloudServiceInterfaceServerThreadPtr_ = nullptr;
    if ((SensorLidarPacketServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorLidarPacketServiceInterfaceServerThreadPtr_->joinable())) {
        SensorLidarPacketServiceInterfaceServerThreadPtr_->join();
    }
    SensorLidarPacketServiceInterfaceServerThreadPtr_ = nullptr;
    
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}


}
