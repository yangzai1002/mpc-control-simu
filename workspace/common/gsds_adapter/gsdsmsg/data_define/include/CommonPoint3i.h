#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
整型3维点数据结构
*/

typedef struct CommonPoint3i{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	x;	//x坐标
	int	y;	//y坐标
	int	z;	//z坐标

	CommonPoint3i(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1807901464));
		key_hash_list.push_back(msghandle::type::fix_int32(600613140));
		key_hash_list.push_back(msghandle::type::fix_int32(1540808464));
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
} CommonPoint3i;
REGISTER(CommonPoint3i,CommonPoint3i_Func);
