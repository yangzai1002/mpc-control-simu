/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::hw_fr671_radar::HwFr671RadarSwC>()), workFlag_(true)
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
    RadarPointCloudServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::RadarPointCloudServiceInterfaceServerRun, this));SensorRadarPacketServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorRadarPacketServiceInterfaceServerRun, this));
    LocationServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationServiceInterfaceClientRun, this));
    VehicleInfomationServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::VehicleInfomationServiceInterfaceClientRun, this));
    
}

void Sample::RadarPointCloudServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto radarPointCloudServiceInterfaceServerPortVec = swcPtr_->GetRadarPointCloudServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : radarPointCloudServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetRadarPointCloudServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventRadarPointCloudDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventRadarPointCloudData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}void Sample::SensorRadarPacketServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto sensorRadarPacketServiceInterfaceServerPortVec = swcPtr_->GetSensorRadarPacketServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : sensorRadarPacketServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetSensorRadarPacketServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventSensorRadarPacketDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventSensorRadarPacketData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}
void Sample::LocationServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto locationServiceInterfaceClientPortVec = swcPtr_->GetLocationServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : locationServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetLocationServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterLocationEventNotifyHandler(
                std::bind(&Sample::ReceiveMdcLocationLocationEventDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetLocationEventOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetLocationEventNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetLocationEventOneDataBlocking(1000U);
            
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
    if ((RadarPointCloudServiceInterfaceServerThreadPtr_ != nullptr) &&
        (RadarPointCloudServiceInterfaceServerThreadPtr_->joinable())) {
        RadarPointCloudServiceInterfaceServerThreadPtr_->join();
    }
    RadarPointCloudServiceInterfaceServerThreadPtr_ = nullptr;
    if ((SensorRadarPacketServiceInterfaceServerThreadPtr_ != nullptr) &&
        (SensorRadarPacketServiceInterfaceServerThreadPtr_->joinable())) {
        SensorRadarPacketServiceInterfaceServerThreadPtr_->join();
    }
    SensorRadarPacketServiceInterfaceServerThreadPtr_ = nullptr;
    
    if ((LocationServiceInterfaceClientThreadPtr_ != nullptr) &&
        (LocationServiceInterfaceClientThreadPtr_->joinable())) {
        LocationServiceInterfaceClientThreadPtr_->join();
    }
    LocationServiceInterfaceClientThreadPtr_ = nullptr;
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

void Sample::ReceiveMdcLocationLocationEventDataHandle(const mdc::location::LocationEventDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving LocationEvent data";
}
void Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventVehicleInfo data";
}

}