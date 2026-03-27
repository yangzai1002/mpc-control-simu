#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct AppStatusMessage{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	status;	//状态(IDLE, RUNNING, PAUSED, ERROR)
	std::string	error_msg;	//错误描述

	AppStatusMessage(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(94621530));
		key_hash_list.push_back(msghandle::type::fix_int32(304500555));
		Clear();
	}

void Clear(){
	status={};
	error_msg={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,status,error_msg);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","status","error_msg");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,status,error_msg);
} AppStatusMessage;
REGISTER(AppStatusMessage,AppStatusMessage_Func);
