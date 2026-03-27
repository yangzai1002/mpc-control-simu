#ifndef ANCILLARY_DEV_BINOCULAR_RTSPMSGBASE_HPP
#define ANCILLARY_DEV_BINOCULAR_RTSPMSGBASE_HPP

#include <cstring>
#include <stdexcept>
#include <string_view>

#include "../sdp/sdp.h"
#include "Incomplete.hpp"
#include "picohttpparser.h"
class RtspMsgBase {
public:
    enum class Method : std::int32_t {
        NONE,
        OPTIONS,
        DESCRIBE,
        SETUP,
        PLAY,
        TEARDOWN,
        GET_PARAMETER,
    };

    struct Transport {
        bool          tcp;
        std::uint16_t rtp;
        std::uint16_t rtcp;

        Transport(std::uint16_t rtp, std::uint16_t rtcp, bool tcp = false) : tcp(tcp), rtp(rtp), rtcp(rtcp) {
        }
    };

    [[nodiscard]] std::string header_value(const std::string& key) const {
        for (size_t i = 0; i < num_headers; i++) {
            if (iequal(headers[i].name, headers[i].name_len, key.data(), key.length()))
                return {headers[i].value, headers[i].value_len};
        }

        return {};
    };

    [[nodiscard]] sdp_payload* parse_sdp() const {
        return sdp_parse(std::string(content).c_str());
    };

protected:
    std::size_t parse_headers(const std::uint8_t* p, std::size_t s) {
        num_headers = sizeof(headers) / sizeof(headers[0]);
        memset(headers, 0, sizeof(headers));

        auto r = phr_parse_headers(reinterpret_cast<const char*>(p), s, headers, &num_headers, 0);
        if (r < 0 && r != -2) {
            throw std::runtime_error("headers parse error");
        }

        return r;
    };

    std::size_t parse_content(const std::uint8_t* p, std::size_t s, std::size_t header_content_length) {
        if (s < header_content_length) {
            throw Incomplete();
        }

        content = std::string(reinterpret_cast<const char*>(p), s);
        return header_content_length;
    };

    std::size_t num_headers{};
    phr_header  headers[32]{};

protected:
    static bool iequal(const char* s, size_t l, const char* t, size_t size) {
        if (size != l)
            return false;

        for (size_t i = 0; i < l; i++) {
            if (std::tolower(s[i]) != std::tolower(t[i]))
                return false;
        }

        return true;
    };

    static bool iequal(const std::string& v, const char* t, size_t size) {
        return iequal(v.data(), v.size(), t, size);
    };

    static bool i_n_equal(const std::string& v, const char* t, size_t size) {
        if (v.size() < size) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            if (std::tolower(v[i]) != std::tolower(t[i])) {
                return false;
            }
        }

        return true;
    };

    // static const char* kCRLF;
    std::string        content;
};


#define kCRLF "\r\n"
// const char *RtspMsgBase::kCRLF = "\r\n";

#endif // ANCILLARY_DEV_BINOCULAR_RTSPMSGBASE_HPP
