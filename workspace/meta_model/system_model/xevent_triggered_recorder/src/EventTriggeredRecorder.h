#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "ara/adsfi/impl_type_faultdata.h"

namespace event_recorder {

    /**
     * @brief 事件触发式录包器（V1.6：边界触发式双层存储架构）
     *
     * 功能：
     * 1. rtfbag 写入 tmpfs（RAM），消除非触发数据对 eMMC 的磨损
     * 2. 监听故障码和手动触发信号，在当前录制窗口内记录触发标志
     * 3. 当前文件录制完成（检测到新文件）时，若有触发：将 N（当前完成）和 N-1（上一个）
     *    同步 copy 到 eMMC，同时生成 .meta.json
     * 4. 无触发时，旧文件在新文件检测时立即从 tmpfs 删除（零 eMMC 写入）
     * 5. 基于序列号排序管理 eMMC 存储配额
     */
    class EventTriggeredRecorder {
        static const constexpr char* _ERRORCODE_CONFIG = "2042001";        // 读取配置文件失败
        static const constexpr char* _ERRORCODE_NO_PERMISSOIN = "2042002"; // 数据存储目录不能正常访问
        static const constexpr char* _ERRORCODE_OVER_QUOTA = "2042003";    // 数据超过限额
        static const constexpr char* _ERRORCODE_WRITE_FALED = "2042004";   // 写文件失败
        static const constexpr char* _ERRORCODE_DELETE_FAILED = "2042005"; // 删除文件失败
        static const constexpr char* _ERRORCODE_OPEN_FAILED = "2042006";   // 打开文件失败

        // 常量定义
        static const constexpr char* FILE_PREFIX = "trigger";      // 录包文件名前缀
        static constexpr size_t      FILE_DETECT_INTERVAL = 2;     // 文件检测间隔（秒）
        static constexpr size_t      DISK_CLEANUP_INTERVAL = 10;   // 磁盘清理间隔（秒）
        static constexpr size_t      MANAGEMENT_LOOP_INTERVAL = 1; // 管理循环间隔（秒）
        static constexpr size_t      MAX_SEQUENCE = 99999999;       // 序号上限（8位格式：00000000–99999999）

    public:
        struct Config {
            std::vector<std::string> topics;             // 需要录制的 topic 列表
            std::string              output_path;        // eMMC 持久化存储路径（仅触发文件写入）
            std::string              tmpfs_path;         // tmpfs 临时录包路径（由启动脚本挂载）
            size_t                   buffer_size_kb;     // rtfbag 缓存大小（KB）
            size_t                   duration_sec;       // 单个文件时长（秒）
            size_t                   quota_mb;           // eMMC 存储空间配额（MB）
            size_t                   clean_threshold_mb; // eMMC 清理阈值（MB）
            size_t                   tmpfs_size_mb;      // tmpfs 容量（MB）
            std::vector<std::string> fault_codes;        // 需要触发的故障码列表
        };

        // 生产环境构造函数：从 CustomStack 加载配置
        explicit EventTriggeredRecorder();

        ~EventTriggeredRecorder();

        // 禁止拷贝和移动
        EventTriggeredRecorder(const EventTriggeredRecorder&) = delete;
        EventTriggeredRecorder& operator=(const EventTriggeredRecorder&) = delete;

        /**
         * @brief 更新故障数据（外部以 10Hz 频率调用）
         * @param fault_data 当前系统的故障数据
         */
        void update_fault_data(const ara::adsfi::FaultData& fault_data);

        /**
         * @brief 更新手动触发信号（外部以 10Hz 频率调用）
         * @param trigger_signal 0 表示无请求，1 表示有请求
         */
        void update_manual_trigger(std::int32_t trigger_signal);

    private:
        /**
         * @brief 触发信息
         */
        struct TriggerInfo {
            std::string type;          // "fault" 或 "manual"
            std::string fault_code;    // 故障码（仅用于 fault 类型）
            std::string timestamp;     // ISO 8601 格式时间戳
            double      timestamp_sec; // Unix 时间戳（秒）

            TriggerInfo() : timestamp_sec(0.0) {
            }
        };

        /**
         * @brief 文件记录信息
         */
        struct FileRecord {
            std::string                           filename;
            size_t                                sequence;
            double                                timestamp;
            std::chrono::steady_clock::time_point detect_time;

            FileRecord() : sequence(0), timestamp(0.0) {
            }
        };

        /**
         * @brief 单文件拷贝任务（tmpfs → eMMC）
         */
        struct CopyTask {
            size_t      sequence;  // 用于更新 emmc_files_/tmpfs_files_
            std::string tmpfs_src; // tmpfs 源文件路径
            std::string emmc_dest; // eMMC 目标文件路径
        };

        // 配置加载
        void load_config();

        // 录包管理
        void        file_management_loop();
        void        launch_rtfbag();
        void        restart_rtfbag();
        bool        is_rtfbag_running();
        std::string build_rtfbag_command(const std::string& prefix);

        // 序列号管理
        void                  scan_and_restore_sequence();
        std::optional<size_t> parse_sequence_from_filename(const std::string& filename);
        size_t                get_next_sequence();
        std::string           generate_sequence_filename(size_t sequence, double timestamp);
        size_t                compute_next_sequence_after_wrap(const std::map<size_t, std::string>& files);

        // 文件检测与重命名（扫描 tmpfs）
        void   scan_and_process_tmpfs();
        void   on_file_boundary(const std::string& temp_file, size_t sequence, double timestamp);
        double extract_timestamp_from_temp_file(const std::string& temp_file);
        bool   is_orphaned_active_file(const std::string& filename);
        bool   should_process_temp_file(const std::string& filename, const std::string& extension);

        // 触发处理
        bool should_trigger_by_fault(const ara::adsfi::FaultData& fault_data,
                                     std::vector<std::string>&    triggered_codes);

        // 文件管理
        void        clean_emmc_if_needed();
        size_t      calculate_directory_size();
        void        generate_metadata_file(const std::vector<TriggerInfo>& triggers,
                                           const std::vector<std::string>& emmc_paths, const std::string& meta_dest);
        bool        delete_file_with_metadata(const std::string& filepath, size_t sequence);
        std::string build_metadata_json(const std::vector<TriggerInfo>& triggers,
                                        const std::vector<std::string>& emmc_paths);

        // 同步 copy（tmpfs → eMMC）
        // context_emmc_paths：已在 eMMC 的上下文文件路径，不需要 copy，但写入 meta.json 的 files 列表（方案B）
        void perform_copy_tasks(const std::vector<CopyTask>& tasks, const std::vector<TriggerInfo>& triggers,
                                const std::vector<std::string>& context_emmc_paths = {});

        // rename_to_sequence_format 的辅助函数
        std::string emmc_path_for(const std::string& tmpfs_filename) const;
        FileRecord  register_tmpfs_file(const std::string& temp_file, size_t sequence, double timestamp);
        void        perform_trigger_copy(const FileRecord& new_file, std::vector<TriggerInfo> triggers);
        void        advance_prev_no_trigger();

        // 去重处理
        bool check_manual_trigger_edge(std::int32_t signal);

        // 工具方法
        std::string get_current_timestamp_iso();
        double      get_current_timestamp_sec();


        // 线程和进程
        std::thread       management_thread_; // 文件管理线程（检测/重命名/清理）
        std::atomic<bool> running_{false};
        pid_t             rtfbag_pid_{-1}; // rtfbag 进程 PID

        // 配置
        Config config_;

        // 文件管理（边界触发式，tmpfs 最多保留 2 个完成文件：prev_file_ + 当前 .bag.active）
        std::optional<FileRecord> prev_file_; // N-1：上一个完成文件（在 tmpfs）
        bool prev_file_copied_{false}; // prev_file_ 是否已同步 copy 到 eMMC（tmpfs 源已删，不可重复操作）
        bool                     trigger_pending_{false}; // 当前录制窗口内是否有待处理的触发
        std::vector<TriggerInfo> pending_triggers_;       // 本窗口内累积的触发信息

        std::string file_prefix_;      // 文件名前缀（tmpfs_path + "/trigger"）
        size_t      current_sequence_; // 全局递增序列号

        std::map<size_t, std::string> tmpfs_files_; // tmpfs 文件缓存：序列号 -> 完整路径
        std::map<size_t, std::string> emmc_files_;  // eMMC 文件缓存：序列号 -> 完整路径

        // 触发去重
        std::map<std::string, std::chrono::steady_clock::time_point> fault_trigger_times_;
        std::int32_t                                                 last_manual_trigger_signal_;

        // 互斥锁：保护跨线程共享的数据
        //   trigger_pending_ + pending_triggers_：外部线程（10Hz）写，management_thread_ 读
        std::mutex mutex_;
    };

} // namespace event_recorder
