#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
车辆横向控制
*/

typedef struct VehicleLatControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	target_angle;	//目标方向盘转角
	float	target_torque;	//目标方向盘扭矩
	float	target_curvature;	//目标转向曲率
	float	target_left_vel;	//左轮目标线速度
	float	target_right_vel;	//右轮目标线速度
	int	steer_control_mode;	//横向控制模式 0:默认 1: 原地转向

	VehicleLatControl(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1077665743));
		key_hash_list.push_back(msghandle::type::fix_int32(1608697743));
		key_hash_list.push_back(msghandle::type::fix_int32(1743478668));
		key_hash_list.push_back(msghandle::type::fix_int32(2012930484));
		key_hash_list.push_back(msghandle::type::fix_int32(1721394788));
		key_hash_list.push_back(msghandle::type::fix_int32(1491085018));
		Clear();
	}

void Clear(){
	target_angle={};
	target_torque={};
	target_curvature={};
	target_left_vel={};
	target_right_vel={};
	steer_control_mode={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,target_angle,target_torque,target_curvature,target_left_vel,target_right_vel,steer_control_mode);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","target_angle","target_torque","target_curvature","target_left_vel","target_right_vel","steer_control_mode");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,target_angle,target_torque,target_curvature,target_left_vel,target_right_vel,steer_control_mode);
} VehicleLatControl;
REGISTER(VehicleLatControl,VehicleLatControl_Func);
