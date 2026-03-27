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
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "Ec406.hpp"
#include "LocalH265RTPSender.hpp"
#include "XH265RtpPusher.h"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "fmt/format.h"

void XH265RtpPusher(const VideoRTPParameter &rtp_param, const VideoEncodedFrame &frame) {
    STATIC_DATA(std::shared_ptr<Ec406>, _ec, nullptr);
    STATIC_DATA(std::shared_ptr<std::atomic<double>>, _last_data_time, nullptr);

    STATIC_DATA(VideoRTPParameter, _last_para, {})
    STATIC_DATA(std::shared_ptr<LocalH265RTPSender>, _sender_rtp, nullptr);

    if (nullptr == _ec) {
        _ec = std::make_shared<Ec406>();
        _last_data_time = std::make_shared<std::atomic<double>>(0.0);
        if (nullptr == _ec || nullptr == _last_data_time) {
            ApError("h265_rtp_pusher") << "create ec failed";
            return;
        }
        std::thread([_ec, _last_data_time]() {
            pthread_setname_np(pthread_self(), "data_monitor");
            while (true) {
                auto now = app_common::DatetimeUtil::current_second_double();
                auto last = _last_data_time->load();
                if (fabs(now - last) >= 2.0) {
                    _ec->ec_add(_ERRORCODE_NO_DATA);
                } else {
                    _ec->ec_remove(_ERRORCODE_NO_DATA);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }).detach();
    }

    _ec->check_freq(frame.header.time, frame.fps);
    _last_data_time->store(frame.header.time);

    if (0 == rtp_param.push_enabled) {
        _sender_rtp = nullptr;
        return;
    } else if (_last_para.ssrc != rtp_param.ssrc || _last_para.peer_port != rtp_param.peer_port ||
               _last_para.peer_host != rtp_param.peer_host) {
        ApInfo("h265_rtp_pusher") << "old RTP param, " << _last_para.ssrc << ", " << _last_para.peer_port << ", "
                                  << _last_para.peer_host;
        ApInfo("h265_rtp_pusher") << "new RTP param, " << rtp_param.ssrc << ", " << rtp_param.peer_port << ", "
                                  << rtp_param.peer_host;
        _sender_rtp = nullptr;
    }
    _last_para = rtp_param;

    if (_sender_rtp == nullptr) {
        addrinfo  hints;
        addrinfo *res = nullptr;
        int       status;
        char      ipstr[INET6_ADDRSTRLEN];

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;     // 支持 IPv4 和 IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP

        if ((status = getaddrinfo(rtp_param.peer_host.c_str(), nullptr, &hints, &res)) != 0) {
            ApError("h265_rtp_pusher") << "getaddrinfo error: " << gai_strerror(status);
            _ec->ec_add(_ERRORCODE_ADDRESS_RESOLVE, rtp_param.peer_host);
            return;
        }
        _ec->ec_remove(_ERRORCODE_ADDRESS_RESOLVE);

        std::list<std::string> ips;
        for (auto p = res; p != nullptr; p = p->ai_next) {
            void *addr;
            char *ipver;

            if (p->ai_family != AF_INET) {
                continue;
            }
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            memset(ipstr, 0, sizeof(ipstr));
            inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
            ips.emplace_back(ipstr);
        }

        ApInfo("h265_rtp_pusher") << "ipv4 addresses of " << rtp_param.peer_host << ": ";
        for (const auto &ip : ips) {
            ApInfo("h265_rtp_pusher") << ip;
        }
        if (ips.empty()) {
            ApError("h265_rtp_pusher") << "no valid ip address found";
            _ec->ec_add(_ERRORCODE_ADDRESS_RESOLVE, rtp_param.peer_host);
            return;
        }
        _ec->ec_remove(_ERRORCODE_ADDRESS_RESOLVE);
        auto &ip = ips.front();
        ApInfo("h265_rtp_pusher") << "sending to " << ip << ":" << rtp_param.peer_port;
        try {
            _sender_rtp =
                std::make_shared<LocalH265RTPSender>(rtp_param.ssrc, frame.fps, "0.0.0.0", ip, rtp_param.peer_port);
        } catch (const std::exception &e) {
            ApError("h265_rtp_pusher") << "ERROR: " << e.what();
            return;
        }
    } else {
        try {
            _sender_rtp->send(frame.frame_data.data(), frame.frame_data.size());
            ApInfo("h265_rtp_pusher") << "sent " << frame.frame_data.size() << " bytes";
            ApInfo("h265_rtp_pusher") << fmt::format("获取的时间: {:.15f}", frame.header.time);
            _ec->ec_remove(_ERRORCODE_FORMAT);
            _ec->ec_remove(_ERRORCODE_SEND);
        } catch (const ExceptionParseNalus &e) {
            _ec->ec_add(_ERRORCODE_FORMAT);
        } catch (const std::exception &e) {
            ApError("h265_rtp_pusher") << "error: " << e.what();
            _ec->ec_add(_ERRORCODE_SEND);
            _sender_rtp.reset();
        }
    }
}
