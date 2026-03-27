#pragma once

#include <map>
#include <mutex>
#include <string>

#include "PerfIndicators.hpp"
#include "monitor/faulthandle_api.hpp"

class EC204 {
public:
    EC204() = default;

    static EC204& Instance() {
        static EC204 instance;
        return instance;
    }

    std::uint32_t ec_add(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            auto r = _ec_map.insert({ec, 0});
            if (!r.second) {
                return 0;
            }
            ptr = r.first;
        }
        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec);
        }
        return ptr->second;
    }

    std::uint32_t ec_add(const std::string& ec, const std::string& ec_detail) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            _ec_map[ec] = 0;
        }
        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            return 0;
        }
        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, ec_detail);
        }
        return ptr->second;
    }

    void ec_remove(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        if (_ec_map.find(ec) != _ec_map.end()) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
            _ec_map.erase(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lg(_mtx);
        for (const auto& ec : _ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        _ec_map.clear();
    }

    // ec:             specific error code for this message type
    // data_time:      current timestamp in seconds (wall clock or data timestamp)
    // threshold_freq: minimum expected frequency in Hz; <= 0 = check disabled
    void check_freq(const std::string& ec, double data_time, double threshold_freq) {
        if (threshold_freq <= 0.0) {
            return;
        }
        double avg = 0.0;
        {
            std::lock_guard<std::mutex> lg(_mtx);
            _freq_indicators[ec].add(data_time * 1000, 1, 1);
            avg = _freq_indicators[ec].avg_per_second().first;
        }
        if (avg < threshold_freq) {
            ec_add(ec, std::to_string(avg));
        } else {
            ec_remove(ec);
        }
    }

private:
    std::mutex                                                            _mtx;
    std::map<std::string, std::uint32_t>                                 _ec_map;
    std::map<std::string, app_common::PerSecondIndicator<10, 100>>       _freq_indicators;

public:
    // ══════════════════════════════════════════════════════════════════
    // 配置 / 初始化  2040001 – 2040004
    // ══════════════════════════════════════════════════════════════════

    // 配置参数读取失败
    static const constexpr char* _ERRORCODE_PARAMETER_ERROR = "2040001";
    static const constexpr char* _ERRORCODE_CONFIG          = _ERRORCODE_PARAMETER_ERROR;

    // 编码参数错误（编码器构造阶段参数非法）
    static const constexpr char* _ERRORCODE_ENCODING_PARAM = "2040002";

    // 编码器创建失败（CMdcImgDvppInterface Init 失败）
    static const constexpr char* _ERRORCODE_CREATE_ENCODER = "2040003";

    // freetype 加载字符文件失败（loadFontData 抛出异常）
    static const constexpr char* _ERRORCODE_FREETYPE_LOAD_FONT = "2040004";

    // ══════════════════════════════════════════════════════════════════
    // 数据无输入  2040005 – 2040012
    // ══════════════════════════════════════════════════════════════════

    // 按消息类型
    static const constexpr char* _ERRORCODE_NO_DATA_VEHICLE_INFORMATION = "2040005";
    static const constexpr char* _ERRORCODE_NO_DATA_FAULT_DATA          = "2040006";
    static const constexpr char* _ERRORCODE_NO_DATA_LOCATION            = "2040007";
    static const constexpr char* _ERRORCODE_NO_DATA_IMAGE_DATA_LIST     = "2040008"; // MsgImageDataList 整包无数据

    // 按摄像头（MsgImageDataList 中具体某路无图像数据）
    static const constexpr char* _ERRORCODE_NO_DATA_LEFT_FRONT  = "2040009";
    static const constexpr char* _ERRORCODE_NO_DATA_RIGHT_FRONT = "2040010";
    static const constexpr char* _ERRORCODE_NO_DATA_LEFT_BACK   = "2040011";
    static const constexpr char* _ERRORCODE_NO_DATA_RIGHT_BACK  = "2040012";

    // ══════════════════════════════════════════════════════════════════
    // 图像格式错误  2040013 – 2040016
    // （MsgImageDataList 中某路图像 rawData 大小与 width×height×3/2 不符）
    // ══════════════════════════════════════════════════════════════════

    static const constexpr char* _ERRORCODE_IMAGE_FORMAT_LEFT_FRONT  = "2040013";
    static const constexpr char* _ERRORCODE_IMAGE_FORMAT_RIGHT_FRONT = "2040014";
    static const constexpr char* _ERRORCODE_IMAGE_FORMAT_LEFT_BACK   = "2040015";
    static const constexpr char* _ERRORCODE_IMAGE_FORMAT_RIGHT_BACK  = "2040016";

    // ══════════════════════════════════════════════════════════════════
    // 数据频率异常  2040017 – 2040020
    // （实际消息频率低于配置阈值）
    // ══════════════════════════════════════════════════════════════════

    static const constexpr char* _ERRORCODE_FREQUENCY_ERROR_VEHICLE_INFORMATION = "2040017";
    static const constexpr char* _ERRORCODE_FREQUENCY_ERROR_FAULT_DATA          = "2040018";
    static const constexpr char* _ERRORCODE_FREQUENCY_ERROR_LOCATION            = "2040019";
    static const constexpr char* _ERRORCODE_FREQUENCY_ERROR_IMAGE_DATA_LIST     = "2040020";

    // ══════════════════════════════════════════════════════════════════
    // 编码  2040021 – 2040022
    // ══════════════════════════════════════════════════════════════════

    // H.265 编码失败（对 compose 后的图像调用 DVPP 编码返回空帧）
    static const constexpr char* _ERRORCODE_ENCODING_ERROR = "2040021";

    // H.265 编码耗时过长（avg 编码时间超过采样间隔）
    static const constexpr char* _ERRORCODE_ENCODE_COST = "2040022";

    // ══════════════════════════════════════════════════════════════════
    // 其他  2040023 – 2040024
    // ══════════════════════════════════════════════════════════════════

    // 共享内存申请失败
    static const constexpr char* _ERRORCODE_SHM_ALLOC_FAILED = "2040023";

    // 合并数据结果为空（compose 产物异常）
    static const constexpr char* _ERRORCODE_DATARESULT_ERROR = "2040024";

    // ══════════════════════════════════════════════════════════════════
    // 文件 I/O  2040025 – 2040029
    // ══════════════════════════════════════════════════════════════════

    // 数据存储目录无访问权限（不存在、非目录或 rwx 权限不足）
    static const constexpr char* _ERRORCODE_DIRECTORY_ACCESS = "2040025";

    // 数据总量超过配额上限
    static const constexpr char* _ERRORCODE_OVER_QUOTA = "2040026";

    // 文件写入失败
    static const constexpr char* _ERRORCODE_WRITE = "2040027";

    // 文件删除失败
    static const constexpr char* _ERRORCODE_DELETE = "2040028";

    // 文件打开失败
    static const constexpr char* _ERRORCODE_OPEN = "2040029";

    // ══════════════════════════════════════════════════════════════════
    // 码流解析  2040030
    // ══════════════════════════════════════════════════════════════════

    // H.265 码流 NALU 解析失败（H265BitstreamParser::ParseBitstream 返回空）
    static const constexpr char* _ERRORCODE_NALU_PARSE_ERROR = "2040030";
};
