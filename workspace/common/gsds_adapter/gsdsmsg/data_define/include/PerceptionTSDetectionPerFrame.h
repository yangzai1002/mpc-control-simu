#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionSingleTSBbox.h"
/*

*/

typedef struct PerceptionTSDetectionPerFrame{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	camera_name;	//相机名称
	std::vector<PerceptionSingleTSBbox>	detect_objs;	//当前帧标识牌bbox

	PerceptionTSDetectionPerFrame(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1651279343));
		key_hash_list.push_back(msghandle::type::fix_int32(1480634724));
		Clear();
	}

void Clear(){
	camera_name={};
	detect_objs={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,camera_name,detect_objs);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","camera_name","detect_objs");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,camera_name,detect_objs);
} PerceptionTSDetectionPerFrame;
REGISTER(PerceptionTSDetectionPerFrame,PerceptionTSDetectionPerFrame_Func);
