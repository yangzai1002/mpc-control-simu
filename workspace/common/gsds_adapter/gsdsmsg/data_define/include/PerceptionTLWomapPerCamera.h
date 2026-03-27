#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionTLWomapPerObject.h"
/*

*/

typedef struct PerceptionTLWomapPerCamera{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	camera_name;	//相机名称
	std::vector<PerceptionTLWomapPerObject>	trafficlights;	//当前相机的所有light

	PerceptionTLWomapPerCamera(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1651279343));
		key_hash_list.push_back(msghandle::type::fix_int32(1954148300));
		Clear();
	}

void Clear(){
	camera_name={};
	trafficlights={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,camera_name,trafficlights);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","camera_name","trafficlights");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,camera_name,trafficlights);
} PerceptionTLWomapPerCamera;
REGISTER(PerceptionTLWomapPerCamera,PerceptionTLWomapPerCamera_Func);
