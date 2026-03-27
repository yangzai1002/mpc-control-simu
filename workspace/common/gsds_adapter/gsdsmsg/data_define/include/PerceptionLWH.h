#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
感知障碍物目标长宽高尺寸,单位：米
*/

typedef struct PerceptionLWH{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	width;	//长度，单位:米
	float	length;	//宽度，单位:米
	float	height;	//高度，单位:米

	PerceptionLWH(){
		key_hash_list.resize(7);
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	width={};
	length={};
	height={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,width,length,height);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","width","length","height");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,width,length,height);
} PerceptionLWH;
REGISTER(PerceptionLWH,PerceptionLWH_Func);
