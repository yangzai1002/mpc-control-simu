#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
浮点型2维点数据结构
*/

typedef struct CommonPoint2f{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//x坐标
	float	y;	//y坐标

	CommonPoint2f(){
		key_hash_list.resize(6);
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	x={};
	y={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,x,y);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y);
} CommonPoint2f;
REGISTER(CommonPoint2f,CommonPoint2f_Func);
