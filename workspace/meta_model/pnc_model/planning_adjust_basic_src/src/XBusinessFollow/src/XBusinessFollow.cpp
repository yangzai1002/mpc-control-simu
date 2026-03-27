#include "ap_log/ap_log_interface.h"
#include "log/log.h"
#include "XBusinessFollow.h"
#include "base_control/adsfi_struct_adapter.h"

static int current_mission_status{0};
static int previous_drive_mode{-1};
static bool force_zero_after_quit{false};
static int start_cmd_consecutive_frames{0};
static int pause_cmd_consecutive_frames{0};
static bool is_internal_paused = false;
// int command_status; // 0: 空闲  1: 执行中  2: 主动结束任务  3: 退出自主

void ProcessFollowLogic(const avos::planning::VehicleStatus& vehicle_status,
                        const avos::planning::PlanningInfo& planning_info_in,
                        avos::planning::PlanningInfo& planning_info_out,
                        ara::adsfi::PlanningStatus& mission_status) {
    
    AINFO_IF(FLAGS_enable_log) << "[XbusinessFollow] : Start processing follow logic...";
    
    // 优先级1: 如果标志位为true，说明上一帧是退出状态3，本帧必须强制为0
    if (force_zero_after_quit) {
        AINFO_IF(FLAGS_enable_log) << "退出序列第2步: 上一帧为3，本帧强制置为0";
        current_mission_status = 0;
        force_zero_after_quit = false; // 重置标志位，退出序列完成
    
    // 优先级2: 检测到“刚从自主切换到手动”的瞬间
    } else if (previous_drive_mode == 1 && vehicle_status.current_drive_mode == 0) {
        AINFO_IF(FLAGS_enable_log) << "退出序列第1步: 检测到模式切换，本帧状态设置为3";
        current_mission_status = 3;
        force_zero_after_quit = true; // 设置标志位，要求下一帧强制置为0
        
    // 优先级3: 常规状态机逻辑
    } else {
        switch (current_mission_status) {
            case 0:
                // 检查是否收到了“开始”指令 (task_status == 1)
                if (planning_info_in.follow_info.task_status == 1) {
                    // 如果是“开始”指令，则计数器加1
                    start_cmd_consecutive_frames++;
                    AINFO_IF(FLAGS_enable_log) << "检测到开始指令，连续帧计数: " 
                          << start_cmd_consecutive_frames << "/3";
                    
                    // 检查计数器是否已达到3帧
                    if (start_cmd_consecutive_frames >= 3) {
                        AINFO_IF(FLAGS_enable_log) << "连续3帧检测到开始指令，任务正式启动！";
                        current_mission_status = 1; // 切换到“执行中”状态
                        is_internal_paused = false; // 明确：这是运行
                        start_cmd_consecutive_frames = 0; // 任务成功启动，重置计数器
                    }
                    pause_cmd_consecutive_frames = 0;
                } else if (planning_info_in.follow_info.task_status == 2) {
                    // 收到暂停指令，计数器 +1
                    pause_cmd_consecutive_frames++;
                    AINFO_IF(FLAGS_enable_log) << "空闲状态检测到暂停指令，计数: " << pause_cmd_consecutive_frames << "/3";

                    // 同时也把启动计数清零，防止两个计数器同时累加的边缘情况
                    start_cmd_consecutive_frames = 0;

                    // 检查是否连续 2 帧 (>=2 即可，通常2-3帧最合适)
                    if (pause_cmd_consecutive_frames >= 3) {
                        AINFO_IF(FLAGS_enable_log) << "连续3帧收到暂停指令，确认切换至暂停状态";
                        current_mission_status = 1;
                        is_internal_paused = true;  // 明确：这是暂停
                        // 状态切换完成，计数器归零
                        pause_cmd_consecutive_frames = 0;
                    }
                } else {
                    // 如果收到的指令不是1 (例如是0或其他)，则说明连续性被中断
                    // 1. 如果之前正在尝试“开始”
                    if (start_cmd_consecutive_frames > 0) {
                        AINFO_IF(FLAGS_enable_log) << "【启动序列中断】原计划启动，已累计 Start 帧数: " 
                              << start_cmd_consecutive_frames 
                              << "/3. 本帧收到打断指令: " << planning_info_in.follow_info.task_status 
                              << " -> 重置计数器。";
                    }

                    // 2. 如果之前正在尝试“暂停”
                    if (pause_cmd_consecutive_frames > 0) {
                        AINFO_IF(FLAGS_enable_log) << "【暂停序列中断】原计划暂停，已累计 Pause 帧数: " 
                              << pause_cmd_consecutive_frames 
                              << "/3. 本帧收到打断指令: " << planning_info_in.follow_info.task_status 
                              << " -> 重置计数器。";
                    }
                    start_cmd_consecutive_frames = 0; // 重置计数器
                    pause_cmd_consecutive_frames = 0;
                }
                break; // case 0 结束
            case 1:
                if (planning_info_in.follow_info.task_status == 0) {
                    AINFO_IF(FLAGS_enable_log) << "任务执行中收到停止指令";
                    current_mission_status = 0;
                } else if (planning_info_in.follow_info.task_status == 1) {
                    AINFO_IF(FLAGS_enable_log) << "任务执行中收到运行指令，持续运行";
                    is_internal_paused = false;
                } else if (planning_info_in.follow_info.task_status == 2) {
                    AINFO_IF(FLAGS_enable_log) << "任务执行中收到暂停指令, 保持任务执行状态";
                    current_mission_status = 1;
                    is_internal_paused = true;
                } else if (planning_info_in.follow_info.task_status == 3) {
                    AINFO_IF(FLAGS_enable_log) << "收到任务完成指令，切换至完成态。";
                    current_mission_status = 2; // 进入完成状态
                }
                break;
            case 2:
                if (planning_info_in.follow_info.task_status == 0) {
                    AINFO_IF(FLAGS_enable_log) << "任务已完成(2)，收到复位信号(0)，重置为空闲。";
                    current_mission_status = 0;
                } else if (planning_info_in.follow_info.task_status == 1) {
                    AINFO_IF(FLAGS_enable_log) << "任务已完成(2)，恢复执行？？";
                    current_mission_status = 1;
                }
                break;
            default:
                break;
        }
    }
    
    AINFO_IF(FLAGS_enable_log) << "本帧最终状态 current_mission_status = " << current_mission_status;

    // 将最终确定的状态赋值给上游
    mission_status.command_status = current_mission_status;	
    mission_status.current_limit_speed =
         const_cast<avos::planning::PlanningInfo&>(planning_info_in).speedplan_info.GetVelocityLimit() * 3.6;

    // 在函数末尾更新驾驶模式记录
    previous_drive_mode = vehicle_status.current_drive_mode;

    planning_info_out = std::move(planning_info_in);
    
    // 根据本帧最终的 current_mission_status 来决定输出
	if (current_mission_status == 2) {
		planning_info_out.speedplan_info.emgency_stop = true;
        planning_info_out.follow_info.task_status = 3; // 转换回内部协议的 3
	} else if (current_mission_status == 1) {
        // 执行态：区分运行与暂停
        planning_info_out.follow_info.task_status = (is_internal_paused ? 2 : 1);
    } else {
		planning_info_out.speedplan_info.emgency_stop = false;
        planning_info_out.follow_info.task_status = current_mission_status;
	}

    // 仅在状态为3的那一帧，才需要向下游发送清理信号
    // if (current_mission_status == 3) {
    //     planning_info_out.follow_info.task_status = 3;
    //     AINFO_IF(FLAGS_enable_log) << "状态为3，向下游发送清理信号";
    // } else if (current_mission_status == 0) {
    //     planning_info_out.follow_info.task_status = 0;
    //     AINFO_IF(FLAGS_enable_log) << "状态为0，向下游发送清理信号";
    // }
    // if (current_mission_status == 2) {
    //     planning_info_out.follow_info.task_status = 3; // 转换回内部协议的 3
    // } else {
    //     planning_info_out.follow_info.task_status = current_mission_status;
    // }

	AINFO_IF(FLAGS_enable_log) << "[XbusinessFollow] : End processing follow logic...";
}

void XBusinessFollow ( const avos::planning::VehicleStatus& vehicle_status , 	// 车辆状态	
const avos::planning::PlanningInfo& planning_info_in , 	// 输入规划信息	
avos::planning::PlanningInfo& planning_info_out , 	// 规划相关信息	
ara::adsfi::PlanningStatus& mission_status ){
	ProcessFollowLogic(vehicle_status, planning_info_in, planning_info_out, mission_status);
}

