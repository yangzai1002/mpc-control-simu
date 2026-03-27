#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
感知目标车道线位置信息
*/

typedef struct PerceptionSLInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	cam_id;	//摄像头编号
	bool	left_laneline_valid;	//目标左侧车道线是否有效
	int	left_laneline_id;	//目标左侧车道线id
	float	left_to_laneline;	//目标到其左侧车道线的距离 为负值 (sl坐标系的L坐标 左正右负)，单位：米
	bool	right_laneline_valid;	//目标右侧车道线是否有效
	int	right_laneline_id;	//目标右侧车道线id
	float	right_to_laneline;	//目标到其右侧车道线的距离 为正值 (sl坐标系的L坐标 左正右负)，单位：米
	float	left_to_right;	//目标自身左右距离（宽度），单位：米

	PerceptionSLInfo(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1729537797));
		key_hash_list.push_back(msghandle::type::fix_int32(1113037093));
		key_hash_list.push_back(msghandle::type::fix_int32(2003009708));
		key_hash_list.push_back(msghandle::type::fix_int32(1625776818));
		key_hash_list.push_back(msghandle::type::fix_int32(143397123));
		key_hash_list.push_back(msghandle::type::fix_int32(1515650794));
		key_hash_list.push_back(msghandle::type::fix_int32(938757987));
		key_hash_list.push_back(msghandle::type::fix_int32(604244108));
		Clear();
	}

void Clear(){
	cam_id={};
	left_laneline_valid={};
	left_laneline_id={};
	left_to_laneline={};
	right_laneline_valid={};
	right_laneline_id={};
	right_to_laneline={};
	left_to_right={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,cam_id,left_laneline_valid,left_laneline_id,left_to_laneline,right_laneline_valid,right_laneline_id,right_to_laneline,left_to_right);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","cam_id","left_laneline_valid","left_laneline_id","left_to_laneline","right_laneline_valid","right_laneline_id","right_to_laneline","left_to_right");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,cam_id,left_laneline_valid,left_laneline_id,left_to_laneline,right_laneline_valid,right_laneline_id,right_to_laneline,left_to_right);
} PerceptionSLInfo;
REGISTER(PerceptionSLInfo,PerceptionSLInfo_Func);
