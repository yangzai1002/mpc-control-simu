/*
 * MsgBuffer — 存储序列化后字节流的环形缓冲区
 *
 * MsgFrame 持有两个时间戳：
 *   receiveTimeMs  — steady_clock 收包时刻，仅用于 Snapshot 时间窗口切片
 *   msgTimestampMs — 消息 header 中的 Unix epoch 时间，仅用于 bag 写入
 *
 * 两者分离的原因：传感器 header 时间戳可能基于硬件单调钟（与 steady_clock
 * 同源但与 system_clock 基准不同），若混用会导致 Snapshot 找不到数据。
 */
#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace rtfbag_util {

struct MsgFrame {
    uint64_t             receiveTimeMs;   // steady_clock：收包时刻，Snapshot 窗口索引用
    uint64_t             msgTimestampMs;  // Unix epoch ms：消息 header 时间，bag 写入用
    std::string          key;
    std::vector<uint8_t> payload;
};

class MsgBuffer {
public:
    explicit MsgBuffer(size_t maxMemoryBytes = 512UL * 1024 * 1024)
        : maxBytes_(maxMemoryBytes), usedBytes_(0) {}

    // 推入一帧。receiveTimeMs 由调用方传入（steady_clock），msgTimestampMs 来自消息 header
    void Push(const std::string& key,
              std::vector<uint8_t> payload,
              uint64_t receiveTimeMs,
              uint64_t msgTimestampMs) {
        size_t sz = payload.size();
        std::lock_guard<std::mutex> lk(mutex_);
        while (!queue_.empty() && usedBytes_ + sz > maxBytes_) {
            usedBytes_ -= queue_.front().payload.size();
            queue_.pop_front();
        }
        usedBytes_ += sz;
        queue_.push_back({receiveTimeMs, msgTimestampMs, key, std::move(payload)});
    }

    // 按 receiveTimeMs 做时间窗口切片（steady_clock 基准，与 Trigger 锚点一致）
    std::vector<MsgFrame> Snapshot(uint64_t anchorMs,
                                   uint64_t preMs,
                                   uint64_t postMs) const {
        std::lock_guard<std::mutex> lk(mutex_);
        uint64_t lo = (anchorMs > preMs) ? (anchorMs - preMs) : 0;
        uint64_t hi = anchorMs + postMs;
        std::vector<MsgFrame> result;
        for (const auto& f : queue_) {
            if (f.receiveTimeMs >= lo && f.receiveTimeMs <= hi) {
                result.push_back(f);
            }
        }
        return result;
    }

    size_t UsedBytes() const {
        std::lock_guard<std::mutex> lk(mutex_);
        return usedBytes_;
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lk(mutex_);
        return queue_.size();
    }

    // steady_clock：用于收包时刻和 Trigger 锚点（两者必须同源）
    static uint64_t SteadyNowMs() {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
    }

    // system_clock：用于 bag 写入时间戳的兜底（消息 header 为 0 时使用）
    static uint64_t SystemNowMs() {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

private:
    mutable std::mutex   mutex_;
    std::deque<MsgFrame> queue_;
    size_t               maxBytes_;
    size_t               usedBytes_;
};

} // namespace rtfbag_util
