#ifndef _H264_ROTATING_FILE_RECEIVER_HPP
#define _H264_ROTATING_FILE_RECEIVER_HPP

#include <ctime>
#include <fcntl.h>

#include <filesystem>
#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "DatetimeUtil.hpp"
#include "FaultInfoSaver.hpp"
#include "StringUtils.hpp"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
#include "fmt/format.h"

class RotatingFaultInfoSaver : public FaultInfoSaver {
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

    static const constexpr char*         _ERRORCODE_CONFIG     = "2012002"; // 读取配置文件失败
    static const constexpr char*         _ERRORCODE_PERMISSOIN = "2012003"; // 数据存储目录没有权限
    static const constexpr char*         _ERRORCODE_OVER_QUOTA = "2012004"; // 数据超过限额
    static const constexpr char*         _ERRORCODE_WRITE      = "2012005"; // 写文件失败
    static const constexpr char*         _ERRORCODE_DELETE     = "2012006"; // 删除文件失败
    static const constexpr char*         _ERRORCODE_OPEN       = "2012007"; // 打开文件失败
    static const constexpr std::size_t   _FILE_NAME_LEN        = 34;        // faults_NNNNNNNN_YYYYMMDDHHMMSS.sss
    static const constexpr std::uint32_t _SEQ_MAX              = 99999999u;

    class Ec {
    public:
        Ec() = default;

        std::uint32_t ec_add(const std::string& ec) {
            std::lock_guard<std::mutex> lock(_mtx);
            auto                        ptr = _ec_map.find(ec);
            if (ptr == _ec_map.end()) {
                auto r = _ec_map.insert({ec, 0});
                if (r.second) {
                    ptr = r.first;
                } else {
                    return 0;
                }
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

    // 校验并解析文件名，格式：faults_NNNNNNNN_YYYYMMDDHHMMSS.sss（共34字符）
    // 合规返回 true 并设置 out_seq，否则返回 false
    static bool parse_filename(const std::string& name, std::uint32_t& out_seq) {
        if (name.size() != _FILE_NAME_LEN) return false;
        // 前缀 "faults_"（7字符）
        if (name.compare(0, 7, "faults_") != 0) return false;
        // 位置 7-14：8位十进制序号
        for (int i = 7; i < 15; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(name[i]))) return false;
        }
        // 位置 15：下划线分隔符
        if (name[15] != '_') return false;
        // 位置 16-29：14位日期时间（YYYYMMDDHHMMSS）
        for (int i = 16; i < 30; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(name[i]))) return false;
        }
        // 位置 30：小数点
        if (name[30] != '.') return false;
        // 位置 31-33：3位毫秒
        for (int i = 31; i < 34; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(name[i]))) return false;
        }
        out_seq = static_cast<std::uint32_t>(std::stoul(name.substr(7, 8)));
        return true;
    }

    // 生成当前时刻的时间戳字符串，格式：YYYYMMDDHHMMSS.sss
    static std::string make_timestamp_str() {
        auto      ms   = app_common::DatetimeUtil::current_millisecond();
        time_t    secs = static_cast<time_t>(ms / 1000);
        int       msec = static_cast<int>(ms % 1000);
        struct tm t{};
        localtime_r(&secs, &t);
        return fmt::format("{:04d}{:02d}{:02d}{:02d}{:02d}{:02d}.{:03d}", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                           t.tm_hour, t.tm_min, t.tm_sec, msec);
    }

public:
    // clang-format off
    RotatingFaultInfoSaver()
    : FaultInfoSaver()
    , _file_limit_duration(0)
    , _quota_bytes(0)
    , _clean_threshold_bytes(0)
    , _total_size(0)
    , _next_seq(0)
    , _current_fd(-1)
    , _current_size(0)
    , _current_start(0) {
        // clang-format on
        std::string  prefix;
        std::int32_t quota_mb;
        std::int32_t clean_threshold_mb;
        auto         ptr = CustomStack::Instance();

        auto full_path = prefix + "system/recorder/fault/location";
        if (!ptr->GetProjectConfigValue(full_path, _location) || _location.empty()) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            throw std::runtime_error("read project config " + full_path + " failed");
        }
        full_path = prefix + "system/recorder/fault/single_file_duration";
        if (!ptr->GetProjectConfigValue(full_path, _file_limit_duration) || 0 == _file_limit_duration) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            throw std::runtime_error("read project config " + full_path + " failed");
        }
        full_path = prefix + "system/recorder/fault/quota";
        if (!ptr->GetProjectConfigValue(full_path, quota_mb) || 0 == quota_mb) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            throw std::runtime_error("read project config " + full_path + " failed");
        }
        full_path = prefix + "system/recorder/fault/clean_threshold";
        if (!ptr->GetProjectConfigValue(full_path, clean_threshold_mb) || 0 == clean_threshold_mb ||
            clean_threshold_mb > quota_mb) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            throw std::runtime_error("read project config " + full_path + " failed");
        }
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);

        ApInfo("fault_handler") << "location: " << _location;
        ApInfo("fault_handler") << "single_file_duration: " << _file_limit_duration;
        ApInfo("fault_handler") << "quota: " << quota_mb;
        ApInfo("fault_handler") << "clean_threshold: " << clean_threshold_mb;

        _quota_bytes           = (std::uint64_t)quota_mb * 1024 * 1024;
        _clean_threshold_bytes = (std::uint64_t)clean_threshold_mb * 1024 * 1024;
        ApInfo("fault_handler") << "quota bytes: " << _quota_bytes;
        ApInfo("fault_handler") << "clean threshold bytes: " << _clean_threshold_bytes;

        if (std::filesystem::exists(_location)) {
            if (!std::filesystem::is_directory(_location)) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_PERMISSOIN);
                throw std::runtime_error("data location is not a directory");
            }

            if (access(_location.c_str(), W_OK | R_OK | X_OK) != 0) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_PERMISSOIN);
                throw std::runtime_error("data location is not accessiable");
            }
        } else {
            std::error_code ec;
            if (!std::filesystem::create_directories(_location, ec)) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_PERMISSOIN);
                throw std::runtime_error("create data location failed");
            }
        }
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_PERMISSOIN);

        enum_exist_files();
        check_limit_and_open();
    }

    ~RotatingFaultInfoSaver() override {
        if (_current_fd >= 0) {
            close(_current_fd);
            _current_fd = -1;
        }
    }

    [[nodiscard]] std::string string() const override {
        return _location;
    }

    void push(const std::string& fault_record) noexcept(false) override {
        check_limit_and_open();

        std::size_t offset = 0;
        auto        p      = fault_record.data();
        auto        s      = fault_record.size();
        while (offset < s) {
            auto r = write(_current_fd, p + offset, s - offset);
            if (r < 0) {
                ApError("fault_handler") << "write error: " << strerror(errno);
                _ec.ec_add(_ERRORCODE_WRITE);
                throw std::runtime_error(std::string("write error with ") + strerror(errno));
            }
            offset += r;
            _current_size += r;
        }

        _ec.ec_remove(_ERRORCODE_WRITE);
    }

protected:
    void check_limit_and_open() {
        bool need_create = false;

        if (_current_fd < 0) {
            need_create = true;
        } else if ((_current_start + _file_limit_duration) <= time(nullptr)) {
            close(_current_fd);
            _records.emplace_back(Record(std::move(_current_name), _current_size));
            _total_size += _current_size;

            _current_name  = "";
            _current_size  = 0;
            _current_fd    = -1;
            _current_start = 0;
            need_create    = true;
        }

        if (!need_create) {
            return;
        }

        delete_until_has_free_space();

        std::filesystem::path p(_location);
        p.append(fmt::format("faults_{:08d}_{}", _next_seq, make_timestamp_str()));
        _current_name = p.string();
        _current_fd   = open(_current_name.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR);
        if (_current_fd < 0) {
            ApError("fault_handler") << fmt::format("open {} error {}", p.string(),
                                                    app_common::StringUtils::str_error(errno));
            _ec.ec_add(_ERRORCODE_OPEN);
            return;
        }
        _ec.ec_remove(_ERRORCODE_OPEN);
        // 文件成功创建后推进序号，溢出时回绕到 0
        _next_seq      = (_next_seq >= _SEQ_MAX) ? 0u : _next_seq + 1u;
        _current_start = time(nullptr);
        _current_size  = 0;
    }

    // 三阶段遍历：先收集，再分类处理，最后排序+回绕检测，避免在迭代器中操作文件系统
    void enum_exist_files() {
        // 阶段一：收集目录下所有条目路径
        std::vector<std::filesystem::path> all_entries;
        std::error_code                    iter_ec;
        for (const auto& x : std::filesystem::directory_iterator(_location, iter_ec)) {
            all_entries.push_back(x.path());
        }
        if (iter_ec) {
            ApError("fault_handler") << fmt::format("enum directory {} error: {}", _location, iter_ec.message());
        }

        // 阶段二：分类——合规文件加入 _records，不合规条目记录待删
        std::vector<std::filesystem::path> to_delete;

        for (const auto& path : all_entries) {
            std::uint32_t seq = 0;
            if (parse_filename(path.filename().string(), seq)) {
                try {
                    auto size = std::filesystem::file_size(path);
                    _records.emplace_back(Record(path.string(), size));
                    _total_size += size;
                } catch (...) {
                    ApError("fault_handler") << fmt::format("get size of {} failed", path.string());
                }
            } else {
                to_delete.push_back(path);
            }
        }

        // 阶段三：删除不合规文件/目录
        for (const auto& p : to_delete) {
            ApInfo("fault_handler") << fmt::format("removing non-conforming entry: {}", p.string());
            std::error_code rm_ec;
            std::filesystem::remove_all(p, rm_ec);
            if (rm_ec) {
                ApError("fault_handler") << fmt::format("remove {} failed: {}", p.string(), rm_ec.message());
            }
        }

        // 阶段四：按文件名字典序排序（序号零填充8位，字典序 = 序号升序）
        _records.sort();

        // 阶段五：用循环最大 gap 法恢复下一个序号，并在回绕时旋转 _records
        // 算法说明：
        //   磁盘上的文件序号在循环空间 [0, _SEQ_MAX] 内构成一个连续窗口（FIFO 删旧保新）。
        //   找到相邻序号中的最大"空槽数"（含尾→头的回绕空槽），该间隔左侧序号即为最新文件的序号，
        //   其 +1（取模）即为 _next_seq。
        //   若最大间隔在中间（非回绕间隔），说明发生了回绕，需旋转 _records 保证 FIFO 正确性。
        if (_records.empty()) {
            _next_seq = 0;
        } else {
            // 从已排序的 _records 中提取序号（已升序）
            std::vector<std::uint32_t> seqs;
            seqs.reserve(_records.size());
            for (const auto& r : _records) {
                std::uint32_t s = 0;
                if (parse_filename(std::filesystem::path(r._file).filename().string(), s)) {
                    seqs.push_back(s);
                }
            }

            // 初始化：回绕空槽数 = _SEQ_MAX - last + first（含 last→0→first 之间的空槽）
            std::uint32_t max_gap    = _SEQ_MAX - seqs.back() + seqs.front();
            std::size_t   wrap_split = seqs.size(); // sentinel：最大间隔在回绕处（无中间回绕）

            for (std::size_t i = 0; i + 1 < seqs.size(); ++i) {
                std::uint32_t gap = seqs[i + 1] - seqs[i] - 1u; // 两相邻序号间的空槽数
                if (gap > max_gap) {
                    max_gap    = gap;
                    wrap_split = i + 1; // 间隔右侧第一个序号是高序号段（旧文件）起点
                }
            }

            if (wrap_split == seqs.size()) {
                // 最大间隔在回绕处：序号未回绕，直接取 last+1（取模）
                _next_seq = (seqs.back() >= _SEQ_MAX) ? 0u : seqs.back() + 1u;
            } else {
                // 最大间隔在中间：已发生回绕
                // seqs[wrap_split-1] 是低序号段（新文件）的最大值，即最新文件序号
                _next_seq = (seqs[wrap_split - 1] >= _SEQ_MAX) ? 0u : seqs[wrap_split - 1] + 1u;

                // 旋转 _records：将低序号（新文件）部分从链表头移至尾，
                // 使高序号（旧文件）排在前面，保证 FIFO 删除顺序正确。
                // 旋转前: [faults_00000000..., faults_00000001..., faults_99999997..., faults_99999998...]
                // 旋转后: [faults_99999997..., faults_99999998..., faults_00000000..., faults_00000001...]
                auto it = _records.begin();
                std::advance(it, wrap_split);
                _records.splice(_records.end(), _records, _records.begin(), it);
                ApInfo("fault_handler") << fmt::format(
                    "sequence wrap-around detected: split={}, rotated records for correct FIFO order",
                    wrap_split);
            }
        }

        ApInfo("fault_handler") << fmt::format("enum_exist_files: {} valid files, {} removed, next_seq={}",
                                               _records.size(), to_delete.size(), _next_seq);
    }

    void dump_exist_files() {
        for (const auto& x : _records) {
            ApInfo("fault_handler") << fmt::format("file: {}    size: {}", x._file, x._size);
        }
    }

    void delete_until_has_free_space() {
        ApInfo("fault_handler") << fmt::format("total size: {}, quota_bytes: {}, clean threshold bytes: {}",
                                               _total_size, _quota_bytes, _clean_threshold_bytes);
        if (_total_size > _quota_bytes) {
            _ec.ec_add(_ERRORCODE_OVER_QUOTA);
        } else {
            _ec.ec_remove(_ERRORCODE_OVER_QUOTA);
        }

        while (_total_size > _clean_threshold_bytes && !_records.empty()) {
            // 先拷贝成员值，再 pop，避免 pop_front 后悬空引用
            auto file = _records.front()._file;
            auto size = _records.front()._size;
            _records.pop_front();
            try {
                std::filesystem::remove(file);
                _total_size -= size;
                ApInfo("fault_handler") << fmt::format("del file {} of size {}, rest size {}", file, size,
                                                       _total_size);
                _ec.ec_remove(_ERRORCODE_DELETE);
            } catch (const std::exception& e) {
                ApError("fault_handler") << fmt::format("del file {} of size {} error: {}", file, size, e.what());
                _ec.ec_add(_ERRORCODE_DELETE);
            }
        }
    }

protected:
    std::string   _location;
    std::uint64_t _file_limit_duration;
    std::uint64_t _quota_bytes;
    std::uint64_t _clean_threshold_bytes;

    std::uint64_t     _total_size;
    std::uint32_t     _next_seq;
    std::list<Record> _records;
    std::string       _current_name;
    std::int32_t      _current_fd;
    std::uint64_t     _current_size;
    time_t            _current_start;
    Ec                _ec;
};

#endif
