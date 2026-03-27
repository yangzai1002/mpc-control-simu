
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "StringUtils.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"

#define ZMQ_CPP11
#include <zmq.h>

#include "ZmqRC3.hpp"
#include "zmq.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
    }
    ~AdsfiInterface() {
    }

    void Init();

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::AppRemoteControl> &msg);

private:
    std::shared_ptr<ZmqRC3> handler_ptr;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
