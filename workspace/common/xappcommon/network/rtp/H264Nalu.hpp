#ifndef _H264_H264Nalu_HPP_
#define _H264_H264Nalu_HPP_

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "log/log.h"

class H264Nalu {
public:
    enum class NaluType {
        // Table 7-1 NAL unit type codes
        NAL_UNIT_TYPE_UNSPECIFIED = 0,                  // Unspecified
        NAL_UNIT_TYPE_CODED_SLICE_NON_IDR = 1,          // Coded slice of a non-IDR picture
        NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A = 2, // Coded slice data partition A
        NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B = 3, // Coded slice data partition B
        NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C = 4, // Coded slice data partition C
        NAL_UNIT_TYPE_CODED_SLICE_IDR = 5,              // Coded slice of an IDR picture
        NAL_UNIT_TYPE_SEI = 6,                          // Supplemental enhancement information (SEI)
        NAL_UNIT_TYPE_SPS = 7,                          // Sequence parameter set
        NAL_UNIT_TYPE_PPS = 8,                          // Picture parameter set
        NAL_UNIT_TYPE_AUD = 9,                          // Access unit delimiter
        NAL_UNIT_TYPE_END_OF_SEQUENCE = 10,             // End of sequence
        NAL_UNIT_TYPE_END_OF_STREAM = 11,               // End of stream
        NAL_UNIT_TYPE_FILLER = 12,                      // Filler data
        NAL_UNIT_TYPE_SPS_EXT = 13,                     // Sequence parameter set extension
        NAL_UNIT_TYPE_PREFIX_NAL = 14,                  // Prefix NAL unit
        NAL_UNIT_TYPE_SUBSET_SPS = 15,                  // Subset Sequence parameter set
        NAL_UNIT_TYPE_DPS = 16,                         // Depth Parameter Set
        // 17..18    // Reserved
        NAL_UNIT_TYPE_CODED_SLICE_AUX = 19,           // Coded slice of an auxiliary coded picture without partitioning
        NAL_UNIT_TYPE_CODED_SLICE_SVC_EXTENSION = 20, // Coded slice of SVC extension
        // 20..23    // Reserved
        // 24..31    // Unspecified
    };

    struct NaluHeader {
        uint8_t nal_unit_type : 5;
        uint8_t nal_ref_idc : 2;
        uint8_t forbidden_zero_bit : 1;
    } __attribute__((packed));

public:
    H264Nalu() : _ts(0), _start_code_len(0), _data(nullptr), _data_len(0) {
    }

    H264Nalu(std::int64_t ts, std::uint32_t start_code_len, const uint8_t *data1, size_t data_len1)
        : _ts(ts), _start_code_len(start_code_len), _data((std::uint8_t *)data1), _data_len(data_len1) {
    }

    ~H264Nalu() = default;

    H264Nalu(const H264Nalu &o) {
        _ts = o._ts;
        _start_code_len = o._start_code_len;
        _data = o._data;
        _data_len = o._data_len;
        _data_copy = o._data_copy;
        if (!_data_copy.empty()) {
            _data = _data_copy.data();
            _data_len = _data_copy.size();
        }
    }

    H264Nalu(H264Nalu &&o) noexcept {
        _data_copy = std::move(o._data_copy);
        _ts = o._ts;
        _start_code_len = o._start_code_len;
        _data = o._data;
        _data_len = o._data_len;
        if (!_data_copy.empty()) {
            _data = _data_copy.data();
            _data_len = _data_copy.size();
        }

        o._ts = 0;
        o._start_code_len = 0;
        o._data = nullptr;
        o._data_len = 0;
    }

    H264Nalu &operator=(H264Nalu &&o) {
        _data_copy = std::move(o._data_copy);
        _ts = o._ts;
        _start_code_len = o._start_code_len;
        _data = o._data;
        _data_len = o._data_len;
        if (!_data_copy.empty()) {
            _data = _data_copy.data();
            _data_len = _data_copy.size();
        }

        o._ts = 0;
        o._start_code_len = 0;
        o._data = nullptr;
        o._data_len = 0;
        return *this;
    }

    H264Nalu &operator=(const H264Nalu &o) {
        _ts = o._ts;
        _start_code_len = o._start_code_len;
        _data = o._data;
        _data_len = o._data_len;
        _data_copy = o._data_copy;
        if (!_data_copy.empty()) {
            _data = _data_copy.data();
            _data_len = _data_copy.size();
        }
        return *this;
    }

    H264Nalu &operator=(std::vector<uint8_t> &&data) {
        _data_copy = std::move(data);
        _data = _data_copy.data();
        _data_len = _data_copy.size();
        return *this;
    }

    void copy() {
        if (_data_copy.empty() && _data != nullptr && _data_len > 0) {
            _data_copy.resize(_data_len);
            memcpy(_data_copy.data(), _data, _data_len);
            _data = _data_copy.data();
            _data_len = _data_copy.size();
        }
    }

    NaluType type() const {
        const auto lol = (NaluHeader *)(_data + 4);
        assert(lol->nal_unit_type == (_data[4] & 0x1f));
        return (NaluType)lol->nal_unit_type;
    }

    std::string name() const {
        if (!_name.empty()) {
            return _name;
        }
        switch (type()) {
            case NaluType::NAL_UNIT_TYPE_UNSPECIFIED:
                _name = "NAL_UNIT_TYPE_UNSPECIFIED";
                break;
            case NaluType::NAL_UNIT_TYPE_CODED_SLICE_NON_IDR:
                _name = "NAL_UNIT_TYPE_CODED_SLICE_NON_IDR";
                break;
            case NaluType::NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A:
                _name = "NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A";
                break;
            case NaluType::NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B:
                _name = "NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B";
                break;
            case NaluType::NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C:
                _name = "NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C";
                break;
            case NaluType::NAL_UNIT_TYPE_CODED_SLICE_IDR:
                _name = "NAL_UNIT_TYPE_CODED_SLICE_IDR";
                break;
            case NaluType::NAL_UNIT_TYPE_SEI:
                _name = "NAL_UNIT_TYPE_SEI";
                break;
            case NaluType::NAL_UNIT_TYPE_SPS:
                _name = "NAL_UNIT_TYPE_SPS";
                break;
            case NaluType::NAL_UNIT_TYPE_PPS:
                _name = "NAL_UNIT_TYPE_PPS";
                break;
            case NaluType::NAL_UNIT_TYPE_AUD:
                _name = "NAL_UNIT_TYPE_AUD";
                break;
            case NaluType::NAL_UNIT_TYPE_END_OF_SEQUENCE:
                _name = "NAL_UNIT_TYPE_END_OF_SEQUENCE";
                break;
            case NaluType::NAL_UNIT_TYPE_END_OF_STREAM:
                _name = "NAL_UNIT_TYPE_END_OF_STREAM";
                break;
            case NaluType::NAL_UNIT_TYPE_FILLER:
                _name = "NAL_UNIT_TYPE_FILLER";
                break;
            case NaluType::NAL_UNIT_TYPE_SPS_EXT:
                _name = "SNAL_UNIT_TYPE_SPS_EXT";
                break;
                // 14..18    // Reserved
            case NaluType::NAL_UNIT_TYPE_CODED_SLICE_AUX:
                _name = "NAL_UNIT_TYPE_CODED_SLICE_AUX";
                break;
            // Coded slice of an auxiliary coded picture without partitioning
            // 20..23    // Reserved
            // 24..31    // Unspecified
            default:
                _name = std::string("Unknown") + std::to_string((std::int32_t)type());
                break;
        }
        return _name;
    }

    const uint8_t *data() const {
        return _data;
    }

    const size_t size() const {
        return _data_len;
    }

    const std::uint32_t start_code_len() const {
        return _start_code_len;
    }

    std::int64_t ts() const {
        return _ts;
    }

    bool is_sps() const {
        return (type() == NaluType::NAL_UNIT_TYPE_SPS);
    }

    bool is_pps() const {
        return (type() == NaluType::NAL_UNIT_TYPE_PPS);
    }

    bool is_idr() const {
        return (type() == NaluType::NAL_UNIT_TYPE_CODED_SLICE_IDR);
    }

    bool is_aud() const {
        return (type() == NaluType::NAL_UNIT_TYPE_AUD);
    }

    void write_to(std::int32_t fd) {
        std::size_t offset = 0;
        while (offset < _data_len) {
            auto r = write(fd, _data + offset, _data_len - offset);
            if (r < 0) {
                AERROR << "write error: " << strerror(errno);
                throw std::runtime_error(std::string("write error with ") + strerror(errno));
            }
            offset += r;
        }
    }

    // 判断是否是 H264Nalu 起始码
    static std::vector<std::uint8_t> is_startcode(const uint8_t *data, size_t len) {
        if (len >= 4 && std::memcmp(data, "\x00\x00\x00\x01", 4) == 0) {
            return {00, 00, 00, 01};
        }
        // 3 字节的 prefix 不行，不稳定。
        //    if (len >= 3 && std::memcmp(data, "\x00\x00\x01", 3) == 0) {
        //        return {00, 00, 01};
        //    }

        return {};
    }

    struct StartCode {
        StartCode() : pos(-1) {
        }

        StartCode(std::int32_t p, const std::vector<std::uint8_t> &c) : pos(p), code(c){};

        std::int32_t              pos;
        std::vector<std::uint8_t> code;
    };

    static std::vector<StartCode> find_startcodes(const uint8_t *data, size_t len) {
        std::vector<StartCode> positions;
        std::int32_t           i = 0;
        while (i < len) {
            auto code = is_startcode(data + i, len - i);
            if (!code.empty()) {
                positions.push_back(StartCode{i, code}); // 记录起始码的位置
                i += code.size();
            } else {
                ++i;
            }
        }
        positions.push_back({-1, {}});

        return positions;
    }

    static std::vector<H264Nalu> nalus(std::int64_t ts, const std::uint8_t *d, size_t s) {
        auto startcodes = find_startcodes(d, s);
        if (startcodes.empty() || startcodes.front().pos < 0 || startcodes.front().pos != 0) {
            AERROR << "nalu not found";
            return {};
        }
        for (const auto code : startcodes) {
            AINFO << "nalu, pos: " << code.pos << ", codelen: " << code.code.size();
        }

        std::vector<H264Nalu> nalus;

        for (std::int32_t i = 0; i < startcodes.size() - 1; ++i) {
            const auto & _this = startcodes[i];
            const auto & _next = startcodes[i + 1];
            std::int32_t nalu_len = 0;
            if (_next.pos < 0) { // 说明是一直到末尾的
                nalu_len = s - _this.pos;
            } else {
                nalu_len = _next.pos - _this.pos;
            }
            nalus.push_back(H264Nalu(ts, _this.code.size(), d + _this.pos, nalu_len));
        }

        return nalus;
    }

private:
    std::int64_t              _ts;
    std::uint32_t             _start_code_len;
    std::uint8_t *            _data;
    std::size_t               _data_len;
    std::vector<std::uint8_t> _data_copy;
    mutable std::string       _name;
};
#endif