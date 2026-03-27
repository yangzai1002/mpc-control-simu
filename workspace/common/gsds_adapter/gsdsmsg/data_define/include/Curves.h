#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "Curve.h"
/*

*/

typedef struct Curves{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<Curve>	curves;	//多条曲线

	Curves(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2137447667));
		Clear();
	}

void Clear(){
	curves={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,curves);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","curves");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,curves);
} Curves;
REGISTER(Curves,Curves_Func);
