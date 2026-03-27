#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "CommonHeader.h"
#include "vector"
#include "PlanningTrajectoryPoint.h"
#include "PlanningAdditionalInfo.h"
/*
来源于planning模块，含义为被控车辆未来一段时间内的行驶轨迹
*/

typedef struct PlanningTrajectory{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	CommonHeader	header;	//数据头，包含时间戳等信息
	std::vector<PlanningTrajectoryPoint>	trajectory;	//轨迹点集合，表示自车未来一段时间内的行驶轨迹
	PlanningAdditionalInfo	additional_info;	//附加规划信息，例如轨迹方向，期望加速度，紧急制动等

PlanningTrajectory(){
	trajectory.clear();
	key_hash_list.reserve(8);
	key_hash_list.push_back(msghandle::type::fix_int32(1839877640));
	key_hash_list.push_back(msghandle::type::fix_int32(992394614));
	key_hash_list.push_back(msghandle::type::fix_int32(1349158437));
}

void Clear(){
	trajectory.clear();
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list, header,trajectory,additional_info);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","trajectory","additional_info");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, header,trajectory,additional_info);
} PlanningTrajectory;
REGISTER(PlanningTrajectory,PlanningTrajectory_Func);
