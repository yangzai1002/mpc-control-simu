#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
车辆模型说明
*/

typedef struct VehicleModel{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	length;	//车辆长度
	float	width;	//车辆宽度
	float	wheelbase;	//车辆轴距
	float	front_axle_tofront;	//前轴距车头距离
	float	wheel_center;	//方向盘零点中心
	float	min_turning_radius;	//最小转弯半径
	float	max_speed;	//车辆最大速度(m/s)
	float	max_accelaration;	//车辆最大加速度(m/s^2)
	float	max_braking_value;	//车辆最大减速度(m/s^2)
	float	min_drive_speed;	//最小前进速度(m/s)
	float	slide_accelaration;	//滑行加速度
	float	eps_transmission_ratio;	//EPS传动比
	float	right_max_steerangle;	//方向盘右转最大值
	double	front_over_hang;	//default_description todo
	double	back_over_hang;	//default_description todo
	float	half_wheel;	//车轮半径
	float	target_min_turn_radius;	//目标最小转弯半径
	int	trafficability;	//车辆通行能力
	int	negative_trafficability;	//负障碍物通行能力
	double	turning_around_min_radius;	//default_description todo

	VehicleModel(){
		key_hash_list.resize(24);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1685781467));
		key_hash_list.push_back(msghandle::type::fix_int32(468194448));
		key_hash_list.push_back(msghandle::type::fix_int32(873663248));
		key_hash_list.push_back(msghandle::type::fix_int32(304909569));
		key_hash_list.push_back(msghandle::type::fix_int32(492017774));
		key_hash_list.push_back(msghandle::type::fix_int32(61532821));
		key_hash_list.push_back(msghandle::type::fix_int32(1281545332));
		key_hash_list.push_back(msghandle::type::fix_int32(222893062));
		key_hash_list.push_back(msghandle::type::fix_int32(1129712872));
		key_hash_list.push_back(msghandle::type::fix_int32(910258133));
		key_hash_list.push_back(msghandle::type::fix_int32(801760815));
		key_hash_list.push_back(msghandle::type::fix_int32(151151727));
		key_hash_list.push_back(msghandle::type::fix_int32(2145386440));
		key_hash_list.push_back(msghandle::type::fix_int32(571811108));
		key_hash_list.push_back(msghandle::type::fix_int32(1427323192));
		key_hash_list.push_back(msghandle::type::fix_int32(239639170));
		key_hash_list.push_back(msghandle::type::fix_int32(976340730));
		key_hash_list.push_back(msghandle::type::fix_int32(1054492741));
		key_hash_list.push_back(msghandle::type::fix_int32(850361639));
		key_hash_list.push_back(msghandle::type::fix_int32(836309772));
		Clear();
	}

void Clear(){
	length={};
	width={};
	wheelbase={};
	front_axle_tofront={};
	wheel_center={};
	min_turning_radius={};
	max_speed={};
	max_accelaration={};
	max_braking_value={};
	min_drive_speed={};
	slide_accelaration={};
	eps_transmission_ratio={};
	right_max_steerangle={};
	front_over_hang={};
	back_over_hang={};
	half_wheel={};
	target_min_turn_radius={};
	trafficability={};
	negative_trafficability={};
	turning_around_min_radius={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,length,width,wheelbase,front_axle_tofront,wheel_center,min_turning_radius,max_speed,max_accelaration,max_braking_value,min_drive_speed,slide_accelaration,eps_transmission_ratio,right_max_steerangle,front_over_hang,back_over_hang,half_wheel,target_min_turn_radius,trafficability,negative_trafficability,turning_around_min_radius);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","length","width","wheelbase","front_axle_tofront","wheel_center","min_turning_radius","max_speed","max_accelaration","max_braking_value","min_drive_speed","slide_accelaration","eps_transmission_ratio","right_max_steerangle","front_over_hang","back_over_hang","half_wheel","target_min_turn_radius","trafficability","negative_trafficability","turning_around_min_radius");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,length,width,wheelbase,front_axle_tofront,wheel_center,min_turning_radius,max_speed,max_accelaration,max_braking_value,min_drive_speed,slide_accelaration,eps_transmission_ratio,right_max_steerangle,front_over_hang,back_over_hang,half_wheel,target_min_turn_radius,trafficability,negative_trafficability,turning_around_min_radius);
} VehicleModel;
REGISTER(VehicleModel,VehicleModel_Func);
