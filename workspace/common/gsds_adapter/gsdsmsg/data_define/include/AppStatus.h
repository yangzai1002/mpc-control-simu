#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "AppStatusMessage.h"
/*

*/

typedef struct AppStatus{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	AppStatusMessage	record_status_track;	//路径录制状态
	AppStatusMessage	record_status_fence;	//围栏录制状态
	AppStatusMessage	record_status_map;	//地图录制状态

	AppStatus(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(668135335));
		key_hash_list.push_back(msghandle::type::fix_int32(1492046849));
		key_hash_list.push_back(msghandle::type::fix_int32(1946930274));
		Clear();
	}

void Clear(){
	record_status_track={};
	record_status_fence={};
	record_status_map={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,record_status_track,record_status_fence,record_status_map);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","record_status_track","record_status_fence","record_status_map");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,record_status_track,record_status_fence,record_status_map);
} AppStatus;
REGISTER(AppStatus,AppStatus_Func);
