/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::xvehicle_status_recorder_base::XvehicleStatusRecorderBaseSwC>()), workFlag_(true)
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
    
    RemoteControlServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::RemoteControlServiceInterfaceClientRun, this));
    BodyCommandServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::BodyCommandServiceInterfaceClientRun, this));
    LocationInfoIntfClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::LocationInfoIntfClientRun, this));
    SensorGpsServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::SensorGpsServiceInterfaceClientRun, this));
    VehicleInfomationServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::VehicleInfomationServiceInterfaceClientRun, this));
    FaultDataServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::FaultDataServiceInterfaceClientRun, this));
    MsgImageDataListServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::MsgImageDataListServiceInterfaceClientRun, this));
    
}


void Sample::RemoteControlServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto remoteControlServiceInterfaceClientPortVec = swcPtr_->GetRemoteControlServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : remoteControlServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetRemoteControlServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventRCAppRemoteCtlNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventRCAppRemoteCtlDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventRCAppRemoteCtlOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventRCAppRemoteCtlNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventRCAppRemoteCtlOneDataBlocking(1000U);
            
        }
    }
}
void Sample::BodyCommandServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto bodyCommandServiceInterfaceClientPortVec = swcPtr_->GetBodyCommandServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : bodyCommandServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetBodyCommandServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventVehicleActControlNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventVehicleActControlDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventVehicleActControlOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventVehicleActControlNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventVehicleActControlOneDataBlocking(1000U);
            
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
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventFaultDataNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventFaultDataDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventFaultDataOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventFaultDataNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventFaultDataOneDataBlocking(1000U);
            
        }
    }
}
void Sample::MsgImageDataListServiceInterfaceClientRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的接收端口，获取到的端口名对应MMC上本应用配置的接收端口名 */
    auto msgImageDataListServiceInterfaceClientPortVec = swcPtr_->GetMsgImageDataListServiceInterfaceClientVec();
    while (workFlag_) {
        /* 遍历port name，获取客户端对象并注册接收到数据后的回调函数 */
        for (const auto& portName : msgImageDataListServiceInterfaceClientPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取接收端服务，其中portName对应MMC上配置的此应用对应的接收端portName <关键接口> */
            auto clientPtr = swcPtr_->GetMsgImageDataListServiceInterfaceClient(portName);
            if (!clientPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            /* Event事件，注册接收到数据后的回调函数, 此操作必须且在获取数据前执行，执行后才能正常接收数据, 详情可参考用户文档 */
            clientPtr->RegisterEventImageListNotifyHandler(
                std::bind(&Sample::ReceiveAraAdsfiEventImageListDataHandle,
                std::placeholders::_1));
            /* 获取数据，非阻塞方式 <关键接口> */
            auto oneData = clientPtr->GetEventImageListOneData();
            /* 获取数据，一次获取5个数据，非阻塞方式 <关键接口> */
            auto nData = clientPtr->GetEventImageListNdata(5U);
            /* 获取数据，阻塞式接口，超时时间为1000ms <关键接口> */
            auto oneDataBlocking = clientPtr->GetEventImageListOneDataBlocking(1000U);
            
        }
    }
}

void Sample::Stop()
{
    workFlag_ = false;
    
    if ((RemoteControlServiceInterfaceClientThreadPtr_ != nullptr) &&
        (RemoteControlServiceInterfaceClientThreadPtr_->joinable())) {
        RemoteControlServiceInterfaceClientThreadPtr_->join();
    }
    RemoteControlServiceInterfaceClientThreadPtr_ = nullptr;
    if ((BodyCommandServiceInterfaceClientThreadPtr_ != nullptr) &&
        (BodyCommandServiceInterfaceClientThreadPtr_->joinable())) {
        BodyCommandServiceInterfaceClientThreadPtr_->join();
    }
    BodyCommandServiceInterfaceClientThreadPtr_ = nullptr;
    if ((LocationInfoIntfClientThreadPtr_ != nullptr) &&
        (LocationInfoIntfClientThreadPtr_->joinable())) {
        LocationInfoIntfClientThreadPtr_->join();
    }
    LocationInfoIntfClientThreadPtr_ = nullptr;
    if ((SensorGpsServiceInterfaceClientThreadPtr_ != nullptr) &&
        (SensorGpsServiceInterfaceClientThreadPtr_->joinable())) {
        SensorGpsServiceInterfaceClientThreadPtr_->join();
    }
    SensorGpsServiceInterfaceClientThreadPtr_ = nullptr;
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
    if ((MsgImageDataListServiceInterfaceClientThreadPtr_ != nullptr) &&
        (MsgImageDataListServiceInterfaceClientThreadPtr_->joinable())) {
        MsgImageDataListServiceInterfaceClientThreadPtr_->join();
    }
    MsgImageDataListServiceInterfaceClientThreadPtr_ = nullptr;
    
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Thread resources reclaimed successfully..";
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    swcPtr_->Stop();
}

void Sample::ReceiveAraAdsfiEventRCAppRemoteCtlDataHandle(const ara::adsfi::EventRCAppRemoteCtlDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventRCAppRemoteCtl data";
}
void Sample::ReceiveAraAdsfiEventVehicleActControlDataHandle(const ara::adsfi::EventVehicleActControlDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventVehicleActControl data";
}
void Sample::ReceiveAdsfiLocationInfoDataHandle(const adsfi::LocationInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving LocationInfo data";
}
void Sample::ReceiveAraAdsfiEventGpsDataHandle(const ara::adsfi::EventGpsDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventGps data";
}
void Sample::ReceiveAraAdsfiEventVehicleInfoDataHandle(const ara::adsfi::EventVehicleInfoDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventVehicleInfo data";
}
void Sample::ReceiveAraAdsfiEventFaultDataDataHandle(const ara::adsfi::EventFaultDataDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventFaultData data";
}
void Sample::ReceiveAraAdsfiEventImageListDataHandle(const ara::adsfi::EventImageListDataType& data)
{
    SPL_LOG_SPACE::GetLoggerIns("SPL")->LogInfo()<< "Do something when receiving EventImageList data";
}

}