#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionObject2D.h"
#include "PerceptionTrackState.h"
#include "PerceptionObject3D.h"
#include "PerceptionInstanceMask.h"
#include "PerceptionCarLightState.h"
#include "PerceptionPedestrianState.h"
#include "PerceptionTrafficSign.h"
/*

*/

typedef struct PerceptionVisionObject{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	source_crop;	//1:crop，默认值0
	PerceptionObject2D	object_2d;	//default_description
	PerceptionTrackState	track_state;	//default_description
	PerceptionObject3D	object_3d;	//default_description
	PerceptionInstanceMask	instance_mask;	//default_description
	PerceptionCarLightState	car_light_state;	//default_description
	PerceptionPedestrianState	pedestrian_state;	//default_description
	PerceptionTrafficSign	trafficsign;	//default_description

	PerceptionVisionObject(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(679783648));
		key_hash_list.push_back(msghandle::type::fix_int32(1867339398));
		key_hash_list.push_back(msghandle::type::fix_int32(1626059650));
		key_hash_list.push_back(msghandle::type::fix_int32(155628228));
		key_hash_list.push_back(msghandle::type::fix_int32(42080314));
		key_hash_list.push_back(msghandle::type::fix_int32(55346215));
		key_hash_list.push_back(msghandle::type::fix_int32(1757517468));
		key_hash_list.push_back(msghandle::type::fix_int32(46524599));
		Clear();
	}

void Clear(){
	source_crop={};
	object_2d={};
	track_state={};
	object_3d={};
	instance_mask={};
	car_light_state={};
	pedestrian_state={};
	trafficsign={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,source_crop,object_2d,track_state,object_3d,instance_mask,car_light_state,pedestrian_state,trafficsign);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","source_crop","object_2d","track_state","object_3d","instance_mask","car_light_state","pedestrian_state","trafficsign");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,source_crop,object_2d,track_state,object_3d,instance_mask,car_light_state,pedestrian_state,trafficsign);
} PerceptionVisionObject;
REGISTER(PerceptionVisionObject,PerceptionVisionObject_Func);
