/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::bd21_imu::Bd21ImuSwC>()), workFlag_(true)
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
    LocationServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationServiceInterfaceServerRun, this));SensorWheelSpeedServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorWheelSpeedServiceInterfaceServerRun, this));SensorGpsServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorGpsServiceInterfaceServerRun, this));SensorImuServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorImuServiceInterfaceServerRun, this));
    
}

void Sample::LocationServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto locationServiceInterfaceServerPortVec = swcPtr_->GetLocationServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : locationServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetLocationServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<mdc::location::LocationEventDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendLocationEventData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}void Sample::SensorWheelSpeedServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorWheelSpeedServiceInterfaceServerPortVec = swcPtr_->GetSensorWheelSpeedServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorWheelSpeedServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorWheelSpeedServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventWheelSpeedDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventWheelSpeedData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}void Sample::SensorGpsServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorGpsServiceInterfaceServerPortVec = swcPtr_->GetSensorGpsServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorGpsServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorGpsServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventGpsDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventGpsData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}void Sample::SensorImuServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorImuServiceInterfaceServerPortVec = swcPtr_->GetSensorImuServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorImuServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorImuServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventSensorImuDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventSensorImuData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((LocationServiceInterfaceServerThreadPtr_ != nullptr) &&
        (LocationServiceInterfaceServerThreadPtr_->joinable())) {
        LocationServiceInterfaceServerThreadPtr_->join();
    }
    LocationServiceInterfaceServerThreadPtr_ = nullptr;
    if ((SensorWheelSpeedServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorWheelSpeedServiceInterfaceServerThreadPtr_->joinable())) {
        SensorWheelSpeedServiceInterfaceServerThreadPtr_->join();
    }
    SensorWheelSpeedServiceInterfaceServerThreadPtr_ = nullptr;
    if ((SensorGpsServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorGpsServiceInterfaceServerThreadPtr_->joinable())) {
        SensorGpsServiceInterfaceServerThreadPtr_->join();
    }
    SensorGpsServiceInterfaceServerThreadPtr_ = nullptr;
    if ((SensorImuServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorImuServiceInterfaceServerThreadPtr_->joinable())) {
        SensorImuServiceInterfaceServerThreadPtr_->join();
    }
    SensorImuServiceInterfaceServerThreadPtr_ = nullptr;
    
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}


}