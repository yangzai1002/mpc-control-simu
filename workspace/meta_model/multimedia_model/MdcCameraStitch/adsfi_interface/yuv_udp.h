#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>
#include <netinet/in.h>

/**
 * UDP 分包协议（YUV420SP 帧传输）
 *
 * 每个 UDP 包结构：
 *   [PktHeader][payload...]
 *
 * 分包策略：
 *   - 每包 payload 最大 kMaxPayload 字节
 *   - 接收端按 frame_id + pkt_index 组帧，全部到齐后回调
 */

static constexpr size_t kMaxPayload  = 60000; // 单包最大 payload（< 65507）
static constexpr size_t kMaxUdpSize  = kMaxPayload + 32;

#pragma pack(push, 1)
struct PktHeader {
    uint32_t magic;       // 0xYUV420SP
    uint32_t frame_id;    // 帧序号
    uint16_t pkt_index;   // 当前包索引（0-based）
    uint16_t pkt_total;   // 本帧总包数
    uint32_t frame_width;
    uint32_t frame_height;
    uint32_t payload_size; // 本包 payload 字节数
    uint32_t frame_size;   // 整帧总字节数
};
#pragma pack(pop)

static constexpr uint32_t kMagic = 0x59555650; // "YUVP"

// ─────────────────────────────────────────────
// 发送端
// ─────────────────────────────────────────────
class YuvUdpSender {
public:
    YuvUdpSender();
    ~YuvUdpSender();

    // 初始化，记录目标地址（不 connect，支持接收端后启动）
    // pkt_interval_us：包间隔微秒数，0=不限速，建议设为 1000000/fps/pkt_per_frame
    bool init(const std::string& dst_ip, uint16_t dst_port,
              uint32_t pkt_interval_us = 0);

    // 发送一帧 YUV420SP 数据，内部按 pkt_interval_us 均匀发包
    bool sendFrame(uint32_t frame_id,
                   int width, int height,
                   const uint8_t* data, size_t size);

    void close();

private:
    int           sock_;
    uint32_t      frame_id_;
    sockaddr_in   dst_addr_;
    uint32_t      pkt_interval_us_; // 包间隔（微秒）
};

// ─────────────────────────────────────────────
// 接收端
// ─────────────────────────────────────────────

// 组帧完成回调：frame_id, width, height, yuv数据
using FrameCallback = std::function<void(uint32_t frame_id,
                                         int width, int height,
                                         const std::vector<uint8_t>& yuv)>;

class YuvUdpReceiver {
public:
    YuvUdpReceiver();
    ~YuvUdpReceiver();

    // 绑定本地端口，设置回调
    bool init(uint16_t local_port, FrameCallback cb);

    // 阻塞接收循环，调用 stop() 后退出
    void run();

    void stop();

private:
    struct FrameAssembler {
        uint32_t              frame_id;
        int                   width;
        int                   height;
        uint32_t              frame_size;
        uint16_t              pkt_total;
        uint16_t              received;
        std::vector<uint8_t>  data;
        std::vector<bool>     got;
    };

    void handlePacket(const uint8_t* buf, size_t len);

    int           sock_;
    bool          running_;
    FrameCallback cb_;

    // 最多缓存 4 帧（应对乱序）
    static constexpr int kMaxFrames = 4;
    std::vector<FrameAssembler> assemblers_;
};
