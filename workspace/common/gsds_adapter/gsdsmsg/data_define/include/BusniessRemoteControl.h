#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
遥控数据类型
*/

typedef struct BusniessRemoteControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	steering_wheel;	//转向
	int	gear;	//档位
	double	brake;	//制动
	double	accelerator;	//加速
	int	remotectrl_flag;	//遥控模式
	int	estop_flag;	//紧急制动
	double	steer_torque;	//转向力矩
	bool	eng_status;	//reserve
	int	ctrl_level;	//reserve
	int	tick;	//心跳
	bool	in_place;	//reserve

	BusniessRemoteControl(){
		key_hash_list.resize(15);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(634752889));
		key_hash_list.push_back(msghandle::type::fix_int32(985861354));
		key_hash_list.push_back(msghandle::type::fix_int32(411618176));
		key_hash_list.push_back(msghandle::type::fix_int32(1185688207));
		key_hash_list.push_back(msghandle::type::fix_int32(1446400430));
		key_hash_list.push_back(msghandle::type::fix_int32(678181460));
		key_hash_list.push_back(msghandle::type::fix_int32(1761565123));
		key_hash_list.push_back(msghandle::type::fix_int32(196500561));
		key_hash_list.push_back(msghandle::type::fix_int32(118978835));
		key_hash_list.push_back(msghandle::type::fix_int32(2089642761));
		key_hash_list.push_back(msghandle::type::fix_int32(379039243));
		Clear();
	}

void Clear(){
	steering_wheel={};
	gear={};
	brake={};
	accelerator={};
	remotectrl_flag={};
	estop_flag={};
	steer_torque={};
	eng_status={};
	ctrl_level={};
	tick={};
	in_place={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,steering_wheel,gear,brake,accelerator,remotectrl_flag,estop_flag,steer_torque,eng_status,ctrl_level,tick,in_place);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","steering_wheel","gear","brake","accelerator","remotectrl_flag","estop_flag","steer_torque","eng_status","ctrl_level","tick","in_place");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,steering_wheel,gear,brake,accelerator,remotectrl_flag,estop_flag,steer_torque,eng_status,ctrl_level,tick,in_place);
} BusniessRemoteControl;
REGISTER(BusniessRemoteControl,BusniessRemoteControl_Func);
