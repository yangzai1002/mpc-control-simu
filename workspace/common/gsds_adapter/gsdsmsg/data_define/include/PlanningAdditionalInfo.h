#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
/*

*/

typedef struct PlanningAdditionalInfo{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	int	direction;	//轨迹的方向，0为前进，1为倒车
	float	acc_desired;	//期望加速度，单位m/s^2
	bool	emergency_stop;	//紧急制动标志位，0为default，1为需要紧急制动
	bool	is_follow;	//跟随标志位

PlanningAdditionalInfo(){
	direction = 0;
	acc_desired = 0.0;
	emergency_stop = false;
	is_follow = false;
}

void Clear(){
	direction = 0;
	acc_desired = 0.0;
	emergency_stop = false;
	is_follow = false;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list, direction,acc_desired,emergency_stop,is_follow);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","direction","acc_desired","emergency_stop","is_follow");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, direction,acc_desired,emergency_stop,is_follow);
} PlanningAdditionalInfo;
REGISTER(PlanningAdditionalInfo,PlanningAdditionalInfo_Func);
