#ifndef NET_RTP_RECEIVER_HPP
#define NET_RTP_RECEIVER_HPP

#include <netinet/in.h>

#include <cstdint>
#include <string>

#include "../media/Receiver.hpp"

class RtpReceiver : public Receiver {
public:
    explicit RtpReceiver(std::uint32_t ssrc) : _ssrc(ssrc) {
    }

    ~RtpReceiver() override = default;

    [[nodiscard]] std::uint32_t ssrc() const {
        return _ssrc;
    }

    virtual std::string ip() = 0;

    virtual std::uint16_t rtp_port() = 0;

    virtual std::uint16_t rtcp_port() = 0;

    virtual void push_rtcp(std::int32_t fd, const std::uint8_t *p, std::size_t s) noexcept(false) = 0;

    virtual void push_rtcp(std::int32_t fd, const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2,
                           std::size_t s2) noexcept(false) = 0;

protected:
    std::uint32_t _ssrc;
};

#endif
