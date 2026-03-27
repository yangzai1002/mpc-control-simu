#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
激光雷达点的定义
*/

typedef struct SensorLidarPoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	time_stamp;	//时间戳（s）
	float	x;	//x位置信息
	float	y;	//y位置信息
	float	z;	//z位置信息
	float	angle;	//角度信息
	unsigned char	intensity;	//反射率信息
	unsigned char	ring;	//线束信息

	SensorLidarPoint(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(582187140));
		key_hash_list.push_back(msghandle::type::fix_int32(361322340));
		key_hash_list.push_back(msghandle::type::fix_int32(318207488));
		key_hash_list.push_back(msghandle::type::fix_int32(275092636));
		key_hash_list.push_back(msghandle::type::fix_int32(1696651539));
		key_hash_list.push_back(msghandle::type::fix_int32(1768772278));
		key_hash_list.push_back(msghandle::type::fix_int32(1017817489));
		Clear();
	}

void Clear(){
	time_stamp={};
	x={};
	y={};
	z={};
	angle={};
	intensity={};
	ring={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,time_stamp,x,y,z,angle,intensity,ring);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","time_stamp","x","y","z","angle","intensity","ring");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,time_stamp,x,y,z,angle,intensity,ring);
} SensorLidarPoint;
REGISTER(SensorLidarPoint,SensorLidarPoint_Func);
