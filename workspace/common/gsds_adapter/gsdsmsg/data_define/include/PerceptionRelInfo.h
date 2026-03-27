#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint3f.h"
#include "CommonPoint2f.h"
/*
感知目标相对信息（位置、朝向、速度、栅格、轮廓）
*/

typedef struct PerceptionRelInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//x坐标
	float	y;	//y坐标
	float	z;	//z坐标
	float	heading;	//朝向, 单位：度, 范围(-180~180]
	float	vx;	//x方向相对速度
	float	vy;	//y方向相对速度
	std::vector<CommonPoint3f>	cells;	//目标栅格点
	std::vector<CommonPoint2f>	polys;	//目标轮廓点

	PerceptionRelInfo(){
		key_hash_list.resize(12);
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	x={};
	y={};
	z={};
	heading={};
	vx={};
	vy={};
	cells={};
	polys={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z,heading,vx,vy,cells,polys);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z","heading","vx","vy","cells","polys");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z,heading,vx,vy,cells,polys);
} PerceptionRelInfo;
REGISTER(PerceptionRelInfo,PerceptionRelInfo_Func);
