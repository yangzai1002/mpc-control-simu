#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct Restricts{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	restrict_left;	//default_description
	float	restrict_right;	//default_description

	Restricts(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(609362950));
		key_hash_list.push_back(msghandle::type::fix_int32(1221752673));
		Clear();
	}

void Clear(){
	restrict_left={};
	restrict_right={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,restrict_left,restrict_right);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","restrict_left","restrict_right");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,restrict_left,restrict_right);
} Restricts;
REGISTER(Restricts,Restricts_Func);
