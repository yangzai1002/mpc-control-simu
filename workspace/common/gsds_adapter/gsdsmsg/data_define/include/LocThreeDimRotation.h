#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
三维旋转角度
*/

typedef struct LocThreeDimRotation{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	roll;	//滚转角 度  [-90 90]
	float	pitch;	//俯仰角 度  [-90 90]
	float	yaw;	//偏航角 度  [0,360]

	LocThreeDimRotation(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(627663370));
		key_hash_list.push_back(msghandle::type::fix_int32(670757136));
		key_hash_list.push_back(msghandle::type::fix_int32(1009468541));
		Clear();
	}

void Clear(){
	roll={};
	pitch={};
	yaw={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,roll,pitch,yaw);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","roll","pitch","yaw");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,roll,pitch,yaw);
} LocThreeDimRotation;
REGISTER(LocThreeDimRotation,LocThreeDimRotation_Func);
