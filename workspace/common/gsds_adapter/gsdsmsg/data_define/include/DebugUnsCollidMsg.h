#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "DebugSemanticInfo.h"
/*

*/

typedef struct DebugUnsCollidMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	DebugSemanticInfo	front_collision_info;	//
	DebugSemanticInfo	result_collision_info;	//
	double	slow_down_dis;	//

DebugUnsCollidMsg(){
	slow_down_dis = 0.0;
	key_hash_list.reserve(8);
	key_hash_list.push_back(msghandle::type::fix_int32(759747473));
	key_hash_list.push_back(msghandle::type::fix_int32(1840278345));
	key_hash_list.push_back(msghandle::type::fix_int32(1479141489));
}

void Clear(){
	slow_down_dis = 0.0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, front_collision_info,result_collision_info,slow_down_dis);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","front_collision_info","result_collision_info","slow_down_dis");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, front_collision_info,result_collision_info,slow_down_dis);
} DebugUnsCollidMsg;
REGISTER(DebugUnsCollidMsg,DebugUnsCollidMsg_Func);
