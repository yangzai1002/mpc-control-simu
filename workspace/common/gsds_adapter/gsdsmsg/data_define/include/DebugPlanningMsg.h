#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "DebugStateMsg.h"
#include "DebugUnsCollidMsg.h"
#include "DebugRefpathMsg.h"
#include "DebugDWAMsg.h"
#include "DebugASTARMsg.h"
#include "DebugDriveModeMsg.h"
#include "MotionPlanDebugMsg.h"
/*

*/

typedef struct DebugPlanningMsg{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	DebugStateMsg	main_stream_msg;	//
	DebugStateMsg	task_fsm_condition;	//
	DebugStateMsg	task_fsm_state;	//
	DebugStateMsg	cognition_msg;	//
	DebugStateMsg	business_msg;	//
	DebugStateMsg	pathplan_msg;	//
	DebugStateMsg	behavior_fsm_condition;	//
	DebugStateMsg	behavior_fsm_state;	//
	DebugStateMsg	thread_state;	//
	DebugUnsCollidMsg	uns_collid_msg;	//
	DebugRefpathMsg	refpath_msg;	//
	DebugDWAMsg	DWA_message;	//
	DebugASTARMsg	ASTAR_message;	//
	DebugDriveModeMsg	drive_mode_message;	//
	MotionPlanDebugMsg	motionplan_debug;	//

DebugPlanningMsg(){
	key_hash_list.reserve(20);
	key_hash_list.push_back(msghandle::type::fix_int32(1865318780));
	key_hash_list.push_back(msghandle::type::fix_int32(2135769082));
	key_hash_list.push_back(msghandle::type::fix_int32(1118153728));
	key_hash_list.push_back(msghandle::type::fix_int32(523716828));
	key_hash_list.push_back(msghandle::type::fix_int32(1696169554));
	key_hash_list.push_back(msghandle::type::fix_int32(517421812));
	key_hash_list.push_back(msghandle::type::fix_int32(1364546183));
	key_hash_list.push_back(msghandle::type::fix_int32(416196989));
	key_hash_list.push_back(msghandle::type::fix_int32(904308708));
	key_hash_list.push_back(msghandle::type::fix_int32(1818644158));
	key_hash_list.push_back(msghandle::type::fix_int32(1402128927));
	key_hash_list.push_back(msghandle::type::fix_int32(377690333));
	key_hash_list.push_back(msghandle::type::fix_int32(1289983665));
	key_hash_list.push_back(msghandle::type::fix_int32(694507276));
	key_hash_list.push_back(msghandle::type::fix_int32(11485973));
}

void Clear(){
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, main_stream_msg,task_fsm_condition,task_fsm_state,cognition_msg,business_msg,pathplan_msg,behavior_fsm_condition,behavior_fsm_state,thread_state,uns_collid_msg,refpath_msg,DWA_message,ASTAR_message,drive_mode_message,motionplan_debug);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","main_stream_msg","task_fsm_condition","task_fsm_state","cognition_msg","business_msg","pathplan_msg","behavior_fsm_condition","behavior_fsm_state","thread_state","uns_collid_msg","refpath_msg","DWA_message","ASTAR_message","drive_mode_message","motionplan_debug");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, main_stream_msg,task_fsm_condition,task_fsm_state,cognition_msg,business_msg,pathplan_msg,behavior_fsm_condition,behavior_fsm_state,thread_state,uns_collid_msg,refpath_msg,DWA_message,ASTAR_message,drive_mode_message,motionplan_debug);
} DebugPlanningMsg;
REGISTER(DebugPlanningMsg,DebugPlanningMsg_Func);
