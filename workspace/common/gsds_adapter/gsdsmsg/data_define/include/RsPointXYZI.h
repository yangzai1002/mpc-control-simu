#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
robosense驱动里的PointXYZI
*/

typedef struct RsPointXYZI{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//x
	float	y;	//y
	float	z;	//z
	float	intensity;	//intensity

	RsPointXYZI(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(741032086));
		key_hash_list.push_back(msghandle::type::fix_int32(1625152754));
		key_hash_list.push_back(msghandle::type::fix_int32(361789774));
		key_hash_list.push_back(msghandle::type::fix_int32(361664010));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
	intensity={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z,intensity);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z","intensity");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z,intensity);
} RsPointXYZI;
REGISTER(RsPointXYZI,RsPointXYZI_Func);
