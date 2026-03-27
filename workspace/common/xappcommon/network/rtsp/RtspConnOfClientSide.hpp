#ifndef RTSP_RTSP_CONN_OF_CLIENT_SIDE_HPP
#define RTSP_RTSP_CONN_OF_CLIENT_SIDE_HPP

#include <atomic>
#include <iostream>

#include "../net/TcpConnOfClientSide.hpp"
#include "../rtp/RtpUDPEndpoint.hpp"
#include "../rtp/h264tortp.hpp"
#include "../rtsp/Incomplete.hpp"
#include "../sdp/sdp.h"
#include "RtspResponse.hpp"
#include "fmt/format.h"

class RtspConnOfClientSide : public TcpConnOfClientSide {
public:
    // clang-format off
    RtspConnOfClientSide(
    std::shared_ptr<UsageEnvironment> env
    , const std::string &local_ip
    , std::uint16_t local_port
    , const std::string &peer_ip
    , std::uint16_t peer_port
    , std::string path
    , std::function<void()> cb_disconn)
    : TcpConnOfClientSide(std::move(env), local_ip, local_port, peer_ip, peer_port)
    , _path(std::move(path))
    , _callback_disconn(std::move(cb_disconn))
    , _current_waiting_req(RtspMsgBase::Method::NONE)
    , _current_waiting_seq(0)
    , _seq(rand())
    , _rtp_input(1500)
    , _rtp_output(8192) {
        // clang-format on
    }
#ifdef __arm__

#if 1
    void RtspConnOfClientSide::on_rtp_recv() {
    }
#else
    void RtspConnOfClientSide::on_rtp_recv() {
        static std::uint8_t header[] = {0, 0, 0, 1};

        try {
            auto n = _rtp_input.read(_rtp_endpoint->rtp_fd());
        } catch (const std::runtime_error& e) {
            SPDLOG_ERROR("read error: {}", e.what());
            return;
        }

        if (nullptr == c) {
            c = new RkmediaVdecH264RtpConsumer;
            if (nullptr == c) {
                SPDLOG_ERROR("failed to oepen test.h264");
                return;
            }
        }

        auto           d = _rtp_input.untouched_data();
        fu_indicator_t fu{};
        memcpy(&fu, d.first + sizeof(rtp_header_t), sizeof(fu));

        if (fu.type >= 0 && fu.type < 24) { // one nalu
            _rtp_output.append(header, 4);
            _rtp_output.append(d.first + sizeof(rtp_header_t), d.second - sizeof(rtp_header_t));
        } else if (fu.type == 28) { // fu-a slice
            fu_header_t f;
            memcpy(&f, d.first + sizeof(rtp_header_t) + 1, sizeof(f));
            //        char in = buf[RTP_OFFSET + 1];
            //        fu.S = in >> 7;
            //        fu.E = (in >> 6) & 0x01;
            //        fu.R = (in >> 5) & 0x01;
            //        fu.type = in & 0x1f;

            if (f.s == 1) {
                char naluType = fu.f << 7 | fu.nri << 5 | fu.type;
                _rtp_output.append(header, 4);
                _rtp_output.push_back(naluType);
                _rtp_output.append(d.first + sizeof(rtp_header_t) + 2, d.second - sizeof(rtp_header_t) - 2);
            } else if (f.e == 1) {
                _rtp_output.append(d.first + sizeof(rtp_header_t) + 2, d.second - sizeof(rtp_header_t) - 2);
            } else {
                _rtp_output.append(d.first + sizeof(rtp_header_t) + 2, d.second - sizeof(rtp_header_t) - 2);
            }
        }

        _rtp_input.reset();

        auto x = _rtp_output.untouched_data();
        if (x.second >= 4096) {
            c->data_arrive(x.first, 4096);
            _rtp_output.touch(4096);
            _rtp_output.tidy();
        }
    }
#endif
#else
    void RtspConnOfClientSide::on_rtp_recv() {
        static FILE*        fp = nullptr;
        static std::uint8_t header[] = {0, 0, 0, 1};

        try {
            auto n = _rtp_input.read(_rtp_endpoint->rtp_fd());
        } catch (const std::runtime_error& e) {
            std::cout << fmt::format("read error: {}", e.what()) << std::endl;
            return;
        }

        if (nullptr == fp) {
            fp = ::fopen("test.h264", "w+");
            if (nullptr == fp) {
                std::cout << fmt::format("failed to oepen test.h264") << std::endl;
                return;
            }
        }

        auto           d = _rtp_input.untouched_data();
        fu_indicator_t fu{};
        memcpy(&fu, d.first + sizeof(rtp_header_t), sizeof(fu));

        if (fu.type >= 0 && fu.type < 24) { // one nalu
            ::fwrite(header, 4, 1, fp);
            ::fwrite(d.first + sizeof(rtp_header_t), d.second - sizeof(rtp_header_t), 1, fp);
            ::fflush(fp);
        } else if (fu.type == 28) { // fu-a slice
            fu_header_t f;
            memcpy(&f, d.first + sizeof(rtp_header_t) + 1, sizeof(f));
            //        char in = buf[RTP_OFFSET + 1];
            //        fu.S = in >> 7;
            //        fu.E = (in >> 6) & 0x01;
            //        fu.R = (in >> 5) & 0x01;
            //        fu.type = in & 0x1f;

            if (f.s == 1) {
                char naluType = fu.f << 7 | fu.nri << 5 | fu.type;
                ::fwrite(header, 4, 1, fp);
                ::fwrite(&naluType, 1, 1, fp);
                ::fwrite(d.first + sizeof(rtp_header_t) + 2, d.second - sizeof(rtp_header_t) - 2, 1, fp);
                ::fflush(fp);
            } else if (f.e == 1) {
                ::fwrite(d.first + sizeof(rtp_header_t) + 2, d.second - sizeof(rtp_header_t) - 2, 1, fp);
                ::fflush(fp);
            } else {
                ::fwrite(d.first + sizeof(rtp_header_t) + 2, d.second - sizeof(rtp_header_t) - 2, 1, fp);
                ::fflush(fp);
            }
        }

        _rtp_input.reset();
    };
#endif

    ~RtspConnOfClientSide() {
        if (_rtp_poller_desc) {
            _environment->poller_event_remove(_rtp_poller_desc);
        }
        if (_rtcp_poller_desc) {
            _environment->poller_event_remove(_rtcp_poller_desc);
        }
    }

    void on_recv() override {
        std::size_t touched_len = 0;
        auto        d = _input.untouched_data();
        //    if (_rtp_over_tcp && d.first[0] == '$') { // 对应的是 RTSP 推流的情况？？？
        //        try {
        //            touched_len = on_rtp_over_tcp(d.first, d.second);
        //            _input.touch(touched_len);
        //        } catch (const Incomplete& e) {
        //        }
        //        return;
        //    }

        try {
            touched_len = _rsp.parse(d.first, d.second);
        } catch (const Incomplete& e) {
            std::cout << fmt::format("message incomplete") << std::endl;
            return;
        } catch (const std::runtime_error& e) {
            std::cout << fmt::format("message error with {}", e.what()) << std::endl;
            _input.reset();
            return;
        }

        std::cout << fmt::format("messge: {}", std::string(reinterpret_cast<const char*>(d.first), touched_len))
                  << std::endl;

        auto cseq = _rsp.header_value("CSeq");
        if (_current_waiting_seq != strtol(std::string(cseq).c_str(), nullptr, 10)) {
            std::cout << fmt::format("cseq mismatch, expected {}, received {}", _current_waiting_seq.load(), cseq)
                      << std::endl;
        } else {
            try {
                on_response();
            } catch (const std::runtime_error& e) {
                std::cout << fmt::format("message proc error with {}", e.what()) << std::endl;
            }
        }
        _input.touch(touched_len);
    };

    void on_conn() override {
        try {
            send_req_describe();

        } catch (const std::runtime_error& e) {
            std::cout << fmt::format("send describe error: {}", e.what()) << std::endl;
        }
    };

    void on_disconn() override {
        if (_callback_disconn) {
            _callback_disconn();
        }
    };

    void on_response() {
        switch (_current_waiting_req.load()) {
            case RtspMsgBase::Method::OPTIONS:
                break;
            case RtspMsgBase::Method::DESCRIBE:
                on_response_describe();
                send_req_setup();
                break;
            case RtspMsgBase::Method::SETUP:
                on_response_setup();
                send_req_play();
                break;

            case RtspMsgBase::Method::PLAY:
                on_response_play();
                break;

            default:
                break;
        }
    };

    void on_response_describe() {
        auto sdp = _rsp.parse_sdp();
        if (nullptr == sdp) {
            throw std::runtime_error("sdp error");
        }

        // TODO: 对 SDP 做处理

        sdp_destroy(sdp);
    };

    void on_response_setup() {
        _session = _rsp.header_value("Session");
        if (_session.empty()) {
            throw std::runtime_error("header session expected");
        }
    };

    void on_response_play() {
        auto s = _rsp.header_value("Session");
        if (s != _session) {
            throw std::runtime_error(fmt::format("session mismatch, expected {}, received {}", _session, s));
        }
    };

    void send_req_describe() {
        auto q = fmt::format(
            "DESCRIBE rtsp://{}:{}{} RTSP/1.0\r\n"
            "Accept: application/sdp\r\n"
            "CSeq: {}\r\n"
            "User-Agent: landshipr\n"
            "\r\n",
            _tcp_socket.peer_ip(), _tcp_socket.peer_port(), _path, _seq);
        send_sync(q.c_str(), q.size());
        _current_waiting_req = RtspMsgBase::Method::DESCRIBE;
        _current_waiting_seq = _seq;
        ++_seq;
    };

    void send_req_setup() {
        _rtp_endpoint = std::make_shared<RtpUDPEndpoint>(
            _environment, _tcp_socket.local_ip(), 1024 * 1024, [this]() { on_rtp_recv(); }, [this]() {});

        _rtp_poller_desc = std::make_shared<PollerDesc>(
            _rtp_endpoint->rtp_fd(), (std::uint32_t)PollerDesc::PollerEventType::EVENT_RDEX,
            [this]() { on_rtp_recv(); }, std::nullopt, std::nullopt);
        // clang-format on
        _environment->poller_event_add(_rtp_poller_desc);

        // clang-format off
        _rtcp_poller_desc = std::make_shared<PollerDesc>(
            _rtp_endpoint->rtcp_fd()
            , (std::uint32_t)PollerDesc::PollerEventType::EVENT_RDEX
            , [this]() {}, std::nullopt, std::nullopt);
        // clang-format on
        _environment->poller_event_add(_rtcp_poller_desc);

        auto s = fmt::format(
            "SETUP rtsp://{}:{}{} RTSP/1.0\r\n"
            "CSeq: {}\r\n"
            "User-Agent: landship\r\n"
            "Transport: {};unicast;client_port={}-{}\r\n"
            "\r\n",
            _tcp_socket.peer_ip(), _tcp_socket.peer_port(), _path, _seq, "RTP/AVP", _rtp_endpoint->rtp_port(),
            _rtp_endpoint->rtcp_port());

        send_sync(s.c_str(), s.size());
        _current_waiting_req = RtspMsgBase::Method::SETUP;
        _current_waiting_seq = _seq;
        ++_seq;
    };

    void send_req_play() {
        auto s = fmt::format(
            "PLAY rtsp://{}:{}{} RTSP/1.0\r\n"
            "CSeq: {}\r\n"
            "Session: {}\r\n"
            "Range: npt=0.000-\r\n" // Range
            "User-Agent: landship\r\n"
            "\r\n",
            _tcp_socket.peer_ip(), _tcp_socket.peer_port(), _path, _seq, _session);
        send_sync(s.c_str(), s.size());
        _current_waiting_req = RtspMsgBase::Method::PLAY;
        _current_waiting_seq = _seq;
        ++_seq;
    };

private:
    const std::string           _path;
    const std::function<void()> _callback_disconn;

    std::atomic<RtspMsgBase::Method> _current_waiting_req;
    std::atomic<std::uint32_t>       _current_waiting_seq;
    std::uint32_t                    _seq;

    RtspResponse _rsp;

    std::string                     _session;
    std::shared_ptr<RtpUDPEndpoint> _rtp_endpoint;
    std::shared_ptr<PollerDesc>     _rtp_poller_desc;
    std::shared_ptr<PollerDesc>     _rtcp_poller_desc;
    app_common::SimpleSocketBuffer  _rtp_input;
    app_common::SimpleSocketBuffer  _rtp_output;
};

#endif
