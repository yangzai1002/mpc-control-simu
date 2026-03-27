#ifndef RTSP_RTSP_REQUEST_HPP
#define RTSP_RTSP_REQUEST_HPP

#include <algorithm>
#include <cstring>

#include "Incomplete.hpp"
#include "RtspMsgBase.hpp"

struct RtspResponse : public RtspMsgBase {
    std::string version;
    std::string status_code;
    std::string status_phrase;

    void reset() {
        version = std::string();
        status_code = std::string();
        status_phrase = std::string();
        num_headers = 0;
        content = std::string();
    }

    std::size_t parse(const std::uint8_t* p, std::size_t s) noexcept(false) {
        std::size_t touched_len = 0;
        auto        r = parse_status_line(p, s);
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

private:
    std::size_t parse_status_line(const std::uint8_t* p, std::size_t s) {
        auto f = std::search(p, p + s, kCRLF, kCRLF + 2);
        if (f == (p + s)) {
            throw Incomplete();
        }
        std::string v((const char*)p, f - p);

        auto pos = v.find(' ');
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        version = std::string(v.data(), pos);

        pos = v.find_first_not_of(' ', pos + 1);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        auto saved = pos;
        pos = v.find(' ', pos);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        status_code = std::string(v.data() + saved, pos - saved);

        pos = v.find_first_not_of(' ', pos + 1);
        if (pos == std::string::npos) {
            throw std::runtime_error("headline format error");
        }
        status_phrase = std::string(v.data() + pos, pos - saved);

        return (f - p + 2);
    };
};

#endif
