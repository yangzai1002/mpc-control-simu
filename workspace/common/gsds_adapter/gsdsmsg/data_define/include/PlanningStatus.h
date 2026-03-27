#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "CommonHeader.h"
#include "PlanningTrajectory.h"
#include "DebugPlanningMsg.h"
/*

*/

typedef struct PlanningStatus{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	CommonHeader	header;	//数据头，包含时间戳等信息
	int	command_status;	//0: 空闲  1: 执行中  2: 主动结束任务  3: 退出自主
	double	current_limit_speed;	//当前限速值 km/h
	double	current_set_speed;	//当前定速值 km/h
	double	current_set_angle;	//当前目标航向 degrees
	int	decision_info;	//决策信息【待定】
	PlanningTrajectory	driving_center_trajectory;	//预测自车行驶中心轨迹
	PlanningTrajectory	driving_left_boundary;	//预测自车行驶区域左边界
	PlanningTrajectory	driving_right_boundary;	//预测自车行驶区域右边界
	int	risk_level;	//风险等级 0:安全, 1:距离预警, 2:碰撞预警, 3:紧急制动
	DebugPlanningMsg	debug_message;	//规划Debug信息

PlanningStatus(){
	command_status = 0;
	current_limit_speed = 0.0;
	current_set_speed = 0.0;
	current_set_angle = 0.0;
	decision_info = 0;
	risk_level = 0;
	key_hash_list.reserve(16);
	key_hash_list.push_back(msghandle::type::fix_int32(1839877640));
	key_hash_list.push_back(msghandle::type::fix_int32(299606970));
	key_hash_list.push_back(msghandle::type::fix_int32(1229309071));
	key_hash_list.push_back(msghandle::type::fix_int32(1205909538));
	key_hash_list.push_back(msghandle::type::fix_int32(195417314));
	key_hash_list.push_back(msghandle::type::fix_int32(1575053149));
	key_hash_list.push_back(msghandle::type::fix_int32(32384622));
	key_hash_list.push_back(msghandle::type::fix_int32(1337368435));
	key_hash_list.push_back(msghandle::type::fix_int32(1819833632));
	key_hash_list.push_back(msghandle::type::fix_int32(993967888));
	key_hash_list.push_back(msghandle::type::fix_int32(890441265));
}

void Clear(){
	command_status = 0;
	current_limit_speed = 0.0;
	current_set_speed = 0.0;
	current_set_angle = 0.0;
	decision_info = 0;
	risk_level = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, header,command_status,current_limit_speed,current_set_speed,current_set_angle,decision_info,driving_center_trajectory,driving_left_boundary,driving_right_boundary,risk_level,debug_message);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","command_status","current_limit_speed","current_set_speed","current_set_angle","decision_info","driving_center_trajectory","driving_left_boundary","driving_right_boundary","risk_level","debug_message");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, header,command_status,current_limit_speed,current_set_speed,current_set_angle,decision_info,driving_center_trajectory,driving_left_boundary,driving_right_boundary,risk_level,debug_message);
} PlanningStatus;
REGISTER(PlanningStatus,PlanningStatus_Func);
