#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
/*

*/

typedef struct DebugRefpathMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	int	local_path_index;	//
	int	cut_path_index;	//
	bool	is_avoid;	//
	bool	cur_reverse;	//
	int	cur_nearest_index;	//
	int	cur_nearest_origin_index;	//

DebugRefpathMsg(){
	local_path_index = 0;
	cut_path_index = 0;
	is_avoid = false;
	cur_reverse = false;
	cur_nearest_index = 0;
	cur_nearest_origin_index = 0;
}

void Clear(){
	local_path_index = 0;
	cut_path_index = 0;
	is_avoid = false;
	cur_reverse = false;
	cur_nearest_index = 0;
	cur_nearest_origin_index = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list, local_path_index,cut_path_index,is_avoid,cur_reverse,cur_nearest_index,cur_nearest_origin_index);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","local_path_index","cut_path_index","is_avoid","cur_reverse","cur_nearest_index","cur_nearest_origin_index");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, local_path_index,cut_path_index,is_avoid,cur_reverse,cur_nearest_index,cur_nearest_origin_index);
} DebugRefpathMsg;
REGISTER(DebugRefpathMsg,DebugRefpathMsg_Func);
