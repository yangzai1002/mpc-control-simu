#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "VehicleKeyBrife.h"
/*
抽象车辆按键状态
*/

typedef struct VehicleKeyState{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<unsigned char>	key_state_info;	//车辆key的二进制信息
	std::vector<VehicleKeyBrife>	key_brifes;	//车辆key的详细信息

	VehicleKeyState(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1451377727));
		key_hash_list.push_back(msghandle::type::fix_int32(1846881094));
		Clear();
	}

void Clear(){
	key_state_info={};
	key_brifes={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,key_state_info,key_brifes);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","key_state_info","key_brifes");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,key_state_info,key_brifes);
} VehicleKeyState;
REGISTER(VehicleKeyState,VehicleKeyState_Func);
