/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
#include "adsfi_manager/adsfi_data_manager.h"
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::mid_track_chassis_send::MidTrackChassisSendSwC>()), workFlag_(true)
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
    	ApLogInterface::Instance()->Init("VehicleChassisSend");
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
    
    FaultDataServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::FaultDataServiceInterfaceClientRun, this));
    
}


void Sample::FaultDataServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto faultDataServiceInterfaceClientPortVec = swcPtr_->GetFaultDataServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : faultDataServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetFaultDataServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
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
}

void Sample::Stop()
{
    workFlag_ = false;
    
    if ((FaultDataServiceInterfaceClientThreadPtr_ != nullptr) &&
        (FaultDataServiceInterfaceClientThreadPtr_->joinable())) {
        FaultDataServiceInterfaceClientThreadPtr_->join();
    }
    FaultDataServiceInterfaceClientThreadPtr_ = nullptr;
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}

void Sample::ReceiveAraAdsfiEventFaultDataDataHandle(const ara::adsfi::EventFaultDataDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventFaultData data";
}

}
