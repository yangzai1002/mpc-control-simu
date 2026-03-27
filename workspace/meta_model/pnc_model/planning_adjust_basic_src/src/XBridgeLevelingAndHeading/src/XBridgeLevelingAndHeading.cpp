#include "XBridgeLevelingAndHeading.h"
#include "ap_log/ap_log_interface.h"
#include "log/log.h"
#include "base_control/adsfi_struct_adapter.h"
#include <queue>
#include <limits>
#include <cmath>

// // ======================= 引入/复用 FixedSizeQueue 类 =======================
// // 创建一个固定大小的队列，自动维护最近N帧的数据，这里用来缓存本车定位历史。
// class FixedSizeQueue {
// private:
//     std::queue<avos::planning::LocalizationData> queue;
//     size_t maxSize;

// public:
//     FixedSizeQueue(size_t size) : maxSize(size) {}

//     void push(const avos::planning::LocalizationData& value) {
//         if (queue.size() >= maxSize) {
//             queue.pop();
//         }
//         queue.push(value);
//     }

//     // 在定位历史中，查找与给定时间戳最匹配的一帧
//     bool Match(double timestamp, avos::planning::LocalizationData& matched_loc) const {
//         if (queue.empty()) {
//             return false;
//         }
//         double min_time_diff = std::numeric_limits<double>::max();
//         bool found = false;
//         std::queue<avos::planning::LocalizationData> temp = queue;
//         while (!temp.empty()) {
//             double diff = std::abs(timestamp - temp.front().time);
//             if (diff < min_time_diff) {
//                 min_time_diff = diff;
//                 matched_loc = temp.front();
//                 found = true;
//             }
//             temp.pop();
//         }
//         // 增加一个匹配有效性的检查，防止匹配到的时间差过大
//         if (found && min_time_diff > 1.0) { // 阈值可配置，先设为1秒
//             AWARN_IF(FLAGS_enable_log) << "[XBridgeLeveling] Found loc match but time diff is too large: " << min_time_diff << "s.";
//             return false;
//         }
//         return found;
//     }
// };


// 静态变量，用于跟踪任务是否已经开始运行
static bool has_already_running{false};
static bool is_waiting_command_reset = false;
static int finish_check_count = 0;
// /**
//  * @brief 对输入的感知信息进行时间同步
//  * @param perception_timestamp  输入的感知帧时间戳
//  * @param latest_loc            输入的当前最新定位
//  * @param loc_history           [in/out] 用于匹配的定位历史数据队列
//  * @param matched_loc           [out] 输出的、与感知时间戳匹配的定位信息
//  * @return bool                 匹配是否成功
//  */
// bool SynchronizePerceptionTime(double perception_timestamp,
//                                const avos::planning::LocalizationData& latest_loc,
//                                FixedSizeQueue& loc_history,
//                                avos::planning::LocalizationData& matched_loc) {
//     // 1. 将最新定位存入历史队列
//     loc_history.push(latest_loc);

//     // 2. 在历史定位中查找与感知时间戳最匹配的一帧
//     bool match_ok = loc_history.Match(perception_timestamp, matched_loc);

//     if (!match_ok) {
//         AERROR_IF(FLAGS_enable_log) << "[Sync] Loc-Perception sync failed! Timestamp: " << perception_timestamp;
//     }
    
//     return match_ok;
// }


/**
 * @brief 处理业务层下发的姿态调整和找平指令，并更新规划信息
 * @param app_cmd           业务层指令
 * @param localization_data 定位信息
 * @param perception_obj    感知融合障碍物信息
 * @param planning_info_in  输入规划信息
 * @param pln_info_out      输出规划信息
 */
void XBridgeLevelingAndHeading ( const ara::adsfi::BusinessCommand& app_cmd , 	// 业务层指令	
const avos::planning::LocalizationData& localization_data , 	// 定位信息	
const PerceptionFinalObjects& perception_obj , 	// 感知融合障碍物信息	
const avos::planning::PlanningInfo& planning_info_in , 	// 输入规划信息	
avos::planning::PlanningInfo& pln_info_out ){

    // // 创建一个静态的、持久化的定位历史数据队列
    // static FixedSizeQueue loc_history_queue(20); // 缓存20帧定位数据

    // // ======================= 1. 对感知信息进行时间同步 =======================
    // avos::planning::LocalizationData matched_loc_at_perception_time;
    // // 使用感知帧头的时间戳进行同步
    // bool sync_ok = SynchronizePerceptionTime(perception_obj.header.time,
    //                                          localization_data,
    //                                          loc_history_queue,
    //                                          matched_loc_at_perception_time);

    // ======================= 2. 处理业务指令与状态机 =======================
    // 拷贝输入信息到输出，作为默认值
    pln_info_out = planning_info_in;
    pln_info_out.speedplan_info.SetVelocityLimit(app_cmd.param.max_speed / 3.6, "TaskCommand Max Speed");

    // 前置条件检查
    bool loc_ok = true;//(localization_data.xg > 1 && localization_data.yg > 1);
    // 感知有效性检查：同步成功且感知时间戳有效
    // bool perception_ok = sync_ok && (perception_obj.header.time > 1.0);

    AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling] Recv command: " << app_cmd.param.command <<" mode: "<< app_cmd.business_mode
          << ", Set max_speed: " << app_cmd.param.max_speed / 3.6 << " m/s";

    // ======================= 3. 根据任务指令处理特定逻辑 =======================
    switch (app_cmd.param.command) {
        case 1: // 指令：开始任务
            AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling]: xTak request {Start/Continue}";
            // 刚刚报告了完成，但上游还在发 Start (1)，必须拦截，防止重启
            if (is_waiting_command_reset) {
                AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling] 任务已完成，等待上游指令复位...";
                // 这里必须发 3，保持完成态，而不是变为空闲
                pln_info_out.follow_info.task_status = 3; 
                has_already_running = false;
                break; 
            }

            // 判断是“首次启动”还是“持续运行”
            if (!has_already_running) {
                // 启动任务严格检查前置条件
                finish_check_count = 0;
                AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling]: Attempting to START a new task.";
                if (loc_ok /*&& perception_ok*/) {
                    // 条件满足，成功启动任务
                    AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling]: Preconditions met, new task started successfully.";
                    pln_info_out.follow_info.task_status = 1; // 任务状态: 运行中
                    has_already_running = true; // 标记任务已在运行
                } else {
                    // 条件不满足，启动失败，保持之前的状态
                    AERROR_IF(FLAGS_enable_log) << "[XbridgeLeveling]: Failed to start new task! Preconditions NOT met. LocOK: "
                           << loc_ok;
                }
            } else {
                // 任务已在持续运行中, 即使定位或感知短暂丢失，也要继续保持任务状态
                AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling]: Continuing an existing task.";

                // 1. 判断当前单帧是否满足结束条件
                bool is_current_frame_finished = (planning_info_in.follow_info.task_status == 3);

                // 2. 计数器逻辑：满足则累加，不满足则清零
                if (is_current_frame_finished) {
                    finish_check_count++;
                } else {
                    finish_check_count = 0; // 只要中间有一帧被打断(变为1)，计数器立即重置
                }

                // 3. 阈值判定：是否连续 3 帧都满足
                if (finish_check_count >= 3) {
                    AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling] 下游反馈任务完成(Status=3)已持续3帧，确认任务结束。";
                    pln_info_out.follow_info.task_status = 3; // 最终确认状态为停止
                    has_already_running = false;              // 重置内部运行标志
                    finish_check_count = 0;                   // 任务结束后，计数器归零，为下次任务做准备
                    is_waiting_command_reset = true;
                } else {                    
                    // 仅用于调试：如果正在确认中，打印日志
                    if (finish_check_count > 0) {
                         AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling] 收到结束信号，正在防抖确认中... (" << finish_check_count << "/3)";
                    }
                
                    // 强制设置任务为运行状态，确保数据流不断
                    pln_info_out.follow_info.task_status = 1; // 任务状态: 运行中
                }
            }
            break;

        case 2: // 指令：暂停任务
            AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling]: xTak request {Pause}, {RESPONSE}";
            // 暂停任务无特殊前置条件，直接设置状态
            pln_info_out.follow_info.task_status = 2; // 任务状态: 暂停
            break;

        case 0: // 指令：停止任务
            AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling]: xTak request {Stop}, {RESPONSE}";
            // 停止任务无特殊前置条件，直接设置状态并重置标志位
            pln_info_out.follow_info.task_status = 0; // 任务状态: 停止/空闲
            has_already_running = false;
            finish_check_count = 0;
            // 上游终于发了 Stop，说明它已经收到了之前的完成信号，或者人工重置了
            // 解除握手锁，允许下一次 Start
            if (is_waiting_command_reset) {
                AINFO_IF(FLAGS_enable_log) << "[XbridgeLeveling] 指令已复位，解除启动锁定。";
                is_waiting_command_reset = false;
            }
            break;

        default: // 其他未知指令
            AWARN_IF(FLAGS_enable_log) << "[XbridgeLeveling]: Received unknown command: " << app_cmd.param.command;
            break;
    }

}

