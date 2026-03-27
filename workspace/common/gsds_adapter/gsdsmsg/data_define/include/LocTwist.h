#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "Vector3.h"
/*

*/

typedef struct LocTwist{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	Vector3	velocity;	//线速度
	Vector3	angular_velocity;	//角速度

	LocTwist(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(35904134));
		key_hash_list.push_back(msghandle::type::fix_int32(1646374900));
		Clear();
	}

void Clear(){
	velocity={};
	angular_velocity={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,velocity,angular_velocity);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","velocity","angular_velocity");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,velocity,angular_velocity);
} LocTwist;
REGISTER(LocTwist,LocTwist_Func);
