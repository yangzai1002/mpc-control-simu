#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
抽象按键状态
*/

typedef struct VehicleSensorControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	unsigned char	actor_control_enable;	//使能自动驾驶状态
	unsigned char	brake_light;	//刹车灯设置
	unsigned char	double_flash_light;	//双闪灯设置
	unsigned char	low_beam;	//近光灯设置
	unsigned char	high_beam;	//远光灯设置
	unsigned char	turn_left_light;	//左转灯设置
	unsigned char	turn_right_light;	//右转灯设置
	unsigned char	auto_horn;	//鸣笛设置
	unsigned char	front_fog_light;	//前雾灯设置
	unsigned char	rear_fog_light;	//后雾灯设置
	unsigned char	tail_light;	//车尾灯设置
	unsigned char	wiper;	//雨刮配置
	unsigned char	door_lock;	//门锁配置
	unsigned char	emergency_stop;	//紧急停车

	VehicleSensorControl(){
		key_hash_list.resize(19);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1167103888));
		key_hash_list.push_back(msghandle::type::fix_int32(378779375));
		key_hash_list.push_back(msghandle::type::fix_int32(2117056991));
		key_hash_list.push_back(msghandle::type::fix_int32(370459353));
		key_hash_list.push_back(msghandle::type::fix_int32(324819694));
		key_hash_list.push_back(msghandle::type::fix_int32(553330025));
		key_hash_list.push_back(msghandle::type::fix_int32(1593378747));
		key_hash_list.push_back(msghandle::type::fix_int32(195683848));
		key_hash_list.push_back(msghandle::type::fix_int32(825350056));
		key_hash_list.push_back(msghandle::type::fix_int32(1156672311));
		key_hash_list.push_back(msghandle::type::fix_int32(448421349));
		key_hash_list.push_back(msghandle::type::fix_int32(113380362));
		key_hash_list.push_back(msghandle::type::fix_int32(1242417314));
		key_hash_list.push_back(msghandle::type::fix_int32(876317117));
		Clear();
	}

void Clear(){
	header={};
	actor_control_enable={};
	brake_light={};
	double_flash_light={};
	low_beam={};
	high_beam={};
	turn_left_light={};
	turn_right_light={};
	auto_horn={};
	front_fog_light={};
	rear_fog_light={};
	tail_light={};
	wiper={};
	door_lock={};
	emergency_stop={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,actor_control_enable,brake_light,double_flash_light,low_beam,high_beam,turn_left_light,turn_right_light,auto_horn,front_fog_light,rear_fog_light,tail_light,wiper,door_lock,emergency_stop);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","actor_control_enable","brake_light","double_flash_light","low_beam","high_beam","turn_left_light","turn_right_light","auto_horn","front_fog_light","rear_fog_light","tail_light","wiper","door_lock","emergency_stop");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,actor_control_enable,brake_light,double_flash_light,low_beam,high_beam,turn_left_light,turn_right_light,auto_horn,front_fog_light,rear_fog_light,tail_light,wiper,door_lock,emergency_stop);
} VehicleSensorControl;
REGISTER(VehicleSensorControl,VehicleSensorControl_Func);
