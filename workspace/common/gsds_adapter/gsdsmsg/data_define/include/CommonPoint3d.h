#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
双精度浮点型3维点数据结构
*/

typedef struct CommonPoint3d{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//x坐标
	double	y;	//y坐标
	double	z;	//z坐标

	CommonPoint3d(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(571140852));
		key_hash_list.push_back(msghandle::type::fix_int32(1511336176));
		key_hash_list.push_back(msghandle::type::fix_int32(304047852));
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
} CommonPoint3d;
REGISTER(CommonPoint3d,CommonPoint3d_Func);
