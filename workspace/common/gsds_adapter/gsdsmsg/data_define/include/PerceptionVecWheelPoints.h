#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionWheelPoints.h"
/*

*/

typedef struct PerceptionVecWheelPoints{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<PerceptionWheelPoints>	camera_wheel_pts;	//车轮接地点检测结果中间存放容器

	PerceptionVecWheelPoints(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1316853217));
		Clear();
	}

void Clear(){
	camera_wheel_pts={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,camera_wheel_pts);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","camera_wheel_pts");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,camera_wheel_pts);
} PerceptionVecWheelPoints;
REGISTER(PerceptionVecWheelPoints,PerceptionVecWheelPoints_Func);
