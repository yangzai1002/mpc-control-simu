#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "DebugDynamicObstacle.h"
/*
跟随速度规划模块的总调试信息
*/

typedef struct SpeedPlanDebugInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::vector<DebugDynamicObstacle>	dynamic_obstacles_info;	//所有动态检测器处理过的障碍物的完整信息

	SpeedPlanDebugInfo(){
		key_hash_list.resize(5);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1263673147));
		Clear();
	}

void Clear(){
	dynamic_obstacles_info={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,dynamic_obstacles_info);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","dynamic_obstacles_info");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,dynamic_obstacles_info);
} SpeedPlanDebugInfo;
REGISTER(SpeedPlanDebugInfo,SpeedPlanDebugInfo_Func);
