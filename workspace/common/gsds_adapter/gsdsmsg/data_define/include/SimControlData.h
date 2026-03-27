#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct SimControlData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	throttle;	//
	float	brake;	//
	float	steer_angle;	//
	float	is_back;	//
	int	turn_light;	//

	SimControlData(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1663496856));
		key_hash_list.push_back(msghandle::type::fix_int32(1186032803));
		key_hash_list.push_back(msghandle::type::fix_int32(1441206205));
		key_hash_list.push_back(msghandle::type::fix_int32(356547650));
		key_hash_list.push_back(msghandle::type::fix_int32(822539685));
		Clear();
	}

void Clear(){
	throttle={};
	brake={};
	steer_angle={};
	is_back={};
	turn_light={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,throttle,brake,steer_angle,is_back,turn_light);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","throttle","brake","steer_angle","is_back","turn_light");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,throttle,brake,steer_angle,is_back,turn_light);
} SimControlData;
REGISTER(SimControlData,SimControlData_Func);
