#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"

// 使用命名空间隔离自定义的Pose，避免和生成的impl_type_pose.h冲突
namespace gsds {

/*
姿态信息
*/
typedef struct Pose{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	x;	//坐标x
	double	y;	//坐标y
	double	angle;	//角度

	Pose(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1457385738));
		key_hash_list.push_back(msghandle::type::fix_int32(394424190));
		key_hash_list.push_back(msghandle::type::fix_int32(1258167122));
		Clear();
	}

void Clear(){
	x={};
	y={};
	angle={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,angle);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","angle");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,angle);
} Pose;

} // namespace gsds

REGISTER(gsds::Pose, Pose_Func);
