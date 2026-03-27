#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionPairObject2D.h"
/*

*/

typedef struct PerceptionVecPairObject2D{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<PerceptionPairObject2D>	ht_obj;	//PerceptionPairObject2D容器

	PerceptionVecPairObject2D(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(686732066));
		Clear();
	}

void Clear(){
	ht_obj={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,ht_obj);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","ht_obj");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,ht_obj);
} PerceptionVecPairObject2D;
REGISTER(PerceptionVecPairObject2D,PerceptionVecPairObject2D_Func);
