#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "LocPoseEuler.h"
/*
定位DR信息
*/

typedef struct LocPoseDR{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	week;	//星期数
	double	utc_time;	//utc时间
	double	dr_time;	//dr 时间
	LocPoseEuler	pose_euler;	//dr 位置信息

	LocPoseDR(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1725452674));
		key_hash_list.push_back(msghandle::type::fix_int32(1197279706));
		key_hash_list.push_back(msghandle::type::fix_int32(1254629416));
		key_hash_list.push_back(msghandle::type::fix_int32(714963895));
		Clear();
	}

void Clear(){
	week={};
	utc_time={};
	dr_time={};
	pose_euler={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,week,utc_time,dr_time,pose_euler);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","week","utc_time","dr_time","pose_euler");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,week,utc_time,dr_time,pose_euler);
} LocPoseDR;
REGISTER(LocPoseDR,LocPoseDR_Func);
