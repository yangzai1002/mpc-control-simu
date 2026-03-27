#ifndef RECORDER_SQLITE3_HPP
#define RECORDER_SQLITE3_HPP

#include <sqlite3.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include "StringUtils.hpp"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msghaftime.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "config_loader/custom_stack.h"
#include "fmt/format.h"
#include "ap_log/ap_log_interface.h"
#include "EC204.hpp"

namespace vehicle_recorder {

    /**
     * @brief Helper class to flatten nested structures into column names and values
     */
    class FieldFlattener {
    public:
        /**
         * @brief Flatten a structure recursively
         */
        template <typename T>
        static void flatten(const std::string& prefix, const T& obj, std::map<std::string, std::string>& fields) {
            FieldCollector collector(prefix, fields);
            obj.enumerate_internal(collector); // Call const version
        }

    private:
        class FieldCollector {
        public:
            FieldCollector(const std::string& prefix, std::map<std::string, std::string>& fields)
                : _prefix(prefix), _fields(fields) {
            }

            template <typename T> void operator()(const char* name, const T& value) const {
                std::string full_name = _prefix.empty() ? name : _prefix + "_" + name;
                process_field(full_name, value);
            }

        private:
            template <typename T> void process_field(const std::string& name, const T& value) const {
                // Check if T has enumerate_internal method (nested structure)
                if constexpr (has_enumerate_internal<T>::value) {
                    // Recursively flatten nested structure
                    FieldCollector nested_collector(name, _fields);
                    value.enumerate_internal(nested_collector); // Call const version
                } else {
                    // Primitive type - convert to string
                    _fields[name] = this->template to_string<T>(value);
                }
            }

            // Type trait to check if type has enumerate_internal
            template <typename T, typename = void> struct has_enumerate_internal : std::false_type {};

            template <typename T> struct has_enumerate_internal<
                T, std::void_t<decltype(std::declval<T&>().enumerate_internal(std::declval<FieldCollector&>()))>>
                : std::true_type {};

            // Type trait to check if T is ara::adsfi::FaultInfoVec specifically
            // FaultInfoVec is ara::core::Vector<FaultInfo> which is std::vector<FaultInfo>
            template <typename T> struct is_fault_info_vector : std::is_same<T, ara::adsfi::FaultInfoVec> {};

            // Type trait to check if T is any vector-like container (for ignoring)
            template <typename T, typename = void> struct is_vector_like : std::false_type {};

            template <typename T>
            struct is_vector_like<T, std::void_t<typename T::value_type, decltype(std::declval<T>().begin()),
                                                 decltype(std::declval<T>().end()), decltype(std::declval<T>().size())>>
                : std::true_type {};

            // Convert various types to string
            template <typename T> std::string to_string(const T& value) const {
                if constexpr (std::is_same_v<T, ara::adsfi::MsgHafTime>) {
                    // Format MsgHafTime as YYYYMMDDHH24MISS.sss
                    return format_msghaftime(value);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    return value ? "1" : "0";
                } else if constexpr (std::is_arithmetic_v<T>) {
                    return std::to_string(value);
                } else if constexpr (std::is_enum_v<T>) {
                    return std::to_string(static_cast<std::underlying_type_t<T>>(value));
                } else if constexpr (is_fault_info_vector<T>::value) {
                    // ONLY serialize ara::adsfi::FaultInfoVec to JSON
                    return fault_info_vector_to_json(value);
                } else if constexpr (is_vector_like<T>::value) {
                    // Ignore all other vector-like containers - return empty string
                    return "";
                } else {
                    // For complex types that don't have enumerate_internal,
                    // we'll just return empty string or handle specially
                    return "";
                }
            }

            // Format MsgHafTime to YYYYMMDDHH24MISS.sss (local time)
            static std::string format_msghaftime(const ara::adsfi::MsgHafTime& time) {
                // Convert to time_t
                std::time_t sec = static_cast<std::time_t>(time.sec);

                // Get milliseconds from nanoseconds
                uint32_t milliseconds = time.nsec / 1000000;

                // Convert to tm structure using local time
                std::tm tm_time;
                if (::localtime_r(&sec, &tm_time) == nullptr) {
                    // Fallback if conversion fails
                    return std::to_string(time.sec) + "." + std::to_string(time.nsec);
                }

                // Format as YYYYMMDDHH24MISS.sss
                char buffer[32];
                std::snprintf(buffer, sizeof(buffer), "%04d%02d%02d%02d%02d%02d.%03u",
                              tm_time.tm_year + 1900, // Year
                              tm_time.tm_mon + 1,     // Month (0-11 -> 1-12)
                              tm_time.tm_mday,        // Day
                              tm_time.tm_hour,        // Hour
                              tm_time.tm_min,         // Minute
                              tm_time.tm_sec,         // Second
                              milliseconds);          // Milliseconds

                return std::string(buffer);
            }

            // Serialize ara::adsfi::FaultInfoVec to JSON array string
            std::string fault_info_vector_to_json(const ara::adsfi::FaultInfoVec& vec) const {
                std::ostringstream json;
                json << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0)
                        json << ",";

                    json << "{";
                    // Manually serialize FaultInfo fields
                    // Note: FaultInfo has ::String type which is typically std::string
                    json << "\"code\":\"" << escape_json(std::string(vec[i].code)) << "\"";
                    json << ",\"handle\":\"" << std::to_string(vec[i].handle) << "\"";
                    json << ",\"desc\":\"" << escape_json(std::string(vec[i].desc)) << "\"";
                    json << ",\"extra_desc\":\"" << escape_json(std::string(vec[i].extra_desc)) << "\"";
                    json << ",\"timestamp\":\"" << std::to_string(vec[i].timestamp) << "\"";
                    json << ",\"from\":\"" << escape_json(std::string(vec[i].from)) << "\"";
                    json << "}";
                }
                json << "]";
                return json.str();
            }

            // Escape special characters for JSON
            static std::string escape_json(const std::string& s) {
                std::ostringstream o;
                for (char c : s) {
                    switch (c) {
                        case '"':
                            o << "\\\"";
                            break;
                        case '\\':
                            o << "\\\\";
                            break;
                        case '\b':
                            o << "\\b";
                            break;
                        case '\f':
                            o << "\\f";
                            break;
                        case '\n':
                            o << "\\n";
                            break;
                        case '\r':
                            o << "\\r";
                            break;
                        case '\t':
                            o << "\\t";
                            break;
                        default:
                            if ('\x00' <= c && c <= '\x1f') {
                                o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                            } else {
                                o << c;
                            }
                    }
                }
                return o.str();
            }

            std::string                         _prefix;
            std::map<std::string, std::string>& _fields;
        };
    };

    /**
     * @brief Manages SQLite3 database with automatic rotation
     */
    class RotatingDbManager {
    public:
        struct DbFile {
            std::string  path;
            std::size_t  size;    // .db 文件大小 + 对应 .db-wal 大小之和（WAL 归并到此，不单独追踪）
            std::time_t  created;
            std::uint32_t seq;   // 单调序号；旧格式文件统一为 0

            bool operator<(const DbFile& other) const {
                if (seq != other.seq) return seq < other.seq;
                return path < other.path; // 相同 seq 时（旧格式文件）按路径名排序
            }
        };

        RotatingDbManager(const std::string& location, const std::string& name, std::uint64_t file_limit_duration_sec,
                          std::uint64_t quota_bytes, std::uint64_t clean_threshold_bytes)
            : _location(location)
            , _name(name)
            , _file_limit_duration(file_limit_duration_sec)
            , _quota_bytes(quota_bytes)
            , _clean_threshold_bytes(clean_threshold_bytes)
            , _total_size(0)
            , _next_seq(1)
            , _current_db(nullptr)
            , _current_size(0)
            , _current_created(0)
            , _current_seq(0) {
            // Ensure location directory exists
            if (std::filesystem::exists(_location)) {
                if (!std::filesystem::is_directory(_location)) {
                    EC204::Instance().ec_add(EC204::_ERRORCODE_DIRECTORY_ACCESS, _location);
                    throw std::runtime_error("data location is not a directory: " + _location);
                }
            } else {
                std::error_code err;
                std::filesystem::create_directories(_location, err);
                if (err) {
                    EC204::Instance().ec_add(EC204::_ERRORCODE_DIRECTORY_ACCESS, _location);
                    throw std::runtime_error("failed to create data location: " + _location);
                }
            }
            EC204::Instance().ec_remove(EC204::_ERRORCODE_DIRECTORY_ACCESS);

            // 启动时扫描磁盘：删除非法文件、填充 _db_files、清理超配额旧文件
            delete_until_has_free_space();

            // Try to resume last file if still within time limit (Q1)
            try_resume_last_file();

            // Open or create initial database (no-op if try_resume_last_file succeeded)
            rotate_if_needed();
        }

        ~RotatingDbManager() {
            close_current_db();
        }

        sqlite3* get_db() {
            std::lock_guard<std::mutex> lock(_mutex);
            rotate_if_needed();
            return _current_db;
        }

    private:
        void close_current_db() {
            if (_current_db) {
                // Use sqlite3_close_v2 to wait for all statements to finish
                int rc = sqlite3_close_v2(_current_db);
                if (rc != SQLITE_OK) {
                    ApError("status_recorder") << "Failed to close database: " << sqlite3_errmsg(_current_db);
                }
                _current_db = nullptr;
            }
        }

        void rotate_if_needed() {
            bool need_rotate = false;

            if (!_current_db) {
                need_rotate = true;
            } else {
                std::time_t now = std::time(nullptr);
                if ((_current_created + _file_limit_duration) <= now) {
                    need_rotate = true;
                    ApInfo("status_recorder") << "Database rotation triggered by time limit";
                }
            }

            if (!need_rotate) {
                return;
            }

            // Close current database
            if (_current_db) {
                close_current_db();
            }

            // Delete old files if needed（内部重新扫描磁盘，刚关闭的文件会被自动发现）
            delete_until_has_free_space();

            // Create new database file with monotonic seq + localtime timestamp
            std::filesystem::path p(_location);
            if (_next_seq == 0 || _next_seq > 99999999u) _next_seq = 1u;
            const std::uint32_t new_seq = _next_seq++;
            p.append(make_db_filename(_name, new_seq));
            _current_path    = p.string();
            _current_created = std::time(nullptr);
            _current_size    = 0;
            _current_seq     = new_seq;

            // Open new database
            int rc = sqlite3_open(_current_path.c_str(), &_current_db);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to open database: " << _current_path << " - " << sqlite3_errmsg(_current_db);
                EC204::Instance().ec_add(EC204::_ERRORCODE_OPEN, _current_path);
                throw std::runtime_error("Failed to open database");
            }
            EC204::Instance().ec_remove(EC204::_ERRORCODE_OPEN);

            // Enable WAL mode for better performance
            char* err_msg = nullptr;
            rc = sqlite3_exec(_current_db, "PRAGMA journal_mode=WAL", nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to enable WAL mode: " << err_msg;
                sqlite3_free(err_msg);
            }

            ApInfo("status_recorder") << "Created new database: " << _current_path;
        }

        void enumerate_existing_files() {
            // 可重入：每次从磁盘重新扫描，清空并重建内存状态
            _db_files.clear();
            _total_size = 0;

            if (!std::filesystem::exists(_location)) {
                return;
            }

            // ── 第一趟：收集所有条目路径，关闭迭代器后再处理 ──────────────────────
            std::vector<std::filesystem::path> all_paths;
            {
                std::error_code ec;
                std::filesystem::directory_iterator di(_location, ec);
                if (ec) {
                    ApError("status_recorder") << "Failed to open dir " << _location << ": " << ec.message();
                    return;
                }
                for (const auto& entry : di) {
                    all_paths.push_back(entry.path());
                }
            }

            std::vector<std::filesystem::path> to_delete;
            std::uint32_t max_seq = 0;

            // ── 第二趟：处理非辅助文件（-wal / -shm 后缀的文件跳过，第三趟处理）──
            // 合规 .db → _db_files；其余（目录、命名不合规等）→ to_delete
            for (const auto& path : all_paths) {
                const std::string& s = path.string();

                // SQLite 辅助文件（-wal / -shm）留待第三趟处理
                if (s.size() > 4 &&
                    (s.compare(s.size() - 4, 4, "-wal") == 0 ||
                     s.compare(s.size() - 4, 4, "-shm") == 0)) {
                    continue;
                }

                if (!std::filesystem::is_regular_file(path)) {
                    to_delete.push_back(path);
                    continue;
                }

                std::uint32_t seq = 0;
                if (!parse_seq_from_filename(path.filename().string(), _name, seq)) {
                    to_delete.push_back(path);
                    continue;
                }

                try {
                    std::size_t size = std::filesystem::file_size(path);
                    // 若存在残留 WAL 文件（突然下电所致），也计入配额；
                    // 该 WAL 在下次打开数据库时由 SQLite 自动恢复，或随 .db 一起被清理删除
                    {
                        std::error_code ec;
                        auto wal_size = std::filesystem::file_size(s + "-wal", ec);
                        if (!ec) size += wal_size;
                    }
                    std::time_t created =
                        std::filesystem::last_write_time(path).time_since_epoch().count() /
                            10000000 -
                        11644473600LL;
                    _db_files.push_back({s, size, created, seq});
                    _total_size += size;
                    if (seq > max_seq) max_seq = seq;
                } catch (...) {
                    ApError("status_recorder") << "Failed to get info for " << s;
                    to_delete.push_back(path);
                }
            }

            // ── 第三趟：处理辅助文件（-wal / -shm）────────────────────────────────
            // 规则：去掉后缀得到父 .db 路径；父 .db 在 _db_files 中 → 保留（SQLite 恢复时需要）
            //                                ；父 .db 不在    → 孤儿文件，删除
            for (const auto& path : all_paths) {
                const std::string& s = path.string();
                if (s.size() <= 4) continue;
                if (s.compare(s.size() - 4, 4, "-wal") != 0 &&
                    s.compare(s.size() - 4, 4, "-shm") != 0) {
                    continue;
                }
                const std::string parent_path = s.substr(0, s.size() - 4);
                const bool parent_tracked = std::any_of(
                    _db_files.begin(), _db_files.end(),
                    [&parent_path](const DbFile& f) { return f.path == parent_path; });
                if (!parent_tracked) {
                    to_delete.push_back(path);
                }
            }

            // 删除不合规及孤儿文件（迭代器已关闭，避免迭代过程中修改目录）
            for (const auto& path : to_delete) {
                std::error_code ec;
                std::filesystem::remove_all(path, ec);
                if (ec) {
                    ApError("status_recorder") << "Failed to remove " << path.string() << ": " << ec.message();
                } else {
                    ApInfo("status_recorder") << "Removed non-conforming entry: " << path.string();
                }
            }

            std::sort(_db_files.begin(), _db_files.end());
            // 保留 _next_seq 单调性：重入时不能让序号倒退
            const std::uint32_t derived = (max_seq >= 99999999u) ? 1u : max_seq + 1u;
            if (derived > _next_seq) _next_seq = derived;
            ApInfo("status_recorder") << "Enumerated " << _db_files.size() << " db files"
                  << ", " << to_delete.size() << " removed"
                  << ", total size: " << _total_size
                  << ", next seq: " << _next_seq;
        }

        // 生成新 DB 文件名：name_NNNNNNNN_YYYYMMDDHH24MISS.sss.db（本地时间）
        static std::string make_db_filename(const std::string& name, std::uint32_t seq) {
            auto now     = std::chrono::system_clock::now();
            auto ms_part = std::chrono::duration_cast<std::chrono::milliseconds>(
                               now.time_since_epoch()) % 1000;
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm     tm_time{};
            if (::localtime_r(&t, &tm_time) == nullptr) {
                // 时间转换失败时用全零时间戳，序号仍保证唯一性
                char buf[64];
                std::snprintf(buf, sizeof(buf), "%s_%08u_00000000000000.000.db",
                              name.c_str(), seq);
                return std::string(buf);
            }
            char buf[64];
            std::snprintf(buf, sizeof(buf), "%s_%08u_%04d%02d%02d%02d%02d%02d.%03lld.db",
                          name.c_str(), seq,
                          tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                          tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                          static_cast<long long>(ms_part.count()));
            return std::string(buf);
        }

        // 从文件名解析序号：
        //   新格式 name_NNNNNNNN_timestamp.db  → seq = NNNNNNNN
        //   旧格式 name_YYYYMMDDHHMISS.db      → seq = 0
        //   不匹配                             → 返回 false
        static bool parse_seq_from_filename(const std::string& filename,
                                            const std::string& name,
                                            std::uint32_t&     seq_out) {
            // 必须以 name + "_" 开头，以 ".db" 结尾
            const std::string prefix = name + "_";
            if (filename.size() < prefix.size() + 3) {
                return false;
            }
            if (filename.compare(0, prefix.size(), prefix) != 0) {
                return false;
            }
            if (filename.size() < 3 || filename.compare(filename.size() - 3, 3, ".db") != 0) {
                return false;
            }

            // prefix 之后的部分
            const std::string rest = filename.substr(prefix.size());
            if (rest.size() < 8) {
                return false;
            }

            // 前 8 个字符必须全为数字
            for (int i = 0; i < 8; ++i) {
                if (!std::isdigit(static_cast<unsigned char>(rest[i]))) {
                    return false;
                }
            }

            if (rest.size() > 8 && rest[8] == '_') {
                // 新格式：name_NNNNNNNN_timestamp.db
                seq_out = static_cast<std::uint32_t>(std::stoul(rest.substr(0, 8)));
            } else {
                // 旧格式：name_YYYYMMDDHHMISS.db（前 8 位恰好是 YYYYMMDD）
                seq_out = 0;
            }
            return true;
        }

        // 从新格式文件名中解析创建时间（本地时间）：
        //   name_NNNNNNNN_YYYYMMDDHHMMSS.sss.db → time_t（失败返回 0）
        static std::time_t parse_created_time_from_path(const std::string& path, const std::string& name) {
            const std::string filename = std::filesystem::path(path).filename().string();
            const std::string prefix   = name + "_";
            // 最短长度：prefix + 8(seq) + 1(_) + 14(YYYYMMDDHHMMSS) + 1(.) + 3(sss) + 3(.db)
            if (filename.size() < prefix.size() + 30) return 0;
            if (filename.compare(0, prefix.size(), prefix) != 0) return 0;
            if (filename.compare(filename.size() - 3, 3, ".db") != 0) return 0;

            // prefix 之后：NNNNNNNN_YYYYMMDDHHMMSS.sss.db
            const std::string rest = filename.substr(prefix.size());
            if (rest.size() < 30 || rest[8] != '_') return 0; // 非新格式

            // 时间戳从 rest[9] 开始，长度 18：YYYYMMDDHHMMSS.sss
            const std::string ts = rest.substr(9, 18);
            int               year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, ms = 0;
            if (std::sscanf(ts.c_str(), "%4d%2d%2d%2d%2d%2d.%3d",
                            &year, &month, &day, &hour, &min, &sec, &ms) != 7) {
                return 0;
            }

            std::tm tm_time{};
            tm_time.tm_year  = year - 1900;
            tm_time.tm_mon   = month - 1;
            tm_time.tm_mday  = day;
            tm_time.tm_hour  = hour;
            tm_time.tm_min   = min;
            tm_time.tm_sec   = sec;
            tm_time.tm_isdst = -1;
            return ::mktime(&tm_time);
        }

        // 若最后一个文件为新格式且尚在有效期内，则复用而非新建（Q1）
        void try_resume_last_file() {
            if (_db_files.empty()) return;

            const DbFile& last = _db_files.back();
            if (last.seq == 0) return; // 旧格式文件，不复用

            std::time_t created = parse_created_time_from_path(last.path, _name);
            if (created == 0) return; // 解析失败

            const std::time_t now = std::time(nullptr);
            if (created + static_cast<std::time_t>(_file_limit_duration) <= now) {
                ApInfo("status_recorder") << "Last db file expired, will create new one: " << last.path;
                return;
            }

            // 重新打开该文件
            int rc = sqlite3_open(last.path.c_str(), &_current_db);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to reopen last db file: " << last.path
                       << " (" << sqlite3_errmsg(_current_db) << "), will create new one";
                sqlite3_close_v2(_current_db);
                _current_db = nullptr;
                return;
            }

            // 重新启用 WAL 模式
            char* err_msg = nullptr;
            rc = sqlite3_exec(_current_db, "PRAGMA journal_mode=WAL", nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to enable WAL mode on resumed file: " << err_msg;
                sqlite3_free(err_msg);
            }

            _current_path    = last.path;
            _current_created = created;
            _current_seq     = last.seq;
            try {
                _current_size = std::filesystem::file_size(last.path);
            } catch (...) {
                _current_size = 0;
            }

            // 从历史列表中移出（当前打开的文件由 _current_* 成员单独跟踪）
            _total_size -= last.size;
            _db_files.pop_back();

            ApInfo("status_recorder") << "Resumed last database file: " << _current_path;
        }

        // 删除 .db 文件并同时清理对应的 SQLite WAL/SHM 辅助文件（防止突然下电后的孤儿文件积压）
        // 注意：仅对已关闭（不在当前连接上）的文件调用；当前打开的数据库的 WAL 由 SQLite 自己管理
        static void remove_db_with_aux(const std::string& path) {
            std::filesystem::remove(path);
            // 清理可能残留的 WAL/SHM（文件不存在时 remove 静默忽略错误）
            for (const char* suffix : {"-wal", "-shm"}) {
                std::error_code ec;
                std::filesystem::remove(path + suffix, ec);
            }
        }

        void delete_until_has_free_space() {
            // 重新扫描磁盘：删除非法/孤儿文件，更新各文件实际大小，修正 _total_size
            // 同时捕获外部手动拷贝进来的文件，确保配额检查基于真实磁盘状态
            enumerate_existing_files();

            const bool had_files = !_db_files.empty();

            if (_total_size > _quota_bytes) {
                EC204::Instance().ec_add(EC204::_ERRORCODE_OVER_QUOTA);
            }

            while (_total_size >= _clean_threshold_bytes && !_db_files.empty()) {
                const auto& file = _db_files.front();
                try {
                    remove_db_with_aux(file.path); // 同步清理 .db-wal / .db-shm 孤儿文件
                    _total_size -= file.size;
                    ApInfo("status_recorder") << "Deleted old database: " << file.path << " (size: " << file.size << ")";
                    _db_files.erase(_db_files.begin());
                    EC204::Instance().ec_remove(EC204::_ERRORCODE_DELETE);
                } catch (const std::exception& e) {
                    ApError("status_recorder") << "Failed to delete " << file.path << ": " << e.what();
                    EC204::Instance().ec_add(EC204::_ERRORCODE_DELETE);
                    break; // 无法删除则停止，避免 _total_size 与 _db_files 失去同步
                }
            }

            // Q3: 如果历史文件全被删光，说明配额设置过小，告警提示用户
            if (had_files && _db_files.empty()) {
                ApError("status_recorder") << "All history database files deleted due to quota pressure. "
                       << "Consider increasing quota (current: "
                       << _quota_bytes / 1024 / 1024 << " MB).";
            }

            if (_total_size > _quota_bytes) {
                ApWarn("status_recorder") << "Total database size (" << _total_size << ") exceeds quota (" << _quota_bytes << ")";
            } else {
                EC204::Instance().ec_remove(EC204::_ERRORCODE_OVER_QUOTA);
            }
        }

        std::string   _location;
        std::string   _name;
        std::uint64_t _file_limit_duration;
        std::uint64_t _quota_bytes;
        std::uint64_t _clean_threshold_bytes;

        std::mutex            _mutex;
        std::uint64_t         _total_size;
        std::vector<DbFile>   _db_files;
        std::uint32_t         _next_seq;    // 下一次轮转使用的序号，单调递增

        sqlite3*      _current_db;
        std::string   _current_path;
        std::size_t   _current_size;
        std::time_t   _current_created;
        std::uint32_t _current_seq;  // 当前打开文件的序号
    };

    /**
     * @brief Single record entry pushed to the async queue by producers
     */
    struct RecordEntry {
        std::string                        table_name;
        std::map<std::string, std::string> fields;
    };

    /**
     * @brief Record batch grouped by table, consumed by the writer thread
     */
    struct RecordBatch {
        std::string                                     table_name;
        std::vector<std::map<std::string, std::string>> records;
    };

    /**
     * @brief Main vehicle database recorder class
     */
    class RecorderSqlite3 {
    public:
        RecorderSqlite3() : _running(false), _db_manager(nullptr), _flush_interval_ms(2000) {
            // Load configuration
            load_config();

            // Create database manager
            _db_manager = std::make_unique<RotatingDbManager>(_location, _name, _file_limit_duration, _quota_bytes,
                                                              _clean_threshold_bytes);

            // Create tables
            create_tables();

            // Start async writer thread
            _running = true;
            _writer_thread = std::thread(&RecorderSqlite3::async_writer, this);

            // Start flush timer thread
            _flush_thread = std::thread(&RecorderSqlite3::flush_timer, this);
        }

        ~RecorderSqlite3() {
            // Stop threads
            _running = false;
            _cv.notify_all();
            _flush_cv.notify_all();

            if (_writer_thread.joinable()) {
                _writer_thread.join();
            }
            if (_flush_thread.joinable()) {
                _flush_thread.join();
            }
        }

        /**
         * @brief Force flush all pending records to disk
         * This blocks until all queued records are written
         */
        void flush() {
            std::unique_lock<std::mutex> lock(_queue_mutex);

            // Wait until queue is empty
            while (!_record_queue.empty() && _running) {
                _cv.notify_one(); // Wake up writer
                _flush_cv.wait_for(lock, std::chrono::milliseconds(100));
            }

            ApInfo("status_recorder") << "Flush completed, all records written to database";
        }

        // Record methods for each data type
        template <typename T> void record(const std::string& table_name, const T& data) {
            // Flatten the data structure
            std::map<std::string, std::string> fields;
            FieldFlattener::flatten("", data, fields);

            // Push single entry; writer thread groups by table before committing
            {
                std::lock_guard<std::mutex> lock(_queue_mutex);
                _record_queue.push({table_name, std::move(fields)});
            }
            _cv.notify_one();
        }

        void record_vehicle_information(const ara::adsfi::VehicleInformation& data) {
            record("vehicle_information", data);
        }

        void record_location(const ara::adsfi::MsgHafLocation& data) {
            record("location", data);
        }

        void record_fault_data(const ara::adsfi::FaultData& data) {
            record("fault_data", data);
        }

        void record_gnss_info(const ara::adsfi::MsgHafGnssInfo& data) {
            record("gnss_info", data);
        }

        void record_vehicle_act_control(const ara::adsfi::VehicleActControl& data) {
            record("vehicle_act_control", data);
        }

        void record_business_command(const ara::adsfi::BusinessCommand& data) {
            record("business_command", data);
        }

        void record_ego_trajectory(const ara::adsfi::MsgHafEgoTrajectory& data) {
            record("ego_trajectory", data);
        }

        void record_planning_status(const ara::adsfi::PlanningStatus& data) {
            record("planning_status", data);
        }

        void record_remote_control(const ara::adsfi::AppRemoteControl& data) {
            record("remote_control", data);
        }

    private:
        // Escape SQL identifier (table/column names) to prevent SQL injection
        static std::string escape_identifier(const std::string& identifier) {
            // SQLite uses double quotes for identifiers
            // Escape any double quotes by doubling them
            std::string result = "\"";
            for (char c : identifier) {
                if (c == '"') {
                    result += "\"\""; // Escape double quote
                } else {
                    result += c;
                }
            }
            result += "\"";
            return result;
        }

        // Validate identifier to ensure it's safe (alphanumeric + underscore only)
        static bool is_valid_identifier(const std::string& identifier) {
            if (identifier.empty() || std::isdigit(identifier[0])) {
                return false; // Can't start with digit
            }
            for (char c : identifier) {
                if (!std::isalnum(c) && c != '_') {
                    return false;
                }
            }
            return true;
        }

        void load_config() {
            auto        ptr = CustomStack::Instance();
            std::string prefix = "";

            // Load location
            auto full_path = prefix + "system/recorder/status/location";
            if (!ptr->GetProjectConfigValue(full_path, _location) || _location.empty()) {
                _location = "/opt/app/vehicle_db";
                ApWarn("status_recorder") << "Using default location: " << _location;
            }

            // Load name
            _name = "blackbox";

            // Load duration
            full_path = prefix + "system/recorder/status/single_file_duration";
            if (!ptr->GetProjectConfigValue(full_path, _file_limit_duration) || _file_limit_duration == 0) {
                _file_limit_duration = 3600; // Default: 1 hour
                ApWarn("status_recorder") << "Using default duration: " << _file_limit_duration;
            }

            // Load quota
            std::int32_t quota_mb;
            full_path = prefix + "system/recorder/status/quota";
            if (!ptr->GetProjectConfigValue(full_path, quota_mb) || quota_mb == 0) {
                quota_mb = 1024; // Default: 1GB
                ApWarn("status_recorder") << "Using default quota: " << quota_mb;
            }
            _quota_bytes = static_cast<std::uint64_t>(quota_mb) * 1024 * 1024;

            // Q3: 配额过小会导致记录无法持续，提前告警
            constexpr std::uint64_t MIN_QUOTA_BYTES = 10ULL * 1024 * 1024; // 10 MB
            if (_quota_bytes < MIN_QUOTA_BYTES) {
                ApError("status_recorder") << "Configured quota (" << quota_mb
                       << " MB) is unreasonably small (minimum recommended: 10 MB). "
                       << "Recording may not function correctly.";
            }

            // Load clean threshold
            std::int32_t clean_threshold_mb;
            full_path = prefix + "system/recorder/status/clean_threshold";
            if (!ptr->GetProjectConfigValue(full_path, clean_threshold_mb) || clean_threshold_mb == 0 ||
                clean_threshold_mb > quota_mb) {
                clean_threshold_mb = quota_mb * 80 / 100; // Default: 80% of quota
                ApWarn("status_recorder") << "Using default clean threshold: " << clean_threshold_mb;
            }
            _clean_threshold_bytes = static_cast<std::uint64_t>(clean_threshold_mb) * 1024 * 1024;

            // Load flush interval
            std::int32_t flush_interval_sec;
            full_path = prefix + "system/recorder/status/flush_interval";
            if (!ptr->GetProjectConfigValue(full_path, flush_interval_sec) || flush_interval_sec == 0) {
                flush_interval_sec = 2; // Default: 2 seconds
                ApWarn("status_recorder") << "Using default flush interval: " << flush_interval_sec;
            }
            _flush_interval_ms = static_cast<std::uint64_t>(flush_interval_sec) * 1000;

            ApInfo("status_recorder") << "RecorderSqlite3 config:";
            ApInfo("status_recorder") << "  location: " << _location;
            ApInfo("status_recorder") << "  name: " << _name;
            ApInfo("status_recorder") << "  duration: " << _file_limit_duration;
            ApInfo("status_recorder") << "  quota: " << quota_mb << " MB";
            ApInfo("status_recorder") << "  clean_threshold: " << clean_threshold_mb << " MB";
            ApInfo("status_recorder") << "  flush_interval: " << flush_interval_sec << " seconds";
        }

        void create_tables() {
            // Get database handle
            sqlite3* db = _db_manager->get_db();
            if (!db) {
                throw std::runtime_error("Failed to get database handle");
            }

            // Create tables for each data type
            // Note: We'll create tables with dynamic schema based on actual data
            // For now, we'll create them on first insert

            ApInfo("status_recorder") << "Tables will be created on first insert";
        }

        void create_table_if_not_exists(const std::string&                        table_name,
                                        const std::map<std::string, std::string>& fields) {
            // Validate table name to prevent SQL injection
            if (!is_valid_identifier(table_name)) {
                ApError("status_recorder") << "Invalid table name: " << table_name;
                return;
            }

            sqlite3* db = _db_manager->get_db();
            if (!db) {
                ApError("status_recorder") << "No database connection";
                return;
            }

            // Check if table exists using parameterized query
            std::string   check_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?";
            sqlite3_stmt* stmt = nullptr;
            int           rc = sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr);
            if (rc == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, table_name.c_str(), -1, SQLITE_STATIC);
                rc = sqlite3_step(stmt);
            }
            if (stmt) {
                sqlite3_finalize(stmt);
            }
            if (rc == SQLITE_ROW) {
                // Table exists
                return;
            }

            // Create table with all fields as TEXT type (with escaped identifiers)
            std::ostringstream create_sql;
            create_sql << "CREATE TABLE IF NOT EXISTS " << escape_identifier(table_name) << " (";
            create_sql << "record_id INTEGER PRIMARY KEY"; // Removed AUTOINCREMENT

            for (const auto& [field_name, field_value] : fields) {
                // Skip record_id and empty values (ignored array fields)
                if (field_name != "record_id" && !field_value.empty()) {
                    if (!is_valid_identifier(field_name)) {
                        ApWarn("status_recorder") << "Skipping invalid field name: " << field_name;
                        continue;
                    }
                    create_sql << ", " << escape_identifier(field_name) << " TEXT";
                }
            }
            create_sql << ")";

            char* err_msg = nullptr;
            rc = sqlite3_exec(db, create_sql.str().c_str(), nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to create table " << table_name << ": " << err_msg;
                sqlite3_free(err_msg);
            } else {
                ApInfo("status_recorder") << "Created table: " << table_name;
            }
        }

        void async_writer() {
            pthread_setname_np(pthread_self(), "async_writer");
            while (_running) {
                std::queue<RecordEntry> local;
                {
                    std::unique_lock<std::mutex> lock(_queue_mutex);
                    _cv.wait(lock, [this] { return !_record_queue.empty() || !_running; });
                    std::swap(local, _record_queue); // O(1)，持锁时间极短
                }

                if (!local.empty()) {
                    write_grouped(local);
                    _flush_cv.notify_all();
                }
            }

            // 退出后排空队列中剩余记录
            std::queue<RecordEntry> remaining;
            {
                std::lock_guard<std::mutex> lock(_queue_mutex);
                std::swap(remaining, _record_queue);
            }
            if (!remaining.empty()) {
                write_grouped(remaining);
            }
        }

        void flush_timer() {
            pthread_setname_np(pthread_self(), "flush_writer");
            ApInfo("status_recorder") << "Flush timer started, interval: " << _flush_interval_ms << "ms";

            while (_running) {
                // Wait for flush interval or until stopped
                std::unique_lock<std::mutex> lock(_flush_timer_mutex);
                _flush_timer_cv.wait_for(lock, std::chrono::milliseconds(_flush_interval_ms),
                                         [this] { return !_running; });

                if (!_running) {
                    break;
                }

                // Trigger flush
                flush();
            }

            ApInfo("status_recorder") << "Flush timer stopped";
        }

        // 将 local 队列中的 entry 按 table 分组后各提交一次事务
        void write_grouped(std::queue<RecordEntry>& q) {
            std::map<std::string, std::vector<std::map<std::string, std::string>>> grouped;
            while (!q.empty()) {
                RecordEntry& e = q.front();
                grouped[e.table_name].push_back(std::move(e.fields));
                q.pop();
            }
            for (auto& [tname, records] : grouped) {
                write_batch({tname, std::move(records)});
            }
        }

        void write_batch(const RecordBatch& batch) {
            sqlite3* db = _db_manager->get_db();
            if (!db) {
                ApError("status_recorder") << "No database connection";
                return;
            }

            // Create table if not exists
            if (!batch.records.empty()) {
                create_table_if_not_exists(batch.table_name, batch.records[0]);
            }

            // Begin transaction
            char* err_msg = nullptr;
            int   rc = sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to begin transaction: " << err_msg;
                sqlite3_free(err_msg);
                EC204::Instance().ec_add(EC204::_ERRORCODE_WRITE);
                return;
            }

            // Insert records
            for (const auto& record : batch.records) {
                insert_record(db, batch.table_name, record);
            }

            // Commit transaction
            rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to commit transaction: " << err_msg;
                sqlite3_free(err_msg);
                sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
                EC204::Instance().ec_add(EC204::_ERRORCODE_WRITE);
            } else {
                EC204::Instance().ec_remove(EC204::_ERRORCODE_WRITE);
            }
        }

        void insert_record(sqlite3* db, const std::string& table_name,
                           const std::map<std::string, std::string>& fields) {
            // Validate table name
            if (!is_valid_identifier(table_name)) {
                ApError("status_recorder") << "Invalid table name for insert: " << table_name;
                return;
            }

            // Build INSERT statement with escaped identifiers (skip empty fields - ignored arrays)
            std::ostringstream sql;
            sql << "INSERT INTO " << escape_identifier(table_name) << " (";

            std::ostringstream values;
            values << "VALUES (";

            bool                                             first = true;
            std::vector<std::pair<std::string, std::string>> valid_fields; // Store valid fields

            for (const auto& [field_name, field_value] : fields) {
                // Skip empty values (ignored array fields) and validate field names
                if (!field_value.empty() && is_valid_identifier(field_name)) {
                    if (!first) {
                        sql << ", ";
                        values << ", ";
                    }
                    sql << escape_identifier(field_name);
                    values << "?";
                    valid_fields.push_back({field_name, field_value});
                    first = false;
                }
            }

            sql << ") " << values.str() << ")";

            // Prepare statement
            sqlite3_stmt* stmt = nullptr;
            int           rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                ApError("status_recorder") << "Failed to prepare statement: " << sqlite3_errmsg(db);
                return;
            }

            // Bind values (only validated ones)
            int param_index = 1;
            for (const auto& [field_name, field_value] : valid_fields) {
                sqlite3_bind_text(stmt, param_index++, field_value.c_str(), -1, SQLITE_TRANSIENT);
            }

            // Execute
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                ApError("status_recorder") << "Failed to insert record: " << sqlite3_errmsg(db);
                EC204::Instance().ec_add(EC204::_ERRORCODE_WRITE);
            }

            sqlite3_finalize(stmt);
        }

        // Configuration
        std::string   _location;
        std::string   _name;
        std::uint64_t _file_limit_duration;
        std::uint64_t _quota_bytes;
        std::uint64_t _clean_threshold_bytes;
        std::uint64_t _flush_interval_ms;

        // Database manager
        std::unique_ptr<RotatingDbManager> _db_manager;

        // Async writer
        std::atomic<bool>        _running;
        std::thread              _writer_thread;
        std::queue<RecordEntry>  _record_queue;
        std::mutex               _queue_mutex;
        std::condition_variable  _cv;
        std::condition_variable  _flush_cv;

        // Flush timer
        std::thread             _flush_thread;
        std::mutex              _flush_timer_mutex;
        std::condition_variable _flush_timer_cv;
    };

} // namespace vehicle_recorder

#endif // RECORDER_SQLITE3_HPP
