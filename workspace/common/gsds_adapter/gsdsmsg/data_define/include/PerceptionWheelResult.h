#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionVecPairObject2D.h"
/*

*/

typedef struct PerceptionWheelResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//默认值false
	std::vector<PerceptionVecPairObject2D>	camera_wheel_objects;	//default_description

	PerceptionWheelResult(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1993905844));
		key_hash_list.push_back(msghandle::type::fix_int32(961171874));
		Clear();
	}

void Clear(){
	valid={};
	camera_wheel_objects={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,camera_wheel_objects);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","camera_wheel_objects");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,camera_wheel_objects);
} PerceptionWheelResult;
REGISTER(PerceptionWheelResult,PerceptionWheelResult_Func);
