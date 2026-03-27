#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
车辆传感器相关信息
*/

typedef struct VehicleSensorState{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned char	brake_light_state;	//刹车灯状态
	unsigned char	doubleflash_light_state;	//双闪灯状态
	unsigned char	lowbeam_state;	//近光灯状态
	unsigned char	highbeam_state;	//远光灯状态
	unsigned char	leftturn_light_state;	//左转灯状态
	unsigned char	rightturn_light_state;	//右转灯状态
	unsigned char	frontfog_light_state;	//前雾灯状态
	unsigned char	rearfog_light_state;	//后雾灯状态
	unsigned char	tail_light_state;	//尾灯状态
	unsigned char	horn_state;	//鸣笛状态
	unsigned char	wiper_state;	//状态
	unsigned char	cruise_switch_state;	//自适应巡航
	unsigned char	driver_door_lock_state;	//驾驶门锁状态
	unsigned char	passager_door_lock_state;	//乘客门锁状态
	unsigned char	left_rear_door_lock_state;	//左后门锁状态
	unsigned char	right_rear_door_lock_state;	//右后门锁状态
	unsigned char	trunk_state;	//后备箱门状态
	unsigned char	driver_door_state;	//驾驶门状态
	unsigned char	passager_door_state;	//乘客门状态
	unsigned char	left_rear_door_state;	//左后门状态
	unsigned char	right_rear_door_state;	//右后门状态
	unsigned char	driver_seatbeltrst;	//驾驶员安全带状态
	unsigned char	passager_seatbeltrst;	//乘客安全带状态
	float	bms_battery_soc;	//BMS电池状态
	float	fuel_left_over;	//剩余油量【电量】
	float	vehicle_total_mile;	//车辆行驶里程
	unsigned char	turn_light_switch;	//灯光开关状态
	unsigned char	vehicle_mode;	//车辆模式
	unsigned char	aeb_active;	//AEB状态
	unsigned char	remote_en_state;	//遥控使能状态

	VehicleSensorState(){
		key_hash_list.resize(34);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1902556745));
		key_hash_list.push_back(msghandle::type::fix_int32(176376512));
		key_hash_list.push_back(msghandle::type::fix_int32(1195203158));
		key_hash_list.push_back(msghandle::type::fix_int32(1642026291));
		key_hash_list.push_back(msghandle::type::fix_int32(856205960));
		key_hash_list.push_back(msghandle::type::fix_int32(544828162));
		key_hash_list.push_back(msghandle::type::fix_int32(600811133));
		key_hash_list.push_back(msghandle::type::fix_int32(1404566678));
		key_hash_list.push_back(msghandle::type::fix_int32(2124645432));
		key_hash_list.push_back(msghandle::type::fix_int32(2031416168));
		key_hash_list.push_back(msghandle::type::fix_int32(665159808));
		key_hash_list.push_back(msghandle::type::fix_int32(415383521));
		key_hash_list.push_back(msghandle::type::fix_int32(2021083148));
		key_hash_list.push_back(msghandle::type::fix_int32(271250142));
		key_hash_list.push_back(msghandle::type::fix_int32(2078078447));
		key_hash_list.push_back(msghandle::type::fix_int32(2045245647));
		key_hash_list.push_back(msghandle::type::fix_int32(1743675698));
		key_hash_list.push_back(msghandle::type::fix_int32(888925612));
		key_hash_list.push_back(msghandle::type::fix_int32(1147561282));
		key_hash_list.push_back(msghandle::type::fix_int32(576723793));
		key_hash_list.push_back(msghandle::type::fix_int32(265345995));
		key_hash_list.push_back(msghandle::type::fix_int32(2046370456));
		key_hash_list.push_back(msghandle::type::fix_int32(20131128));
		key_hash_list.push_back(msghandle::type::fix_int32(705223895));
		key_hash_list.push_back(msghandle::type::fix_int32(526384332));
		key_hash_list.push_back(msghandle::type::fix_int32(827711057));
		key_hash_list.push_back(msghandle::type::fix_int32(1081617732));
		key_hash_list.push_back(msghandle::type::fix_int32(1890301305));
		key_hash_list.push_back(msghandle::type::fix_int32(870649454));
		key_hash_list.push_back(msghandle::type::fix_int32(958771087));
		Clear();
	}

void Clear(){
	brake_light_state={};
	doubleflash_light_state={};
	lowbeam_state={};
	highbeam_state={};
	leftturn_light_state={};
	rightturn_light_state={};
	frontfog_light_state={};
	rearfog_light_state={};
	tail_light_state={};
	horn_state={};
	wiper_state={};
	cruise_switch_state={};
	driver_door_lock_state={};
	passager_door_lock_state={};
	left_rear_door_lock_state={};
	right_rear_door_lock_state={};
	trunk_state={};
	driver_door_state={};
	passager_door_state={};
	left_rear_door_state={};
	right_rear_door_state={};
	driver_seatbeltrst={};
	passager_seatbeltrst={};
	bms_battery_soc={};
	fuel_left_over={};
	vehicle_total_mile={};
	turn_light_switch={};
	vehicle_mode={};
	aeb_active={};
	remote_en_state={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,brake_light_state,doubleflash_light_state,lowbeam_state,highbeam_state,leftturn_light_state,rightturn_light_state,frontfog_light_state,rearfog_light_state,tail_light_state,horn_state,wiper_state,cruise_switch_state,driver_door_lock_state,passager_door_lock_state,left_rear_door_lock_state,right_rear_door_lock_state,trunk_state,driver_door_state,passager_door_state,left_rear_door_state,right_rear_door_state,driver_seatbeltrst,passager_seatbeltrst,bms_battery_soc,fuel_left_over,vehicle_total_mile,turn_light_switch,vehicle_mode,aeb_active,remote_en_state);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","brake_light_state","doubleflash_light_state","lowbeam_state","highbeam_state","leftturn_light_state","rightturn_light_state","frontfog_light_state","rearfog_light_state","tail_light_state","horn_state","wiper_state","cruise_switch_state","driver_door_lock_state","passager_door_lock_state","left_rear_door_lock_state","right_rear_door_lock_state","trunk_state","driver_door_state","passager_door_state","left_rear_door_state","right_rear_door_state","driver_seatbeltrst","passager_seatbeltrst","bms_battery_soc","fuel_left_over","vehicle_total_mile","turn_light_switch","vehicle_mode","aeb_active","remote_en_state");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,brake_light_state,doubleflash_light_state,lowbeam_state,highbeam_state,leftturn_light_state,rightturn_light_state,frontfog_light_state,rearfog_light_state,tail_light_state,horn_state,wiper_state,cruise_switch_state,driver_door_lock_state,passager_door_lock_state,left_rear_door_lock_state,right_rear_door_lock_state,trunk_state,driver_door_state,passager_door_state,left_rear_door_state,right_rear_door_state,driver_seatbeltrst,passager_seatbeltrst,bms_battery_soc,fuel_left_over,vehicle_total_mile,turn_light_switch,vehicle_mode,aeb_active,remote_en_state);
} VehicleSensorState;
REGISTER(VehicleSensorState,VehicleSensorState_Func);
