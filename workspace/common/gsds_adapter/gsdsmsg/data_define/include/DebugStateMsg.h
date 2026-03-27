#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
/*

*/

typedef struct DebugStateMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	std::string	str;	//
	int	value;	//

DebugStateMsg(){
	str = "";
	value = 0;
	key_hash_list.reserve(7);
	key_hash_list.push_back(msghandle::type::fix_int32(1394205942));
	key_hash_list.push_back(msghandle::type::fix_int32(1900193509));
}

void Clear(){
	str = "";
	value = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, str,value);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","str","value");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, str,value);
} DebugStateMsg;
REGISTER(DebugStateMsg,DebugStateMsg_Func);
