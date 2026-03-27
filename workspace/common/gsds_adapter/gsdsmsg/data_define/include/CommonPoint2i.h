#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
整型2维点数据结构
*/

typedef struct CommonPoint2i{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	x;	//x坐标
	int	y;	//y坐标

	CommonPoint2i(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1807901464));
		key_hash_list.push_back(msghandle::type::fix_int32(600613140));
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
} CommonPoint2i;
REGISTER(CommonPoint2i,CommonPoint2i_Func);
