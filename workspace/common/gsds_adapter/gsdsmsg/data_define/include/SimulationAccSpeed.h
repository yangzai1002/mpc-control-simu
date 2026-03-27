#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct SimulationAccSpeed{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	speed;	//速度
	float	acceleration;	//加速度

	SimulationAccSpeed(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(515640322));
		key_hash_list.push_back(msghandle::type::fix_int32(88003538));
		Clear();
	}

void Clear(){
	speed={};
	acceleration={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,speed,acceleration);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","speed","acceleration");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,speed,acceleration);
} SimulationAccSpeed;
REGISTER(SimulationAccSpeed,SimulationAccSpeed_Func);
