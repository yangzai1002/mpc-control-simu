#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
/*
车辆纵向控制Debug信息
*/

typedef struct LoncontrolDebugStruct{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	double	sbj_speed;	//车辆速度
	double	virtual_speed;	//预瞄速度
	double	speed_p;	//
	double	speed_i;	//
	double	target_acel;	//
	double	acel_real_raw;	//
	double	sbj_acel;	//
	double	acel_p;	//
	double	acel_i;	//
	double	torque;	//
	double	pressure;	//
	double	temp1;	//
	double	temp2;	//
	double	temp3;	//
	double	temp4;	//
	double	path_length;	//
	double	point_zero_length;	//
	int	emergency_obstacle;	//
	double	real_steer_angle;	//
	int	is_shift_position;	//
	double	preview_distance;	//
	int	path_direction;	//
	double	pitch;	//
	double	collision_obj_x;	//
	double	collision_obj_y;	//
	double	collision_cell_x;	//
	double	collision_cell_y;	//
	double	follow_acc_input;	//
	double	follow_acc_offset;	//
	double	follow_acc_offset_p;	//
	double	follow_acc_offset_i;	//
	double	follow_acc_output;	//
	int	emergency_stop_flag;	//
	int	openloop_flag;	//开环控制停车标志位

LoncontrolDebugStruct(){
	sbj_speed = 0.0;
	virtual_speed = 0.0;
	speed_p = 0.0;
	speed_i = 0.0;
	target_acel = 0.0;
	acel_real_raw = 0.0;
	sbj_acel = 0.0;
	acel_p = 0.0;
	acel_i = 0.0;
	torque = 0.0;
	pressure = 0.0;
	temp1 = 0.0;
	temp2 = 0.0;
	temp3 = 0.0;
	temp4 = 0.0;
	path_length = 0.0;
	point_zero_length = 0.0;
	emergency_obstacle = 0;
	real_steer_angle = 0.0;
	is_shift_position = 0;
	preview_distance = 0.0;
	path_direction = 0;
	pitch = 0.0;
	collision_obj_x = 0.0;
	collision_obj_y = 0.0;
	collision_cell_x = 0.0;
	collision_cell_y = 0.0;
	follow_acc_input = 0.0;
	follow_acc_offset = 0.0;
	follow_acc_offset_p = 0.0;
	follow_acc_offset_i = 0.0;
	follow_acc_output = 0.0;
	emergency_stop_flag = 0;
	openloop_flag = 0;
}

void Clear(){
	sbj_speed = 0.0;
	virtual_speed = 0.0;
	speed_p = 0.0;
	speed_i = 0.0;
	target_acel = 0.0;
	acel_real_raw = 0.0;
	sbj_acel = 0.0;
	acel_p = 0.0;
	acel_i = 0.0;
	torque = 0.0;
	pressure = 0.0;
	temp1 = 0.0;
	temp2 = 0.0;
	temp3 = 0.0;
	temp4 = 0.0;
	path_length = 0.0;
	point_zero_length = 0.0;
	emergency_obstacle = 0;
	real_steer_angle = 0.0;
	is_shift_position = 0;
	preview_distance = 0.0;
	path_direction = 0;
	pitch = 0.0;
	collision_obj_x = 0.0;
	collision_obj_y = 0.0;
	collision_cell_x = 0.0;
	collision_cell_y = 0.0;
	follow_acc_input = 0.0;
	follow_acc_offset = 0.0;
	follow_acc_offset_p = 0.0;
	follow_acc_offset_i = 0.0;
	follow_acc_output = 0.0;
	emergency_stop_flag = 0;
	openloop_flag = 0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list, sbj_speed,virtual_speed,speed_p,speed_i,target_acel,acel_real_raw,sbj_acel,acel_p,acel_i,torque,pressure,temp1,temp2,temp3,temp4,path_length,point_zero_length,emergency_obstacle,real_steer_angle,is_shift_position,preview_distance,path_direction,pitch,collision_obj_x,collision_obj_y,collision_cell_x,collision_cell_y,follow_acc_input,follow_acc_offset,follow_acc_offset_p,follow_acc_offset_i,follow_acc_output,emergency_stop_flag,openloop_flag);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","sbj_speed","virtual_speed","speed_p","speed_i","target_acel","acel_real_raw","sbj_acel","acel_p","acel_i","torque","pressure","temp1","temp2","temp3","temp4","path_length","point_zero_length","emergency_obstacle","real_steer_angle","is_shift_position","preview_distance","path_direction","pitch","collision_obj_x","collision_obj_y","collision_cell_x","collision_cell_y","follow_acc_input","follow_acc_offset","follow_acc_offset_p","follow_acc_offset_i","follow_acc_output","emergency_stop_flag","openloop_flag");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, sbj_speed,virtual_speed,speed_p,speed_i,target_acel,acel_real_raw,sbj_acel,acel_p,acel_i,torque,pressure,temp1,temp2,temp3,temp4,path_length,point_zero_length,emergency_obstacle,real_steer_angle,is_shift_position,preview_distance,path_direction,pitch,collision_obj_x,collision_obj_y,collision_cell_x,collision_cell_y,follow_acc_input,follow_acc_offset,follow_acc_offset_p,follow_acc_offset_i,follow_acc_output,emergency_stop_flag,openloop_flag);
} LoncontrolDebugStruct;
REGISTER(LoncontrolDebugStruct,LoncontrolDebugStruct_Func);
