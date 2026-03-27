#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
一个灯板框Bbox信息
*/

typedef struct PerceptionTLWomapBbox{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	id;	//唯一标识id
	unsigned int	xmin;	//左上角X
	unsigned int	ymin;	//左上角Y
	unsigned int	xmax;	//有下角X
	unsigned int	ymax;	//右下角Y
	float	score;	//模型输出得分
	bool	is_valid;	//是否有效？true表示有效

	PerceptionTLWomapBbox(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(780018070));
		key_hash_list.push_back(msghandle::type::fix_int32(1638636711));
		key_hash_list.push_back(msghandle::type::fix_int32(1257064819));
		key_hash_list.push_back(msghandle::type::fix_int32(1571849787));
		key_hash_list.push_back(msghandle::type::fix_int32(990697409));
		key_hash_list.push_back(msghandle::type::fix_int32(1295243733));
		key_hash_list.push_back(msghandle::type::fix_int32(1181971357));
		Clear();
	}

void Clear(){
	id={};
	xmin={};
	ymin={};
	xmax={};
	ymax={};
	score={};
	is_valid={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,xmin,ymin,xmax,ymax,score,is_valid);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","xmin","ymin","xmax","ymax","score","is_valid");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,xmin,ymin,xmax,ymax,score,is_valid);
} PerceptionTLWomapBbox;
REGISTER(PerceptionTLWomapBbox,PerceptionTLWomapBbox_Func);
