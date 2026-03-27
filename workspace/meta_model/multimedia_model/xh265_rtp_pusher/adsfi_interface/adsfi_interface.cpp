
#include <fmt/format.h>

#include "DatetimeUtil.hpp"
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "log/avos_node.h"

#define FIX_RTP_DST_IP "26.28.1.110"
#define FPS 30

void AdsfiInterface::Init() {
    SetScheduleInfo("message");

    // ApLogInterface::Instance()->Init("XH265RtpPusher");

    _ec = std::make_shared<Ec406>();
    if (nullptr == _ec) {
        ApError("h265_rtp_pusher") << "create ec failed";
        return;
    }

    _thread = std::thread([&]() {
        pthread_setname_np(pthread_self(), "data_monitor");
        while (!_stopped) {
            // TODO: use data plane time
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
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::VideoRTPParameter>& ptr) {
    param_deque.push(ptr);
}

void AdsfiInterface::Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgImageData>& ptr) {
    if (nullptr == ptr) {
        ApError("h265_rtp_pusher") << "ptr is null";
        return;
    }

    param_deque.getLatestAndClear(_param_ptr);
    if (__glibc_unlikely(nullptr == _param_ptr)) {
        ApError("h265_rtp_pusher") << "_param_ptr is null";
        return;
    }
    if (__glibc_unlikely(0 == _param_ptr->push_enabled)) {
        _sender_rtp = nullptr;
        return;
    }

    if (__glibc_likely(_last_para != nullptr)) {
        if (__glibc_unlikely(_last_para->ssrc != _param_ptr->ssrc || _last_para->peer_port != _param_ptr->peer_port ||
                             _last_para->peer_host != _param_ptr->peer_host)) {
            ApInfo("h265_rtp_pusher") << "old RTP param, " << _last_para->ssrc << ", " << _last_para->peer_port << ", "
                                      << _last_para->peer_host;
            ApInfo("h265_rtp_pusher") << "new RTP param, " << _param_ptr->ssrc << ", " << _param_ptr->peer_port << ", "
                                      << _param_ptr->peer_host;
            _sender_rtp = nullptr;
        }
    } else {
        _sender_rtp = nullptr;
    }
    _last_para = _param_ptr;

    _last_data_time = app_common::DatetimeUtil::double_time_from_sec_nsec(ptr->timestamp);

    if (__glibc_unlikely(_sender_rtp == nullptr)) {
        addrinfo  hints;
        addrinfo* res = nullptr;
        int       status;
        char      ipstr[INET6_ADDRSTRLEN];

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;     // 支持 IPv4 和 IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP

        if ((status = getaddrinfo(_param_ptr->peer_host.c_str(), nullptr, &hints, &res)) != 0) {
            ApError("h265_rtp_pusher") << "getaddrinfo error: " << gai_strerror(status);
            std::lock_guard lg(_ec_mtx);
            _ec->ec_add(_ERRORCODE_ADDRESS_RESOLVE, _param_ptr->peer_host);
            return;
        }
        {
            std::lock_guard lg(_ec_mtx);
            _ec->ec_remove(_ERRORCODE_ADDRESS_RESOLVE);
        }

        std::list<std::string> ips;
        for (auto p = res; p != nullptr; p = p->ai_next) {
            void* addr;
            char* ipver;

            if (p->ai_family != AF_INET) {
                continue;
            }
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
            memset(ipstr, 0, sizeof(ipstr));
            inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
            ips.emplace_back(ipstr);
        }

        ApInfo("h265_rtp_pusher") << "ipv4 addresses of " << _param_ptr->peer_host << ": ";
        for (const auto& ip : ips) {
            ApInfo("h265_rtp_pusher") << ip;
        }
        if (ips.empty()) {
            ApError("h265_rtp_pusher") << "no valid ip address found";
            std::lock_guard lg(_ec_mtx);
            _ec->ec_add(_ERRORCODE_ADDRESS_RESOLVE, _param_ptr->peer_host);
            return;
        }
        {
            std::lock_guard lg(_ec_mtx);
            _ec->ec_remove(_ERRORCODE_ADDRESS_RESOLVE);
        }
        auto& ip = ips.front();
        ApInfo("h265_rtp_pusher") << "sending to " << ip << ":" << _param_ptr->peer_port;
        try {
            _sender_rtp = std::make_shared<LocalH265RTPSender>(_param_ptr->ssrc, FPS /*ptr->fps*/, "0.0.0.0", ip,
                                                               _param_ptr->peer_port);
        } catch (const std::exception& e) {
            ApError("h265_rtp_pusher") << "ERROR: " << e.what();
            return;
        }
    } else {
        try {
            if (__glibc_unlikely(ptr->imageType != 53)) {
                throw ExceptionParseNalus("unknown image type: " +
                                          std::to_string(static_cast<std::int32_t>(ptr->imageType)));
            }
            if (__glibc_unlikely(ptr->dataSize > ptr->rawData.size())) {
                throw ExceptionParseNalus(fmt::format("invalid datasize {} against to rawdata.size() {}", ptr->dataSize,
                                                      ptr->rawData.size()));
            }

            _sender_rtp->send(ptr->rawData.data(), ptr->dataSize);
            ApInfo("h265_rtp_pusher") << "sent " << ptr->dataSize << " bytes";
            std::lock_guard lg(_ec_mtx);
            _ec->ec_remove(_ERRORCODE_FORMAT);
            _ec->ec_remove(_ERRORCODE_SEND);
        } catch (const ExceptionParseNalus& e) {
            std::lock_guard lg(_ec_mtx);
            _ec->ec_add(_ERRORCODE_FORMAT);
        } catch (const std::exception& e) {
            ApError("h265_rtp_pusher") << "error: " << e.what();
            std::lock_guard lg(_ec_mtx);
            _ec->ec_add(_ERRORCODE_SEND);
            _sender_rtp.reset();
        }
    }
}
