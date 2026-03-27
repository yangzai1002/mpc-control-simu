#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionCarLightState{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//默认值false
	unsigned int	tail_light_state;	//default_description
	float	tail_light_score;	//默认值0

	PerceptionCarLightState(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(7539101));
		key_hash_list.push_back(msghandle::type::fix_int32(861445871));
		key_hash_list.push_back(msghandle::type::fix_int32(40511993));
		Clear();
	}

void Clear(){
	enable={};
	tail_light_state={};
	tail_light_score={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,tail_light_state,tail_light_score);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","tail_light_state","tail_light_score");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,tail_light_state,tail_light_score);
} PerceptionCarLightState;
REGISTER(PerceptionCarLightState,PerceptionCarLightState_Func);
