#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
/*

*/

typedef struct MotionPlanDebugMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	float	first_point_v;	//
	float	ego_speed;	//
	float	cruise_speed_max;	//
	float	first_point_kappa;	//
	float	first_point_a;	//
	int	following_object_id;	//
	float	following_object_v;	//
	float	following_object_dis;	//
	bool	is_new_path;	//
	bool	is_fallback_trajectory;	//
	bool	is_replan;	//
	int	replan_reason;	//
	int	yield_object_id;	//
	float	yield_object_v;	//
	float	yield_object_current_s;	//
	float	yield_object_collision_min_s;	//
	float	yield_object_collision_min_t;	//

MotionPlanDebugMsg(){
	first_point_v = 0.0;
	ego_speed = 0.0;
	cruise_speed_max = 0.0;
	first_point_kappa = 0.0;
	first_point_a = 0.0;
	following_object_id = 0;
	following_object_v = 0.0;
	following_object_dis = 0.0;
	is_new_path = false;
	is_fallback_trajectory = false;
	is_replan = false;
	replan_reason = 0;
	yield_object_id = 0;
	yield_object_v = 0.0;
	yield_object_current_s = 0.0;
	yield_object_collision_min_s = 0.0;
	yield_object_collision_min_t = 0.0;
}

void Clear(){
	first_point_v = 0.0;
	ego_speed = 0.0;
	cruise_speed_max = 0.0;
	first_point_kappa = 0.0;
	first_point_a = 0.0;
	following_object_id = 0;
	following_object_v = 0.0;
	following_object_dis = 0.0;
	is_new_path = false;
	is_fallback_trajectory = false;
	is_replan = false;
	replan_reason = 0;
	yield_object_id = 0;
	yield_object_v = 0.0;
	yield_object_current_s = 0.0;
	yield_object_collision_min_s = 0.0;
	yield_object_collision_min_t = 0.0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list, first_point_v,ego_speed,cruise_speed_max,first_point_kappa,first_point_a,following_object_id,following_object_v,following_object_dis,is_new_path,is_fallback_trajectory,is_replan,replan_reason,yield_object_id,yield_object_v,yield_object_current_s,yield_object_collision_min_s,yield_object_collision_min_t);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","first_point_v","ego_speed","cruise_speed_max","first_point_kappa","first_point_a","following_object_id","following_object_v","following_object_dis","is_new_path","is_fallback_trajectory","is_replan","replan_reason","yield_object_id","yield_object_v","yield_object_current_s","yield_object_collision_min_s","yield_object_collision_min_t");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, first_point_v,ego_speed,cruise_speed_max,first_point_kappa,first_point_a,following_object_id,following_object_v,following_object_dis,is_new_path,is_fallback_trajectory,is_replan,replan_reason,yield_object_id,yield_object_v,yield_object_current_s,yield_object_collision_min_s,yield_object_collision_min_t);
} MotionPlanDebugMsg;
REGISTER(MotionPlanDebugMsg,MotionPlanDebugMsg_Func);
