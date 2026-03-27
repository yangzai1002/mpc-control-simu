#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionCameraDetectResult.h"
/*

*/

typedef struct PerceptionDetectionResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//默认值false
	std::vector<PerceptionCameraDetectResult>	camera_vision_objects;	//感知模块输出的目标检测结果,从INP迁移

	PerceptionDetectionResult(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1993905844));
		key_hash_list.push_back(msghandle::type::fix_int32(923988179));
		Clear();
	}

void Clear(){
	valid={};
	camera_vision_objects={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,camera_vision_objects);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","camera_vision_objects");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,camera_vision_objects);
} PerceptionDetectionResult;
REGISTER(PerceptionDetectionResult,PerceptionDetectionResult_Func);
