#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "LocPoseEuler.h"
#include "LocPoseQuaternion.h"
/*
定位详细信息
*/

typedef struct LocPosition{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	LocPoseEuler	pose_euler;	//定位 欧拉角信息
	LocPoseQuaternion	pose_quaternion;	//定位 四元数信息
	char	status;	//定位 状态信息 todo

	LocPosition(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1348857520));
		key_hash_list.push_back(msghandle::type::fix_int32(729693251));
		key_hash_list.push_back(msghandle::type::fix_int32(830277985));
		Clear();
	}

void Clear(){
	pose_euler={};
	pose_quaternion={};
	status={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,pose_euler,pose_quaternion,status);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","pose_euler","pose_quaternion","status");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,pose_euler,pose_quaternion,status);
} LocPosition;
REGISTER(LocPosition,LocPosition_Func);
