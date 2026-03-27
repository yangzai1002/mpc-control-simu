#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
浮点型3维点数据结构
*/

typedef struct CommonPoint3f{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//x坐标
	float	y;	//y坐标
	float	z;	//z坐标

	CommonPoint3f(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1837225697));
		key_hash_list.push_back(msghandle::type::fix_int32(629937373));
		key_hash_list.push_back(msghandle::type::fix_int32(1570132697));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z);
} CommonPoint3f;
REGISTER(CommonPoint3f,CommonPoint3f_Func);
