#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
感知单个红绿灯的状态信息
*/

typedef struct PerceptionTrafficLight{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	id;	//红绿灯id,与地图中的id标记相匹配
	int	flicker;	//红绿灯闪烁状态, 0:正常 1:闪烁红灯 2:闪烁黄灯 3:闪烁绿灯 4:长黑 5:长黄闪烁 6:其他异常
	int	light_type;	//红绿灯类型，待定
	bool	occlude;	//false: no occlude; true: occluded
	int	number;	//红绿灯数字
	float	confidence;	//How confidence about the detected results, between 0 and 1.
	float	tracking_time;	//Duration of the traffic light since detected.

	PerceptionTrafficLight(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1968160117));
		key_hash_list.push_back(msghandle::type::fix_int32(1336167394));
		key_hash_list.push_back(msghandle::type::fix_int32(110282096));
		key_hash_list.push_back(msghandle::type::fix_int32(349071112));
		key_hash_list.push_back(msghandle::type::fix_int32(1343564573));
		key_hash_list.push_back(msghandle::type::fix_int32(208253382));
		key_hash_list.push_back(msghandle::type::fix_int32(1938334530));
		Clear();
	}

void Clear(){
	id={};
	flicker={};
	light_type={};
	occlude={};
	number={};
	confidence={};
	tracking_time={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,flicker,light_type,occlude,number,confidence,tracking_time);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","flicker","light_type","occlude","number","confidence","tracking_time");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,flicker,light_type,occlude,number,confidence,tracking_time);
} PerceptionTrafficLight;
REGISTER(PerceptionTrafficLight,PerceptionTrafficLight_Func);
