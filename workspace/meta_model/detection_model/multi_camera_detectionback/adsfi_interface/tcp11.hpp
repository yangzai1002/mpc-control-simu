#ifndef TCP11_HPP
#define TCP11_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <memory>
#include <sys/socket.h>   // AF_INET / AF_INET6 / sockaddr_*
#include <netinet/in.h>   // sockaddr_in / sockaddr_in6 / htons …
#include <arpa/inet.h>    // inet_pton / inet_ntop

namespace tcp11
{

class Socket
{
public:
    // domain = AF_INET 或 AF_INET6
    explicit Socket(int domain = AF_INET);
    ~Socket() noexcept;

    // 移动语义
    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;
    // 禁止拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 服务端用：绑定 + 监听
    void bind(const std::string& host, std::uint16_t port);
    void listen(int backlog = 128);
    // 阻塞接受连接，返回新的已连接 Socket
    Socket accept(std::string* clientHost = nullptr,
                  std::uint16_t* clientPort = nullptr);

    // 客户端用：连接
    void connect(const std::string& host, std::uint16_t port);

    // 收发（字节流，无边界）
    std::size_t send(const void* buf, std::size_t len);
    std::size_t recv(void* buf, std::size_t len);

    // 方便重载
    std::size_t send(const std::vector<uint8_t>& d)
    { return send(d.data(), d.size()); }

    std::vector<uint8_t> recv(std::size_t maxLen = 65536);

    // 超时（仅阻塞模式）
    void send_timeout(std::chrono::milliseconds ms);
    void recv_timeout(std::chrono::milliseconds ms);

    // 阻塞/非阻塞切换
    void set_nonblock(bool on = true);

    // 获取原始 fd，便于 select/poll
    int native_handle() const noexcept { return fd_; }
    // 阻塞直到全部发出
    std::size_t send_all(const void* buf, std::size_t len);
    // 阻塞直到正好收满 len 字节
    std::size_t recv_all(void* buf, std::size_t len);
private:
    int fd_ = -1;
    int  domain_ = AF_INET;
};

} // namespace tcp11
#endif // TCP11_HPP
