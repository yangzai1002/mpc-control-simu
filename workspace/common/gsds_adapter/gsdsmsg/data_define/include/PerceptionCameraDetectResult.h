#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionVisionObject.h"
/*

*/

typedef struct PerceptionCameraDetectResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<PerceptionVisionObject>	vision_objects;	//感知模块输出的目标检测结果,从INP迁移

	PerceptionCameraDetectResult(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(869833855));
		Clear();
	}

void Clear(){
	vision_objects={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,vision_objects);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","vision_objects");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,vision_objects);
} PerceptionCameraDetectResult;
REGISTER(PerceptionCameraDetectResult,PerceptionCameraDetectResult_Func);
