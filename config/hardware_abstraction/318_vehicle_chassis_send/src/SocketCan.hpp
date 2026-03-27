#ifndef __SOCKET_CAN_FD_TRANSMITTER_HPP__
#define __SOCKET_CAN_FD_TRANSMITTER_HPP__

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "custom_stack.h"
#include "log/log.h"

// CAN FD帧结构定义，支持64字节数据
struct SocketCanFdFrame {
    int    id;          // CAN ID
    double time_stamp;  // 时间戳
    int    remote_flag; // 远程帧标志
    int    extend_flag; // 扩展帧标志
    int    fd_flag;     // CAN FD标志
    int    brs_flag;    // 比特率切换标志
    int    data_len;    // 数据长度 (0-64)
    char   data[64];    // 数据缓冲区，支持64字节
    SocketCanFdFrame() {
        id = -1;
        time_stamp = 0.0;
        remote_flag = 0;
        extend_flag = 0;
        fd_flag = 0;
        brs_flag = 0;
        data_len = 0;
        memset(data, 0, 64);
    }
};

struct SocketCanFdConfig {
    std::string can_name;  // CAN接口名称，如"can0"
    bool        enable_fd; // 是否启用CAN FD模式
    SocketCanFdConfig() {
        can_name = "can0";
        enable_fd = true; // 默认启用CAN FD
    }
};

class SocketcanFdTransmitter {
public:
    SocketcanFdTransmitter(const SocketCanFdConfig &config, const std::vector<struct can_filter> &rfilter)
        : is_connected_(false), config_(config) {
        Init(rfilter);
    }

    ~SocketcanFdTransmitter() {
        if (is_connected_ && socketcan_fd_ >= 0) {
            close(socketcan_fd_);
        }
    }

    bool IsConnected() const {
        return is_connected_;
    }

    bool ReInit(const std::vector<struct can_filter> &rfilter) {
        if (is_connected_ && socketcan_fd_ >= 0) {
            close(socketcan_fd_);
            is_connected_ = false;
        }
        return Init(rfilter);
    }

    bool Init(const std::vector<struct can_filter> &rfilter) {
        sockaddr_can addr;
        ifreq        ifr;

        // 创建CAN FD套接字，使用CAN_RAW_FD_FRAMES标志
        socketcan_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (socketcan_fd_ < 0) {
            perror("socket_can_fd--> socket:");
            std::cout << "socket_can_fd 初始化失败!!!" << std::endl;
            return false;
        }

        // 如果启用CAN FD模式，设置相应选项
        if (config_.enable_fd) {
            int enable = 1;
            if (setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable, sizeof(enable)) < 0) {
                perror("socket_can_fd--> setsockopt(CAN_RAW_FD_FRAMES):");
                close(socketcan_fd_);
                return false;
            }

            // if (fcntl(socketcan_fd_, F_SETFL, O_NONBLOCK) < 0) {
            //     perror("fcntl O_NONBLOCK");
            //     close(socketcan_fd_);
            //     return false;
            // }

            // int sndbuf_size = 8 * 1024 * 1024;  // 512KB
            // if (setsockopt(socketcan_fd_, SOL_SOCKET, SO_SNDBUF,
            //             &sndbuf_size, sizeof(sndbuf_size)) < 0) {
            //     perror("setsockopt SO_SNDBUF");
            // }
        }

        // 设置CAN接口名称
        strncpy(ifr.ifr_name, config_.can_name.c_str(), IFNAMSIZ - 1);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(socketcan_fd_, SIOCGIFINDEX, &ifr) < 0) {
            perror("socket_can_fd--> ioctl:");
            close(socketcan_fd_);
            return false;
        }

        // 绑定CAN接口
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(socketcan_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("socket_can_fd--> bind:");
            std::cout << "socket_can_fd 绑定失败!!!" << std::endl;
            close(socketcan_fd_);
            return false;
        }

        // 设置过滤器
        if (!rfilter.empty()) {
            setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter.data(),
                       rfilter.size() * sizeof(struct can_filter));
        }

        is_connected_ = true;
        return true;
    }

    // 读取CAN FD帧
    int ReadCanFrame(SocketCanFdFrame &can_frame_rev) {
        if (!is_connected_)
            return -1;

        // 使用canfd_frame结构接收CAN FD帧
        struct canfd_frame frame;
        memset(&frame, 0, sizeof(frame));

        // 读取帧数据，CAN FD帧大小为sizeof(canfd_frame)
        int nbytes = read(socketcan_fd_, &frame, sizeof(struct canfd_frame));
        if (nbytes < 0) {
            fprintf(stderr, "cannot read socketcan fd data\n");
            return -1;
        }

        // 检查接收的数据长度是否正确
        if (nbytes != sizeof(struct canfd_frame) && nbytes != sizeof(struct can_frame)) {
            fprintf(stderr, "socketcan fd read: incomplete frame (received %d bytes)\n", nbytes);
            return -1;
        }

        // 解析帧信息
        can_frame_rev.id = frame.can_id & CAN_EFF_MASK;
        can_frame_rev.remote_flag = (frame.can_id & CAN_RTR_FLAG) ? 1 : 0;
        can_frame_rev.extend_flag = (frame.can_id & CAN_EFF_FLAG) ? 1 : 0;
        can_frame_rev.fd_flag = (nbytes == sizeof(struct canfd_frame)) ? 1 : 0;
        can_frame_rev.brs_flag = (frame.flags & CANFD_BRS) ? 1 : 0; // 比特率切换标志
        can_frame_rev.data_len = frame.len;

        // 复制数据（最多64字节）
        size_t copy_len = (frame.len > 64) ? 64 : frame.len;
        memcpy(can_frame_rev.data, frame.data, copy_len);

        return 0;
    }

    // 发送CAN FD帧
    int WriteCanFrame(const SocketCanFdFrame &can_frame_send) {
        // AINFO << "进入"  << is_connected_;

        if (!is_connected_) {
            AINFO << "连接失败";
            return -1;
        }

        // AINFO << "连接成功";

        // 检查数据长度是否合法（CAN FD最大64字节）
        if (can_frame_send.data_len < 0 || can_frame_send.data_len > 64) {
            fprintf(stderr, "invalid data length for CAN FD frame (must be 0-64)\n");
            return -1;
        }
        // AINFO << "长度符合标准" ;

        struct canfd_frame frame;
        memset(&frame, 0, sizeof(frame));

        // 设置CAN ID和标志
        frame.can_id = can_frame_send.id;
        if (can_frame_send.remote_flag)
            frame.can_id |= CAN_RTR_FLAG;
        if (can_frame_send.extend_flag)
            frame.can_id |= CAN_EFF_FLAG;

        // 设置CAN FD标志和数据长度
        frame.len = can_frame_send.data_len;
        // AINFO << "长度：" << can_frame_send.data_len;

        if (can_frame_send.fd_flag) {
            frame.flags = 0;
            if (can_frame_send.brs_flag) {
                frame.flags |= CANFD_BRS; // 启用比特率切换
                // AINFO << "是CANFD";
            }
        }

        // 复制数据
        memcpy(frame.data, can_frame_send.data, can_frame_send.data_len);

        // AINFO << "fd_flg" << can_frame_send.fd_flag;
        // AINFO << "FD:" << sizeof(struct canfd_frame);
        // AINFO << "CAN：" << sizeof(struct can_frame);
        // 发送帧（根据是否是FD帧选择不同的大小）
        int nbytes = write(socketcan_fd_, &frame,
                           can_frame_send.fd_flag ? sizeof(struct canfd_frame) : sizeof(struct can_frame));
        if (nbytes < 0) {
            perror("socketcan fd write failed");
            AINFO << "write failed with errno: " << errno << " (" << strerror(errno) << ")";
            AINFO << "写入失败" << nbytes;
            return -1;
        }
        // AINFO << "写入成功：" << nbytes;
        return 0;
    }

private:
    int               socketcan_fd_;         // CAN套接字描述符
    bool              is_connected_ = false; // 连接状态
    SocketCanFdConfig config_;               // 配置信息
};

#endif
