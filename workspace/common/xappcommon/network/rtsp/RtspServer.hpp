#ifndef RTSP_RTSP_SERVER_HPP
#define RTSP_RTSP_SERVER_HPP

#include <atomic>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../base/UsageEnvironment.hpp"
#include "../net/TcpServer.hpp"
#include "../net/TcpSocketOfServerSide.hpp"
#include "../rtp/TcpRtpReceiver.hpp"
#include "../rtp/UdpRtpReceiver.hpp"
#include "DatetimeUtil.hpp"
#include "HexUtils.hpp"
#include "NetUtils.hpp"
#include "RandomUtils.hpp"
#include "RtspRequest.hpp"
#include "fmt/format.h"

class RtspConnOfServerSide : public TcpConnOfServerSide {
public:
    RtspConnOfServerSide(std::shared_ptr<UsageEnvironment> env, std::function<void(std::int32_t)> cb_disconn,
                         const std::shared_ptr<TcpSocketOfServerSide>& c)
        : TcpConnOfServerSide(
              env, [this](std::int32_t) { on_disconn(); }, c)
        , _callback_disconn_of_server(std::move(cb_disconn))
        , _last_heartbeat_time(app_common::DatetimeUtil::current_millisecond())
        , _rtp_over_tcp(false){};

    ~RtspConnOfServerSide() override {
        release_rtp_receivers();
    }

    std::string local_ip() const {
        return _environment->listen_ip();
    }

    std::uint16_t local_port() const {
        return _environment->listen_port();
    }

    std::string peer_ip() const {
        return _tcp_socket->peer_ip();
    }

    std::uint16_t peer_port() const {
        return _tcp_socket->peer_port();
    }

    bool connection_expired() const {
        return (_last_heartbeat_time + _environment->connection_expiry_ms()) <
               app_common::DatetimeUtil::current_millisecond();
    }

protected:
    void handleReadBytes() override {
        while (true) {
            auto d = _input.untouched_data();
            if (0 == d.second) {
                break;
            }

            std::size_t touched_len = 0;

            try {
                if (_rtp_over_tcp && d.first[0] == '$') { // 对应的是 RTSP 推流的情况？？？
                    touched_len = on_rtp_over_tcp(d.first, d.second);
                    _input.touch(touched_len);
                } else {
                    touched_len = _req.parse(d.first, d.second);
                    std::cout << fmt::format("messge: {}",
                                             std::string(reinterpret_cast<const char*>(d.first), touched_len))
                              << std::endl;
                    on_request();
                }
                _input.touch(touched_len);
            } catch (const Incomplete& e) {
                std::cout << fmt::format("message incomplete") << std::endl;
                break;
            } catch (const std::runtime_error& e) {
                std::cout << fmt::format("message error with {}", e.what()) << std::endl;
                _input.reset();
                break;
            }
        }
    }

private:
    void release_rtp_receivers() {
        if (!_session_id.empty() && _media_man) {
            _media_man->del_receiver(SENDER_RTSP, _session_id);
        }
    }

    std::size_t on_rtp_over_tcp(const std::uint8_t* d, std::size_t s) {
        std::size_t explen = 4;
        if (s < explen) {
            throw Incomplete();
        }

        std::uint16_t len = 0;
        memcpy(&len, d + 2, sizeof(len));
        len = ntohs(len);

        explen += len;
        if (s < explen) {
            throw Incomplete();
        }

        std::cout << fmt::format("ignore receive(from {}:{}) rtp/rtcp: {}", peer_ip(), peer_port(),
                                 app_common::HexUtils::to_hex_string(d, explen))
                  << std::endl;

        return explen;
    }

    void on_disconn() {
        std::cout << fmt::format("connection of {}:{} disconnected", peer_ip(), peer_port()) << std::endl;
        release_rtp_receivers();
        if (_callback_disconn_of_server) {
            _callback_disconn_of_server(_tcp_socket->fd());
        }
    }

    void on_request() {
        switch (_req.method) {
            case RtspRequest::Method::OPTIONS:
                on_cmd_options();
                break;
            case RtspRequest::Method::DESCRIBE:
                on_cmd_descirbe();
                break;
            case RtspRequest::Method::SETUP:
                on_cmd_setup();
                break;
            case RtspRequest::Method::PLAY:
                on_cmd_play();
                break;
            case RtspRequest::Method::TEARDOWN:
                on_cmd_teardown();
                break;
            default:
                throw std::runtime_error(fmt::format("method {} error", (std::int32_t)_req.method));
        }
    }

    void on_cmd_options() {
        auto now = app_common::DatetimeUtil::current_millisecond();
        auto s = fmt::format(
            "RTSP/1.0 200 OK\r\n"
            "CSeq: {}\r\n"
            "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
            "X-SERVER-STARTTIME-MS: {}\r\n"
            "X-SERVER-TIME-MS: {}\r\n"
            "X-CLIENT-TIME-MS: {}\r\n"
            "\r\n",
            _req.header_value("CSeq"), _environment->start_time(), now, _req.header_value("X-CLIENT-TIME-MS"));
        _last_heartbeat_time = now;
        send_sync(reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }

    void on_cmd_descirbe() {
        std::shared_ptr<MediaMan> mm;

        try {
            mm = _environment->media_man_get(std::string(_req.suffix));
        } catch (const std::runtime_error& e) {
            response_not_found();
            std::rethrow_exception(std::current_exception());
        }

        std::string sdp = fmt::format(
            "v=0\r\n"
            "o={} 0 0 IN IP4 {}\r\n" /*会话源*/ /*用户名/会话ID/版本/网络类型/地址类型/地址*/
            "s=landship\r\n"                    /*会话名*/
            "i={}\r\n"
            "t=0 0\r\n" /*时间*/ /*开始时间/结束时间*/
            "a=type:broadcast\r\n"
            "a=control:*\r\n",
            "xxxxxxxxxxxxx", mm->local_ip(_req.track, SENDER_RTSP), _req.suffix);
        sdp.append(mm->sdp(_req.track, SENDER_RTSP));

        auto s = fmt::format(
            "RTSP/1.0 200 OK\r\n"
            "CSeq: {}\r\n"
            "Content-Base: {}\r\n"
            "Content-Length: {}\r\n"
            "Content-Type: application/sdp\r\n"
            "\r\n"
            "{}",
            _req.header_value("CSeq"), _req.url, sdp.size(), sdp);
        send_sync(reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }

    void on_cmd_setup() {
        std::shared_ptr<MediaMan> mm;
        try {
            mm = _environment->media_man_get(std::string(_req.suffix));
        } catch (const std::runtime_error& e) {
            response_not_found();
            std::rethrow_exception(std::current_exception());
        }

        auto cseq = _req.header_value("CSeq");
        auto ssid = app_common::RandomUtil::strN(8); // TODO：多个 track 时的处理
        if (mm->is_multicast(_req.track, SENDER_RTSP)) {
            auto s = fmt::format(
                "RTSP/1.0 200 OK\r\n"
                "CSeq: {}\r\n"
                "Transport: RTP/AVP;multicast;destination={};source={};port={}-{};ttl=255\r\n"
                "Session: {}\r\n"
                "\r\n",
                cseq, mm->mcast_ip(_req.track, SENDER_RTSP), mm->local_ip(_req.track, SENDER_RTSP),
                mm->mcast_rtp_port(_req.track, SENDER_RTSP), mm->mcast_rtcp_port(_req.track, SENDER_RTSP), ssid);
            send_sync(reinterpret_cast<const uint8_t*>(s.c_str()), s.size());
            _session_id = ssid;
            return;
        }

        auto transport = _req.get_transport();
        if (transport.tcp) {
            auto r = std::make_shared<TcpRtpReceiver>(rand(), _tcp_socket->fd(), transport.rtp, transport.rtcp);
            auto s = fmt::format(
                "RTSP/1.0 200 OK\r\n"
                "CSeq: {}\r\n"
                "Transport: RTP/AVP/TCP;unicast;interleaved={}-{}\r\n"
                "Session: {}\r\n"
                "\r\n",
                cseq, transport.rtp, transport.rtcp, ssid);
            send_sync(reinterpret_cast<const uint8_t*>(s.c_str()), s.size());
            _rtp_over_tcp = true;
            _rtp_receivers = r;
            _session_id = ssid;
            return;
        }

        auto r = std::make_shared<UdpRtpReceiver>(rand(), _tcp_socket->peer_ip(), transport.rtp, transport.rtcp);
        auto s = fmt::format(
            "RTSP/1.0 200 OK\r\n"
            "CSeq: {}\r\n"
            "Transport: RTP/AVP;unicast;destination={};source={};client_port={}-{};server_port={}-{}\r\n"
            "Session: {}\r\n"
            "\r\n",
            cseq, _tcp_socket->peer_ip(), _environment->listen_ip(), transport.rtp, transport.rtcp,
            mm->local_rtp_port(_req.track, SENDER_RTSP), mm->local_rtcp_port(_req.track, SENDER_RTSP), ssid);
        send_sync(reinterpret_cast<const uint8_t*>(s.c_str()), s.size());
        _rtp_receivers = r;
        _session_id = ssid;
    }

    void on_cmd_play() {
        std::shared_ptr<MediaMan> mm;
        try {
            mm = _environment->media_man_get(std::string(_req.suffix));
        } catch (const std::runtime_error& e) {
            response_not_found();
            std::rethrow_exception(std::current_exception());
        }

        auto header_session_id = _req.header_value("session");
        if (header_session_id != _session_id) {
            throw std::runtime_error(fmt::format("session id {} mismatch {}", header_session_id, _session_id));
        }

        // TODO: 扩展了几个 header 用于与客户端交换信息
        auto s = fmt::format(
            "RTSP/1.0 200 OK\r\n"
            "CSeq: {}\r\n"
            "Range: npt=0.000-\r\n"
            "Session: {}\r\n"
            "RTP-Info: url={};seq=0;rtptime=0\r\n"
            "X-SERVER-STARTTIME-MS: {}\r\n"
            "X-SERVER-TIME-MS: {}\r\n"
            "X-CLIENT-TIME-MS: {}\r\n"
            "\r\n", // TODO: seq, rpttime
            _req.header_value("CSeq"), _session_id, _req.url, _environment->start_time(),
            app_common::DatetimeUtil::current_millisecond(), _req.header_value("X-CLIENT-TIME-MS"));
        send_sync(reinterpret_cast<const uint8_t*>(s.data()), s.size());

        _media_man = mm;
        _media_man->add_receiver(_req.track, SENDER_RTSP, _session_id, _rtp_receivers);
    }

    void on_cmd_teardown() {
        auto header_session_id = _req.header_value("session");
        if (header_session_id != _session_id) {
            throw std::runtime_error(fmt::format("session id {} mismatch {}", header_session_id, _session_id));
        }

        // if (_media_man) {
        //     _rtp_receivers.reset();
        //     _media_man->del_receiver(SENDER_RTSP, _session_id);
        //     _media_man.reset();
        //     _session_id.clear();
        // }

        auto s = fmt::format(
            "RTSP/1.0 200 OK\r\n"
            "CSeq: {}\r\n"
            "\r\n",
            _req.header_value("CSeq"));
        send_sync(reinterpret_cast<const uint8_t*>(s.data()), s.size());

        on_disconn();
    }

    void response_not_found() {
        auto s = fmt::format(
            "RTSP/1.0 404 Stream Not Found\r\n"
            "CSeq: {}\r\n"
            "\r\n",
            _req.header_value("CSeq"));
        send_sync(reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }

private:
    std::function<void(std::int32_t)> _callback_disconn_of_server;

    RtspRequest                  _req;
    std::shared_ptr<RtpReceiver> _rtp_receivers;
    std::string                  _session_id;
    std::shared_ptr<MediaMan>    _media_man;
    std::int64_t                 _last_heartbeat_time; //单位：毫秒
    bool                         _rtp_over_tcp;
};

class RtspServer : public TcpServer, public std::enable_shared_from_this<RtspServer> {
public:
    RtspServer(const std::shared_ptr<UsageEnvironment>& env) : TcpServer(env), _stopped(false){};

    ~RtspServer() {
        RtspServer::stop();
    }

    void start() override {
        TcpServer::start();

        _thd = std::thread([this]() {
            while (!_stopped) {
                try {
                    _environment->loop();
                } catch (const std::runtime_error& e) {
                    std::cout << "ERROR: " << e.what() << std::endl;
                } catch (...) {
                    std::cout << "ERROR: "
                              << "unknown" << std::endl;
                }

                on_check_connection_expiry();
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
        });
    }

    void stop() override {
        _stopped = true;
        if (_thd.joinable()) {
            _thd.join();
        }

        TcpServer::stop();

        std::lock_guard<std::mutex> guard(_mutex);
        _connections.clear();
    }

    [[nodiscard]] std::shared_ptr<UsageEnvironment> envir() const {
        return _environment;
    }

protected:
    void on_new_connection(const std::shared_ptr<TcpSocketOfServerSide>& c) override {
        auto conn = std::make_shared<RtspConnOfServerSide>(
            _environment, [this](std::int32_t connfd) { on_end_connection(connfd); }, c);
        _connections.insert(std::make_pair(c->fd(), conn));
    }

    void on_end_connection(std::int32_t sockfd) {
        std::lock_guard<std::mutex> guard(_mutex);
        _connections.erase(sockfd);
    }

    void on_check_connection_expiry() {
        std::lock_guard<std::mutex> guard(_mutex);

        auto iter = _connections.begin();
        while (iter != _connections.end()) {
            if (iter->second->connection_expired()) {
                std::cout << "connection " << iter->second->peer_ip() << ":" << iter->second->peer_port() << " expired"
                          << std::endl;
                iter = _connections.erase(iter);
            } else {
                ++iter;
            }
        }
    }

private:
    std::atomic_bool _stopped;
    std::thread      _thd;

    std::mutex                                                    _mutex;
    std::map<std::int32_t, std::shared_ptr<RtspConnOfServerSide>> _connections;
};

#endif
