/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::fusion_semantic_map_base::FusionSemanticMapBaseSwC>()), workFlag_(true)
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
    
    SensorLidarPointCloudServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorLidarPointCloudServiceInterfaceClientRun, this));
    LocationInfoIntfClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationInfoIntfClientRun, this));
    
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
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventPointCloud2NotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventPointCloud2DataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventPointCloud2OneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventPointCloud2Ndata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventPointCloud2OneDataBlocking(1000U);
            
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

void Sample::Stop()
{
    workFlag_ = false;
    
    if ((SensorLidarPointCloudServiceInterfaceClientThreadPtr_ != nullptr) &&
        (SensorLidarPointCloudServiceInterfaceClientThreadPtr_->joinable())) {
        SensorLidarPointCloudServiceInterfaceClientThreadPtr_->join();
    }
    SensorLidarPointCloudServiceInterfaceClientThreadPtr_ = nullptr;
    if ((LocationInfoIntfClientThreadPtr_ != nullptr) &&
        (LocationInfoIntfClientThreadPtr_->joinable())) {
        LocationInfoIntfClientThreadPtr_->join();
    }
    LocationInfoIntfClientThreadPtr_ = nullptr;
    
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
void Sample::ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving LocationInfo data";
}

}