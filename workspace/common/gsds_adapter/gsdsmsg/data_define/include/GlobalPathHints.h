#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "LocLLHH.h"
/*
点结构
*/

typedef struct GlobalPathHints{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<LocLLHH>	loc_llhhs;	//多个关键点

	GlobalPathHints(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1897181017));
		Clear();
	}

void Clear(){
	loc_llhhs={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,loc_llhhs);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","loc_llhhs");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,loc_llhhs);
} GlobalPathHints;
REGISTER(GlobalPathHints,GlobalPathHints_Func);
