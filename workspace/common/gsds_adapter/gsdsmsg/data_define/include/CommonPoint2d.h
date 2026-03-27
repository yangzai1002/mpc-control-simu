#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
双精度2维点数据结构
*/

typedef struct CommonPoint2d{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//x坐标
	double	y;	//y坐标

	CommonPoint2d(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(571140852));
		key_hash_list.push_back(msghandle::type::fix_int32(1511336176));
		Clear();
	}

void Clear(){
	x={};
	y={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y);
} CommonPoint2d;
REGISTER(CommonPoint2d,CommonPoint2d_Func);
