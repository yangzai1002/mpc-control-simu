#ifndef _H265_ROTATING_FILE_RECEIVER_HPP
#define _H265_ROTATING_FILE_RECEIVER_HPP

#include <fcntl.h>

#include <Datetime14Util.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "H265Receiver.hpp"
#include "StringUtils.hpp"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
#include "fmt/format.h"
#include "h265_bitstream_parser.h"
#include "h265_bitstream_parser_state.h"
#include "ap_log/ap_log_interface.h"

class H265RotatingFileReceiver : public H265Receiver {
public:
    struct Record {
        Record(std::string&& n, std::size_t size) : _file(std::move(n)), _size(size) {
        }
        std::string _file;
        std::size_t _size;

        bool operator<(const Record& o) const {
            // 比较当前对象的_file成员变量与传入对象的_file成员变量
            return _file < o._file;
        }
    };

    static const constexpr char*     _ERRORCODE_CONFIG    = "4032001";     // 读取配置文件失败
    static const constexpr char*     _ERRORCODE_NO_DATA   = "4032002";    // 无数据输入
    static const constexpr char*     _ERRORCODE_PERMISSOIN = "4032003"; // 数据存储目录没有权限
    static const constexpr char*     _ERRORCODE_OVER_QUOTA = "4032004"; // 数据超过限额
    static const constexpr char*     _ERRORCODE_WRITE     = "4032005";      // 写文件失败
    static const constexpr char*     _ERRORCODE_DELETE    = "4032006";     // 删除文件失败
    static const constexpr char*     _ERRORCODE_OPEN      = "4032007";       // 打开文件失败
    // 序号上界：8位十进制最大值，超过后回绕到 0
    static const constexpr uint64_t  MAX_SEQUENCE         = 99999999ULL;

    class Ec {
    public:
        Ec() = default;

        std::uint32_t ec_add(const std::string& ec) {
            std::lock_guard<std::mutex> lock(_mtx);
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

        void ec_remove(const std::string& ec) {
            std::lock_guard<std::mutex> lock(_mtx);
            if (_ec_map.find(ec) != _ec_map.end()) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
                _ec_map.erase(ec);
            }
        }

        void ec_clear() {
            std::lock_guard<std::mutex> lock(_mtx);
            for (const auto& ec : _ec_map) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
            }
            _ec_map.clear();
        }

    private:
        std::mutex                           _mtx;
        std::map<std::string, std::uint32_t> _ec_map;
    };

public:
    // clang-format off
    H265RotatingFileReceiver()
    : H265Receiver()
    , _name("push_record")
    , _file_limit_duration(0)
    , _quota_bytes(0)
    , _clean_threshold_bytes(0)
    , _total_size(0)
    , _current_fd(-1)
    , _current_size(0)
    , _current_start(0)
    , _next_sequence(0) {
        // clang-format on
        auto         ptr = CustomStack::Instance();
        std::string  prefix = "media/";
        std::int32_t quota_mb;
        std::int32_t clean_threshold_mb;

        auto full_config = prefix + "video/recorder/location";
        if (!ptr->GetProjectConfigValue(full_config, _location) || _location.empty()) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        full_config = prefix + "video/recorder/single_file_duration";
        if (!ptr->GetProjectConfigValue(full_config, _file_limit_duration) || 0 == _file_limit_duration) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        full_config = prefix + "video/recorder/quota";
        if (!ptr->GetProjectConfigValue(full_config, quota_mb) || 0 == quota_mb) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        full_config = prefix + "video/recorder/clean_threshold";
        if (!ptr->GetProjectConfigValue(full_config, clean_threshold_mb) || 0 == clean_threshold_mb ||
            clean_threshold_mb > quota_mb) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);

        ApInfo("h265_recorder") << "location: " << _location;
        ApInfo("h265_recorder") << "single_file_duration: " << _file_limit_duration;
        ApInfo("h265_recorder") << "quota: " << quota_mb;
        ApInfo("h265_recorder") << "clean_threshold: " << clean_threshold_mb;

        _quota_bytes = (std::uint64_t)quota_mb * 1024 * 1024;
        _clean_threshold_bytes = (std::uint64_t)clean_threshold_mb * 1024 * 1024;
        ApInfo("h265_recorder") << "quota bytes: " << _quota_bytes;
        ApInfo("h265_recorder") << "clean threshold bytes: " << _clean_threshold_bytes;

        if (std::filesystem::exists(_location)) {
            if (!std::filesystem::is_directory(_location)) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_PERMISSOIN, _location);
                throw std::runtime_error("data location is not a directory");
            }

            if (access(_location.c_str(), W_OK | R_OK | X_OK) != 0) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_PERMISSOIN, _location);
                throw std::runtime_error("data location is not accessiable");
            }
        } else {
            std::error_code ec;
            if (!std::filesystem::create_directories(_location, ec)) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_PERMISSOIN, _location);
                throw std::runtime_error("create data location failed");
            }
        }
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_PERMISSOIN);

        enum_exist_files();
        check_limit_and_open();
    }

    ~H265RotatingFileReceiver() override {
        if (_current_fd >= 0) {
            close(_current_fd);
            _current_fd = -1;
        }
    }

    [[nodiscard]] std::string string() const override {
        return std::filesystem::path(_location).append(_name).string();
    }

    void push(const std::uint8_t* p, std::size_t s) noexcept(false) override {
        h265nal::ParsingOptions parsing_options;
        parsing_options.add_checksum = false;
        auto nalus = h265nal::H265BitstreamParser::ParseBitstream(p, s, parsing_options);
        if (nullptr == nalus) {
            ApError("h265_recorder") << "cannot find any nalu";
            return;
        }

        for (auto& nalu : nalus->nal_units) {
            auto type = nalu->nal_unit_header->nal_unit_type;
            auto is_sps_pps = (h265nal::NalUnitType::SPS_NUT == type || h265nal::NalUnitType::PPS_NUT == type ||
                               h265nal::NalUnitType::VPS_NUT == type);

            ApInfo("h265_recorder") << "nalu, type: " << type << ", offset " << nalu->offset
                                    << ", payload_length: " << nalu->length;
            if (is_sps_pps) {
                check_limit_and_open();
            }

            std::size_t offset = 0;
            while (offset < nalu->length) {
                auto r = write(_current_fd, "\x00\x00\x00\x01", 4);
                if (r < 0) {
                    ApError("h265_recorder") << "write error: " << strerror(errno);
                    break;
                }
                r = write(_current_fd, p + nalu->offset, nalu->length);
                if (r < 0) {
                    ApError("h265_recorder") << "write error: " << strerror(errno);
                    break;
                }
                offset += r;
            }
            if (offset < nalu->length) {
                _ec.ec_add(_ERRORCODE_WRITE);
            } else {
                _ec.ec_remove(_ERRORCODE_WRITE);
            }
        }
    }

protected:
    void check_limit_and_open() {
        bool need_create = false;

        if (_current_fd < 0) {
            need_create = true;
        } else if ((_current_start + _file_limit_duration) <= time(nullptr) || _current_fd < 0) {
            close(_current_fd);
            _records.emplace_back(Record(std::move(_current_name), _current_size));
            _total_size += _current_size;

            _current_name = "";
            _current_size = 0;
            _current_fd = -1;
            _current_start = 0;
            need_create = true;
        }

        if (!need_create) {
            return;
        }

        delete_until_has_free_space();

        std::filesystem::path p(_location);
        p.append(generate_filename(_next_sequence));
        _next_sequence = (_next_sequence + 1) % (MAX_SEQUENCE + 1);
        _current_name = p.string();
        _current_fd = open(_current_name.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR);
        if (_current_fd < 0) {
            ApError("h265_recorder") << fmt::format("open {} error {}", p.string(),
                                                    app_common::StringUtils::str_error(errno));
            _ec.ec_add(_ERRORCODE_OPEN);
            return;
        }
        _ec.ec_remove(_ERRORCODE_OPEN);
        _current_start = time(nullptr);
        _current_size = 0;
    }

    void enum_exist_files() {
        // 第一步：迭代收集所有条目，不在迭代过程中操作文件系统
        struct EntryInfo {
            std::string              path;
            bool                     is_regular;
            std::optional<uint64_t>  seq;
        };
        std::vector<EntryInfo> entries;
        for (const auto& x : std::filesystem::directory_iterator(_location)) {
            EntryInfo info;
            info.path       = x.path().string();
            info.is_regular = x.is_regular_file();
            if (info.is_regular) {
                info.seq = parse_sequence(x.path().filename().string());
            }
            entries.emplace_back(std::move(info));
        }

        // 第二步：迭代完成后分类处理
        std::vector<std::string> to_delete;

        for (auto& e : entries) {
            if (e.is_regular && e.seq.has_value()) {
                try {
                    auto size = std::filesystem::file_size(e.path);
                    _records.emplace_back(Record(std::string(e.path), size));
                    _total_size += size;
                } catch (...) {
                    ApError("h265_recorder") << fmt::format("get size of {} failed", e.path);
                }
            } else {
                to_delete.emplace_back(e.path);
            }
        }

        // 第三步：删除不符合命名规范的文件/目录
        for (const auto& path : to_delete) {
            ApWarn("h265_recorder") << fmt::format("removing non-conforming entry: {}", path);
            try {
                std::filesystem::remove_all(path);
            } catch (const std::exception& ex) {
                ApError("h265_recorder") << fmt::format("failed to remove {}: {}", path, ex.what());
            }
        }

        // 第四步：按文件名字典序排序（序号零填充8位，字典序 = 序号升序）
        _records.sort();

        // 第五步：用循环最大 gap 法恢复下一个序号，并在回绕时旋转 _records
        // 算法说明：
        //   磁盘上的文件序号在循环空间 [0, MAX_SEQUENCE] 内构成一个连续窗口（FIFO 删旧保新）。
        //   找到相邻序号中的最大"间隔"（含尾→头的回绕间隔），该间隔左侧序号即为最新文件的序号，
        //   其 +1（取模）即为 _next_sequence。
        //   若最大间隔在中间（非回绕间隔），说明发生了回绕，需旋转 _records 保证 FIFO 正确性。
        if (_records.empty()) {
            _next_sequence = 0;
        } else {
            // 从已排序的 _records 中提取序号（已升序）
            std::vector<uint64_t> seqs;
            seqs.reserve(_records.size());
            for (const auto& r : _records) {
                auto seq = parse_sequence(std::filesystem::path(r._file).filename().string());
                if (seq.has_value()) {
                    seqs.push_back(*seq);
                }
            }

            // 初始化：回绕间隔大小 = MAX_SEQUENCE - last + first
            uint64_t    max_gap    = MAX_SEQUENCE - seqs.back() + seqs.front();
            std::size_t wrap_split = seqs.size(); // sentinel：最大间隔在回绕处（无中间回绕）

            for (std::size_t i = 0; i + 1 < seqs.size(); ++i) {
                uint64_t gap = seqs[i + 1] - seqs[i] - 1;
                if (gap > max_gap) {
                    max_gap    = gap;
                    wrap_split = i + 1; // 间隔右侧第一个序号是最旧文件的起点
                }
            }

            if (wrap_split == seqs.size()) {
                // 最大间隔在回绕处：序号未回绕（或全部在低端），直接取 last+1
                _next_sequence = (seqs.back() + 1) % (MAX_SEQUENCE + 1);
            } else {
                // 最大间隔在中间：已发生回绕
                // seqs[wrap_split - 1] 是高序号段的最后一个（最新文件）
                _next_sequence = (seqs[wrap_split - 1] + 1) % (MAX_SEQUENCE + 1);

                // 旋转 _records：将低序号（新文件）部分从链表头移至尾，
                // 使高序号（旧文件）排在前面，保证 FIFO 删除顺序正确。
                // 旋转前: [file_00000000..file_00000002, file_99999997..file_99999999]
                // 旋转后: [file_99999997..file_99999999, file_00000000..file_00000002]
                auto it = _records.begin();
                std::advance(it, wrap_split);
                _records.splice(_records.end(), _records, _records.begin(), it);
                ApInfo("h265_recorder") << fmt::format(
                    "sequence wrap-around detected: split={}, rotated records for correct FIFO order",
                    wrap_split);
            }
        }

        ApInfo("h265_recorder") << fmt::format(
            "enum_exist_files: {} valid files, {} removed, next_sequence={}",
            _records.size(), to_delete.size(), _next_sequence);
    }

    // 校验文件名格式并提取序号
    // 合法格式：push_record_NNNNNNNN_YYYYMMDDHHMMSS.sss
    //   前缀(push_record_) = 12字符
    //   序号(NNNNNNNN)     = 8位纯数字
    //   分隔符(_)          = 1字符
    //   时间戳(YYYYMMDDHHMMSS) = 14位纯数字
    //   小数点(.)          = 1字符
    //   毫秒(sss)          = 3位纯数字
    //   总长               = 39字符
    std::optional<uint64_t> parse_sequence(const std::string& filename) const {
        const std::string expected_prefix = _name + "_";
        const std::size_t seq_start       = expected_prefix.size(); // 12
        const std::size_t expected_len    = seq_start + 8 + 1 + 14 + 1 + 3; // 39

        if (filename.size() != expected_len) {
            return std::nullopt;
        }
        if (filename.substr(0, seq_start) != expected_prefix) {
            return std::nullopt;
        }
        // 检查8位序号
        for (std::size_t i = 0; i < 8; ++i) {
            if (!isdigit(static_cast<unsigned char>(filename[seq_start + i]))) {
                return std::nullopt;
            }
        }
        // 检查序号后的分隔符 '_'
        if (filename[seq_start + 8] != '_') {
            return std::nullopt;
        }
        // 检查14位时间戳
        for (std::size_t i = 0; i < 14; ++i) {
            if (!isdigit(static_cast<unsigned char>(filename[seq_start + 9 + i]))) {
                return std::nullopt;
            }
        }
        // 检查小数点
        if (filename[seq_start + 23] != '.') {
            return std::nullopt;
        }
        // 检查3位毫秒
        for (std::size_t i = 0; i < 3; ++i) {
            if (!isdigit(static_cast<unsigned char>(filename[seq_start + 24 + i]))) {
                return std::nullopt;
            }
        }
        return static_cast<uint64_t>(std::stoull(filename.substr(seq_start, 8)));
    }

    // 生成文件名：push_record_NNNNNNNN_YYYYMMDDHHMMSS.sss
    std::string generate_filename(uint64_t seq) const {
        auto now      = std::chrono::system_clock::now();
        auto t        = std::chrono::system_clock::to_time_t(now);
        auto ms       = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch()) % 1000;
        std::tm tm_buf{};
        localtime_r(&t, &tm_buf);
        char datetime_buf[16];
        strftime(datetime_buf, sizeof(datetime_buf), "%Y%m%d%H%M%S", &tm_buf);
        return fmt::format("{}_{:08d}_{}.{:03d}", _name, seq, datetime_buf, ms.count());
    }

    void dump_exist_files() {
        for (const auto& x : _records) {
            ApInfo("h265_recorder") << fmt::format("----- file: {}    size: {}", x._file, x._size);
        }
    }

    void delete_until_has_free_space() {
        ApInfo("h265_recorder") << fmt::format("total size: {}, quota_bytes: {}, clean threshold bytes: {}",
                                               _total_size, _quota_bytes, _clean_threshold_bytes);
        if (_total_size > _quota_bytes) {
            _ec.ec_add(_ERRORCODE_OVER_QUOTA);
        } else {
            _ec.ec_remove(_ERRORCODE_OVER_QUOTA);
        }

        while (_total_size > _clean_threshold_bytes) {
            auto& x = _records.front();
            try {
                std::filesystem::remove(x._file);
                _records.pop_front();
                _total_size -= x._size;
                ApInfo("h265_recorder") << fmt::format("del file {} of size {}, rest size {}", x._file, x._size,
                                                       _total_size);
                _ec.ec_remove(_ERRORCODE_DELETE);
            } catch (const std::exception& e) {
                ApError("h265_recorder") << fmt::format("del file {} of size {} error: {}", x._file, x._size, e.what());
                _ec.ec_add(_ERRORCODE_DELETE);
            }
        }
    }

protected:
    std::string   _location;
    std::string   _name;
    std::uint64_t _file_limit_duration;
    std::uint64_t _quota_bytes;
    std::uint64_t _clean_threshold_bytes;

    std::uint64_t     _total_size;
    std::list<Record> _records;
    std::string       _current_name;
    std::int32_t      _current_fd;
    std::uint64_t     _current_size;
    time_t            _current_start;
    std::uint64_t     _next_sequence;
    Ec                _ec;
};

#endif
