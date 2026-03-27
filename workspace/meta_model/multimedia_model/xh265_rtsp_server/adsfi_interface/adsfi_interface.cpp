
#include "DatetimeUtil.hpp"
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "log/avos_node.h"

#define FPS 30

AdsfiInterface::AdsfiInterface() : _stopped(false), _port(0) {
    _ec = std::make_shared<Ec407>();
    if (_ec == nullptr) {
        ApError("h265_rtsp_server") << "cannot create ec manager";
        throw std::runtime_error("cannot create ec manager");
    }

    auto         ptr = CustomStack::Instance();
    std::string  prefix = "";
    std::int32_t expiry;
    std::int32_t rtp_packer;
    std::int32_t save_stream;
    auto         full_path = prefix + "video/rtsp/port";
    if (!ptr->GetProjectConfigValue(full_path, _port) || _port <= 0) {
        _ec->ec_add(_ERRORCODE_CONFIG, full_path);
        ApError("h265_rtsp_server") << "read project config " << full_path << "failed";
        return;
    }
    full_path = prefix + "video/rtsp/expiry";
    if (!ptr->GetProjectConfigValue(full_path, expiry) || expiry <= 0) {
        _ec->ec_add(_ERRORCODE_CONFIG, full_path);
        ApError("h265_rtsp_server") << "read project config " << full_path << "failed";
        return;
    }
    full_path = prefix + "video/rtsp/rtp_packer";
    if (!ptr->GetProjectConfigValue(full_path, rtp_packer)) {
        _ec->ec_add(_ERRORCODE_CONFIG, full_path);
        ApError("h265_rtsp_server") << "read project config " << full_path << "failed";
        return;
    }
    full_path = prefix + "video/rtsp/save_stream";
    if (!ptr->GetProjectConfigValue(full_path, save_stream)) {
        _ec->ec_add(_ERRORCODE_CONFIG, full_path);
        ApError("h265_rtsp_server") << "read project config " << full_path << "failed";
        return;
    }
    _ec->ec_remove(_ERRORCODE_CONFIG);

    ApInfo("h265_rtsp_server") << "port: " << _port;
    ApInfo("h265_rtsp_server") << "expiry: " << expiry;
    ApInfo("h265_rtsp_server") << "rtp_packer: " << rtp_packer;
    ApInfo("h265_rtsp_server") << "save_stream: " << save_stream;

    try {
        auto que = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>>>();
        _env =
            std::make_shared<UsageEnvironment>(UsageEnvironment::PollerType::POLLER_SELECT, "0.0.0.0", _port, expiry);
        std::shared_ptr<RtpSender> sender_rtsp;
        if (0 == rtp_packer) {
            sender_rtsp = std::make_shared<H265NaluRtpSender>(30, "0.0.0.0");
        } else if (1 == rtp_packer) {
            sender_rtsp = std::make_shared<RawRtpSender>(_env, 30, "0.0.0.0");
        } else {
            throw std::runtime_error("rtp packer not support");
        }

        auto h265_sdp_rtsp =
            "m=video 0 RTP/AVP 98\r\n"
            "c=IN IP4 0.0.0.0\r\n"
            "b=AS:5000\r\n"
            "a=rtpmap:98 H265/90000\r\n"
            "a=fmtp:98\r\n"
            "profile-id=1;"
            "level-id=93;\r\n";
        //        "a=control:track1\r\n";
        sender_rtsp->sdp(h265_sdp_rtsp);
        _mm_x265 = std::make_shared<EncodedH265MediaMan>(que);
        _mm_x265->add_sender(TrackId::Track0, SENDER_RTSP, sender_rtsp);
        // 在本地保存数据流
        if (1 == save_stream) {
            auto s = std::make_shared<RawSender>();
            auto r = std::make_shared<ThreadedRotatingFileReceiver>(".", "x265", 60, 100);
            s->add_receiver("f", r);
            _mm_x265->add_sender(TrackId::Track0, SENDER_LOCALRECORD, s);
        }
        _env->media_man_add("v4l2", _mm_x265);

        auto rtsp_server = std::make_shared<RtspServer>(_env);
        rtsp_server->start();
        _server = rtsp_server;
        _ec->ec_remove(_ERRORCODE_ADDRESS_BIND);

        _thread = std::thread([this]() {
            pthread_setname_np(pthread_self(), "data_monitor");
            while (!_stopped) {
                auto now = app_common::DatetimeUtil::current_second_double();
                auto last = _last_data_time.load();
                if (fabs(now - last) >= 2.0) {
                    std::lock_guard lg(_ec_mtx);
                    _ec->ec_add(_ERRORCODE_NO_DATA);
                } else {
                    std::lock_guard lg(_ec_mtx);
                    _ec->ec_remove(_ERRORCODE_NO_DATA);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    } catch (const std::exception &e) {
        ApInfo("h265_rtsp_server") << "ERROR: " << e.what();
        std::lock_guard lg(_ec_mtx);
        _ec->ec_add(_ERRORCODE_ADDRESS_BIND, "0.0.0.0:" + std::to_string(_port));
        return;
    }
}

AdsfiInterface::~AdsfiInterface() {
    _stopped = true;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void AdsfiInterface::Init() {
    SetScheduleInfo("message");

    // ApLogInterface::Instance()->Init("XH265RtspServer");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &msg_ptr) {
    if (__glibc_unlikely(!msg_ptr)) {
        ApError("h265_rtsp_server") << "msg_ptr is null";
        return;
    }

    if (__glibc_unlikely(msg_ptr->imageType != 53)) {
        ApError("h265_rtsp_server") << "invalid imagetype: " << static_cast<std::int32_t>(msg_ptr->imageType);
        return;
    }

    if (__glibc_unlikely(0 == msg_ptr->dataSize)) {
        ApError("h265_rtsp_server") << "datasize is zero";
        return;
    }

    if (__glibc_unlikely(msg_ptr->dataSize > msg_ptr->rawData.size())) {
        ApError("h265_rtsp_server") << "datasize overflow, datasize: " << msg_ptr->dataSize
               << ", rawData.size(): " << msg_ptr->rawData.size();
        return;
    }

    auto double_time = app_common::DatetimeUtil::double_time_from_sec_nsec(msg_ptr->timestamp);
    {
        std::lock_guard lg(_ec_mtx);
        _ec->check_freq(double_time, FPS /*msg_ptr->fps*/);
    }
    _last_data_time.store(double_time);

    timeval tv{};
    gettimeofday(&tv, nullptr);
    ApInfo("h265_rtsp_server") << "new frame of time: " << double_time;

    _mm_x265->new_frame(msg_ptr);
}
