#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
跟随debug静态障碍物信息
*/

typedef struct DebugStaticObstacle{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//障碍物ID
	double	xg;	//全局x坐标
	double	yg;	//全局y坐标
	double	global_angle;	//全局航向角 (单位：度)
	double	length;	//长度
	double	width;	//宽度
	bool	is_colliding_obstacle;	//是否碰撞
	double	time_to_collision;	//该碰撞点对应的自车到达时间 (TTC)
	double	collision_s;	//该碰撞点在自车路径上的弧长

	DebugStaticObstacle(){
		key_hash_list.resize(13);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1111549495));
		key_hash_list.push_back(msghandle::type::fix_int32(1173710937));
		key_hash_list.push_back(msghandle::type::fix_int32(435573183));
		key_hash_list.push_back(msghandle::type::fix_int32(680085183));
		key_hash_list.push_back(msghandle::type::fix_int32(28107049));
		key_hash_list.push_back(msghandle::type::fix_int32(204742748));
		key_hash_list.push_back(msghandle::type::fix_int32(1908349563));
		key_hash_list.push_back(msghandle::type::fix_int32(421663147));
		key_hash_list.push_back(msghandle::type::fix_int32(1109193210));
		Clear();
	}

void Clear(){
	id={};
	xg={};
	yg={};
	global_angle={};
	length={};
	width={};
	is_colliding_obstacle={};
	time_to_collision={};
	collision_s={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,xg,yg,global_angle,length,width,is_colliding_obstacle,time_to_collision,collision_s);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","xg","yg","global_angle","length","width","is_colliding_obstacle","time_to_collision","collision_s");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,xg,yg,global_angle,length,width,is_colliding_obstacle,time_to_collision,collision_s);
} DebugStaticObstacle;
REGISTER(DebugStaticObstacle,DebugStaticObstacle_Func);
