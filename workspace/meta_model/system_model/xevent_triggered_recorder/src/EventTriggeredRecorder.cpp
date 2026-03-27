#include <fmt/format.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <set>
#include <sstream>

#include "EventTriggeredRecorder.h"
#include "ap_log/ap_log_interface.h"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"
#include "monitor/faulthandle_api.hpp"

namespace event_recorder {

    EventTriggeredRecorder::EventTriggeredRecorder()
        : current_sequence_(0), last_manual_trigger_signal_(0), rtfbag_pid_(-1) {
        // 加载配置
        load_config();

        // 验证配置
        if (config_.clean_threshold_mb > config_.quota_mb) {
            ApError("trigger_record") << "clean_threshold (" << config_.clean_threshold_mb
                                      << " MB) cannot be greater than quota (" << config_.quota_mb << " MB)";
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG);
            throw std::invalid_argument("Invalid config: clean_threshold > quota");
        }

        // 确保 eMMC 输出目录存在
        std::filesystem::create_directories(config_.output_path);
        if (!std::filesystem::exists(config_.output_path) || !std::filesystem::is_directory(config_.output_path)) {
            ApError("trigger_record") << "Output directory cannot be accessed: " << config_.output_path;
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_NO_PERMISSOIN);
            throw std::runtime_error("Output directory access failed");
        }

        // 确保 tmpfs 目录存在（应由启动脚本挂载，此处做兜底 create）
        if (!std::filesystem::exists(config_.tmpfs_path)) {
            ApWarn("trigger_record") << "tmpfs directory does not exist, creating: " << config_.tmpfs_path
                                     << " (expected to be mounted by startup script)";
            std::filesystem::create_directories(config_.tmpfs_path);
        }
        if (!std::filesystem::is_directory(config_.tmpfs_path)) {
            ApError("trigger_record") << "tmpfs path is not a directory: " << config_.tmpfs_path;
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_NO_PERMISSOIN);
            throw std::runtime_error("tmpfs path is not a directory");
        }

        // 清空 tmpfs 目录（确保从空目录开始录制，不受上次运行残留影响）
        // 注意：必须先收集所有路径，再统一删除，不可在 directory_iterator 迭代过程中直接调用
        // remove_all()——C++ 标准对迭代中修改目录结构的行为定义为 unspecified，
        // 在本平台（AArch64 + libc++）上会导致迭代器内部 DIR 缓冲区堆元数据损坏，
        // 进而引发后续 malloc 调用时崩溃（corrupted size vs. prev_size in fastbins）。
        try {
            std::vector<std::filesystem::path> to_delete;
            for (const auto& entry : std::filesystem::directory_iterator(config_.tmpfs_path)) {
                to_delete.push_back(entry.path());
            }
            for (const auto& path : to_delete) {
                std::filesystem::remove_all(path);
            }
            ApInfo("trigger_record") << "Cleared tmpfs directory: " << config_.tmpfs_path;
        } catch (const std::exception& e) {
            ApWarn("trigger_record") << "Failed to clear tmpfs directory " << config_.tmpfs_path << ": " << e.what();
        }

        // 文件名前缀指向 tmpfs（rtfbag 写入 tmpfs）
        file_prefix_ = (std::filesystem::path(config_.tmpfs_path) / FILE_PREFIX).string();

        // 扫描 eMMC 目录并恢复序列号
        scan_and_restore_sequence();

        // 重置故障码（初始化成功）
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_NO_PERMISSOIN);

        // 启动 rtfbag 进程（录入 tmpfs）
        launch_rtfbag();

        // 启动文件管理线程
        running_.store(true);
        ApInfo("trigger_record") << "Starting event triggered recorder (tmpfs dual-tier mode)...";
        management_thread_ = std::thread(&EventTriggeredRecorder::file_management_loop, this);
    }

    EventTriggeredRecorder::~EventTriggeredRecorder() {
        running_.store(false);
        ApInfo("trigger_record") << "Stopping event triggered recorder...";

        // 等待文件管理线程结束
        if (management_thread_.joinable()) {
            management_thread_.join();
        }

        // 终止 rtfbag 进程
        if (rtfbag_pid_ > 0) {
            ApInfo("trigger_record") << "Terminating rtfbag process (pid=" << rtfbag_pid_ << ")...";
            kill(rtfbag_pid_, SIGTERM);
            waitpid(rtfbag_pid_, nullptr, 0);
        }

        ApInfo("trigger_record") << "Recorder stopped";
    }

    void EventTriggeredRecorder::load_config() {
        auto        ptr = CustomStack::Instance();
        std::string full_path;

        // 加载 topics
        full_path = "system/recorder/trigger/topics";
        if (!ptr->GetProjectConfigArray<std::string>(full_path, config_.topics)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.topics.size() << " items";

        // 加载 eMMC output path
        full_path = "system/recorder/trigger/output";
        if (!ptr->GetProjectConfigValue<std::string>(full_path, config_.output_path) || config_.output_path.empty()) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.output_path;

        // 加载 tmpfs path
        full_path = "system/recorder/trigger/tmpfs_path";
        if (!ptr->GetProjectConfigValue<std::string>(full_path, config_.tmpfs_path) || config_.tmpfs_path.empty()) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.tmpfs_path;

        // 加载 buffer_size (KB)
        full_path = "system/recorder/trigger/buffer_size";
        if (!ptr->GetProjectConfigValue<size_t>(full_path, config_.buffer_size_kb)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.buffer_size_kb << " KB";

        // 加载 duration (seconds)
        full_path = "system/recorder/trigger/duration";
        if (!ptr->GetProjectConfigValue<size_t>(full_path, config_.duration_sec)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.duration_sec << " seconds";

        // 加载 eMMC quota (MB)
        full_path = "system/recorder/trigger/quota";
        if (!ptr->GetProjectConfigValue<size_t>(full_path, config_.quota_mb)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.quota_mb << " MB";

        // 加载 clean_threshold (MB)
        full_path = "system/recorder/trigger/clean_threshold";
        if (!ptr->GetProjectConfigValue<size_t>(full_path, config_.clean_threshold_mb)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.clean_threshold_mb << " MB";

        // 加载 tmpfs_size (MB)
        full_path = "system/recorder/trigger/tmpfs_size";
        if (!ptr->GetProjectConfigValue<size_t>(full_path, config_.tmpfs_size_mb)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.tmpfs_size_mb << " MB";

        // 加载 fault_codes
        full_path = "system/recorder/trigger/fault/fault_codes";
        if (!ptr->GetProjectConfigArray<std::string>(full_path, config_.fault_codes)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_CONFIG, full_path);
            auto error = fmt::format("read project config {} failed", full_path);
            ApError("trigger_record") << error;
            throw std::runtime_error(error);
        }
        ApInfo("trigger_record") << full_path << ": " << config_.fault_codes.size() << " items";

        // 配置加载成功，重置故障码
        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_CONFIG);
        ApInfo("trigger_record") << "All configuration loaded successfully";
    }

    void EventTriggeredRecorder::scan_and_restore_sequence() {
        // 清空缓存
        tmpfs_files_.clear();
        emmc_files_.clear();

        // 启动时单次遍历 eMMC：同时完成非规范清理和序列号恢复
        // 合法条目仅有两类（普通文件）：
        //   1. trigger_NNNNNNNN_TIMESTAMP.bag          （序列号格式 bag，收录进 emmc_files_）
        //   2. trigger_NNNNNNNN_TIMESTAMP.bag.meta.json（元数据，保留）
        // .tmp（两阶段 copy 残留）及其余任何条目均立即删除
        // 注：tmpfs 已在构造函数中清空，不存在 tmpfs 遗留文件
        size_t max_sequence = 0;
        bool   found_files = false;

        try {
            // 两阶段处理：先完整扫描（不修改目录），再统一删除非规范条目。
            // 原因同 tmpfs 清空：在 directory_iterator 迭代中调用 remove/remove_all 为
            // C++ unspecified 行为，在本平台会导致堆元数据损坏。
            struct PendingDelete {
                std::filesystem::path path;
                bool                  is_dir;
            };
            std::vector<PendingDelete> to_delete;

            for (const auto& entry : std::filesystem::directory_iterator(config_.output_path)) {
                const std::string filename = entry.path().filename().string();
                const std::string filepath = entry.path().string();

                if (entry.is_regular_file()) {
                    auto seq_opt = parse_sequence_from_filename(filename);
                    if (seq_opt.has_value()) {
                        // 合法序列号 bag：收录并跟踪最大序列号
                        size_t seq = seq_opt.value();
                        emmc_files_[seq] = filepath;
                        found_files = true;
                        max_sequence = std::max(max_sequence, seq);
                        continue;
                    }
                    if (filename.size() > 10 && filename.substr(filename.size() - 10) == ".meta.json") {
                        continue; // 合法元数据：保留
                    }
                }

                // 不合规的普通文件、目录或其他类型：标记待删
                to_delete.push_back({entry.path(), entry.is_directory()});
            }

            // 扫描完成后统一执行删除
            for (const auto& item : to_delete) {
                try {
                    if (item.is_dir) {
                        auto count = std::filesystem::remove_all(item.path);
                        ApWarn("trigger_record")
                            << "Startup cleanup: deleted non-conforming directory from eMMC: " << item.path.string()
                            << " (" << count << " entries)";
                    } else {
                        std::filesystem::remove(item.path);
                        ApWarn("trigger_record")
                            << "Startup cleanup: deleted non-conforming entry from eMMC: " << item.path.string();
                    }
                } catch (const std::exception& e) {
                    ApError("trigger_record") << "Startup cleanup: failed to delete " << item.path.string() << ": "
                                              << e.what();
                }
            }

            current_sequence_ = compute_next_sequence_after_wrap(emmc_files_);
            ApInfo("trigger_record") << "Scanned eMMC directory: found " << emmc_files_.size() << " files"
                                     << ", starting from sequence " << current_sequence_;

        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to scan eMMC directory: " << e.what();
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_NO_PERMISSOIN);
            current_sequence_ = 0;
        }
    }

    std::optional<size_t> EventTriggeredRecorder::parse_sequence_from_filename(const std::string& filename) {
        // 文件名格式: trigger_NNNNNNNN_YYYYMMDDHHMMSS.sss.bag
        const std::string prefix = std::string(FILE_PREFIX) + "_";

        if (filename.find(prefix) != 0) {
            return std::nullopt;
        }

        size_t start_pos = prefix.length();
        if (filename.length() < start_pos + 8) {
            return std::nullopt;
        }

        std::string sequence_str = filename.substr(start_pos, 8);
        for (char c : sequence_str) {
            if (!std::isdigit(c)) {
                return std::nullopt;
            }
        }

        try {
            size_t sequence = std::stoull(sequence_str);
            return sequence;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

    size_t EventTriggeredRecorder::get_next_sequence() {
        size_t seq = current_sequence_++;
        if (current_sequence_ > MAX_SEQUENCE) {
            ApWarn("trigger_record") << "Sequence number wrapped around to 0 (MAX_SEQUENCE=" << MAX_SEQUENCE << ")";
            current_sequence_ = 0;
        }
        return seq;
    }

    std::string EventTriggeredRecorder::generate_sequence_filename(size_t sequence, double timestamp) {
        // 格式: trigger_NNNNNNNN_YYYYMMDDHHMMSS.sss.bag
        // 文件生成在 tmpfs（不是 eMMC）
        auto    seconds = static_cast<time_t>(timestamp);
        auto    milliseconds = static_cast<int>((timestamp - seconds) * 1000);
        std::tm timeinfo;
        localtime_r(&seconds, &timeinfo);

        std::string datetime_str =
            fmt::format("{:04d}{:02d}{:02d}{:02d}{:02d}{:02d}.{:03d}", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
                        timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, milliseconds);

        auto name = fmt::format("{}_{:08d}_{}.bag", FILE_PREFIX, sequence, datetime_str);
        return (std::filesystem::path(config_.tmpfs_path) / name).string();
    }

    // 从已按序号升序排列的 emmc_files_ 计算下一个可用序号。
    // 若检测到回绕（相邻序号最大 gap > MAX_SEQUENCE/2），认为低序号段是新文件（post-wrap），
    // 返回低序号段最大值 + 1；否则返回全局最大序号 + 1。
    // 空文件集返回 0（seq=0 是合法起始序号）。
    size_t EventTriggeredRecorder::compute_next_sequence_after_wrap(
        const std::map<size_t, std::string>& files)
    {
        if (files.empty()) {
            return 0;
        }

        // 查找相邻序号之间的最大 gap 及其位置（gap 右侧即第一个"高序号旧文件"）
        size_t max_gap   = 0;
        size_t split_pos = 0; // 第一个高序号元素在 map 中的偏移量（0-based）
        {
            size_t i        = 0;
            size_t prev_seq = 0;
            bool   first    = true;
            for (const auto& [seq, _] : files) {
                if (!first) {
                    size_t gap = seq - prev_seq;
                    if (gap > max_gap) {
                        max_gap   = gap;
                        split_pos = i;
                    }
                }
                prev_seq = seq;
                first    = false;
                ++i;
            }
        }

        if (max_gap > MAX_SEQUENCE / 2) {
            // 回绕：低序号段 [begin, split_pos) 是新文件，取其最大值（即 split_pos-1 处）
            auto it = files.begin();
            std::advance(it, split_pos - 1); // 指向低序号段最后一项
            size_t max_new = it->first;
            size_t next    = (max_new < MAX_SEQUENCE) ? max_new + 1 : 0;
            ApInfo("trigger_record") << "Startup: sequence wrap detected (max_gap=" << max_gap
                                     << "), continuing from seq=" << next;
            return next;
        }

        // 无回绕：max_sequence + 1
        size_t max_seq = files.rbegin()->first;
        return (max_seq < MAX_SEQUENCE) ? max_seq + 1 : 0;
    }

    bool EventTriggeredRecorder::is_orphaned_active_file(const std::string& filename) {
        const std::string active_suffix = ".bag.active";
        if (filename.size() < active_suffix.size()) {
            return false;
        }
        if (filename.rfind(active_suffix) != filename.size() - active_suffix.size()) {
            return false;
        }

        // rtfbag 未运行：文件必然是孤儿
        if (!is_rtfbag_running()) {
            return true;
        }

        // rtfbag 运行中：通过 /proc/PID/fd 检查该文件是否正被 rtfbag 打开
        // 仅当 rtfbag 打开了此文件，才说明它正在写入；否则为上一个录制周期的遗留孤儿
        const std::string full_path = (std::filesystem::path(config_.tmpfs_path) / filename).string();
        const std::string fd_dir = "/proc/" + std::to_string(rtfbag_pid_) + "/fd";

        try {
            for (const auto& fd_entry : std::filesystem::directory_iterator(fd_dir)) {
                try {
                    auto target = std::filesystem::read_symlink(fd_entry.path());
                    if (target.string() == full_path) {
                        return false; // 文件正被 rtfbag 打开，不是孤儿
                    }
                } catch (...) {
                    // 读取符号链接失败（fd 可能刚被关闭），继续遍历
                }
            }
        } catch (...) {
            // 无法访问 /proc/PID/fd（rtfbag 刚退出？），保守处理：不认定为孤儿
            return false;
        }

        // rtfbag 运行中但未打开此文件：孤儿文件
        return true;
    }

    bool EventTriggeredRecorder::should_process_temp_file(const std::string& filename, const std::string& extension) {
        if (extension != ".bag") {
            return false;
        }
        if (parse_sequence_from_filename(filename).has_value()) {
            return false;
        }
        std::string expected_prefix = std::filesystem::path(file_prefix_).filename().string();
        if (filename.find(expected_prefix) != 0) {
            return false;
        }
        return true;
    }

    void EventTriggeredRecorder::scan_and_process_tmpfs() {
        // 单次遍历 tmpfs：同时完成非规范清理和临时文件重命名
        // 合法条目：普通文件 AND 以本模块前缀开头（.active、临时格式、序列号格式）
        const std::string our_prefix = std::filesystem::path(file_prefix_).filename().string();

        try {
            for (const auto& entry : std::filesystem::directory_iterator(config_.tmpfs_path)) {
                const std::string fname = entry.path().filename().string();
                const std::string full_path = entry.path().string();

                // ── 目录：递归删除 ────────────────────────────────────────────
                if (entry.is_directory()) {
                    try {
                        auto count = std::filesystem::remove_all(entry.path());
                        ApWarn("trigger_record") << "Deleted non-conforming directory from tmpfs: " << full_path << " ("
                                                 << count << " entries)";
                    } catch (const std::exception& e) {
                        ApError("trigger_record")
                            << "Failed to delete directory from tmpfs: " << full_path << ": " << e.what();
                        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                    }
                    continue;
                }

                // ── 非普通文件 或 不以本模块前缀开头：立即删除 ───────────────
                if (!entry.is_regular_file() || fname.find(our_prefix) != 0) {
                    try {
                        std::filesystem::remove(entry.path());
                        ApWarn("trigger_record") << "Deleted non-conforming entry from tmpfs: " << full_path;
                    } catch (const std::exception& e) {
                        ApError("trigger_record")
                            << "Failed to delete non-conforming entry from tmpfs: " << full_path << ": " << e.what();
                        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                    }
                    continue;
                }

                // ── 以下均为普通文件且以 our_prefix 开头 ─────────────────────
                const std::string extension = entry.path().extension().string();

                // 孤儿 .bag.active 文件（rtfbag 已退出但文件未清理）：删除
                if (is_orphaned_active_file(fname)) {
                    try {
                        std::filesystem::remove(full_path);
                        ApInfo("trigger_record") << "Removed orphaned .bag.active file from tmpfs: " << fname;
                    } catch (const std::exception& e) {
                        ApError("trigger_record") << "Failed to remove orphaned file " << fname << ": " << e.what();
                        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                    }
                    continue;
                }

                // 临时文件（rtfbag 已写完、尚未分配序列号）：重命名
                if (should_process_temp_file(fname, extension)) {
                    double timestamp = extract_timestamp_from_temp_file(full_path);
                    size_t sequence = get_next_sequence();
                    on_file_boundary(full_path, sequence, timestamp);
                    continue;
                }

                // 其余（序列号格式文件、.bag.active 正在写入）：保留，无需处理
            }

        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to scan tmpfs: " << e.what();
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_NO_PERMISSOIN);
        }
    }

    void EventTriggeredRecorder::on_file_boundary(const std::string& temp_file, size_t sequence, double timestamp) {
        try {
            FileRecord new_file = register_tmpfs_file(temp_file, sequence, timestamp);

            // 窄锁：原子性地 snapshot + clear trigger 状态（外部线程持续写入这两个变量）
            std::vector<TriggerInfo> triggers;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (trigger_pending_) {
                    triggers = std::move(pending_triggers_);
                    trigger_pending_ = false;
                }
            }

            if (!triggers.empty()) {
                perform_trigger_copy(new_file, std::move(triggers));
            } else {
                advance_prev_no_trigger();
                ApInfo("trigger_record") << "File detected in tmpfs, now prev: " << new_file.filename << " (sequence=" << sequence << ")";
            }
            prev_file_ = std::move(new_file);
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to process file boundary for " << temp_file << ": " << e.what();
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_WRITE_FALED);
        }
    }

    std::string EventTriggeredRecorder::emmc_path_for(const std::string& tmpfs_filename) const {
        return (std::filesystem::path(config_.output_path) / std::filesystem::path(tmpfs_filename).filename()).string();
    }

    EventTriggeredRecorder::FileRecord EventTriggeredRecorder::register_tmpfs_file(const std::string& temp_file,
                                                                                   size_t sequence, double timestamp) {
        std::string new_filename = generate_sequence_filename(sequence, timestamp);

        if (std::filesystem::exists(new_filename)) {
            throw std::runtime_error("Target already exists in tmpfs, skipping rename: " + new_filename);
        }

        std::filesystem::rename(temp_file, new_filename); // 失败时抛出，由调用方 catch
        ApInfo("trigger_record") << "Renamed in tmpfs: " << temp_file << " -> " << new_filename;
        tmpfs_files_[sequence] = new_filename;

        FileRecord f;
        f.filename = new_filename;
        f.sequence = sequence;
        f.timestamp = timestamp;
        f.detect_time = std::chrono::steady_clock::now();
        return f;
    }

    void EventTriggeredRecorder::perform_trigger_copy(const FileRecord& new_file, std::vector<TriggerInfo> triggers) {
        std::vector<CopyTask> tasks;
        tasks.push_back(CopyTask{new_file.sequence, new_file.filename, emmc_path_for(new_file.filename)});

        std::vector<std::string> context_emmc_paths; // 已在 eMMC 的上下文文件（方案B：不重复 copy，但写入 meta.json）
        if (prev_file_.has_value()) {
            if (!prev_file_copied_) {
                tasks.push_back(
                    CopyTask{prev_file_->sequence, prev_file_->filename, emmc_path_for(prev_file_->filename)});
            } else {
                // 方案B：N-1 已在 eMMC，不重复 copy，但仍写入 meta.json 的 files 列表作为上下文
                context_emmc_paths.push_back(emmc_path_for(prev_file_->filename));
            }
        }

        ApInfo("trigger_record") << "Trigger: N=" << new_file.filename
                                 << " N-1=" << (prev_file_.has_value() ? prev_file_->filename : "none");
        ApInfo("trigger_record") << "Copying " << tasks.size() << " file(s) to eMMC, referencing "
                                 << context_emmc_paths.size() << " already-copied context file(s)";
        perform_copy_tasks(tasks, triggers, context_emmc_paths);

        prev_file_copied_ = true;
    }

    void EventTriggeredRecorder::advance_prev_no_trigger() {
        if (prev_file_.has_value() && !prev_file_copied_) {
            auto it = tmpfs_files_.find(prev_file_->sequence);
            if (it != tmpfs_files_.end()) {
                std::string path_to_remove = it->second;
                tmpfs_files_.erase(it);
                try {
                    std::filesystem::remove(path_to_remove);
                    ApInfo("trigger_record") << "Evicted non-triggered file from tmpfs: " << path_to_remove;
                } catch (const std::exception& e) {
                    ApError("trigger_record") << "Failed to evict tmpfs file: " << path_to_remove << ": " << e.what();
                    FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                }
            }
        } else if (prev_file_.has_value() && prev_file_copied_) {
            ApInfo("trigger_record") << "Prev file copy already submitted, skipping evict: " << prev_file_->filename;
        }
        prev_file_copied_ = false;
    }

    double EventTriggeredRecorder::extract_timestamp_from_temp_file(const std::string& temp_file) {
        try {
            auto ftime = std::filesystem::last_write_time(temp_file);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            return std::chrono::duration<double>(sctp.time_since_epoch()).count();
        } catch (const std::exception& e) {
            ApWarn("trigger_record") << "Failed to extract timestamp from file " << temp_file << ": " << e.what()
                                     << ", using current time";
            return get_current_timestamp_sec();
        }
    }

    void EventTriggeredRecorder::update_fault_data(const ara::adsfi::FaultData& fault_data) {
        if (!running_.load()) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        std::vector<std::string> triggered_codes;
        if (should_trigger_by_fault(fault_data, triggered_codes)) {
            for (const auto& code : triggered_codes) {
                TriggerInfo trigger;
                trigger.type = "fault";
                trigger.fault_code = code;
                trigger.timestamp = get_current_timestamp_iso();
                trigger.timestamp_sec = get_current_timestamp_sec();

                trigger_pending_ = true;
                pending_triggers_.push_back(trigger);
                ApInfo("trigger_record") << "Fault trigger recorded: code=" << code
                                         << ", will copy when current file completes";
            }
        }
    }

    void EventTriggeredRecorder::update_manual_trigger(std::int32_t trigger_signal) {
        if (!running_.load()) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        if (check_manual_trigger_edge(trigger_signal)) {
            TriggerInfo trigger;
            trigger.type = "manual";
            trigger.timestamp = get_current_timestamp_iso();
            trigger.timestamp_sec = get_current_timestamp_sec();

            trigger_pending_ = true;
            pending_triggers_.push_back(trigger);
            ApInfo("trigger_record") << "Manual trigger recorded (rising edge), will copy when current file completes";
        }

        last_manual_trigger_signal_ = trigger_signal;
    }

    bool EventTriggeredRecorder::should_trigger_by_fault(const ara::adsfi::FaultData& fault_data,
                                                         std::vector<std::string>&    triggered_codes) {
        auto now = std::chrono::steady_clock::now();
        bool triggered = false;

        for (const auto& fault_info : fault_data.fault_info) {
            const std::string& code = fault_info.code;

            auto it = std::find(config_.fault_codes.begin(), config_.fault_codes.end(), code);
            if (it == config_.fault_codes.end()) {
                continue;
            }

            auto trigger_it = fault_trigger_times_.find(code);
            if (trigger_it != fault_trigger_times_.end()) {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - trigger_it->second).count();
                if (elapsed < config_.duration_sec) {
                    continue;
                }
            }

            ApInfo("trigger_record") << "Fault code " << code << " triggered";
            fault_trigger_times_[code] = now;
            triggered_codes.push_back(code);
            triggered = true;
        }

        return triggered;
    }

    bool EventTriggeredRecorder::check_manual_trigger_edge(std::int32_t signal) {
        bool was_inactive = (last_manual_trigger_signal_ == 0);
        bool is_active = (signal != 0);
        return is_active && was_inactive;
    }

    void EventTriggeredRecorder::file_management_loop() {
        pthread_setname_np(pthread_self(), "file_management");
        ApInfo("trigger_record") << "File management loop started";

        size_t iteration = 0;

        while (running_.load()) {
            // scan_and_process_tmpfs / clean_emmc_if_needed 在函数内部对共享数据做窄锁
            // is_rtfbag_running / restart_rtfbag 仅访问 rtfbag_pid_（management_thread_ 独占），无需锁

            // 2a. 检查 rtfbag 进程是否还在运行（每秒检查）
            if (!is_rtfbag_running()) {
                ApWarn("trigger_record") << "rtfbag process is not running, restarting...";
                try {
                    restart_rtfbag();
                } catch (const std::exception& e) {
                    ApError("trigger_record") << "Failed to restart rtfbag process: " << e.what();
                }
            }

            // 2b. 检测并重命名新文件（从 tmpfs 扫描）
            if (iteration % FILE_DETECT_INTERVAL == 0) {
                scan_and_process_tmpfs();
            }

            // 2c. 检查 eMMC 磁盘空间并清理（降低扫描频率）
            if (iteration % DISK_CLEANUP_INTERVAL == 0) {
                clean_emmc_if_needed();
            }

            ++iteration;
            std::this_thread::sleep_for(std::chrono::seconds(MANAGEMENT_LOOP_INTERVAL));
        }

        ApInfo("trigger_record") << "File management loop stopped";
    }

    void EventTriggeredRecorder::launch_rtfbag() {
        std::string command = build_rtfbag_command(FILE_PREFIX);

        ApInfo("trigger_record") << "Launching rtfbag with command: " << command;

        pid_t pid = fork();
        if (pid == 0) {
            execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
            _exit(1);
        } else if (pid > 0) {
            rtfbag_pid_ = pid;
            ApInfo("trigger_record") << "Started rtfbag process in continuous mode (pid=" << pid << ")";
            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_WRITE_FALED);
        } else {
            ApError("trigger_record") << "Failed to fork rtfbag process";
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_WRITE_FALED);
            throw std::runtime_error("Failed to start rtfbag process");
        }
    }

    void EventTriggeredRecorder::restart_rtfbag() {
        ApWarn("trigger_record") << "Restarting rtfbag process...";

        if (rtfbag_pid_ > 0) {
            kill(rtfbag_pid_, SIGTERM);
            waitpid(rtfbag_pid_, nullptr, 0);
            rtfbag_pid_ = -1;
        }

        launch_rtfbag();
    }

    bool EventTriggeredRecorder::is_rtfbag_running() {
        if (rtfbag_pid_ <= 0) {
            return false;
        }

        int   status;
        pid_t result = waitpid(rtfbag_pid_, &status, WNOHANG);

        if (result == 0) {
            return true;
        } else if (result > 0) {
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                ApError("trigger_record")
                    << "rtfbag process exited with code " << exit_code << " (pid=" << rtfbag_pid_ << ")";
            } else if (WIFSIGNALED(status)) {
                ApError("trigger_record")
                    << "rtfbag process terminated by signal " << WTERMSIG(status) << " (pid=" << rtfbag_pid_ << ")";
            }
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_WRITE_FALED);
            rtfbag_pid_ = -1;
            return false;
        } else {
            ApError("trigger_record") << "Failed to check rtfbag process status (pid=" << rtfbag_pid_ << ")";
            return false;
        }
    }

    std::string EventTriggeredRecorder::build_rtfbag_command(const std::string& prefix) {
        std::ostringstream cmd;
        cmd << "rtfbag record";

        for (const auto& topic : config_.topics) {
            cmd << " " << topic;
        }

        cmd << " -o " << prefix;
        cmd << " --split";
        cmd << " --duration " << config_.duration_sec;
        cmd << " -b " << config_.buffer_size_kb;
        cmd << " -p " << config_.tmpfs_path; // 写入 tmpfs，而非 eMMC

        return cmd.str();
    }

    bool EventTriggeredRecorder::delete_file_with_metadata(const std::string& filepath, size_t sequence) {
        try {
            size_t file_size_mb = std::filesystem::file_size(filepath) / (1024 * 1024);
            std::filesystem::remove(filepath);
            ApInfo("trigger_record") << "Deleted old eMMC file: " << filepath << " (sequence=" << sequence
                                     << ", size=" << file_size_mb << " MB)";
            emmc_files_.erase(sequence);

            std::string metadata_file = filepath + ".meta.json";
            if (std::filesystem::exists(metadata_file)) {
                std::filesystem::remove(metadata_file);
                ApInfo("trigger_record") << "Deleted metadata file: " << metadata_file;
            }

            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_DELETE_FAILED);
            return true;

        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to delete file " << filepath << ": " << e.what();
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
            return false;
        }
    }

    void EventTriggeredRecorder::clean_emmc_if_needed() {
        // ── 始终执行：立即清理 eMMC 上不满足命名规范的条目 ──────────────────
        // eMMC 上合法条目仅有三类（均为普通文件）：
        //   1. trigger_NNNNNNNN_TIMESTAMP.bag          （序列号格式 bag）
        //   2. trigger_NNNNNNNN_TIMESTAMP.bag.meta.json（元数据）
        //   3. trigger_NNNNNNNN_TIMESTAMP.bag.tmp      （copy 工作线程写入中的临时文件，跳过）
        // 其余任何条目（不合规普通文件、目录、符号链接等）均立即删除
        // 注：.bag.active 文件仅存在于 tmpfs，不会出现在 eMMC
        try {
            for (const auto& entry : std::filesystem::directory_iterator(config_.output_path)) {
                const std::string filename = entry.path().filename().string();
                const std::string filepath = entry.path().string();

                // 普通文件：检查是否属于三类合法格式
                if (entry.is_regular_file()) {
                    if (parse_sequence_from_filename(filename).has_value())
                        continue;
                    if (filename.size() > 10 && filename.substr(filename.size() - 10) == ".meta.json") {
                        // 孤立的 meta（对应 .bag 不存在）：立即删除
                        std::string bag_file = filepath.substr(0, filepath.size() - 10);
                        if (!std::filesystem::exists(bag_file)) {
                            try {
                                std::filesystem::remove(filepath);
                                ApInfo("trigger_record") << "Deleted orphaned metadata file: " << filepath;
                            } catch (const std::exception& e) {
                                ApError("trigger_record")
                                    << "Failed to delete orphaned metadata file " << filepath << ": " << e.what();
                                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                            }
                        }
                        continue;
                    }
                    if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".tmp")
                        continue;
                }

                // 不合规的普通文件、目录或其他类型：立即删除
                try {
                    if (entry.is_directory()) {
                        auto count = std::filesystem::remove_all(entry.path());
                        ApWarn("trigger_record") << "Deleted non-conforming directory from eMMC: " << filepath << " ("
                                                 << count << " entries)";
                    } else {
                        size_t size_mb = entry.is_regular_file() ? (entry.file_size() / (1024 * 1024)) : 0;
                        std::filesystem::remove(entry.path());
                        ApWarn("trigger_record") << "Deleted non-conforming entry from eMMC: " << filepath
                                                 << (size_mb > 0 ? " (size=" + std::to_string(size_mb) + " MB)" : "");
                    }
                } catch (const std::exception& e) {
                    ApError("trigger_record")
                        << "Failed to delete non-conforming entry " << filepath << ": " << e.what();
                    FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                }
            }
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to scan eMMC for non-conforming entries: " << e.what();
        }

        // ── 配额检查：仅在超出阈值时才执行进一步清理 ────────────────────────
        // 只计算 eMMC 的大小（tmpfs 文件不计入 eMMC 配额）
        size_t total_size_mb = calculate_directory_size();

        if (total_size_mb <= config_.clean_threshold_mb) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_OVER_QUOTA);
            return;
        }

        ApInfo("trigger_record") << "eMMC directory size (" << total_size_mb << " MB) exceeds threshold ("
                                 << config_.clean_threshold_mb << " MB), cleaning old files...";
        FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_OVER_QUOTA);

        // ── 按触发事件组删除（解析 meta.json，整组删除，跨引用 bag 保护）─────────────────────
        // 事件组：以 trigger_seq 命名的 meta.json + 其 files 字段列出的所有 bag 路径
        // 方案B 会产生跨引用：N.bag 同时出现在 meta_N.json 和 meta_{N+1}.json 的 files 列表中
        // 跨引用的 bag 延迟删除，直到引用它的最后一个 meta.json 被删除

        struct EventGroup {
            std::string              meta_path;
            std::vector<std::string> bag_paths; // meta.json 中 files 字段的 eMMC 路径列表
        };

        // trigger_seq → EventGroup（map 按 key 升序 = 从旧到新）
        std::map<size_t, EventGroup> event_groups;
        // bag_emmc_path → 所有引用它的 trigger_seq 集合（用于跨引用检查）
        std::map<std::string, std::set<size_t>> bag_refs;

        // 扫描 eMMC 上所有 .meta.json，解析事件组
        try {
            for (const auto& entry : std::filesystem::directory_iterator(config_.output_path)) {
                if (!entry.is_regular_file()) {
                    continue;
                }
                const std::string filename = entry.path().filename().string();
                if (filename.size() <= 10 || filename.substr(filename.size() - 10) != ".meta.json") {
                    continue;
                }

                // 从 .meta.json 文件名推导 trigger_seq（去掉 .meta.json 后缀 → 得到 .bag 文件名）
                std::string bag_name = filename.substr(0, filename.size() - 10);
                auto        seq_opt = parse_sequence_from_filename(bag_name);
                if (!seq_opt.has_value()) {
                    continue; // 非法 meta.json，Phase 1 应已清理，跳过
                }

                size_t trigger_seq = seq_opt.value();

                try {
                    std::ifstream ifs(entry.path().string());
                    if (!ifs.is_open()) {
                        ApWarn("trigger_record") << "Cannot open meta.json: " << entry.path().string();
                        continue;
                    }
                    nlohmann::json j;
                    ifs >> j;

                    EventGroup group;
                    group.meta_path = entry.path().string();
                    if (j.contains("files") && j["files"].is_array()) {
                        for (const auto& path_val : j["files"]) {
                            if (path_val.is_string()) {
                                group.bag_paths.push_back(path_val.get<std::string>());
                            }
                        }
                    }
                    event_groups[trigger_seq] = std::move(group);
                } catch (const std::exception& e) {
                    ApWarn("trigger_record")
                        << "Failed to parse meta.json " << entry.path().string() << ": " << e.what() << ", skipping";
                }
            }
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to scan eMMC for meta.json: " << e.what();
        }

        // 构建 bag_refs：bag_path → 引用它的所有 trigger_seq 集合
        for (const auto& [seq, group] : event_groups) {
            for (const auto& bag_path : group.bag_paths) {
                bag_refs[bag_path].insert(seq);
            }
        }

        // 按 trigger_seq 从旧到新删除事件组
        // 回绕检测：若存在回绕，旋转使高序号（旧文件）排在前面，保证 FIFO 删除顺序
        std::vector<std::pair<size_t, EventGroup*>> ordered_groups;
        ordered_groups.reserve(event_groups.size());
        for (auto& [seq, grp] : event_groups) {
            ordered_groups.emplace_back(seq, &grp);
        }
        {
            size_t max_gap = 0;
            size_t split   = ordered_groups.size();
            for (size_t k = 1; k < ordered_groups.size(); ++k) {
                size_t gap = ordered_groups[k].first - ordered_groups[k - 1].first;
                if (gap > max_gap) {
                    max_gap = gap;
                    split   = k;
                }
            }
            if (max_gap > MAX_SEQUENCE / 2) {
                std::rotate(ordered_groups.begin(), ordered_groups.begin() + split, ordered_groups.end());
                ApInfo("trigger_record") << "clean: sequence wrap detected (max_gap=" << max_gap
                                         << "), rotated deletion order (old high-seq first)";
            }
        }

        for (auto& [trigger_seq, group_ptr] : ordered_groups) {
            auto& group = *group_ptr;
            // 处理本组内所有 bag：无引用的立即删除，有跨引用的保留（等待其他 meta.json 也被删时再删）
            for (const auto& bag_path : group.bag_paths) {
                auto& refs = bag_refs[bag_path];
                refs.erase(trigger_seq); // 移除本组对该 bag 的引用
                if (!refs.empty()) {
                    // 还被其他事件组引用（跨引用）：保留 bag，仅删 meta.json
                    ApInfo("trigger_record")
                        << "Bag cross-referenced by " << refs.size() << " other event(s), keeping: " << bag_path;
                    continue;
                }

                // 已无引用：删除 bag
                auto seq_opt = parse_sequence_from_filename(std::filesystem::path(bag_path).filename().string());
                if (!seq_opt.has_value()) {
                    continue;
                }
                size_t bag_seq = seq_opt.value();

                if (!std::filesystem::exists(bag_path)) {
                    emmc_files_.erase(bag_seq);
                    bag_refs.erase(bag_path);
                    continue; // 文件已被其他途径删除
                }

                try {
                    size_t file_size_mb = std::filesystem::file_size(bag_path) / (1024 * 1024);
                    std::filesystem::remove(bag_path);
                    emmc_files_.erase(bag_seq);
                    bag_refs.erase(bag_path);
                    ApInfo("trigger_record")
                        << "Deleted bag: " << bag_path << " (seq=" << bag_seq << ", size=" << file_size_mb << " MB)";
                    FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_DELETE_FAILED);
                } catch (const std::exception& e) {
                    ApError("trigger_record") << "Failed to delete bag " << bag_path << ": " << e.what();
                    FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
                }
            }

            // 删除 meta.json
            try {
                std::filesystem::remove(group.meta_path);
                ApInfo("trigger_record") << "Deleted event meta: " << group.meta_path;
                FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_DELETE_FAILED);
            } catch (const std::exception& e) {
                ApError("trigger_record") << "Failed to delete meta.json " << group.meta_path << ": " << e.what();
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_DELETE_FAILED);
            }

            total_size_mb = calculate_directory_size();
            if (total_size_mb <= config_.clean_threshold_mb) {
                break;
            }
        }

        // ── 清理孤立 bag（不被任何 meta.json 引用）──────────────────────────────────────────
        // 正常情况下不应出现，可能由 meta.json 解析失败或外部干预导致
        if (total_size_mb > config_.clean_threshold_mb) {
            // 构建带回绕旋转的孤立 bag 有序列表（与事件组删除逻辑一致，保证 FIFO 顺序）
            std::vector<std::pair<size_t, std::string>> orphan_list(emmc_files_.begin(), emmc_files_.end());
            {
                size_t max_gap = 0;
                size_t split   = orphan_list.size();
                for (size_t k = 1; k < orphan_list.size(); ++k) {
                    size_t gap = orphan_list[k].first - orphan_list[k - 1].first;
                    if (gap > max_gap) {
                        max_gap = gap;
                        split   = k;
                    }
                }
                if (max_gap > MAX_SEQUENCE / 2) {
                    std::rotate(orphan_list.begin(), orphan_list.begin() + split, orphan_list.end());
                }
            }
            for (const auto& [seq, filepath] : orphan_list) {
                if (bag_refs.count(filepath) > 0) {
                    continue; // 仍被引用，跳过
                }
                ApInfo("trigger_record") << "Deleting orphan bag (no meta.json reference): " << filepath;
                if (delete_file_with_metadata(filepath, seq)) {
                    total_size_mb = calculate_directory_size();
                    if (total_size_mb <= config_.clean_threshold_mb) {
                        break;
                    }
                }
            }
        }

        ApInfo("trigger_record") << "Cleanup completed, current eMMC size: " << calculate_directory_size() << " MB";
    }

    size_t EventTriggeredRecorder::calculate_directory_size() {
        // 只计算 eMMC 目录大小（tmpfs 文件不计入配额）
        size_t total_size = 0;
        try {
            for (const auto& entry : std::filesystem::directory_iterator(config_.output_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".bag") {
                    total_size += entry.file_size();
                }
            }
            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_NO_PERMISSOIN);
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to calculate eMMC directory size: " << e.what();
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_NO_PERMISSOIN);
        }
        return total_size / (1024 * 1024);
    }

    std::string EventTriggeredRecorder::get_current_timestamp_iso() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
        return oss.str();
    }

    double EventTriggeredRecorder::get_current_timestamp_sec() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration<double>(now.time_since_epoch()).count();
    }

    std::string EventTriggeredRecorder::build_metadata_json(const std::vector<TriggerInfo>& triggers,
                                                            const std::vector<std::string>& emmc_paths) {
        using json = nlohmann::json;

        json j;

        json triggers_arr = json::array();
        for (const auto& t : triggers) {
            json entry;
            entry["type"] = t.type;
            if (!t.fault_code.empty()) {
                entry["fault_code"] = t.fault_code;
            }
            entry["timestamp"] = t.timestamp;
            entry["timestamp_sec"] = t.timestamp_sec;
            triggers_arr.push_back(entry);
        }
        j["triggers"] = triggers_arr;
        j["files"] = emmc_paths;

        return j.dump(2) + "\n";
    }

    void EventTriggeredRecorder::generate_metadata_file(const std::vector<TriggerInfo>& triggers,
                                                        const std::vector<std::string>& emmc_paths,
                                                        const std::string&              meta_dest) {
        // meta_dest 为 eMMC 上触发文件的路径，meta.json 写在同一位置
        try {
            std::string meta_filename = meta_dest + ".meta.json";

            std::string json_content = build_metadata_json(triggers, emmc_paths);

            std::ofstream outfile(meta_filename);
            if (!outfile.is_open()) {
                ApError("trigger_record") << "Failed to create metadata file: " << meta_filename;
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_OPEN_FAILED);
                return;
            }

            outfile << json_content;
            if (!outfile.good()) {
                ApError("trigger_record") << "Failed to write metadata file: " << meta_filename;
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_WRITE_FALED);
                outfile.close();
                return;
            }
            outfile.close();

            ApInfo("trigger_record") << "Generated metadata file on eMMC: " << meta_filename;
            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_OPEN_FAILED);
            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_WRITE_FALED);

        } catch (const std::exception& e) {
            ApError("trigger_record") << "Failed to generate metadata file " << meta_dest << ": " << e.what();
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_WRITE_FALED);
        }
    }

    // ── AsyncCopy 实现 ──────────────────────────────────────────────────────────

    /**
     * @brief 两阶段原子写入：先 copy 到 .tmp，再 rename 到目标路径。
     *
     * 若进程在阶段一和阶段二之间崩溃，只会遗留一个 .tmp 文件，
     * 不会产生半写的目标文件。启动时 clean_emmc_if_needed() 会清理 .tmp 残留。
     *
     * @return true  写入成功
     * @return false 任意阶段失败（已记录错误并清理 .tmp）
     */
    static bool copy_file_atomic(const std::string& src, const std::string& dest) {
        const std::string dest_tmp = dest + ".tmp";

        try {
            std::filesystem::copy_file(src, dest_tmp, std::filesystem::copy_options::overwrite_existing);
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Phase-1 copy failed (" << src << " -> " << dest_tmp << "): " << e.what();
            try {
                std::filesystem::remove(dest_tmp);
            } catch (...) {
            }
            return false;
        }

        try {
            std::filesystem::rename(dest_tmp, dest); // 同文件系统内 rename 为原子操作
        } catch (const std::exception& e) {
            ApError("trigger_record") << "Phase-2 rename failed (" << dest_tmp << " -> " << dest << "): " << e.what();
            try {
                std::filesystem::remove(dest_tmp);
            } catch (...) {
            }
            return false;
        }

        return true;
    }

    void EventTriggeredRecorder::perform_copy_tasks(const std::vector<CopyTask>&    tasks,
                                                    const std::vector<TriggerInfo>& triggers,
                                                    const std::vector<std::string>& context_emmc_paths) {
        // tasks[0] 始终是触发文件（N），tasks[1]（若有）是上下文文件（N-1，需要 copy）
        // context_emmc_paths：已在 eMMC 的上下文文件（方案B，N-1 已 copy 过），不 copy 但写入 meta.json
        std::vector<std::string> copied_dests;

        for (const auto& task : tasks) {
            const std::string& src = task.tmpfs_src;
            const std::string& dest = task.emmc_dest;
            size_t             seq = task.sequence;

            // 源文件不存在（如进程重启清理了 tmpfs）：跳过
            if (!std::filesystem::exists(src)) {
                ApWarn("trigger_record") << "Copy source no longer exists, skipping: " << src;
                tmpfs_files_.erase(seq);
                continue;
            }

            ApInfo("trigger_record") << "Copying triggered file to eMMC: " << src << " -> " << dest;

            if (!copy_file_atomic(src, dest)) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_WRITE_FALED);
                continue;
            }

            ApInfo("trigger_record") << "Copy completed: " << dest;

            emmc_files_[seq] = dest;
            tmpfs_files_.erase(seq);

            copied_dests.push_back(dest);

            // 删除 tmpfs 源文件，释放 RAM
            try {
                std::filesystem::remove(src);
                ApInfo("trigger_record") << "Deleted tmpfs source after copy: " << src;
            } catch (const std::exception& e) {
                ApWarn("trigger_record") << "Failed to delete tmpfs source " << src << ": " << e.what();
            }
        }

        if (copied_dests.empty()) {
            return;
        }

        // 合并已复制文件和已在 eMMC 的上下文文件（方案B），一起写入 meta.json 的 files 列表
        std::vector<std::string> all_emmc_paths = copied_dests;
        all_emmc_paths.insert(all_emmc_paths.end(), context_emmc_paths.begin(), context_emmc_paths.end());

        // 为整个触发事件生成一个 meta.json，命名挂靠在 tasks[0]（触发文件 N）上
        generate_metadata_file(triggers, all_emmc_paths, tasks[0].emmc_dest);

        FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_WRITE_FALED);
    }
} // namespace event_recorder
