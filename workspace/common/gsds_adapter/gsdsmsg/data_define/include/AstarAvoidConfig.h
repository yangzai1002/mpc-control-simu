#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct AstarAvoidConfig{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	resolution;	//default_description
	bool	use_ultra;	//default_description
	bool	is_final_adjust;	//default_description

	AstarAvoidConfig(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1303366477));
		key_hash_list.push_back(msghandle::type::fix_int32(1769806308));
		key_hash_list.push_back(msghandle::type::fix_int32(865471777));
		Clear();
	}

void Clear(){
	resolution={};
	use_ultra={};
	is_final_adjust={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,resolution,use_ultra,is_final_adjust);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","resolution","use_ultra","is_final_adjust");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,resolution,use_ultra,is_final_adjust);
} AstarAvoidConfig;
REGISTER(AstarAvoidConfig,AstarAvoidConfig_Func);
