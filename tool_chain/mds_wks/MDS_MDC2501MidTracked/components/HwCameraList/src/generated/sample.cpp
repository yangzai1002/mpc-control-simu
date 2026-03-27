/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::hw_camera_list::HwCameraListSwC>()), workFlag_(true)
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
    
    return true;
}

void Sample::Run()
{
    MsgImageDataListServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::MsgImageDataListServiceInterfaceServerRun, this));SensorCompressedImageListServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorCompressedImageListServiceInterfaceServerRun, this));
    
}

void Sample::MsgImageDataListServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto msgImageDataListServiceInterfaceServerPortVec = swcPtr_->GetMsgImageDataListServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : msgImageDataListServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetMsgImageDataListServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventImageListDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventImageListData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}void Sample::SensorCompressedImageListServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorCompressedImageListServiceInterfaceServerPortVec = swcPtr_->GetSensorCompressedImageListServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorCompressedImageListServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorCompressedImageListServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventCompressedImageListDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventCompressedImageListData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((MsgImageDataListServiceInterfaceServerThreadPtr_ != nullptr) &&
        (MsgImageDataListServiceInterfaceServerThreadPtr_->joinable())) {
        MsgImageDataListServiceInterfaceServerThreadPtr_->join();
    }
    MsgImageDataListServiceInterfaceServerThreadPtr_ = nullptr;
    if ((SensorCompressedImageListServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorCompressedImageListServiceInterfaceServerThreadPtr_->joinable())) {
        SensorCompressedImageListServiceInterfaceServerThreadPtr_->join();
    }
    SensorCompressedImageListServiceInterfaceServerThreadPtr_ = nullptr;
    
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}


}