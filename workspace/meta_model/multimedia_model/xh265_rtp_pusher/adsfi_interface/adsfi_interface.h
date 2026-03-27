
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "Ec406.hpp"
#include "LocalH265RTPSender.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_videortpparameter.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() : _stopped(false), _last_data_time(0) {
    }

    ~AdsfiInterface() {
    }

    void Init() override;

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VideoRTPParameter> &ptr);

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &ptr);

private:
    std::atomic_bool _stopped;
    std::thread      _thread;

    std::shared_ptr<Ec406> _ec;
    std::mutex             _ec_mtx;

    ThreadSafeQueue<std::shared_ptr<ara::adsfi::VideoRTPParameter>> param_deque;
    std::atomic<double>                                             _last_data_time;
    std::shared_ptr<LocalH265RTPSender>                             _sender_rtp;
    std::shared_ptr<ara::adsfi::VideoRTPParameter>                  _last_para;
    std::shared_ptr<ara::adsfi::VideoRTPParameter>                  _param_ptr;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
