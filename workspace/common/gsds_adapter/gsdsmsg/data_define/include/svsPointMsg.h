#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "sPointMsg.h"
/*

*/

typedef struct svsPointMsg{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<sPointMsg>	data;	//

	svsPointMsg(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1596771062));
		Clear();
	}

void Clear(){
	data={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,data);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","data");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,data);
} svsPointMsg;
REGISTER(svsPointMsg,svsPointMsg_Func);
