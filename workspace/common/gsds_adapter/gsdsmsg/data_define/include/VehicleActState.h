#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
车辆执行器相关信息
*/

typedef struct VehicleActState{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	steer_angle;	//方向盘转角
	float	steer_curvature;	//车辆转向曲率
	float	speed;	//车辆速度
	float	acceleration;	//车辆加速度
	float	accpedal_position;	//加速踏板位置
	float	brkpedal_position;	//刹车踏板位置
	unsigned char	shift_position;	//档位信息
	unsigned char	epb_status;	//EPB状态信息
	unsigned char	accpedal_state;	//加速踏板状态
	unsigned char	brkpedal_state;	//刹车踏板状态
	unsigned char	drive_override;	//控制权限 0: 驾驶员未接管 1: 驾驶员接管
	unsigned char	drive_mode;	//车辆模式
	int	fault_code;	//底盘故障码反馈

	VehicleActState(){
		key_hash_list.resize(17);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(358388284));
		key_hash_list.push_back(msghandle::type::fix_int32(279750888));
		key_hash_list.push_back(msghandle::type::fix_int32(1250491903));
		key_hash_list.push_back(msghandle::type::fix_int32(1509303737));
		key_hash_list.push_back(msghandle::type::fix_int32(559706369));
		key_hash_list.push_back(msghandle::type::fix_int32(1264994141));
		key_hash_list.push_back(msghandle::type::fix_int32(31668487));
		key_hash_list.push_back(msghandle::type::fix_int32(871502994));
		key_hash_list.push_back(msghandle::type::fix_int32(974430986));
		key_hash_list.push_back(msghandle::type::fix_int32(1588877498));
		key_hash_list.push_back(msghandle::type::fix_int32(1223952435));
		key_hash_list.push_back(msghandle::type::fix_int32(955415695));
		key_hash_list.push_back(msghandle::type::fix_int32(16029146));
		Clear();
	}

void Clear(){
	steer_angle={};
	steer_curvature={};
	speed={};
	acceleration={};
	accpedal_position={};
	brkpedal_position={};
	shift_position={};
	epb_status={};
	accpedal_state={};
	brkpedal_state={};
	drive_override={};
	drive_mode={};
	fault_code={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,steer_angle,steer_curvature,speed,acceleration,accpedal_position,brkpedal_position,shift_position,epb_status,accpedal_state,brkpedal_state,drive_override,drive_mode,fault_code);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","steer_angle","steer_curvature","speed","acceleration","accpedal_position","brkpedal_position","shift_position","epb_status","accpedal_state","brkpedal_state","drive_override","drive_mode","fault_code");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,steer_angle,steer_curvature,speed,acceleration,accpedal_position,brkpedal_position,shift_position,epb_status,accpedal_state,brkpedal_state,drive_override,drive_mode,fault_code);
} VehicleActState;
REGISTER(VehicleActState,VehicleActState_Func);
