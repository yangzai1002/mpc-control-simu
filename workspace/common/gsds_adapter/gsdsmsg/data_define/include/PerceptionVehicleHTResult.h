#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionVecPairObject2D.h"
/*

*/

typedef struct PerceptionVehicleHTResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//默认值false
	std::vector<PerceptionVecPairObject2D>	camera_vehicle_ht_objects;	//车头车尾灯检测结果

	PerceptionVehicleHTResult(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1993905844));
		key_hash_list.push_back(msghandle::type::fix_int32(667820067));
		Clear();
	}

void Clear(){
	valid={};
	camera_vehicle_ht_objects={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,camera_vehicle_ht_objects);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","camera_vehicle_ht_objects");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,camera_vehicle_ht_objects);
} PerceptionVehicleHTResult;
REGISTER(PerceptionVehicleHTResult,PerceptionVehicleHTResult_Func);
