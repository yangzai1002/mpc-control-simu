#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "VehicleSensorState.h"
#include "VehicleActState.h"
#include "VehicleKeyState.h"
#include "SensorWheelSpeed.h"
/*
车辆底层相关信息
*/

typedef struct VehicleInformation{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	VehicleSensorState	vehicle_sensor_state;	//车辆传感器状态
	VehicleActState	vehicle_act_state;	//车辆执行器状态
	VehicleKeyState	key_status;	//车辆附加按键状态
	SensorWheelSpeed	wheel_speed;	//车辆轮速信息

	VehicleInformation(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1497263368));
		key_hash_list.push_back(msghandle::type::fix_int32(1906140536));
		key_hash_list.push_back(msghandle::type::fix_int32(872863532));
		key_hash_list.push_back(msghandle::type::fix_int32(902974463));
		Clear();
	}

void Clear(){
	header={};
	vehicle_sensor_state={};
	vehicle_act_state={};
	key_status={};
	wheel_speed={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,vehicle_sensor_state,vehicle_act_state,key_status,wheel_speed);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","vehicle_sensor_state","vehicle_act_state","key_status","wheel_speed");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,vehicle_sensor_state,vehicle_act_state,key_status,wheel_speed);
} VehicleInformation;
REGISTER(VehicleInformation,VehicleInformation_Func);
