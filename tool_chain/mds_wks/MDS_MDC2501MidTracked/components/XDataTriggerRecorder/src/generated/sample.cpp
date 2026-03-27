/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: adsfi sample
 */

#include "mdc/sample.h"
#include <chrono>
#include <functional>
namespace mdc {
Sample::Sample(): swcPtr_(std::make_unique<mdc::xdata_trigger_recorder_base::XdataTriggerRecorderBaseSwC>()), workFlag_(true)
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
    BagStatusServiceInterfaceServerThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::BagStatusServiceInterfaceServerRun, this));
    FaultDataServiceInterfaceClientThreadPtr_ =
        std::make_unique<std::thread>(std::bind(&Sample::FaultDataServiceInterfaceClientRun, this));
    
}

void Sample::BagStatusServiceInterfaceServerRun()
{
    if (!swcPtr_) {
        SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "The swc object is empty.";
        return;
    }
    /* 获取本应用所有的发送端口，获取到的端口名对应MMC上本应用配置的发送端口名 */
    auto bagStatusServiceInterfaceServerPortVec = swcPtr_->GetBagStatusServiceInterfaceServerVec();
    while (workFlag_) {
        /* 遍历port name，获取服务端对象并发送数据 */
        for (const auto& portName : bagStatusServiceInterfaceServerPortVec) {
            if (!workFlag_) {
                return;
            }
            /* 获取发送端服务，其中portName对应MMC上配置的此应用对应的发送端portName <关键接口> */
            auto serverPtr = swcPtr_->GetBagStatusServiceInterfaceServer(portName);
            if (!serverPtr) {
                SPL_LOG_SPACE::GetLoggerIns("SPL")->LogError()<< "Failed to initialize the instance: " << portName;
                continue;
            }
            auto data = std::make_shared<ara::adsfi::EventBagStatusDataType>();
            /* Event事件，发送数据，非阻塞接口 <关键接口> */
            serverPtr->SendEventBagStatusData(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
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

void Sample::Stop()
{
    workFlag_ = false;
    if ((BagStatusServiceInterfaceServerThreadPtr_ != nullptr) &&
        (BagStatusServiceInterfaceServerThreadPtr_->joinable())) {
        BagStatusServiceInterfaceServerThreadPtr_->join();
    }
    BagStatusServiceInterfaceServerThreadPtr_ = nullptr;
    
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