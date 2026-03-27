#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
供数据包录制使用
*/

typedef struct RecordControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	record_state;	//录制开关（0：关闭；1：开启）

	RecordControl(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1216854091));
		Clear();
	}

void Clear(){
	record_state={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,record_state);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","record_state");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,record_state);
} RecordControl;
REGISTER(RecordControl,RecordControl_Func);
