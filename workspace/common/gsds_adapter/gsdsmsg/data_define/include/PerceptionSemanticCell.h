#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
语义地图栅格
*/

typedef struct PerceptionSemanticCell{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//栅格x坐标
	float	y;	//栅格y坐标
	float	dh;	//栅格高度m,衡量不同的通行能力

	PerceptionSemanticCell(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1811352140));
		key_hash_list.push_back(msghandle::type::fix_int32(1285453472));
		key_hash_list.push_back(msghandle::type::fix_int32(286179531));
		Clear();
	}

void Clear(){
	x={};
	y={};
	dh={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,dh);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","dh");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,dh);
} PerceptionSemanticCell;
REGISTER(PerceptionSemanticCell,PerceptionSemanticCell_Func);
