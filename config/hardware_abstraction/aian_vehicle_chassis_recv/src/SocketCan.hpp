#ifndef __SOCKET_CAN_TANSMITTER_HPP__
#define __SOCKET_CAN_TANSMITTER_HPP__

#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>
#include <memory.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

struct SocketCanFrame {
    int    id;
    double time_stamp;
    int    remote_flag;
    int    extend_flag;
    int    data_len;
    char   data[8];
    SocketCanFrame() {
        id = -1;
        time_stamp = 0.0;
        remote_flag = 0;
        extend_flag = 0;
        data_len = 0;
        memset(data, 0, 8);
    }
    void print() {
    }
};

struct SocketCanConfig {
    std::string can_name;
    SocketCanConfig() {
        can_name = "can0";
    }
};

class SocketcanTransmitter {
public:
    SocketcanTransmitter(const SocketCanConfig &config, const std::vector<struct can_filter> &rfilter) {
        sockaddr_can addr;
        ifreq        ifr;
        socketcan_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (socketcan_fd_ < 0) {
            perror("scoket_can--> socket:");
            exit(-1); // c++退出函数,
        }

        strcpy(ifr.ifr_name, config.can_name.c_str());
        ioctl(socketcan_fd_, SIOCGIFINDEX, &ifr);
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(socketcan_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("scoket_can--> bind:");
            exit(-1);
        }

        setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter[0], rfilter.size());
    }
    ~SocketcanTransmitter() {
    }

    int ReadCanFrame(SocketCanFrame &can_frame_rev) {
        can_frame frame;
        memset(&frame, 0, sizeof(frame));
        int nbytes = read(socketcan_fd_, &frame, sizeof(frame));
        if (nbytes < 0) {
            fprintf(stderr, "cannot read socketcan data\n");
            return -1;
        }
        if (nbytes < sizeof(struct can_frame)) {
            fprintf(stderr, "socketcan read: incomplete CAN frame\n");
            return -1;
        }

        can_frame_rev.id = frame.can_id;
        can_frame_rev.data_len = frame.can_dlc;
        memcpy(can_frame_rev.data, frame.data, 8);
        return 0;
    }
    int WriteCanFrame(const SocketCanFrame &can_frame_send) {
        can_frame frame;
        memset(&frame, 0, sizeof(frame));
        frame.can_id = can_frame_send.id;
        frame.can_dlc = can_frame_send.data_len;
        memcpy(frame.data, can_frame_send.data, 8);
        int nbytes = write(socketcan_fd_, &frame, sizeof(frame));
        if (nbytes < 0)
            return -1;
        return 0;
    }

private:
    int             socketcan_fd_;
    SocketCanConfig config_;
};

#endif
