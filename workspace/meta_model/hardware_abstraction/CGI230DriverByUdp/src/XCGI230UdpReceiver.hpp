#ifndef __SOCKET_CAN_TANSMITTER_HPP__
#define __SOCKET_CAN_TANSMITTER_HPP__

#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>
#include <memory.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <string>

#include "ap_log/ap_log_interface.h"

class CanFDTransmitter {
public:
    CanFDTransmitter(const CanFDConfig &config, const std::vector<struct can_filter> &rfilter) {
        std::string  error;
        sockaddr_can addr;
        ifreq        ifr;
        std::int32_t enable_canfd = 1;
        socketcan_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (socketcan_fd_ < 0) {
            error = std::string("socket failed: ") + strerror(errno);
            goto error1;
        }

        if (setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd)) != 0) {
            error = std::string("enable canfd failed: ") + strerror(errno);
            goto error2;
        }

        strcpy(ifr.ifr_name, config.can_name.c_str());
        ioctl(socketcan_fd_, SIOCGIFINDEX, &ifr);
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(socketcan_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            error = std::string("bind failed: ") + strerror(errno);
            goto error2;
        }

        if (setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter[0], rfilter.size()) < 0) {
            // error = std::string("set filter failed: ") + strerror(errno);
            // goto error2;
        }

        if (fcntl(socketcan_fd_, F_SETFL, fcntl(socketcan_fd_, F_GETFL) | O_NONBLOCK) < 0) {
            error = std::string("set nonblock failed: ") + strerror(errno);
            goto error2;
        }

        return;

    error2:
        close(socketcan_fd_);
        socketcan_fd_ = -1;

    error1:
        throw std::runtime_error(error);
    }
    ~CanFDTransmitter() {
        if (socketcan_fd_ > 0) {
            close(socketcan_fd_);
            socketcan_fd_ = -1;
        }
    }

    void frame_read(CanFDFrame &can_frame_rev, std::int32_t timeout_ms) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(socketcan_fd_, &readfds);

        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = timeout_ms * 1000;
        auto r = select(socketcan_fd_ + 1, &readfds, nullptr, nullptr, &tv);
        if (r <= 0) {
            throw std::runtime_error("select error(timeout)");
        }

        canfd_frame frame{};
        auto        nbytes = read(socketcan_fd_, &frame, sizeof(frame));
        if (nbytes < sizeof(struct can_frame)) {
            throw std::runtime_error("read error with " + std::to_string(nbytes) + ", errno: " + strerror(errno));
        }

        can_frame_rev.id = frame.can_id;
        can_frame_rev.data_len = frame.len;
        memcpy(can_frame_rev.data, frame.data, sizeof(can_frame_rev.data));
    }

    void frame_write(const CanFDFrame &can_frame_send) {
        canfd_frame frame{};
        frame.can_id = can_frame_send.id;
        frame.len = can_frame_send.data_len;
        memcpy(frame.data, can_frame_send.data, sizeof(can_frame_send.data));
        auto nbytes = write(socketcan_fd_, &frame, sizeof(frame));
        if (nbytes < sizeof(frame)) {
            throw std::runtime_error("write error with " + std::to_string(nbytes) + ", errno: " + strerror(errno));
        }
    }

private:
    std::int32_t socketcan_fd_;
    CanFDConfig  config_;
};

#endif
