#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionPedestrianState{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//默认值false
	unsigned int	pedestrian_pose;	//默认值0
	float	pedestrian_pose_score;	//默认值0

	PerceptionPedestrianState(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(7539101));
		key_hash_list.push_back(msghandle::type::fix_int32(65517784));
		key_hash_list.push_back(msghandle::type::fix_int32(241924595));
		Clear();
	}

void Clear(){
	enable={};
	pedestrian_pose={};
	pedestrian_pose_score={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,pedestrian_pose,pedestrian_pose_score);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","pedestrian_pose","pedestrian_pose_score");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,pedestrian_pose,pedestrian_pose_score);
} PerceptionPedestrianState;
REGISTER(PerceptionPedestrianState,PerceptionPedestrianState_Func);
