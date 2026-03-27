#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
四元数
*/

typedef struct LocQuaternion{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//default_description
	double	y;	//default_description
	double	z;	//default_description
	double	w;	//default_description

	LocQuaternion(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2082294911));
		key_hash_list.push_back(msghandle::type::fix_int32(383420768));
		key_hash_list.push_back(msghandle::type::fix_int32(832030273));
		key_hash_list.push_back(msghandle::type::fix_int32(1633685406));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
	w={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z,w);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z","w");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z,w);
} LocQuaternion;
REGISTER(LocQuaternion,LocQuaternion_Func);
