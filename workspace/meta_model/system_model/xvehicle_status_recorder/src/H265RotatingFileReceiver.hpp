#ifndef _H265_ROTATING_FILE_RECEIVER_HPP
#define _H265_ROTATING_FILE_RECEIVER_HPP

#include <fcntl.h>
#include <sys/time.h>

#include <Datetime14Util.hpp>
#include <filesystem>
#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "EC204.hpp"
#include "H265Receiver.hpp"
#include "StringUtils.hpp"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "h265_bitstream_parser.h"
#include "h265_bitstream_parser_state.h"

class H265RotatingFileReceiver : public H265Receiver {
public:
    struct Record {
        Record(std::string&& n, std::size_t size) : _file(std::move(n)), _size(size) {
        }
        std::string _file;
        std::size_t _size;

        bool operator<(const Record& o) const {
            return _file < o._file;
        }
    };

public:
    // clang-format off
    H265RotatingFileReceiver()
    : H265Receiver()
    , _name("dashcam")
    , _file_limit_duration(0)
    , _quota_bytes(0)
    , _clean_threshold_bytes(0)
    , _total_size(0)
    , _current_fd(-1)
    , _current_size(0)
    , _current_start(0)
    , _next_seq(0) {
        // clang-format on
        auto         ptr = CustomStack::Instance();
        std::string  prefix;
        std::int32_t quota_mb;
        std::int32_t clean_threshold_mb;

        auto full_config = prefix + "system/recorder/video/location";
        if (!ptr->GetProjectConfigValue(full_config, _location) || _location.empty()) {
            EC204::Instance().ec_add(EC204::_ERRORCODE_PARAMETER_ERROR, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        full_config = prefix + "system/recorder/video/single_file_duration";
        if (!ptr->GetProjectConfigValue(full_config, _file_limit_duration) || 0 == _file_limit_duration) {
            EC204::Instance().ec_add(EC204::_ERRORCODE_PARAMETER_ERROR, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        full_config = prefix + "system/recorder/video/quota";
        if (!ptr->GetProjectConfigValue(full_config, quota_mb) || 0 == quota_mb) {
            EC204::Instance().ec_add(EC204::_ERRORCODE_PARAMETER_ERROR, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        full_config = prefix + "system/recorder/video/clean_threshold";
        if (!ptr->GetProjectConfigValue(full_config, clean_threshold_mb) || 0 == clean_threshold_mb ||
            clean_threshold_mb > quota_mb) {
            EC204::Instance().ec_add(EC204::_ERRORCODE_PARAMETER_ERROR, full_config);
            throw std::runtime_error("read project config " + full_config + " failed");
        }

        EC204::Instance().ec_remove(EC204::_ERRORCODE_PARAMETER_ERROR);

        ApInfo("status_recorder") << "location: " << _location;
        ApInfo("status_recorder") << "single_file_duration: " << _file_limit_duration;
        ApInfo("status_recorder") << "quota: " << quota_mb;
        ApInfo("status_recorder") << "clean_threshold: " << clean_threshold_mb;

        _quota_bytes = (std::uint64_t)quota_mb * 1024 * 1024;
        _clean_threshold_bytes = (std::uint64_t)clean_threshold_mb * 1024 * 1024;
        ApInfo("status_recorder") << "quota bytes: " << _quota_bytes;
        ApInfo("status_recorder") << "clean threshold bytes: " << _clean_threshold_bytes;

        if (std::filesystem::exists(_location)) {
            if (!std::filesystem::is_directory(_location)) {
                EC204::Instance().ec_add(EC204::_ERRORCODE_DIRECTORY_ACCESS, _location);
                throw std::runtime_error("data location is not a directory");
            }

            if (access(_location.c_str(), W_OK | R_OK | X_OK) != 0) {
                EC204::Instance().ec_add(EC204::_ERRORCODE_DIRECTORY_ACCESS, _location);
                throw std::runtime_error("data location is not accessiable");
            }
        } else {
            std::error_code ec;
            if (!std::filesystem::create_directories(_location, ec)) {
                EC204::Instance().ec_add(EC204::_ERRORCODE_DIRECTORY_ACCESS, _location);
                throw std::runtime_error("create data location failed");
            }
        }
        EC204::Instance().ec_remove(EC204::_ERRORCODE_DIRECTORY_ACCESS);

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
            ApError("status_recorder") << "cannot find any nalu";
            EC204::Instance().ec_add(EC204::_ERRORCODE_NALU_PARSE_ERROR);
            return;
        }
        EC204::Instance().ec_remove(EC204::_ERRORCODE_NALU_PARSE_ERROR);

        for (auto& nalu : nalus->nal_units) {
            auto type = nalu->nal_unit_header->nal_unit_type;
            auto is_sps_pps = (h265nal::NalUnitType::SPS_NUT == type || h265nal::NalUnitType::PPS_NUT == type ||
                               h265nal::NalUnitType::VPS_NUT == type);

            ApInfo("status_recorder") << "nalu, type: " << type << ", offset " << nalu->offset
                                      << ", payload_length: " << nalu->length;
            if (is_sps_pps) {
                check_limit_and_open();
            }

            std::size_t offset = 0;
            while (offset < nalu->length) {
                auto r = write(_current_fd, "\x00\x00\x00\x01", 4);
                if (r < 0) {
                    ApError("status_recorder") << "write error: " << strerror(errno);
                    break;
                }
                r = write(_current_fd, p + nalu->offset, nalu->length);
                if (r < 0) {
                    ApError("status_recorder") << "write error: " << strerror(errno);
                    break;
                }
                offset += r;
            }
            if (offset < nalu->length) {
                EC204::Instance().ec_add(EC204::_ERRORCODE_WRITE);
            } else {
                EC204::Instance().ec_remove(EC204::_ERRORCODE_WRITE);
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
        if (_next_seq > 99999999u) _next_seq = 0u;
        p.append(make_filename(_name, _next_seq));
        _next_seq = (_next_seq >= 99999999u) ? 0u : _next_seq + 1u;
        _current_name = p.string();
        _current_fd = open(_current_name.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR);
        if (_current_fd < 0) {
            ApError("status_recorder") << fmt::format("open {} error {}", p.string(),
                                                      app_common::StringUtils::str_error(errno));
            EC204::Instance().ec_add(EC204::_ERRORCODE_OPEN);
            return;
        }
        EC204::Instance().ec_remove(EC204::_ERRORCODE_OPEN);
        _current_start = time(nullptr);
        _current_size = 0;
    }

    void enum_exist_files() {
        // ── 第一趟：收集所有条目路径，关闭迭代器后再处理 ──────────────────────
        std::vector<std::filesystem::path> all_paths;
        {
            std::error_code ec;
            std::filesystem::directory_iterator di(_location, ec);
            if (ec) {
                ApError("status_recorder") << fmt::format("open dir {} failed: {}", _location, ec.message());
                return;
            }
            for (const auto& x : di) {
                all_paths.push_back(x.path());
            }
        }

        // ── 第二趟：分类处理，不合规条目收集后统一删除 ────────────────────────
        std::vector<std::filesystem::path> to_delete;

        for (const auto& path : all_paths) {
            if (std::filesystem::is_directory(path)) {
                to_delete.push_back(path);
                continue;
            }
            std::uint32_t seq = 0;
            if (!parse_seq_from_filename(path.filename().string(), _name, seq)) {
                to_delete.push_back(path);
                continue;
            }
            try {
                auto size = std::filesystem::file_size(path);
                _records.emplace_back(Record(path.string(), size));
                _total_size += size;
            } catch (...) {
                ApError("status_recorder") << fmt::format("get size of {} failed", path.string());
                to_delete.push_back(path);
            }
        }

        // 删除不合规条目（迭代器已关闭，避免迭代过程中修改目录）
        for (const auto& path : to_delete) {
            std::error_code ec;
            std::filesystem::remove_all(path, ec);
            if (ec) {
                ApError("status_recorder") << fmt::format("remove {} failed: {}", path.string(), ec.message());
            } else {
                ApInfo("status_recorder") << fmt::format("removed non-conforming entry: {}", path.string());
            }
        }

        // ── 第三趟：按文件名字典序排序（序号零填充8位，字典序 = 序号升序）────
        _records.sort();

        // ── 第四趟：用循环最大 gap 法恢复下一个序号，并在回绕时旋转 _records ──
        // 算法说明：
        //   磁盘上的文件序号在循环空间 [0, 99999999] 内构成一个连续窗口（FIFO 删旧保新）。
        //   找到相邻序号中的最大"空槽数"（含尾→头的回绕空槽），该间隔左侧序号即为最新文件的序号，
        //   其 +1（取模）即为 _next_seq。
        //   若最大间隔在中间（非回绕间隔），说明发生了回绕，需旋转 _records 保证 FIFO 正确性。
        static constexpr std::uint32_t SEQ_MAX = 99999999u;
        if (_records.empty()) {
            _next_seq = 0;
        } else {
            std::vector<std::uint32_t> seqs;
            seqs.reserve(_records.size());
            for (const auto& r : _records) {
                std::uint32_t s = 0;
                if (parse_seq_from_filename(std::filesystem::path(r._file).filename().string(), _name, s)) {
                    seqs.push_back(s);
                }
            }

            // 初始化：回绕空槽数 = SEQ_MAX - last + first（含 last→0→first 之间的空槽）
            std::uint32_t max_gap    = SEQ_MAX - seqs.back() + seqs.front();
            std::size_t   wrap_split = seqs.size(); // sentinel：最大间隔在回绕处（无中间回绕）

            for (std::size_t i = 0; i + 1 < seqs.size(); ++i) {
                std::uint32_t gap = seqs[i + 1] - seqs[i] - 1u;
                if (gap > max_gap) {
                    max_gap    = gap;
                    wrap_split = i + 1;
                }
            }

            if (wrap_split == seqs.size()) {
                // 最大间隔在回绕处：序号未回绕，直接取 last+1（取模）
                _next_seq = (seqs.back() >= SEQ_MAX) ? 0u : seqs.back() + 1u;
            } else {
                // 最大间隔在中间：已发生回绕
                // seqs[wrap_split-1] 是低序号段（新文件）的最大值，即最新文件序号
                _next_seq = (seqs[wrap_split - 1] >= SEQ_MAX) ? 0u : seqs[wrap_split - 1] + 1u;

                // 旋转 _records：将低序号（新文件）部分从链表头移至尾，
                // 使高序号（旧文件）排在前面，保证 FIFO 删除顺序正确。
                // 旋转前: [dashcam_00000000..., dashcam_00000001..., dashcam_99999997..., dashcam_99999998...]
                // 旋转后: [dashcam_99999997..., dashcam_99999998..., dashcam_00000000..., dashcam_00000001...]
                auto it = _records.begin();
                std::advance(it, wrap_split);
                _records.splice(_records.end(), _records, _records.begin(), it);
                ApInfo("status_recorder") << fmt::format(
                    "sequence wrap-around detected: split={}, rotated records for correct FIFO order",
                    wrap_split);
            }
        }

        ApInfo("status_recorder") << fmt::format("enum_exist_files: {} files, {} removed, next_seq: {}",
                                                 _records.size(), to_delete.size(), _next_seq);
    }

    void dump_exist_files() {
        for (const auto& x : _records) {
            ApInfo("status_recorder") << fmt::format("----- file: {}    size: {}", x._file, x._size);
        }
    }

    void delete_until_has_free_space() {
        ApInfo("status_recorder") << fmt::format("total size: {}, quota_bytes: {}, clean threshold bytes: {}",
                                                 _total_size, _quota_bytes, _clean_threshold_bytes);
        if (_total_size > _quota_bytes) {
            EC204::Instance().ec_add(EC204::_ERRORCODE_OVER_QUOTA);
        } else {
            EC204::Instance().ec_remove(EC204::_ERRORCODE_OVER_QUOTA);
        }

        while (_total_size > _clean_threshold_bytes) {
            auto& x = _records.front();
            try {
                std::filesystem::remove(x._file);
                _records.pop_front();
                _total_size -= x._size;
                ApInfo("status_recorder")
                    << fmt::format("del file {} of size {}, rest size {}", x._file, x._size, _total_size);
                EC204::Instance().ec_remove(EC204::_ERRORCODE_DELETE);
            } catch (const std::exception& e) {
                ApError("status_recorder")
                    << fmt::format("del file {} of size {} error: {}", x._file, x._size, e.what());
                EC204::Instance().ec_add(EC204::_ERRORCODE_DELETE);
            }
        }
    }

    // ── 文件名格式：name_NNNNNNNN_YYYYMMDDHHMMSS.sss ──────────────────────
    // 生成新文件名：使用本地时间 + 毫秒
    static std::string make_filename(const std::string& name, std::uint32_t seq) {
        struct timeval  tv {};
        gettimeofday(&tv, nullptr);
        struct tm tm_info {};
        localtime_r(&tv.tv_sec, &tm_info);
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%s_%08u_%04d%02d%02d%02d%02d%02d.%03ld",
                      name.c_str(), seq,
                      tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
                      tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
                      static_cast<long>(tv.tv_usec / 1000));
        return buf;
    }

    // 校验文件名格式并解析序号；不符合格式返回 false
    // 合规格式：name_NNNNNNNN_YYYYMMDDHHMMSS.sss
    static bool parse_seq_from_filename(const std::string& filename,
                                        const std::string& name,
                                        std::uint32_t&     seq_out) {
        const std::string prefix = name + "_";
        // 期望总长度：len(prefix) + 8(seq) + 1(_) + 14(YYYYMMDDHHMMSS) + 1(.) + 3(sss)
        if (filename.size() != prefix.size() + 27) return false;
        if (filename.compare(0, prefix.size(), prefix) != 0) return false;

        const char* p = filename.c_str() + prefix.size();
        for (int i = 0; i < 8; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(p[i]))) return false;
        }
        p += 8;
        if (*p != '_') return false;
        ++p;
        for (int i = 0; i < 14; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(p[i]))) return false;
        }
        p += 14;
        if (*p != '.') return false;
        ++p;
        for (int i = 0; i < 3; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(p[i]))) return false;
        }

        seq_out = static_cast<std::uint32_t>(std::stoul(filename.substr(prefix.size(), 8)));
        return true;
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
    std::uint32_t     _next_seq;   // 单调递增序号，启动时由 enum_exist_files() 恢复
};

#endif
