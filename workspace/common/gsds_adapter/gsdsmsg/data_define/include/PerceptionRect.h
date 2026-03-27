#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
感知视觉2D目标框
*/

typedef struct PerceptionRect{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//目标框左上角点的x坐标
	float	y;	//目标框左上角点的y坐标
	float	w;	//目标框像素宽度
	float	h;	//目标框像素高度

	PerceptionRect(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1582653057));
		key_hash_list.push_back(msghandle::type::fix_int32(683625309));
		key_hash_list.push_back(msghandle::type::fix_int32(1258598694));
		key_hash_list.push_back(msghandle::type::fix_int32(536640725));
		Clear();
	}

void Clear(){
	x={};
	y={};
	w={};
	h={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,w,h);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","w","h");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,w,h);
} PerceptionRect;
REGISTER(PerceptionRect,PerceptionRect_Func);
