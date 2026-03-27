#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "string"
/*

*/

typedef struct DebugDriveModeMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	std::string	drive_mode;	//
	int	obj_id;	//
	float	obj_dis_rel;	//
	float	slow_down_dis;	//
	float	guided_speed;	//
	float	velocity_limit;	//
	std::string	info;	//

DebugDriveModeMsg(){
	drive_mode = "";
	obj_id = 0;
	obj_dis_rel = 0.0;
	slow_down_dis = 0.0;
	guided_speed = 0.0;
	velocity_limit = 0.0;
	info = "";
	key_hash_list.reserve(12);
	key_hash_list.push_back(msghandle::type::fix_int32(1986930699));
	key_hash_list.push_back(msghandle::type::fix_int32(633385423));
	key_hash_list.push_back(msghandle::type::fix_int32(1780865965));
	key_hash_list.push_back(msghandle::type::fix_int32(1623447108));
	key_hash_list.push_back(msghandle::type::fix_int32(1321337787));
	key_hash_list.push_back(msghandle::type::fix_int32(477059056));
	key_hash_list.push_back(msghandle::type::fix_int32(82282985));
}

void Clear(){
	drive_mode = "";
	obj_id = 0;
	obj_dis_rel = 0.0;
	slow_down_dis = 0.0;
	guided_speed = 0.0;
	velocity_limit = 0.0;
	info = "";
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, drive_mode,obj_id,obj_dis_rel,slow_down_dis,guided_speed,velocity_limit,info);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","drive_mode","obj_id","obj_dis_rel","slow_down_dis","guided_speed","velocity_limit","info");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, drive_mode,obj_id,obj_dis_rel,slow_down_dis,guided_speed,velocity_limit,info);
} DebugDriveModeMsg;
REGISTER(DebugDriveModeMsg,DebugDriveModeMsg_Func);
