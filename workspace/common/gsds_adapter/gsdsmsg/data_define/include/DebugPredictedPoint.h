#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
跟随障碍物预测轨迹debug预测轨迹上的一个点
*/

typedef struct DebugPredictedPoint{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	relative_time;	//相对于预测起点的相对时间 (秒)
	double	xg;	//预测点的全局x坐标
	double	yg;	//预测点的全局y坐标
	float	global_angle;	//预测点的全局航向角 (单位：度)
	float	velocity;	//预测点的速度 (m/s)

	DebugPredictedPoint(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(659984672));
		key_hash_list.push_back(msghandle::type::fix_int32(1475160197));
		key_hash_list.push_back(msghandle::type::fix_int32(2128758331));
		key_hash_list.push_back(msghandle::type::fix_int32(304412488));
		key_hash_list.push_back(msghandle::type::fix_int32(468962108));
		Clear();
	}

void Clear(){
	relative_time={};
	xg={};
	yg={};
	global_angle={};
	velocity={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,relative_time,xg,yg,global_angle,velocity);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","relative_time","xg","yg","global_angle","velocity");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,relative_time,xg,yg,global_angle,velocity);
} DebugPredictedPoint;
REGISTER(DebugPredictedPoint,DebugPredictedPoint_Func);
