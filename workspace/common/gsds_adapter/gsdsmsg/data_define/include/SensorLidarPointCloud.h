#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "SensorLidarPoint.h"
/*
激光雷达点的定义
*/

typedef struct SensorLidarPointCloud{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头信息
	std::vector<SensorLidarPoint>	points;	//points

	SensorLidarPointCloud(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1393712646));
		key_hash_list.push_back(msghandle::type::fix_int32(1194917174));
		Clear();
	}

void Clear(){
	header={};
	points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,points);
} SensorLidarPointCloud;
REGISTER(SensorLidarPointCloud,SensorLidarPointCloud_Func);
