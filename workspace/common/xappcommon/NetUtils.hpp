#ifndef AVOS_COMMON_UTILS_NET_UTILS_HPP
#define AVOS_COMMON_UTILS_NET_UTILS_HPP

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>

#include "StringUtils.hpp"

namespace app_common {
    class NetUtils {
    public:
        NetUtils() = delete;

    public:
        [[maybe_unused]] static std::string get_dev_ip(const std::string &dev) {
            auto fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (fd == -1) {
                throw std::runtime_error(std::string("socket error with ") + strerror(errno));
            }

            struct ifreq ifr {};
            strncpy(ifr.ifr_name, dev.c_str(), sizeof(ifr.ifr_name) - 1);
            if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
                close(fd);
                throw std::runtime_error(std::string("ioctl error with ") + strerror(errno));
            }
            close(fd);

            auto sin = (struct sockaddr_in *)&(ifr.ifr_addr);
            return inet_ntoa(sin->sin_addr);
        }

        [[maybe_unused]] static int tcp_connect(const char *ip, std::uint16_t port, std::int32_t linger_time,
                                                std::int32_t timeout_in_ms) {
            auto sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
            if (sock < 0) {
                throw std::runtime_error("socket error-" + StringUtils::str_error(errno));
            }

            linger so_linger{.l_onoff = 1, .l_linger = linger_time};
            if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
                throw std::runtime_error("setsockopt(SO_LINGER) error-" + StringUtils::str_error(errno));
            }

            sockaddr_in dest_address{
                .sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {.s_addr = inet_addr(ip)}};
            auto result = connect(sock, (sockaddr *)&dest_address, sizeof(dest_address));
            if (result < 0 && errno != EINPROGRESS) {
                close(sock);
                throw std::runtime_error("connect error-" + StringUtils::str_error(errno));
            }

            if (0 != result) {
                fd_set rdevents;
                FD_ZERO(&rdevents);
                FD_SET(sock, &rdevents);
                auto wrevents = rdevents;
                auto exevents = rdevents;

                timeval tv{.tv_sec = 0, .tv_usec = timeout_in_ms * 1000};
                result = select(sock + 1, &rdevents, &wrevents, &exevents, &tv);
                if (result <= 0) {
                    close(sock);
                    throw std::runtime_error("connect(select) error-" + StringUtils::str_error(errno));
                } else {
                    int       error;
                    socklen_t len = sizeof(error);
                    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                        close(sock);
                        throw std::runtime_error("getsockopt(SO_ERROR) error-" + StringUtils::str_error(errno));
                    }
                    if (error) {
                        close(sock);
                        throw std::runtime_error("connect error-" + StringUtils::str_error(error));
                    }
                }
            }

            return sock;
        }

        [[maybe_unused]] static std::int32_t udp_listen(std::uint32_t ip, std::uint16_t &port,
                                                        bool broadcast = false) noexcept(false) {
            auto sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
            if (sock < 0) {
                throw std::runtime_error("socket error with " + StringUtils::str_error(errno));
            }

            const int opt = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                close(sock);
                throw std::runtime_error("setsockopt(SO_REUSEADDR) error with " + StringUtils::str_error(errno));
            }

            if (broadcast) {
                if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) < 0) {
                    close(sock);
                    throw std::runtime_error("setsockopt(SO_BROADCAST) error with " + StringUtils::str_error(errno));
                }
            }

            sockaddr_in addrto{.sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {.s_addr = ip}};
            if (bind(sock, (struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) < 0) {
                close(sock);
                throw std::runtime_error("bind error with " + StringUtils::str_error(errno));
            }

            if (0 == port) {
                sockaddr_in c{};
                socklen_t   clen = sizeof(c);
                if (getsockname(sock, (sockaddr *)&c, &clen) < 0) {
                    close(sock);
                    throw std::runtime_error("getsockname error with " + StringUtils::str_error(errno));
                }
                port = ntohs(c.sin_port);
            }

            return sock;
        }

        [[maybe_unused]] static std::int32_t udp_listen(const std::string &ip, std::uint16_t &port,
                                                        bool broadcast = false) noexcept(false) {
            sockaddr_in addr{};

            if (0 == inet_pton(AF_INET, ip.c_str(), &addr)) {
                throw std::runtime_error("inet_pton error");
            }

            return udp_listen(addr.sin_addr.s_addr, port, broadcast);
        }
    };
} // namespace app_common

#endif