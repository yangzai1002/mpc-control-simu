#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
感知目标绝对坐标（DR或者UTM）信息
*/

typedef struct PerceptionAbsInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//有效标识
	double	xg;	//xg绝对坐标，单位:米
	double	yg;	//yg绝对坐标，单位:米
	float	heading;	//绝对坐标下的朝向, 单位：度, 范围(-180~180]
	float	vxg;	//xg方向绝对速度, 单位:米每秒
	float	vyg;	//yg方向绝对速度, 单位:米每秒
	float	speed;	//绝对速度, 单位:米每秒

	PerceptionAbsInfo(){
		key_hash_list.reserve(11);
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	valid={};
	xg={};
	yg={};
	heading={};
	vxg={};
	vyg={};
	speed={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,xg,yg,heading,vxg,vyg,speed);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","xg","yg","heading","vxg","vyg","speed");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,xg,yg,heading,vxg,vyg,speed);
} PerceptionAbsInfo;
REGISTER(PerceptionAbsInfo,PerceptionAbsInfo_Func);
