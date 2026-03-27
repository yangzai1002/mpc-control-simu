#include "yuv_udp.h"
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <time.h>

// ─────────────────────────────────────────────
// YuvUdpSender
// ─────────────────────────────────────────────

YuvUdpSender::YuvUdpSender() : sock_(-1), frame_id_(0), pkt_interval_us_(0) {}

YuvUdpSender::~YuvUdpSender() { close(); }

bool YuvUdpSender::init(const std::string& dst_ip, uint16_t dst_port,
                        uint32_t pkt_interval_us)
{
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) { perror("sender socket"); return false; }

    std::memset(&dst_addr_, 0, sizeof(dst_addr_));
    dst_addr_.sin_family      = AF_INET;
    dst_addr_.sin_port        = htons(dst_port);
    dst_addr_.sin_addr.s_addr = inet_addr(dst_ip.c_str());

    int sndbuf = 8 * 1024 * 1024; // 8MB 发送缓冲
    setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));

    pkt_interval_us_ = pkt_interval_us;
    return true;
}

bool YuvUdpSender::sendFrame(uint32_t frame_id,
                              int width, int height,
                              const uint8_t* data, size_t size)
{
    if (sock_ < 0) return false;

    uint16_t pkt_total = static_cast<uint16_t>((size + kMaxPayload - 1) / kMaxPayload);
    std::vector<uint8_t> buf(sizeof(PktHeader) + kMaxPayload);

    for (uint16_t i = 0; i < pkt_total; ++i) {
        size_t offset       = i * kMaxPayload;
        size_t payload_size = std::min(kMaxPayload, size - offset);

        PktHeader hdr{};
        hdr.magic        = kMagic;
        hdr.frame_id     = frame_id;
        hdr.pkt_index    = i;
        hdr.pkt_total    = pkt_total;
        hdr.frame_width  = static_cast<uint32_t>(width);
        hdr.frame_height = static_cast<uint32_t>(height);
        hdr.payload_size = static_cast<uint32_t>(payload_size);
        hdr.frame_size   = static_cast<uint32_t>(size);

        std::memcpy(buf.data(), &hdr, sizeof(hdr));
        std::memcpy(buf.data() + sizeof(hdr), data + offset, payload_size);

        ssize_t sent = sendto(sock_, buf.data(), sizeof(hdr) + payload_size, 0,
                              reinterpret_cast<const sockaddr*>(&dst_addr_),
                              sizeof(dst_addr_));
        if (sent < 0) {
            if (errno == ECONNREFUSED || errno == ENETUNREACH ||
                errno == EHOSTUNREACH)
                return true; // 接收端未就绪，静默丢弃本帧
            perror("sendto");
            return false;
        }

        // 包间限速：均匀分散发包，避免突发打爆内核缓冲区
        if (pkt_interval_us_ > 0 && i < pkt_total - 1) {
            struct timespec ts;
            ts.tv_sec  = pkt_interval_us_ / 1000000;
            ts.tv_nsec = (pkt_interval_us_ % 1000000) * 1000;
            nanosleep(&ts, nullptr);
        }
    }
    return true;
}

void YuvUdpSender::close()
{
    if (sock_ >= 0) { ::close(sock_); sock_ = -1; }
}

// ─────────────────────────────────────────────
// YuvUdpReceiver
// ─────────────────────────────────────────────

YuvUdpReceiver::YuvUdpReceiver() : sock_(-1), running_(false) {}

YuvUdpReceiver::~YuvUdpReceiver() { stop(); }

bool YuvUdpReceiver::init(uint16_t local_port, FrameCallback cb)
{
    cb_   = cb;
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) { perror("receiver socket"); return false; }

    // 接收缓冲区放大到 8MB
    int rcvbuf = 8 * 1024 * 1024;
    setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(local_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("receiver bind"); return false;
    }

    // 设置接收超时，使 run() 能响应 stop()
    timeval tv{0, 200000}; // 200ms
    setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    assemblers_.resize(kMaxFrames);
    for (auto& a : assemblers_) a.frame_id = UINT32_MAX;

    return true;
}

void YuvUdpReceiver::run()
{
    running_ = true;
    std::vector<uint8_t> buf(kMaxUdpSize);

    while (running_) {
        ssize_t n = recv(sock_, buf.data(), buf.size(), 0);
        if (n <= 0) continue;
        handlePacket(buf.data(), static_cast<size_t>(n));
    }
}

void YuvUdpReceiver::stop()
{
    running_ = false;
    if (sock_ >= 0) { ::close(sock_); sock_ = -1; }
}

void YuvUdpReceiver::handlePacket(const uint8_t* buf, size_t len)
{
    if (len < sizeof(PktHeader)) return;

    PktHeader hdr;
    std::memcpy(&hdr, buf, sizeof(hdr));

    if (hdr.magic != kMagic) return;
    if (hdr.pkt_index >= hdr.pkt_total) return;
    if (len < sizeof(hdr) + hdr.payload_size) return;

    // 找或分配 assembler
    FrameAssembler* slot = nullptr;
    FrameAssembler* oldest = nullptr;

    for (auto& a : assemblers_) {
        if (a.frame_id == hdr.frame_id) { slot = &a; break; }
        if (!oldest || a.frame_id < oldest->frame_id) oldest = &a;
    }

    if (!slot) {
        // 复用最旧的槽
        slot = oldest;
        slot->frame_id   = hdr.frame_id;
        slot->width      = static_cast<int>(hdr.frame_width);
        slot->height     = static_cast<int>(hdr.frame_height);
        slot->frame_size = hdr.frame_size;
        slot->pkt_total  = hdr.pkt_total;
        slot->received   = 0;
        slot->data.assign(hdr.frame_size, 0);
        slot->got.assign(hdr.pkt_total, false);
    }

    if (slot->got[hdr.pkt_index]) return; // 重复包

    // 写入 payload
    size_t offset = static_cast<size_t>(hdr.pkt_index) * kMaxPayload;
    if (offset + hdr.payload_size > slot->frame_size) return;
    std::memcpy(slot->data.data() + offset,
                buf + sizeof(hdr), hdr.payload_size);

    slot->got[hdr.pkt_index] = true;
    ++slot->received;

    // 全部到齐，触发回调
    if (slot->received == slot->pkt_total) {
        if (cb_)
            cb_(slot->frame_id, slot->width, slot->height, slot->data);
        slot->frame_id = UINT32_MAX; // 释放槽
    }
}
