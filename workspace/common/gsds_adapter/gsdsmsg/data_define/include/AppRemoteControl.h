#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
遥控数据类型
*/

typedef struct AppRemoteControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	steering_wheel;	//转向 范围【-32767~32767 左正右负，不区分档位】
	int	gear;	//档位 含义【0-P、 1-R、 2-N,、3-D】
	double	brake;	//制动 含义【0~32767】
	double	accelerator;	//加速 含义【0~32767】
	int	remotectrl_flag;	//遥控模式 含义【1-基础遥控、2-定速、3-定向、4-既定速也定向】
	int	estop_flag;	//紧急制动 含义【0-关闭、1-开启】
	double	steer_torque;	//转向力矩 含义【】
	bool	eng_status;	//reserve
	int	ctrl_level;	//reserve
	bool	in_place;	//reserve
	int	tick;	//心跳 【0~99】
	int	speed_limit;	//限速 【5~60】
	int	set_speed;	//遥控定速值
	int	security_assist_enabled;	//遥控安全预警是否启用：0-关闭、1-开启

	AppRemoteControl(){
		key_hash_list.resize(19);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1012146582));
		key_hash_list.push_back(msghandle::type::fix_int32(2145353667));
		key_hash_list.push_back(msghandle::type::fix_int32(564872456));
		key_hash_list.push_back(msghandle::type::fix_int32(451380103));
		key_hash_list.push_back(msghandle::type::fix_int32(891455714));
		key_hash_list.push_back(msghandle::type::fix_int32(1686856899));
		key_hash_list.push_back(msghandle::type::fix_int32(745700372));
		key_hash_list.push_back(msghandle::type::fix_int32(196500561));
		key_hash_list.push_back(msghandle::type::fix_int32(118978835));
		key_hash_list.push_back(msghandle::type::fix_int32(379039243));
		key_hash_list.push_back(msghandle::type::fix_int32(680131200));
		key_hash_list.push_back(msghandle::type::fix_int32(2129501354));
		key_hash_list.push_back(msghandle::type::fix_int32(1641573579));
		key_hash_list.push_back(msghandle::type::fix_int32(554668879));
		Clear();
	}

void Clear(){
	header={};
	steering_wheel={};
	gear={};
	brake={};
	accelerator={};
	remotectrl_flag={};
	estop_flag={};
	steer_torque={};
	eng_status={};
	ctrl_level={};
	in_place={};
	tick={};
	speed_limit={};
	set_speed={};
	security_assist_enabled={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,steering_wheel,gear,brake,accelerator,remotectrl_flag,estop_flag,steer_torque,eng_status,ctrl_level,in_place,tick,speed_limit,set_speed,security_assist_enabled);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","steering_wheel","gear","brake","accelerator","remotectrl_flag","estop_flag","steer_torque","eng_status","ctrl_level","in_place","tick","speed_limit","set_speed","security_assist_enabled");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,steering_wheel,gear,brake,accelerator,remotectrl_flag,estop_flag,steer_torque,eng_status,ctrl_level,in_place,tick,speed_limit,set_speed,security_assist_enabled);
} AppRemoteControl;
REGISTER(AppRemoteControl,AppRemoteControl_Func);
