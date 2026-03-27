#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "BusinessTaskPoint.h"
/*

*/

typedef struct BusinessTaskPoints{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<BusinessTaskPoint>	task_points;	//default_description

	BusinessTaskPoints(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1477838228));
		Clear();
	}

void Clear(){
	task_points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,task_points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","task_points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,task_points);
} BusinessTaskPoints;
REGISTER(BusinessTaskPoints,BusinessTaskPoints_Func);
