#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "LocThreeDimPose.h"
#include "LocQuaternion.h"
#include "Vector3.h"
/*
定位四元数信息
*/

typedef struct LocPoseQuaternion{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	LocThreeDimPose	position;	//定位三维位置信息
	LocQuaternion	quaternion;	//定位四元数信息
	Vector3	rpy_covariance;	//default_description   todo

	LocPoseQuaternion(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1153404672));
		key_hash_list.push_back(msghandle::type::fix_int32(1505192668));
		key_hash_list.push_back(msghandle::type::fix_int32(1524620045));
		Clear();
	}

void Clear(){
	position={};
	quaternion={};
	rpy_covariance={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,position,quaternion,rpy_covariance);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","position","quaternion","rpy_covariance");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,position,quaternion,rpy_covariance);
} LocPoseQuaternion;
REGISTER(LocPoseQuaternion,LocPoseQuaternion_Func);
