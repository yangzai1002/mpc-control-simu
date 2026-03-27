
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <atomic>
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
#include "Ec407.hpp"
#include "EncodedH265MediaMan.hpp"
#include "SimpleSocketBuffer.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"
#include "network/base/UsageEnvironment.hpp"
#include "network/media/RawSender.hpp"
#include "network/media/ThreadedRotatingFileReceiver.hpp"
#include "network/rtp/H265NaluRtpSender.hpp"
#include "network/rtp/RawRtpSender.hpp"
#include "network/rtsp/RtspServer.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init() override;

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &ptr);

private:
    std::atomic_bool _stopped;
    std::thread      _thread;

    std::int32_t                                    _port;
    std::shared_ptr<UsageEnvironment>               _env;
    std::shared_ptr<RtspServer>                     _server;
    std::shared_ptr<EncodedH265MediaMan>            _mm_x265;
    std::shared_ptr<app_common::SimpleSocketBuffer> _ssb;

    std::shared_ptr<Ec407> _ec;
    std::mutex             _ec_mtx;

    std::atomic<double> _last_data_time;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
