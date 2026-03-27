/*
 * BagRecorder — 触发式录包管理器
 *
 * 用法：
 *   BagRecorder recorder;
 *   recorder.Init("bag_topic_index.json", 512*1024*1024);
 *
 *   // 在 Callback 中（Push 时完成序列化，缓存字节流）：
 *   recorder.Push("key", msg);
 *
 *   // 触发录包（前5秒 + 后3秒）：
 *   recorder.Trigger("output.bag", 5000, 3000);
 *
 *   // 查询状态：
 *   auto status = recorder.GetStatus();
 */
#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>

#include "msg_buffer.h"
#include "rtf_bag_writer.h"

// 消息类型头文件
#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_videortpparameter.h"
#include "ara/adsfi/impl_type_msghaf3ddetectionoutarray.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_vehiclesensorcontrol.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_videoencodingperf.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ara/adsfi/impl_type_msgcameravpresult.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"

namespace rtfbag_util {

// ---------------------------------------------------------------------------
// 消息 header 时间戳提取
//
// 工程中存在两种 header 类型，时间戳字段名不同：
//   MsgHafHeader : header.timestamp  (sec + nsec)
//   CommonHeader : header.time       (sec + nsec)
// 均为 MsgHafTime { uint32_t sec; uint32_t nsec; }，语义是 Unix epoch 时间。
//
// 提取优先级：
//   1. header.timestamp.sec != 0  → MsgHafHeader
//   2. header.time.sec != 0       → CommonHeader
//   3. 均为 0 或字段不存在        → system_clock::now()（兜底）
// ---------------------------------------------------------------------------
namespace detail {

// SFINAE：检测 T::header.timestamp.sec 是否存在（MsgHafHeader）
template<typename T, typename = void>
struct HasHafTimestamp : std::false_type {};
template<typename T>
struct HasHafTimestamp<T,
    decltype(void(std::declval<const T&>().header.timestamp.sec))>
    : std::true_type {};

// SFINAE：检测 T::header.time.sec 是否存在（CommonHeader）
template<typename T, typename = void>
struct HasCommonTime : std::false_type {};
template<typename T>
struct HasCommonTime<T,
    decltype(void(std::declval<const T&>().header.time.sec))>
    : std::true_type {};

inline uint64_t SystemNowMs() {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

inline uint64_t MsgHafTimeToMs(uint32_t sec, uint32_t nsec) {
    return static_cast<uint64_t>(sec) * 1000ULL + nsec / 1000000ULL;
}

// 分支1：有 header.timestamp（MsgHafHeader）
template<typename T>
typename std::enable_if<HasHafTimestamp<T>::value, uint64_t>::type
ExtractImpl(const T& msg) {
    return SystemNowMs();
    const auto& ts = msg.header.timestamp;
    if (ts.sec == 0U && ts.nsec == 0U) return SystemNowMs();
    return MsgHafTimeToMs(ts.sec, ts.nsec);
}

// 分支2：有 header.time（CommonHeader），且无 header.timestamp
template<typename T>
typename std::enable_if<!HasHafTimestamp<T>::value && HasCommonTime<T>::value, uint64_t>::type
ExtractImpl(const T& msg) {
    return SystemNowMs();
    const auto& ts = msg.header.time;
    if (ts.sec == 0U && ts.nsec == 0U) return SystemNowMs();
    return MsgHafTimeToMs(ts.sec, ts.nsec);
}

// 分支3：无 header 时间戳字段，使用系统时钟
template<typename T>
typename std::enable_if<!HasHafTimestamp<T>::value && !HasCommonTime<T>::value, uint64_t>::type
ExtractImpl(const T&) {
    return SystemNowMs();
}

} // namespace detail

// 对外接口：从消息中提取时间戳（毫秒，Unix epoch）
template<typename T>
uint64_t ExtractTimestampMs(const T& msg) {
    return detail::ExtractImpl(msg);
}

enum class RecordStatus {
    IDLE,        // 空闲
    COLLECTING,  // 已触发，等待后M秒数据
    WRITING,     // 正在写包
    DONE,        // 写包完成
    ERROR        // 写包失败
};

inline std::string RecordStatusStr(RecordStatus s) {
    switch (s) {
        case RecordStatus::IDLE:       return "IDLE";
        case RecordStatus::COLLECTING: return "COLLECTING";
        case RecordStatus::WRITING:    return "WRITING";
        case RecordStatus::DONE:       return "DONE";
        case RecordStatus::ERROR:      return "ERROR";
        default:                       return "UNKNOWN";
    }
}

class BagRecorder {
public:
    BagRecorder() : status_(RecordStatus::IDLE), lastWritten_(0) {}
    ~BagRecorder() { WaitFinish(); }

    BagRecorder(const BagRecorder&) = delete;
    BagRecorder& operator=(const BagRecorder&) = delete;

    /**
     * @brief 初始化
     * @param indexFile      bag_topic_index.json 路径
     * @param maxMemoryBytes 缓冲区最大内存（字节）
     */
    bool Init(const std::string& indexFile,
              size_t maxMemoryBytes = 512UL * 1024 * 1024) {
        indexFile_ = indexFile;
        buffer_    = std::make_unique<MsgBuffer>(maxMemoryBytes);
        shmConfig_ = std::make_shared<rtf::common::ShmSerializeConfig>();
        return true;
    }

    /**
     * @brief 在 Callback 中调用：序列化后推入缓冲区
     * @param key  bag_topic_index.json 中的 key
     * @param msg  消息对象
     */
    template <typename T>
    void Push(const std::string& key, const T& msg) {
        if (!buffer_ || !shmConfig_) return;

        // receiveTimeMs：steady_clock，与 Trigger 锚点同源，用于 Snapshot 窗口切片
        // msgTimestampMs：Unix epoch，从消息 header 提取，用于 bag 写入
        uint64_t receiveTimeMs  = MsgBuffer::SteadyNowMs();
        uint64_t msgTimestampMs = ExtractTimestampMs(msg);

        rtf::common::Serializer serializer(shmConfig_);
        auto sizeResult = serializer.GetSize(msg);
        if (sizeResult.GetRetCode() != rtf::common::serialize::ResultCode::OK) {
            return;
        }

        std::vector<uint8_t> buf(sizeResult.GetSize());
        auto serResult = serializer.Serialize(msg, buf.data(), buf.size());
        if (serResult.GetRetCode() != rtf::common::serialize::ResultCode::OK) {
            return;
        }

        buffer_->Push(key, std::move(buf), receiveTimeMs, msgTimestampMs);
    }

    /**
     * @brief 触发录包（异步）
     * @param bagPath 输出 .bag 文件路径
     * @param preMs   触发时刻往前取多少毫秒
     * @param postMs  触发时刻往后等多少毫秒再写包
     * @return false 表示当前已有录包任务进行中
     */
    bool Trigger(const std::string& bagPath, uint64_t preMs, uint64_t postMs) {
        auto s = status_.load();
        if (s == RecordStatus::COLLECTING || s == RecordStatus::WRITING) {
            return false;
        }

        WaitFinish();

        uint64_t anchorMs = MsgBuffer::SteadyNowMs();  // 与 receiveTimeMs 同源
        status_ = RecordStatus::COLLECTING;
        lastError_.clear();

        workerFuture_ = std::async(std::launch::async,
            [this, bagPath, anchorMs, preMs, postMs]() {
                if (postMs > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(postMs));
                }

                status_ = RecordStatus::WRITING;

                auto frames = buffer_->Snapshot(anchorMs, preMs, postMs);
                if (frames.empty()) {
                    lastError_ = "No frames in time window";
                    status_    = RecordStatus::ERROR;
                    return;
                }

                RtfBagWriter writer;
                if (!writer.Init(bagPath, indexFile_)) {
                    lastError_ = "RtfBagWriter::Init failed";
                    status_    = RecordStatus::ERROR;
                    return;
                }

                size_t written = 0;
                for (auto& frame : frames) {
                    // payload 已序列化，直接写入，无需重复序列化
                    if (writer.WriteRaw(frame.key, std::move(frame.payload),
                                        frame.msgTimestampMs)) {
                        ++written;
                    }
                }

                writer.Close();
                lastWritten_ = written;
                status_      = RecordStatus::DONE;
            });

        return true;
    }

    RecordStatus GetStatus() const         { return status_.load(); }
    std::string  GetStatusStr() const      { return RecordStatusStr(status_.load()); }
    size_t       GetLastWrittenFrames() const { return lastWritten_.load(); }
    std::string  GetLastError() const      { return lastError_; }
    size_t       GetBufferUsedBytes() const { return buffer_ ? buffer_->UsedBytes() : 0; }
    size_t       GetBufferSize() const     { return buffer_ ? buffer_->Size() : 0; }

    void WaitFinish() {
        if (workerFuture_.valid()) {
            workerFuture_.wait();
        }
    }

private:
    std::string                              indexFile_;
    std::unique_ptr<MsgBuffer>               buffer_;
    std::shared_ptr<rtf::common::ShmSerializeConfig> shmConfig_;
    std::atomic<RecordStatus>                status_;
    std::atomic<size_t>                      lastWritten_;
    std::string                              lastError_;
    std::future<void>                        workerFuture_;
};

} // namespace rtfbag_util
