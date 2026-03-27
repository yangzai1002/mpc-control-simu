#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
整数类型的 Flag
*/

typedef struct FlagInt{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	value;	//

	FlagInt(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1729917750));
		Clear();
	}

void Clear(){
	value={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,value);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","value");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,value);
} FlagInt;
REGISTER(FlagInt,FlagInt_Func);
