#include "tcp11.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace tcp11
{

static void throw_sys(const char* what)
{
    throw std::runtime_error(std::string(what) + ": " + strerror(errno));
}

Socket::Socket(int domain) : domain_(domain)
{
    fd_ = ::socket(domain, SOCK_STREAM, 0);
    if (fd_ < 0) throw_sys("socket");
    // 端口复用
    int on = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

Socket::~Socket() noexcept
{
    if (fd_ >= 0) ::close(fd_);
}

Socket::Socket(Socket&& o) noexcept : fd_(o.fd_), domain_(o.domain_)
{ o.fd_ = -1; }

Socket& Socket::operator=(Socket&& o) noexcept
{
    if (this != &o)
    {
        if (fd_ >= 0) ::close(fd_);
        fd_ = o.fd_; o.fd_ = -1;
        domain_ = o.domain_;
    }
    return *this;
}

void Socket::bind(const std::string& host, std::uint16_t port)
{
    sockaddr_storage addr = {};
    socklen_t alen = 0;
    if (domain_ == AF_INET)
    {
        auto* a = reinterpret_cast<sockaddr_in*>(&addr);
        a->sin_family = AF_INET;
        a->sin_port   = htons(port);
        if (host.empty() || host == "*")
            a->sin_addr.s_addr = INADDR_ANY;
        else if (::inet_pton(AF_INET, host.c_str(), &a->sin_addr) != 1)
            throw std::invalid_argument("invalid ipv4 bind address");
        alen = sizeof(sockaddr_in);
    }
    else if (domain_ == AF_INET6)
    {
        auto* a = reinterpret_cast<sockaddr_in6*>(&addr);
        a->sin6_family = AF_INET6;
        a->sin6_port   = htons(port);
        if (host.empty() || host == "*")
            a->sin6_addr = in6addr_any;
        else if (::inet_pton(AF_INET6, host.c_str(), &a->sin6_addr) != 1)
            throw std::invalid_argument("invalid ipv6 bind address");
        alen = sizeof(sockaddr_in6);
    }
    else
        throw std::invalid_argument("unsupported domain");

    if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), alen) < 0)
        throw_sys("bind");
}

void Socket::listen(int backlog)
{
    if (::listen(fd_, backlog) < 0) throw_sys("listen");
}

Socket Socket::accept(std::string* host, std::uint16_t* port)
{
    sockaddr_storage addr = {};
    socklen_t alen = sizeof(addr);
    int connfd = ::accept(fd_, reinterpret_cast<sockaddr*>(&addr), &alen);
    if (connfd < 0) throw_sys("accept");

    Socket ret;
    ret.fd_ = connfd;
    ret.domain_ = domain_;

    if (host || port)
    {
        char ipbuf[INET6_ADDRSTRLEN] = {};
        if (addr.ss_family == AF_INET)
        {
            auto* a = reinterpret_cast<sockaddr_in*>(&addr);
            if (host) *host = ::inet_ntop(AF_INET, &a->sin_addr, ipbuf, sizeof(ipbuf));
            if (port) *port = ntohs(a->sin_port);
        }
        else
        {
            auto* a = reinterpret_cast<sockaddr_in6*>(&addr);
            if (host) *host = ::inet_ntop(AF_INET6, &a->sin6_addr, ipbuf, sizeof(ipbuf));
            if (port) *port = ntohs(a->sin6_port);
        }
    }
    return ret;
}

void Socket::connect(const std::string& host, std::uint16_t port)
{
    sockaddr_storage addr = {};
    socklen_t alen = 0;
    if (domain_ == AF_INET)
    {
        auto* a = reinterpret_cast<sockaddr_in*>(&addr);
        a->sin_family = AF_INET;
        a->sin_port   = htons(port);
        if (::inet_pton(AF_INET, host.c_str(), &a->sin_addr) != 1)
            throw std::invalid_argument("invalid ipv4 connect address");
        alen = sizeof(sockaddr_in);
    }
    else
    {
        auto* a = reinterpret_cast<sockaddr_in6*>(&addr);
        a->sin6_family = AF_INET6;
        a->sin6_port   = htons(port);
        if (::inet_pton(AF_INET6, host.c_str(), &a->sin6_addr) != 1)
            throw std::invalid_argument("invalid ipv6 connect address");
        alen = sizeof(sockaddr_in6);
    }
    if (::connect(fd_, reinterpret_cast<sockaddr*>(&addr), alen) < 0)
        throw_sys("connect");
}

std::size_t Socket::send(const void* buf, std::size_t len)
{
    ssize_t n = ::write(fd_, buf, len);
    if (n < 0) throw_sys("send");
    return static_cast<std::size_t>(n);
}

std::size_t Socket::recv(void* buf, std::size_t len)
{
    ssize_t n = ::read(fd_, buf, len);
    if (n < 0) throw_sys("recv");
    if (n == 0) throw std::runtime_error("peer closed");
    return static_cast<std::size_t>(n);
}

std::vector<uint8_t> Socket::recv(std::size_t maxLen)
{
    std::vector<uint8_t> buf(maxLen);
    std::size_t n = recv(buf.data(), buf.size());
    buf.resize(n);
    return buf;
}

void Socket::set_nonblock(bool on)
{
    int fl = ::fcntl(fd_, F_GETFL, 0);
    if (fl < 0) throw_sys("fcntl(F_GETFL)");
    if (on) fl |= O_NONBLOCK;
    else    fl &= ~O_NONBLOCK;
    if (::fcntl(fd_, F_SETFL, fl) < 0) throw_sys("fcntl(F_SETFL)");
}

void Socket::send_timeout(std::chrono::milliseconds ms)
{
    timeval tv{};
    tv.tv_sec  = ms.count() / 1000;
    tv.tv_usec = (ms.count() % 1000) * 1000;
    if (::setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
        throw_sys("setsockopt(SO_SNDTIMEO)");
}

void Socket::recv_timeout(std::chrono::milliseconds ms)
{
    timeval tv{};
    tv.tv_sec  = ms.count() / 1000;
    tv.tv_usec = (ms.count() % 1000) * 1000;
    if (::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        throw_sys("setsockopt(SO_RCVTIMEO)");
}

std::size_t Socket::send_all(const void* buf, std::size_t len)
{
    const char* p   = static_cast<const char*>(buf);
    std::size_t left = len;
    while (left > 0) {
        ssize_t n = ::write(fd_, p, left);
        if (n < 0) {
            if (errno == EINTR) continue;          // 被信号中断，重试
            throw_sys("send_all");
        }
        left -= n;
        p    += n;
    }
    return len;
}

std::size_t Socket::recv_all(void* buf, std::size_t len)
{
    char* p   = static_cast<char*>(buf);
    std::size_t left = len;
    while (left > 0) {
        ssize_t n = ::read(fd_, p, left);
        if (n < 0) {
            if (errno == EINTR) continue;
            throw_sys("recv_all");
        }
        if (n == 0) throw std::runtime_error("peer closed connection");
        left -= n;
        p    += n;
    }
    return len;
}

} // namespace tcp11
