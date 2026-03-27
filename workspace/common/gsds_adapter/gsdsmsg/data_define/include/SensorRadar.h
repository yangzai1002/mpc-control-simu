#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "RadarObstacle.h"
/*
毫米波雷达数据
*/

typedef struct SensorRadar{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//通用数据描述头
	std::vector<RadarObstacle>	obstacles;	//障碍物列表

	SensorRadar(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(224359837));
		key_hash_list.push_back(msghandle::type::fix_int32(1821814320));
		Clear();
	}

void Clear(){
	header={};
	obstacles={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,obstacles);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","obstacles");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,obstacles);
} SensorRadar;
REGISTER(SensorRadar,SensorRadar_Func);
