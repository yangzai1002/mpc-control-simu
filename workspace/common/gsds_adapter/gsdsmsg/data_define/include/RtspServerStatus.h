#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
RTSP服务器工作状态
*/

typedef struct RtspServerStatus{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	has_player;	//是否播放端：0-无；1-有

	RtspServerStatus(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1292268189));
		Clear();
	}

void Clear(){
	has_player={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,has_player);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","has_player");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,has_player);
} RtspServerStatus;
REGISTER(RtspServerStatus,RtspServerStatus_Func);
