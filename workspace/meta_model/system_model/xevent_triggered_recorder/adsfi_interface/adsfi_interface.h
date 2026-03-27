#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "EventTriggeredRecorder.h"
#include "StringUtils.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init();

    void Callback(const std::string& name, std::shared_ptr<ara::adsfi::BusinessCommand>& msg);

    void Callback(const std::string& name, std::shared_ptr<ara::adsfi::FaultData>& msg);

    /**
     * @brief 处理函数（由框架以 10Hz 频率调用）
     * @param name 接口名称
     * @param msg 输出的故障数据
     */
    int Process(const std::string& name, std::shared_ptr<ara::adsfi::FaultData>& msg);

private:
    /**
     * @brief 初始化录包器
     */
    void init_recorder();

private:
    // 事件触发式录包器
    std::mutex                                              recorder_mutex_;
    std::unique_ptr<event_recorder::EventTriggeredRecorder> recorder_;

public:
    BASE_TEMPLATE_FUNCION
};

#endif
