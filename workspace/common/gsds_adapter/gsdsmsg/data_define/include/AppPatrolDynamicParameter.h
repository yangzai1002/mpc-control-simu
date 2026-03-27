#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct AppPatrolDynamicParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	avoid;	//default_description
	double	speed;	//default_description
	bool	paused;	//default_description

	AppPatrolDynamicParameter(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(70818308));
		key_hash_list.push_back(msghandle::type::fix_int32(1907648842));
		key_hash_list.push_back(msghandle::type::fix_int32(1626964090));
		Clear();
	}

void Clear(){
	avoid={};
	speed={};
	paused={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,avoid,speed,paused);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","avoid","speed","paused");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,avoid,speed,paused);
} AppPatrolDynamicParameter;
REGISTER(AppPatrolDynamicParameter,AppPatrolDynamicParameter_Func);
