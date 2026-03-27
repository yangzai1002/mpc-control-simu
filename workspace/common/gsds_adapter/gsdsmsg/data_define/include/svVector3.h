#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "Vector3.h"
/*

*/

typedef struct svVector3{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<Vector3>	data;	//

	svVector3(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(435110910));
		Clear();
	}

void Clear(){
	data={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,data);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","data");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,data);
} svVector3;
REGISTER(svVector3,svVector3_Func);
