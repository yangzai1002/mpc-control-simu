#ifndef RTSP_RTSP_REQUEST_HPP
#define RTSP_RTSP_REQUEST_HPP

#include <algorithm>
#include <cstring>

#include "../media/TrackId.hpp"
#include "Incomplete.hpp"
#include "RtspMsgBase.hpp"

struct RtspRequest : public RtspMsgBase {
    RtspMsgBase::Method method{Method::NONE};
    std::string         url;
    std::string         suffix;
    TrackId             track{TrackId::Track0};

    void reset() {
        method = Method::NONE;
        url = std::string();
        suffix = std::string();
        track = TrackId::Track0;
        num_headers = 0;
        content = std::string();
    }

    std::size_t parse(const std::uint8_t* p, std::size_t s) noexcept(false) {
        std::size_t touched_len = 0;
        auto        r = parse_head_line(p, s);
        touched_len += r;

        r = parse_headers(p + touched_len, s - touched_len);
        touched_len += r;

        auto header_cl = header_value("Content-Length");
        if (!header_cl.empty()) {
            r = parse_content(p + touched_len, s - touched_len, strtol(header_cl.data(), nullptr, 10));
            touched_len += r;
        }

        return touched_len;
    };

    [[nodiscard]] Transport get_transport() const {
        auto v = header_value("transport");
        if (v.empty()) {
            throw std::runtime_error("header transport does not exist");
        }

        auto pos = v.find("RTP/AVP");
        if (std::string::npos == pos) {
            throw std::runtime_error("header transport format error");
        }
        pos += 7;

        bool        is_tcp = false;
        std::string rest(v.data() + pos, v.size() - pos);
        const char* FIND;
        if (i_n_equal(rest, "/TCP", 4)) { // TCP
            FIND = "interleaved=";
            is_tcp = true;
        } else if (i_n_equal(rest, "/UDP", 4)) { // UDP
            FIND = "client_port=";
        } else if (i_n_equal(rest, ";", 1)) { // UDP
            FIND = "client_port=";
        } else {
            throw std::runtime_error("header transport format error");
        }
        pos = rest.find(FIND);
        if (std::string::npos == pos) {
            throw std::runtime_error("header transport format error");
        }
        auto saved = pos + strlen(FIND);

        pos = rest.find('-', saved);
        if (std::string::npos == pos || std::string::npos == (pos + 1)) {
            throw std::runtime_error("header transport format error");
        }

        std::uint16_t rtp = strtol(rest.data() + saved, nullptr, 10);
        std::uint16_t rtcp = strtol(rest.data() + pos + 1, nullptr, 10);
        return {rtp, rtcp, is_tcp};
    };

private:
    std::size_t parse_head_line(const std::uint8_t* p, std::size_t s) {
        auto f = std::search(p, p + s, kCRLF, kCRLF + 2);
        if (f == (p + s)) {
            throw Incomplete();
        }
        std::string v((const char*)p, f - p);

        auto pos = v.find(' ');
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        std::string m(v.data(), pos);

        pos = v.find_first_not_of(' ', pos + 1);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        auto saved = pos;
        pos = v.find(' ', pos);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        url = std::string(v.data() + saved, pos - saved);

        pos = v.find_first_not_of(' ', pos + 1);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        (std::string(v.data() + pos, pos - saved)); // version

        if (iequal(m, "OPTIONS", 7)) {
            this->method = Method::OPTIONS;
        } else if (iequal(m, "DESCRIBE", 8)) {
            this->method = Method::DESCRIBE;
        } else if (iequal(m, "SETUP", 5)) {
            this->method = Method::SETUP;
        } else if (iequal(m, "PLAY", 4)) {
            this->method = Method::PLAY;
        } else if (iequal(m, "TEARDOWN", 8)) {
            this->method = Method::TEARDOWN;
        } else if (iequal(m, "GET_PARAMETER", 13)) {
            this->method = Method::GET_PARAMETER;
        } else {
            throw std::runtime_error("headline method error");
        }

        if (0 != strncasecmp(url.data(), "rtsp://", 7)) {
            throw std::runtime_error("headline url protocol error");
        }

        saved = 7;
        pos = url.find('/', saved);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline url uri error");
        }
        saved = pos + 1;
        pos = url.find('/', saved);
        if (pos == std::string::npos) {
            suffix = std::string(url.data() + saved, url.length() - saved);
            track = TrackId::Track0;
        } else {
            suffix = std::string(url.data() + saved, pos - saved);
            ++pos;
            auto t = std::string(url.data() + pos, url.length() - pos);
            if (t.empty()) {
                track = TrackId::Track0;
            } else {
                if (iequal(t, "track0", 6)) {
                    track = TrackId::Track0;
                } else if (iequal(t, "track1", 6)) {
                    //                track = TrackId::Track1;
                    // TODO: 对于 track 弄得还不是很清楚，先全部设置为 track0
                    track = TrackId::Track0;
                } else {
                    throw std::runtime_error("headline url track error");
                }
            }
        }
        if (suffix.empty()) {
            throw std::runtime_error("headline url suffix error");
        }

        return (f - p + 2);
    };
};

#endif
