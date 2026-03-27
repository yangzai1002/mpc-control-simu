/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::odometry_base::OdometryBaseSwC>()), workFlag_(true)
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
    LocationInfoIntfServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationInfoIntfServerRun, this));
    SensorWheelSpeedServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorWheelSpeedServiceInterfaceClientRun, this));
    SensorImuServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorImuServiceInterfaceClientRun, this));
    LocationServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationServiceInterfaceClientRun, this));
    SensorGpsServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorGpsServiceInterfaceClientRun, this));
    
}

void Sample::LocationInfoIntfServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto locationInfoIntfServerPortVec = swcPtr_->GetLocationInfoIntfServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : locationInfoIntfServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetLocationInfoIntfServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<adsfi::LocationInfoDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendLocationInfoData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }
}
void Sample::SensorWheelSpeedServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto sensorWheelSpeedServiceInterfaceClientPortVec = swcPtr_->GetSensorWheelSpeedServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : sensorWheelSpeedServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetSensorWheelSpeedServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventWheelSpeedNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventWheelSpeedDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventWheelSpeedOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventWheelSpeedNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventWheelSpeedOneDataBlocking(1000U);
            
        }
    }
}
void Sample::SensorImuServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto sensorImuServiceInterfaceClientPortVec = swcPtr_->GetSensorImuServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : sensorImuServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetSensorImuServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventSensorImuNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventSensorImuDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventSensorImuOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventSensorImuNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventSensorImuOneDataBlocking(1000U);
            
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
void Sample::SensorGpsServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto sensorGpsServiceInterfaceClientPortVec = swcPtr_->GetSensorGpsServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : sensorGpsServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetSensorGpsServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventGpsNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventGpsDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventGpsOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventGpsNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventGpsOneDataBlocking(1000U);
            
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    if ((LocationInfoIntfServerThreadPtr_ != nullptr) &&
        (LocationInfoIntfServerThreadPtr_->joinable())) {
        LocationInfoIntfServerThreadPtr_->join();
    }
    LocationInfoIntfServerThreadPtr_ = nullptr;
    
    if ((SensorWheelSpeedServiceInterfaceClientThreadPtr_ != nullptr) &&
        (SensorWheelSpeedServiceInterfaceClientThreadPtr_->joinable())) {
        SensorWheelSpeedServiceInterfaceClientThreadPtr_->join();
    }
    SensorWheelSpeedServiceInterfaceClientThreadPtr_ = nullptr;
    if ((SensorImuServiceInterfaceClientThreadPtr_ != nullptr) &&
        (SensorImuServiceInterfaceClientThreadPtr_->joinable())) {
        SensorImuServiceInterfaceClientThreadPtr_->join();
    }
    SensorImuServiceInterfaceClientThreadPtr_ = nullptr;
    if ((LocationServiceInterfaceClientThreadPtr_ != nullptr) &&
        (LocationServiceInterfaceClientThreadPtr_->joinable())) {
        LocationServiceInterfaceClientThreadPtr_->join();
    }
    LocationServiceInterfaceClientThreadPtr_ = nullptr;
    if ((SensorGpsServiceInterfaceClientThreadPtr_ != nullptr) &&
        (SensorGpsServiceInterfaceClientThreadPtr_->joinable())) {
        SensorGpsServiceInterfaceClientThreadPtr_->join();
    }
    SensorGpsServiceInterfaceClientThreadPtr_ = nullptr;
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}

void Sample::ReceiveAraAdsfiEventWheelSpeedDataHandle(const ara::adsfi::EventWheelSpeedDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventWheelSpeed data";
}
void Sample::ReceiveAraAdsfiEventSensorImuDataHandle(const ara::adsfi::EventSensorImuDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventSensorImu data";
}
void Sample::ReceiveMdcLocationLocationEventDataHandle(const mdc::location::LocationEventDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving LocationEvent data";
}
void Sample::ReceiveAraAdsfiEventGpsDataHandle(const ara::adsfi::EventGpsDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventGps data";
}

}