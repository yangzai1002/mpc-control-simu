#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionVecWheelPoints.h"
/*

*/

typedef struct PerceptionWheelPointsResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//默认值false
	std::vector<PerceptionVecWheelPoints>	camera_wheel_points;	//1:crop，默认值0

	PerceptionWheelPointsResult(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1993905844));
		key_hash_list.push_back(msghandle::type::fix_int32(1782243575));
		Clear();
	}

void Clear(){
	valid={};
	camera_wheel_points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,camera_wheel_points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","camera_wheel_points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,camera_wheel_points);
} PerceptionWheelPointsResult;
REGISTER(PerceptionWheelPointsResult,PerceptionWheelPointsResult_Func);
