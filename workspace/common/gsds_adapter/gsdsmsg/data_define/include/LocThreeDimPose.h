#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
三维位置信息
*/

typedef struct LocThreeDimPose{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//x轴坐标
	double	y;	//y轴坐标
	double	z;	//z轴坐标

	LocThreeDimPose(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1937432363));
		key_hash_list.push_back(msghandle::type::fix_int32(835400383));
		key_hash_list.push_back(msghandle::type::fix_int32(1880852051));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z);
} LocThreeDimPose;
REGISTER(LocThreeDimPose,LocThreeDimPose_Func);
