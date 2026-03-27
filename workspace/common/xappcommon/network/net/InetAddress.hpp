#ifndef NET_INET_ADDRESS_H
#define NET_INET_ADDRESS_H
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdint>
#include <string>

#include "StringUtils.hpp"

class Ipv4Address {
public:
    Ipv4Address() = default;

    explicit Ipv4Address(const sockaddr_in &s) {
        _inaddr = s;
    }

    Ipv4Address(const std::string &ip, std::uint16_t port) : _inaddr() {
        ip_port(ip, port);
    }

    void ip_port(const std::string &ip, std::uint16_t port) {
        _inaddr.sin_family = AF_INET;
        _inaddr.sin_addr.s_addr = inet_addr(ip.c_str());
        _inaddr.sin_port = htons(port);
    }

    [[nodiscard]] std::string string() const {
        return ip() + ":" + std::to_string(port());
    }

    [[nodiscard]] std::string ip() const {
        return app_common::StringUtils::inet_ntoa(_inaddr.sin_addr.s_addr);
    }

    void ip(const std::string &ip) {
        ip_port(ip, port());
    }

    [[nodiscard]] std::uint16_t port() const {
        return ntohs(_inaddr.sin_port);
    }

    void port(std::uint16_t port) {
        ip_port(ip(), port);
    }

    sockaddr_in to_sockaddr_in() const {
        auto x = _inaddr;
        return x;
    }

private:
    sockaddr_in _inaddr{};
};

#endif
