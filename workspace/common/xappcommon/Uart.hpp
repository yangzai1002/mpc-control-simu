#ifndef UART_HPP
#define UART_HPP

#include <fcntl.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include "SimpleBuffer.hpp"
#include "StringUtils.hpp"

namespace app_common {
    class Uart {
    public:
        Uart() = delete;

        Uart(const std::string& name, std::int32_t rate, std::int32_t stopbits = 1) : name(name), fd(-1) {
            fd = open(name.c_str(), O_RDWR | O_NOCTTY);
            if (fd < 0) {
                throw std::runtime_error(StringUtils::str_error(errno));
            }

            struct termios tOptions {};
            bzero(&tOptions, sizeof(tOptions));

            // 对原来配置处理
            tcgetattr(fd, &tOptions);
            tcflush(fd, TCIOFLUSH);

            // 配置波特率
            cfsetispeed(&tOptions, rate);
            cfsetospeed(&tOptions, rate);

            // 立即生效
            tcsetattr(fd, TCSANOW, &tOptions);
            tcflush(fd, TCIOFLUSH);

            // 对原来配置处理
            tcgetattr(fd, &tOptions);
            tcflush(fd, TCIOFLUSH);

            // 8个数据位
            tOptions.c_cflag &= ~CSIZE;
            tOptions.c_cflag |= CS8; // 8个数据位
            // 1个停止位
            if (stopbits > 1) {
                tOptions.c_cflag |= CSTOPB;
            } else {
                tOptions.c_cflag &= ~CSTOPB;
            }
            // 无校验
            tOptions.c_cflag &= ~PARENB; // 关闭校验
            tOptions.c_iflag &= ~INPCK;  // 关闭奇偶校验
            // 设置最小字符和等待时间,都为0跟非阻塞一样
            // open未设置O_NONBLOCK或O_NDELAY的情况下。
            tOptions.c_cc[VTIME] = 10; // s等待
            tOptions.c_cc[VMIN] = 0;   // 最小字符为1

            cfmakeraw(&tOptions); // 设置为原始模式
            // c_cflag
            tOptions.c_cflag &= ~CRTSCTS;         // 禁止硬件流控
            tOptions.c_cflag |= (CLOCAL | CREAD); // 激活选项

            tcflush(fd, TCIFLUSH);
            if ((tcsetattr(fd, TCSANOW, &tOptions)) != 0) {
                close(fd);
                fd = -1;
                throw std::runtime_error(StringUtils::str_error(errno));
            }

            //    serial_struct serial{};
            //    if (0 != ioctl(fd, TIOCGSERIAL, &serial)) {
            //        close(fd);
            //        fd = -1;
            //        throw std::runtime_error(StringUtil::str_error(errno));
            //    }
            //
            //    serial.xmit_fifo_size = 1024 * 1024;
            //    if (0 != ioctl(fd, TIOCSSERIAL, &serial)) {
            //        close(fd);
            //        fd = -1;
            //        throw std::runtime_error(StringUtil::str_error(errno));
            //    }
        }

        ~Uart() {
            if (fd >= 0) {
                close(fd);
                fd = -1;
            }
        }

        const std::string& get_name() {
            return name;
        }

        void send(const void* d, size_t dlen) const noexcept(false) {
            auto r = write(fd, d, dlen);
            if (r != dlen) {
                throw std::runtime_error("ret=" + std::to_string(r) + " error: " + StringUtils::str_error(errno));
            }
        }

        void send_all(const std::uint8_t* d, size_t dlen) const noexcept(false) {
            size_t offset = 0;

            do {
                auto w = write(fd, d + offset, dlen - offset);
                if (w > 0) {
                    offset += w;
                } else if (w < 0 && errno == EINTR) {
                    usleep(1000);
                    continue;
                } else {
                    throw std::runtime_error("ret=" + std::to_string(w) + " error: " + StringUtils::str_error(errno));
                }
            } while (offset < dlen);
        }

        void send(const SimpleBuffer& b) const noexcept(false) {
            send_all(b.data(), b.len());
        }

        int recv(void* r, size_t rsize) const noexcept(false) {
            auto rlen = read(fd, r, rsize);
            if (rlen <= 0) {
                throw std::runtime_error(StringUtils::str_error(errno));
            }
            return rlen;
        }

        int select_and_recv(void* r, size_t rsize, time_t timeout_in_ms = 100) const noexcept(false) {
            fd_set rd{};
            FD_SET(fd, &rd);
            timeval tv{.tv_sec = 0, .tv_usec = 100000};

            auto o = select(fd + 1, &rd, nullptr, nullptr, &tv);
            if (o > 0) {
                return recv(r, rsize);
            }
            return 0;
        }

        [[nodiscard]] SimpleBuffer recv_all() const noexcept(false) {
            SimpleBuffer v;

            bool recvd = false;

            while (true) {
                fd_set rd{};
                FD_ZERO(&rd);
                FD_SET(fd, &rd);
                timeval tv{.tv_sec = 0, .tv_usec = 50000};
                auto    r = select(fd + 1, &rd, nullptr, nullptr, &tv);
                if (r < 0) {
                    if (EINTR == errno) {
                        continue;
                    }
                    throw std::runtime_error("select error: " + StringUtils::str_error(errno));
                } else if (0 == r) {
                    if (!recvd) {
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if (v.full()) {
                        v.resize(v.size() + 256);
                    }

                    auto n = read(fd, v.data() + v.len(), v.size() - v.len());
                    if (n < 0) {
                        if (EAGAIN == errno || EINTR == errno) {
                            continue;
                        }
                        throw std::runtime_error("read error: " + StringUtils::str_error(errno));
                    }
                    if (0 == n) {
                        break;
                    }
                    recvd = true;
                    v.len_add(n);
                }
            }

            return std::move(v);
        };

        [[nodiscard]] std::int32_t get_fd() const {
            return fd;
        }

    private:
        std::string  name;
        std::int32_t fd;
    };
}; // namespace app_common

#endif // HELMET_MCU_UART_HPP
