#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
车辆纵向控制信息
*/

typedef struct VehicleLonControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	target_speed;	//目标速度
	float	target_accelation;	//目标加速度
	float	target_torque;	//目标纵向扭矩
	float	target_pressure;	//目标纵向压力
	unsigned char	actuator_mode;	//执行器模式
	unsigned char	shift_position;	//目标档位0: P 1:R 2:N 3:D 4:T
	unsigned char	epb_mode;	//EPB 模式

	VehicleLonControl(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1046747913));
		key_hash_list.push_back(msghandle::type::fix_int32(1555363648));
		key_hash_list.push_back(msghandle::type::fix_int32(888153552));
		key_hash_list.push_back(msghandle::type::fix_int32(581707644));
		key_hash_list.push_back(msghandle::type::fix_int32(1098138574));
		key_hash_list.push_back(msghandle::type::fix_int32(950190646));
		key_hash_list.push_back(msghandle::type::fix_int32(1330884342));
		Clear();
	}

void Clear(){
	target_speed={};
	target_accelation={};
	target_torque={};
	target_pressure={};
	actuator_mode={};
	shift_position={};
	epb_mode={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,target_speed,target_accelation,target_torque,target_pressure,actuator_mode,shift_position,epb_mode);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","target_speed","target_accelation","target_torque","target_pressure","actuator_mode","shift_position","epb_mode");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,target_speed,target_accelation,target_torque,target_pressure,actuator_mode,shift_position,epb_mode);
} VehicleLonControl;
REGISTER(VehicleLonControl,VehicleLonControl_Func);
