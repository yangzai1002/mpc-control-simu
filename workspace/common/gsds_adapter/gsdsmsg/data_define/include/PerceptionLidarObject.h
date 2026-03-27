#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionRelInfo.h"
#include "PerceptionLWH.h"
/*
感知输出的障碍物目标信息，对外接口
*/

typedef struct PerceptionLidarObject{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//目标id,唯一识别号（预留，用于跟踪用）
	int	type;	//目标种类
	int	age;	//目标存在周期数（预留，用于跟踪用）
	PerceptionRelInfo	vcs_info;	//目标相对坐标 x y z heading vx vy cells ploygen等（vx vy不赋值）
	PerceptionLWH	obj_size;	//目标尺寸 L W H

	PerceptionLidarObject(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1132593808));
		key_hash_list.push_back(msghandle::type::fix_int32(1261709844));
		key_hash_list.push_back(msghandle::type::fix_int32(1965379472));
		key_hash_list.push_back(msghandle::type::fix_int32(81570830));
		key_hash_list.push_back(msghandle::type::fix_int32(436609384));
		Clear();
	}

void Clear(){
	id={};
	type={};
	age={};
	vcs_info={};
	obj_size={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,type,age,vcs_info,obj_size);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","type","age","vcs_info","obj_size");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,type,age,vcs_info,obj_size);
} PerceptionLidarObject;
REGISTER(PerceptionLidarObject,PerceptionLidarObject_Func);