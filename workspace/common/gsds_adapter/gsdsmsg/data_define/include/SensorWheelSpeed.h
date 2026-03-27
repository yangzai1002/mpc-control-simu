#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
标准轮速信息
*/

typedef struct SensorWheelSpeed{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头
	int	week;	//星期数
	double	utc_time;	//utc时间
	float	left_rear_speed;	//左后轮速（m/s）
	float	right_rear_speed;	//右后轮速（m/s）
	float	left_front_speed;	//左前轮速（m/s）
	float	right_front_speed;	//右前轮速（m/s）
	int	pluse_mask;	//帧计数

	SensorWheelSpeed(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1462902908));
		key_hash_list.push_back(msghandle::type::fix_int32(1725452674));
		key_hash_list.push_back(msghandle::type::fix_int32(1197279706));
		key_hash_list.push_back(msghandle::type::fix_int32(946996030));
		key_hash_list.push_back(msghandle::type::fix_int32(97780988));
		key_hash_list.push_back(msghandle::type::fix_int32(70620677));
		key_hash_list.push_back(msghandle::type::fix_int32(1345964241));
		key_hash_list.push_back(msghandle::type::fix_int32(434561517));
		Clear();
	}

void Clear(){
	header={};
	week={};
	utc_time={};
	left_rear_speed={};
	right_rear_speed={};
	left_front_speed={};
	right_front_speed={};
	pluse_mask={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,week,utc_time,left_rear_speed,right_rear_speed,left_front_speed,right_front_speed,pluse_mask);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","week","utc_time","left_rear_speed","right_rear_speed","left_front_speed","right_front_speed","pluse_mask");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,week,utc_time,left_rear_speed,right_rear_speed,left_front_speed,right_front_speed,pluse_mask);
} SensorWheelSpeed;
REGISTER(SensorWheelSpeed,SensorWheelSpeed_Func);
