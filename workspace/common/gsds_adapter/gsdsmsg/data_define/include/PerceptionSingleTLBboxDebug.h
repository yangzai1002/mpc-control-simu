#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionSingleTLBboxDebug{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned short	xmin;	//Bbox左上角X坐标
	unsigned short	ymin;	//Bbox左上角Y坐标
	unsigned short	xmax;	//Bbox右下角X坐标
	unsigned short	ymax;	//Bbox右下角Y坐标
	double	score;	//Bbox置信度
	bool	is_valid;	//Bbox是否有效

	PerceptionSingleTLBboxDebug(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1640891574));
		key_hash_list.push_back(msghandle::type::fix_int32(1363738098));
		key_hash_list.push_back(msghandle::type::fix_int32(547013556));
		key_hash_list.push_back(msghandle::type::fix_int32(269860080));
		key_hash_list.push_back(msghandle::type::fix_int32(38835533));
		key_hash_list.push_back(msghandle::type::fix_int32(1160510282));
		Clear();
	}

void Clear(){
	xmin={};
	ymin={};
	xmax={};
	ymax={};
	score={};
	is_valid={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,xmin,ymin,xmax,ymax,score,is_valid);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","xmin","ymin","xmax","ymax","score","is_valid");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,xmin,ymin,xmax,ymax,score,is_valid);
} PerceptionSingleTLBboxDebug;
REGISTER(PerceptionSingleTLBboxDebug,PerceptionSingleTLBboxDebug_Func);
