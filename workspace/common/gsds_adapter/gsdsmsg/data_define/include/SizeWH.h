#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
尺寸宽高，int
*/

typedef struct SizeWH{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	w;	//宽
	int	h;	//高

	SizeWH(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1718880304));
		key_hash_list.push_back(msghandle::type::fix_int32(1861832227));
		Clear();
	}

void Clear(){
	w={};
	h={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,w,h);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","w","h");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,w,h);
} SizeWH;
REGISTER(SizeWH,SizeWH_Func);
