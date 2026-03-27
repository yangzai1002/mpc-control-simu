#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
常规三维数据
*/

typedef struct Vector3{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//default_description
	double	y;	//default_description
	double	z;	//default_description

	Vector3(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2082294911));
		key_hash_list.push_back(msghandle::type::fix_int32(383420768));
		key_hash_list.push_back(msghandle::type::fix_int32(832030273));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z);
} Vector3;
REGISTER(Vector3,Vector3_Func);
