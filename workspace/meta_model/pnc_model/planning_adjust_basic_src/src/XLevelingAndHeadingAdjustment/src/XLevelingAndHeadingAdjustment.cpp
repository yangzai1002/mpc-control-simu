#include <chrono>
#include "base_control/adsfi_struct_adapter.h"
#include "ap_log/ap_log_interface.h"
#include "faulthandle_api.hpp"
#include "log/log.h"
#include "XLevelingAndHeadingAdjustment.h"
#include "HeadingAdjustment.h"
#include "Leveling.h"

constexpr double kInvalidRollPitchVal = 91.0; // Roll/Pitch 无效值
constexpr double kInvalidUtmAngleVal = 361.0; // UTM Angle 无效值
constexpr double kEpsilon = 1e-2;               // 浮点数比较精度
constexpr double kHeadingFinishThreshold = 20.0; // 原地转向完成阈值 (度)

void XLevelingAndHeadingAdjustment ( const ara::adsfi::BusinessCommand& business_command ,  // 业务信息 
const avos::planning::LocalizationData& localization_data ,     // 定位信息 
const avos::planning::Perception& perception ,  // 感知数据 
const avos::planning::VehicleStatus& vehicle_status ,   // 车辆状态信息   
const avos::planning::PlanningInfo& planning_info_in ,  // 输入规划内部信息
const ara::adsfi::PerceptionStaticEnv& semanticmap ,    // 感知语义地图信息 
PlanningTrajectory& pathplan ,  // 规划轨迹信息   
avos::planning::PlanningInfo& planning_info_out ,   // 输出规划内部信息 
ara::adsfi::PerceptionStaticEnv& terrain_debug){

    auto start_time = std::chrono::high_resolution_clock::now();
    static long long global_seq = 0;
    planning_info_out = planning_info_in;
    pathplan.header.seq = ++global_seq;
    pathplan.header.time = avos::common::NodeTime::Now().ToSecond();
    pathplan.header.module_name = "PlanningAdjust";
    pathplan.header.data_name = "20260303";
    
    static avos::planning::Leveling leveling_processor; 

    // 获取上游下发的任务状态 (0:停止, 1:开始, 2:暂停, 3:任务完成)
    int task_cmd = planning_info_in.follow_info.task_status;

    // 场景 A: 收到空闲指令 (0)
    // 场景 B: 收到完成状态 (3) -> 上一帧已完成，上游回传了完成状态，保持静默
    if (task_cmd == 0 || task_cmd == 3) {
        if (task_cmd == 0) {
            AINFO_IF(FLAGS_enable_log) << ">>> 收到任务空闲指令(0)";
        } else {
            AINFO_IF(FLAGS_enable_log) << ">>> 收到任务完成状态(3)";
        }
        
        pathplan.trajectory.clear(); // 1. 清空轨迹
        leveling_processor.Reset(); 
        return;
    }

    // 根据业务模式(business_mode)选择不同的处理逻辑
    switch (business_command.business_mode) {
        case 4: { // business_mode = 4: 综合调整模式 (找平 / 航向调整 / 组合)
            AINFO_IF(FLAGS_enable_log) << ">>> 进入模式: 综合调整模式 (找平/航向调整组合)";
            
            // 1. 获取 Pose 参数
            double roll_val = business_command.param.pose.orientation.x;       // 对应 Roll
            double pitch_val = business_command.param.pose.orientation.y;      // 对应 Pitch
            double angle_utm_val = business_command.param.pose.orientation.z; // 对应 UTM Angle

            // 2. 判断有效性标志位
            // Flag A: 找平需求 (Roll 或 Pitch 不等于 91)
            bool need_leveling = (std::abs(roll_val - kInvalidRollPitchVal) > kEpsilon) && 
                                 (std::abs(pitch_val - kInvalidRollPitchVal) > kEpsilon);

            // Flag B: 转向需求 (Angle 不等于 361)
            bool need_heading = (std::abs(angle_utm_val - kInvalidUtmAngleVal) > kEpsilon);

            // // --- 辅助 Lambda: 检查航向是否到位 ---
            // auto CheckHeadingFinished = [&](double target_deg, double current_deg) -> bool {
            //     // 1. 直接计算差值 (单位：度)
            //     double diff = target_deg - current_deg;
                
            //     // 2. 角度归一化到 [-180, 180]
            //     while (diff > 180.0) diff -= 360.0;
            //     while (diff < -180.0) diff += 360.0;
                
            //     // 3. 绝对值判断
            //     return std::abs(diff) < kHeadingFinishThreshold;
            // };

            // 3. 执行逻辑分发
            if (need_leveling) {
                // ================== [分支 A: 需要找平] ==================
                bool lev_success = leveling_processor.Process(business_command, localization_data, semanticmap, pathplan, terrain_debug);

                if (!lev_success) {
                    AWARN_IF(FLAGS_enable_log) << "[Mode 4] 找平路径规划失败，无法执行后续动作。";
                    pathplan.trajectory.clear(); 
                } else {
                    if (need_heading) {
                        // --- [组合任务: 找平 -> 转向] ---
                        if (leveling_processor.IsTargetReached(localization_data)) {
                            // 阶段2: 找平已到位，执行转向
                            HeadingAdjustment heading_adjuster;
                            heading_adjuster.Process(business_command, localization_data, perception, pathplan);
                            
                            if (heading_adjuster.IsHeadingReached(angle_utm_val, localization_data.yaw, kHeadingFinishThreshold)) {
                                AINFO_IF(FLAGS_enable_log) << "[Mode 4] 组合任务完成 (找平到位 + 转向到位)，发送结束信号。";
                                planning_info_out.follow_info.task_status = 3; // 置 0 表示任务成功/IDLE
                                pathplan.trajectory.clear();             // 清空轨迹确保停稳
                                leveling_processor.Reset();   // 清理找平内部状态
                            }
                        } else {
                            // 阶段1: 还没到找平点，继续输出找平轨迹
                            AINFO_IF(FLAGS_enable_log) << "[Mode 4] 正在执行第一阶段：自动找平";
                        }
                    } else {
                        // --- [单一任务: 仅找平] ---
                        AINFO_IF(FLAGS_enable_log) << "[Mode 4] 执行单一任务：自动找平。";
                        
                        // ================== 【检查找平是否完成】 ==================
                        if (leveling_processor.IsTargetReached(localization_data)) {
                            AINFO_IF(FLAGS_enable_log) << "[Mode 4] 单一找平任务完成，发送结束信号。";
                            planning_info_out.follow_info.task_status = 3;
                            pathplan.trajectory.clear();
                            // 防止上一帧发了3，下一帧上游还没变，导致这里误重置引发重搜
                            // 1. 本帧发 3，保持锁定，不做新动作。
                            // 2. 上游收到 3，下发 task_cmd = 3 或 0。
                            // 3. 下一帧进入函数开头的 "if (task_cmd == 0 || 3)" 分支，那里会执行 Reset。
                            // leveling_processor.Reset();
                        }
                    }
                }

            } else if (need_heading) {
                // ================== [分支 B: 仅转向] ==================
                AINFO_IF(FLAGS_enable_log) << "[Mode 4] 执行单一任务：原地转向。";
                HeadingAdjustment heading_adjuster;
                heading_adjuster.Process(business_command, localization_data, perception, pathplan);

                // ================== 【检查转向是否完成】 ==================
                if (heading_adjuster.IsHeadingReached(angle_utm_val, localization_data.yaw, kHeadingFinishThreshold)) {
                    AINFO_IF(FLAGS_enable_log) << "[Mode 4] 单一转向任务完成，发送结束信号。";
                    planning_info_out.follow_info.task_status = 3;
                    pathplan.trajectory.clear();
                }
            } else {
                AWARN_IF(FLAGS_enable_log) << "[Mode 4] 参数无效，无动作。";
                pathplan.trajectory.clear();
            }
            break;
        }
        default: { // 其他未定义的模式
            AINFO_IF(FLAGS_enable_log) << ">>> 收到未定义或未处理的业务模式: " << business_command.business_mode;
            pathplan.trajectory.clear();
            break;
        }
    }

    //处理暂停
    if (task_cmd == 2) {
        AINFO_IF(FLAGS_enable_log) << ">>> 处于暂停状态(2)，保留轨迹几何，强制速度置零。";
        
        // 1. 遍历所有轨迹点，将运动属性抹零
        for (auto& point : pathplan.trajectory) {
            point.velocity = 0.0f;      // 速度归零
            point.a = 0.0f;             // 加速度归零
        }
        if (!pathplan.trajectory.empty()) {
            if (pathplan.trajectory.front().type == 1) {
                AINFO_IF(FLAGS_enable_log) << ">>> 暂停状态(2)：检测到原地转向任务，强制清空轨迹。";
                pathplan.trajectory.clear(); // 清空点，保留 Header
            }
        }

        // 2. 设置安全标志位 (双重保险，告知下游这是急停/驻车状态)
        pathplan.additional_info.emergency_stop = true; 
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    AWARN_IF(FLAGS_enable_log) << "规划原地调整和地势找平模块耗时：" << duration.count() / 1000.0 << " ms.";
}

