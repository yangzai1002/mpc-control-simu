#pragma once
//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"


/*

*/

typedef struct PlanningTrajectoryPoint{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	float	x;	// 右手相对坐标系，纵向位置信息（m）
	float	y;	// 右手相对坐标系，横向位置信息（m）
	double	xg;	// 绝对坐标
	double	yg;	// 绝对坐标
	float	angle;	// 相对角度
	float	angleglobal;	// 绝对角度
	float	velocity;	// 规划速度
	float	a;	// 规划加速度
	float	t;	// 预计到达时间
	float	curvature;	// 曲率
	float	length;	// 长度
	char	direction;	// 方向
	int	risk_level;	// 危险等级，0：无，1：低，2：中，3：高（AEB）
	int	type;	// 0:默认 1:原地转向

PlanningTrajectoryPoint(){
	key_hash_list.reserve(19);
	key_hash_list.push_back(msghandle::type::fix_int32(1149891325));
	key_hash_list.push_back(msghandle::type::fix_int32(1149891326));
	key_hash_list.push_back(msghandle::type::fix_int32(1670265429));
	key_hash_list.push_back(msghandle::type::fix_int32(1670265560));
	key_hash_list.push_back(msghandle::type::fix_int32(1608247208));
	key_hash_list.push_back(msghandle::type::fix_int32(1115085967));
	key_hash_list.push_back(msghandle::type::fix_int32(38390912));
	key_hash_list.push_back(msghandle::type::fix_int32(1149891302));
	key_hash_list.push_back(msghandle::type::fix_int32(1149891321));
	key_hash_list.push_back(msghandle::type::fix_int32(1090739032));
	key_hash_list.push_back(msghandle::type::fix_int32(1059731509));
	key_hash_list.push_back(msghandle::type::fix_int32(1895764606));
	key_hash_list.push_back(msghandle::type::fix_int32(993967888));
	key_hash_list.push_back(msghandle::type::fix_int32(682534686));
	Clear();
}

void Clear(){
	x = {};
	y = {};
	xg = {};
	yg = {};
	angle = {};
	angleglobal = {};
	velocity = {};
	a = {};
	t = {};
	curvature = {};
	length = {};
	direction = {};
	risk_level = {};
	type = {};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, x, y, xg, yg, angle, angleglobal, velocity, a, t, curvature, length, direction, risk_level, type);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list", "x", "y", "xg", "yg", "angle", "angleglobal", "velocity", "a", "t", "curvature", "length", "direction", "risk_level", "type");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, x, y, xg, yg, angle, angleglobal, velocity, a, t, curvature, length, direction, risk_level, type);
} PlanningTrajectoryPoint;
REGISTER(PlanningTrajectoryPoint,PlanningTrajectoryPoint_Func);