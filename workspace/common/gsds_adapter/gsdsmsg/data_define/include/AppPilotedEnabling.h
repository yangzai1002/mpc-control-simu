#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
自主模式使能
*/

typedef struct AppPilotedEnabling{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	enabling;	//0-非自主模式；1-自主模式；

	AppPilotedEnabling(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089855586));
		Clear();
	}

void Clear(){
	enabling={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enabling);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enabling");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enabling);
} AppPilotedEnabling;
REGISTER(AppPilotedEnabling,AppPilotedEnabling_Func);
