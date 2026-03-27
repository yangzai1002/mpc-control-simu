#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint2f.h"
/*
超声波栅格数据
*/

typedef struct PerceptionUtraData{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	trigger_stamp;	//数据生成时间
	int	id;	//探头id，一个探头的结果，有多个同id的栅格组成
	float	dis;	//探头检出的距离信息，单位：米
	float	angle;	//探头安装的朝向，单位：度, 范围(-180~180]
	std::vector<CommonPoint2f>	cells;	//栅格相对坐标

	PerceptionUtraData(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(777986188));
		key_hash_list.push_back(msghandle::type::fix_int32(2037287505));
		key_hash_list.push_back(msghandle::type::fix_int32(1109515283));
		key_hash_list.push_back(msghandle::type::fix_int32(1710189551));
		key_hash_list.push_back(msghandle::type::fix_int32(687190157));
		Clear();
	}

void Clear(){
	trigger_stamp={};
	id={};
	dis={};
	angle={};
	cells={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,trigger_stamp,id,dis,angle,cells);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","trigger_stamp","id","dis","angle","cells");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,trigger_stamp,id,dis,angle,cells);
} PerceptionUtraData;
REGISTER(PerceptionUtraData,PerceptionUtraData_Func);
