#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
与任务管理设备进行通讯的结构-目前使用于ZMQ通讯
*/

typedef struct TaskManageStatus{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned char	action_id;	//行动的ID
	unsigned char	task_status;	//任务状态
	unsigned char	task_type;	//任务类型
	unsigned char	task_progress;	//行动进度
	double	distance_to_nearest;	//偏航量

	TaskManageStatus(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(897438635));
		key_hash_list.push_back(msghandle::type::fix_int32(1091162689));
		key_hash_list.push_back(msghandle::type::fix_int32(1987895376));
		key_hash_list.push_back(msghandle::type::fix_int32(2127820373));
		key_hash_list.push_back(msghandle::type::fix_int32(252724055));
		Clear();
	}

void Clear(){
	action_id={};
	task_status={};
	task_type={};
	task_progress={};
	distance_to_nearest={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,action_id,task_status,task_type,task_progress,distance_to_nearest);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","action_id","task_status","task_type","task_progress","distance_to_nearest");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,action_id,task_status,task_type,task_progress,distance_to_nearest);
} TaskManageStatus;
REGISTER(TaskManageStatus,TaskManageStatus_Func);
