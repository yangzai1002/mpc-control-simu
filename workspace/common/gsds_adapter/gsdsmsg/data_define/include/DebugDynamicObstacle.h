#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "DebugPredictedPoint.h"
/*
跟随单个动态障碍物的完整调试信息
*/

typedef struct DebugDynamicObstacle{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//障碍物的ID
	std::vector<DebugPredictedPoint>	predicted_trajectory;	//障碍物的完整预测轨迹
	bool	is_colliding_obstacle;	//是否有碰撞
	double	time_to_collision;	//该障碍物与自车轨迹的最小碰撞时间 (TTC)
	double	collision_s;	//该碰撞点在自车路径上的弧长
	double	length;	//障碍物长度
	double	width;	//障碍物宽度
	bool	is_static;	//0:运动, 1:静止
	double	speed;	//目标物体速度

	DebugDynamicObstacle(){
		key_hash_list.resize(13);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(859216834));
		key_hash_list.push_back(msghandle::type::fix_int32(1731504700));
		key_hash_list.push_back(msghandle::type::fix_int32(1732361624));
		key_hash_list.push_back(msghandle::type::fix_int32(1953958677));
		key_hash_list.push_back(msghandle::type::fix_int32(1109193210));
		key_hash_list.push_back(msghandle::type::fix_int32(1277288359));
		key_hash_list.push_back(msghandle::type::fix_int32(1451439638));
		key_hash_list.push_back(msghandle::type::fix_int32(1089284816));
		key_hash_list.push_back(msghandle::type::fix_int32(1218588029));
		Clear();
	}

void Clear(){
	id={};
	predicted_trajectory={};
	is_colliding_obstacle={};
	time_to_collision={};
	collision_s={};
	length={};
	width={};
	is_static={};
	speed={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,predicted_trajectory,is_colliding_obstacle,time_to_collision,collision_s,length,width,is_static,speed);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","predicted_trajectory","is_colliding_obstacle","time_to_collision","collision_s","length","width","is_static","speed");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,predicted_trajectory,is_colliding_obstacle,time_to_collision,collision_s,length,width,is_static,speed);
} DebugDynamicObstacle;
REGISTER(DebugDynamicObstacle,DebugDynamicObstacle_Func);
