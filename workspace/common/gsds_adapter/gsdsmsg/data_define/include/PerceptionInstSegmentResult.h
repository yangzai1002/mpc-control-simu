#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionCameraInstSegResult.h"
/*

*/

typedef struct PerceptionInstSegmentResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//默认false
	std::vector<PerceptionCameraInstSegResult>	camera_instseg_result;	//default_description

	PerceptionInstSegmentResult(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2016489123));
		key_hash_list.push_back(msghandle::type::fix_int32(1667120603));
		Clear();
	}

void Clear(){
	valid={};
	camera_instseg_result={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,camera_instseg_result);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","camera_instseg_result");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,camera_instseg_result);
} PerceptionInstSegmentResult;
REGISTER(PerceptionInstSegmentResult,PerceptionInstSegmentResult_Func);
