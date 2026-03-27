/*
 * RtfBagWriter — 基于 bag_topic_index.json 的 DDS 数据包写入封装
 *
 * 用法：
 *   RtfBagWriter writer;
 *   writer.Init("output.bag", "bag_topic_index.json");
 *   writer.Write("BusinessCommandServiceInterface/EventBusinessCommand/20219", msg);
 *   writer.Close();
 */
#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// nlohmann/json（header-only，需在 include 路径中）
#include <nlohmann/json.hpp>

#include "rtf/rtfbag/RtfBagWriter.h"
#include "rtf/common/serialize.h"
#include "rtf/common/serialize/shm_serialize_config.h"

namespace rtfbag_util {

// 索引文件中每条记录对应的元信息
struct TopicMeta {
    std::string serviceName;
    std::string eventName;
    uint16_t    instanceId;
    std::string shortName;
    int16_t     domainId;
    std::string vlan;
    std::string etcPath;
};

class RtfBagWriter {
public:
    RtfBagWriter() = default;
    ~RtfBagWriter() { Close(); }

    // 禁止拷贝
    RtfBagWriter(const RtfBagWriter&) = delete;
    RtfBagWriter& operator=(const RtfBagWriter&) = delete;

    /**
     * @brief 初始化：指定输出包名和索引文件路径
     * @param bagPath       输出 .bag 文件路径
     * @param indexFile     bag_topic_index.json 路径
     * @return true 成功
     */
    bool Init(const std::string& bagPath, const std::string& indexFile) {
        if (!LoadIndex(indexFile)) {
            return false;
        }
        writer_ = std::make_unique<rtf::rtfbag::RtfBagWriter>(bagPath);
        auto ret = writer_->Init();
        if (ret != rtf::rtfbag::ErrorCode::SUCCESS) {
            std::cerr << "[RtfBagWriter] RtfBagWriter::Init failed, code="
                      << static_cast<int>(ret) << "\n";
            writer_.reset();
            return false;
        }
        shmConfig_ = std::make_shared<rtf::common::ShmSerializeConfig>();
        return true;
    }

    /**
     * @brief 写入一帧消息
     * @tparam T        消息类型（DDS 数据类型）
     * @param key       索引 key，格式：ServiceShortName/EventName/InstanceId
     * @param msg       消息对象
     * @param timestampMs  时间戳（毫秒），默认取当前系统时间
     * @return true 成功
     */
    template <typename T>
    bool Write(const std::string& key, const T& msg, uint64_t timestampMs = 0) {
        if (!writer_) {
            std::cerr << "[RtfBagWriter] Not initialized\n";
            return false;
        }

        auto it = index_.find(key);
        if (it == index_.end()) {
            std::cerr << "[RtfBagWriter] Unknown key: " << key << "\n";
            return false;
        }
        const TopicMeta& meta = it->second;

        // 获取或创建 EventMsgInfo
        auto eventMsg = GetOrCreateEventMsg(key, meta);
        if (!eventMsg) {
            return false;
        }

        // 序列化
        rtf::common::Serializer serializer(shmConfig_);
        auto sizeResult = serializer.GetSize(msg);
        if (sizeResult.GetRetCode() != rtf::common::serialize::ResultCode::OK) {
            std::cerr << "[RtfBagWriter] GetSize failed, code="
                      << static_cast<int>(sizeResult.GetRetCode()) << "\n";
            return false;
        }

        std::vector<uint8_t> buf(sizeResult.GetSize());
        auto serResult = serializer.Serialize(msg, buf.data(), buf.size());
        if (serResult.GetRetCode() != rtf::common::serialize::ResultCode::OK) {
            std::cerr << "[RtfBagWriter] Serialize failed, code="
                      << static_cast<int>(serResult.GetRetCode()) << "\n";
            return false;
        }

        eventMsg->SetBuffer(std::move(buf));

        if (timestampMs == 0) {
            timestampMs = NowMs();
        }

        auto writeRet = writer_->Write(eventMsg, timestampMs);
        if (writeRet != rtf::rtfbag::ErrorCode::SUCCESS) {
            std::cerr << "[RtfBagWriter] Write failed, code="
                      << static_cast<int>(writeRet) << "\n";
            return false;
        }
        return true;
    }

    /**
     * @brief 写入一帧已序列化的数据（来自缓冲区，无需重复序列化）
     * @param key         索引 key
     * @param payload     序列化后的字节流
     * @param timestampMs 时间戳（毫秒）
     */
    bool WriteRaw(const std::string& key,
                  std::vector<uint8_t> payload,
                  uint64_t timestampMs) {
        if (!writer_) {
            std::cerr << "[RtfBagWriter] Not initialized\n";
            return false;
        }
        auto it = index_.find(key);
        if (it == index_.end()) {
            std::cerr << "[RtfBagWriter] Unknown key: " << key << "\n";
            return false;
        }
        auto eventMsg = GetOrCreateEventMsg(key, it->second);
        if (!eventMsg) return false;

        eventMsg->SetBuffer(std::move(payload));
        auto ret = writer_->Write(eventMsg, timestampMs);
        if (ret != rtf::rtfbag::ErrorCode::SUCCESS) {
            std::cerr << "[RtfBagWriter] Write failed, code="
                      << static_cast<int>(ret) << "\n";
            return false;
        }
        return true;
    }

    /**
     * @brief 关闭并保存数据包
     */
    void Close() {
        if (!writer_) return;
        auto ret = writer_->Stop();
        if (ret != rtf::rtfbag::ErrorCode::SUCCESS) {
            std::cerr << "[RtfBagWriter] Stop failed, code="
                      << static_cast<int>(ret) << "\n";
        }
        writer_.reset();
        eventMsgCache_.clear();
    }

private:
    bool LoadIndex(const std::string& indexFile) {
        std::ifstream f(indexFile);
        if (!f.is_open()) {
            std::cerr << "[RtfBagWriter] Cannot open index: " << indexFile << "\n";
            return false;
        }
        nlohmann::json arr;
        try {
            f >> arr;
        } catch (const std::exception& e) {
            std::cerr << "[RtfBagWriter] Parse index failed: " << e.what() << "\n";
            return false;
        }

        for (const auto& item : arr) {
            TopicMeta meta;
            meta.serviceName = item.at("serviceName").get<std::string>();
            meta.eventName   = item.at("eventName").get<std::string>();
            meta.instanceId  = static_cast<uint16_t>(item.at("instanceId").get<int>());
            meta.shortName   = item.at("shortName").get<std::string>();
            meta.domainId    = static_cast<int16_t>(item.at("domainId").get<int>());
            meta.vlan        = item.at("vlan").get<std::string>();
            meta.etcPath     = item.at("etcPath").get<std::string>();
            index_[item.at("key").get<std::string>()] = std::move(meta);
        }
        std::cout << "[RtfBagWriter] Loaded " << index_.size()
                  << " topics from " << indexFile << "\n";
        return true;
    }

    std::shared_ptr<rtf::rtfbag::EventMsgInfo>
    GetOrCreateEventMsg(const std::string& key, const TopicMeta& meta) {
        auto it = eventMsgCache_.find(key);
        if (it != eventMsgCache_.end()) {
            return it->second;
        }

        rtf::rtfbag::DDSEventIndexInfo ddsIndex(
            meta.serviceName,
            meta.instanceId,
            meta.eventName,
            meta.vlan,
            meta.domainId);

        auto eventMsg = rtf::rtfbag::EventMsgFactory::CreateEventMsgFactory(
            meta.etcPath, ddsIndex, meta.shortName);
        if (!eventMsg) {
            std::cerr << "[RtfBagWriter] CreateEventMsgFactory failed for key: "
                      << key << "\n";
            return nullptr;
        }

        auto initRet = eventMsg->Init();
        if (initRet != rtf::rtfbag::ErrorCode::SUCCESS) {
            std::cerr << "[RtfBagWriter] EventMsgInfo::Init failed, code="
                      << static_cast<int>(initRet) << " key=" << key << "\n";
            return nullptr;
        }

        eventMsgCache_[key] = eventMsg;
        return eventMsg;
    }

    static uint64_t NowMs() {
        // 使用 system_clock 获取 Unix epoch 时间（毫秒），与 rtfbag 时间基准一致
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

    std::unique_ptr<rtf::rtfbag::RtfBagWriter>                              writer_;
    std::shared_ptr<rtf::common::ShmSerializeConfig>                         shmConfig_;
    std::unordered_map<std::string, TopicMeta>                               index_;
    std::unordered_map<std::string, std::shared_ptr<rtf::rtfbag::EventMsgInfo>> eventMsgCache_;
};

} // namespace rtfbag_util
