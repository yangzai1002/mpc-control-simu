#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "LocThreeDimPose.h"
#include "LocThreeDimRotation.h"
/*

*/

typedef struct LocPoseEuler{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	LocThreeDimPose	position;	//定位三维位置信息
	LocThreeDimRotation	rotation;	//三维旋转角度

	LocPoseEuler(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1153404672));
		key_hash_list.push_back(msghandle::type::fix_int32(834136938));
		Clear();
	}

void Clear(){
	position={};
	rotation={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,position,rotation);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","position","rotation");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,position,rotation);
} LocPoseEuler;
REGISTER(LocPoseEuler,LocPoseEuler_Func);
