#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "BusinessTaskPoints.h"
/*
车端任务执行状态
*/

typedef struct TaskExecuteStatus{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	type;	//任务类型，0-无任务；1-循迹；2-跟随；3-自主
	int	status;	//0-空闲；1-执行中；2-暂停中；3-GPS位置异常；4-到达任务点；5-用户结束；6-正常结束；7-退自主结束；
	double	current_limit_speed;	//当前限速
	BusinessTaskPoints	task_points;	//当前已经到达的任务点，任务状态为 3 时有效
	double	distance_to_nearest;	//当前自车到引导轨迹最近点位置

	TaskExecuteStatus(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1096661601));
		key_hash_list.push_back(msghandle::type::fix_int32(259039466));
		key_hash_list.push_back(msghandle::type::fix_int32(359708600));
		key_hash_list.push_back(msghandle::type::fix_int32(1439269769));
		key_hash_list.push_back(msghandle::type::fix_int32(1920274429));
		Clear();
	}

void Clear(){
	type={};
	status={};
	current_limit_speed={};
	task_points={};
	distance_to_nearest={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,type,status,current_limit_speed,task_points,distance_to_nearest);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","type","status","current_limit_speed","task_points","distance_to_nearest");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,type,status,current_limit_speed,task_points,distance_to_nearest);
} TaskExecuteStatus;
REGISTER(TaskExecuteStatus,TaskExecuteStatus_Func);
