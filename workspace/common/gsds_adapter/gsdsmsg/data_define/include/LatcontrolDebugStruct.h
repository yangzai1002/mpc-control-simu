#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
/*
车辆横向控制Debug信息
*/

typedef struct LatcontrolDebugStruct{
	std::vector<msghandle::type::fix_int32> key_hash_list;
	double	PreTra_lat_error;	//预测轨迹的车辆横向偏差
	int	pos_matched_index;	//预测轨迹的车辆位置匹配点
	double	InputAngle_delta_t;	//定向时遥控输入角度间隔时间
	double	direction_init_t;	//定向初始化时间
	double	midback;	//
	double	midfront;	//
	double	heading;	//
	double	leftfront;	//
	double	rightfront;	//
	double	leftback;	//
	double	rightback;	//
	double	dist_preview;	//
	double	x_previewpoint;	//
	double	y_previewpoint;	//
	double	R;	//
	double	purepursuitangle;	//
	double	temp1;	//
	double	temp2;	//
	double	temp3;	//
	double	temp4;	//
	double	num_iteration_final;	//
	double	tolerance_final;	//
	double	num_iteration;	//
	double	tolerance;	//
	double	lqr_angle;	//
	double	purepursuit_angle;	//
	double	x_err_raw;	//
	double	y_err_raw;	//
	double	x_err;	//
	double	y_err;	//
	double	heading_err_normal_before;	//
	double	heading_err_normal_after;	//
	double	heading_err_filter_before;	//
	double	heading_err_filter_after;	//
	double	lqr_angle_raw;	//
	double	steer_angle_xerr_contribution;	//
	double	steer_angle_yerr_contribution;	//
	double	steer_angle_headingerr_contribution;	//
	double	real_delta;	//
	double	delta_out;	//
	double	trace_path_x1;	//
	double	trace_path_x2;	//
	double	trace_path_x3;	//
	double	trace_path_x4;	//
	double	trace_path_y1;	//
	double	trace_path_y2;	//
	double	trace_path_y3;	//
	double	trace_path_y4;	//
	int	path_size;	//路径点数量
	double	InputAngle_start_t;	//定向时遥控输入角度初始时间
	double	direction_start_time;	//定向初始时间

LatcontrolDebugStruct(){
	PreTra_lat_error = 0.0;
	pos_matched_index = 0;
	InputAngle_delta_t = 0.0;
	direction_init_t = 0.0;
	midback = 0.0;
	midfront = 0.0;
	heading = 0.0;
	leftfront = 0.0;
	rightfront = 0.0;
	leftback = 0.0;
	rightback = 0.0;
	dist_preview = 0.0;
	x_previewpoint = 0.0;
	y_previewpoint = 0.0;
	R = 0.0;
	purepursuitangle = 0.0;
	temp1 = 0.0;
	temp2 = 0.0;
	temp3 = 0.0;
	temp4 = 0.0;
	num_iteration_final = 0.0;
	tolerance_final = 0.0;
	num_iteration = 0.0;
	tolerance = 0.0;
	lqr_angle = 0.0;
	purepursuit_angle = 0.0;
	x_err_raw = 0.0;
	y_err_raw = 0.0;
	x_err = 0.0;
	y_err = 0.0;
	heading_err_normal_before = 0.0;
	heading_err_normal_after = 0.0;
	heading_err_filter_before = 0.0;
	heading_err_filter_after = 0.0;
	lqr_angle_raw = 0.0;
	steer_angle_xerr_contribution = 0.0;
	steer_angle_yerr_contribution = 0.0;
	steer_angle_headingerr_contribution = 0.0;
	real_delta = 0.0;
	delta_out = 0.0;
	trace_path_x1 = 0.0;
	trace_path_x2 = 0.0;
	trace_path_x3 = 0.0;
	trace_path_x4 = 0.0;
	trace_path_y1 = 0.0;
	trace_path_y2 = 0.0;
	trace_path_y3 = 0.0;
	trace_path_y4 = 0.0;
	path_size = 0;
	InputAngle_start_t = 0.0;
	direction_start_time = 0.0;
}

void Clear(){
	PreTra_lat_error = 0.0;
	pos_matched_index = 0;
	InputAngle_delta_t = 0.0;
	direction_init_t = 0.0;
	midback = 0.0;
	midfront = 0.0;
	heading = 0.0;
	leftfront = 0.0;
	rightfront = 0.0;
	leftback = 0.0;
	rightback = 0.0;
	dist_preview = 0.0;
	x_previewpoint = 0.0;
	y_previewpoint = 0.0;
	R = 0.0;
	purepursuitangle = 0.0;
	temp1 = 0.0;
	temp2 = 0.0;
	temp3 = 0.0;
	temp4 = 0.0;
	num_iteration_final = 0.0;
	tolerance_final = 0.0;
	num_iteration = 0.0;
	tolerance = 0.0;
	lqr_angle = 0.0;
	purepursuit_angle = 0.0;
	x_err_raw = 0.0;
	y_err_raw = 0.0;
	x_err = 0.0;
	y_err = 0.0;
	heading_err_normal_before = 0.0;
	heading_err_normal_after = 0.0;
	heading_err_filter_before = 0.0;
	heading_err_filter_after = 0.0;
	lqr_angle_raw = 0.0;
	steer_angle_xerr_contribution = 0.0;
	steer_angle_yerr_contribution = 0.0;
	steer_angle_headingerr_contribution = 0.0;
	real_delta = 0.0;
	delta_out = 0.0;
	trace_path_x1 = 0.0;
	trace_path_x2 = 0.0;
	trace_path_x3 = 0.0;
	trace_path_x4 = 0.0;
	trace_path_y1 = 0.0;
	trace_path_y2 = 0.0;
	trace_path_y3 = 0.0;
	trace_path_y4 = 0.0;
	path_size = 0;
	InputAngle_start_t = 0.0;
	direction_start_time = 0.0;
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list, PreTra_lat_error,pos_matched_index,InputAngle_delta_t,direction_init_t,midback,midfront,heading,leftfront,rightfront,leftback,rightback,dist_preview,x_previewpoint,y_previewpoint,R,purepursuitangle,temp1,temp2,temp3,temp4,num_iteration_final,tolerance_final,num_iteration,tolerance,lqr_angle,purepursuit_angle,x_err_raw,y_err_raw,x_err,y_err,heading_err_normal_before,heading_err_normal_after,heading_err_filter_before,heading_err_filter_after,lqr_angle_raw,steer_angle_xerr_contribution,steer_angle_yerr_contribution,steer_angle_headingerr_contribution,real_delta,delta_out,trace_path_x1,trace_path_x2,trace_path_x3,trace_path_x4,trace_path_y1,trace_path_y2,trace_path_y3,trace_path_y4,path_size,InputAngle_start_t,direction_start_time);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","PreTra_lat_error","pos_matched_index","InputAngle_delta_t","direction_init_t","midback","midfront","heading","leftfront","rightfront","leftback","rightback","dist_preview","x_previewpoint","y_previewpoint","R","purepursuitangle","temp1","temp2","temp3","temp4","num_iteration_final","tolerance_final","num_iteration","tolerance","lqr_angle","purepursuit_angle","x_err_raw","y_err_raw","x_err","y_err","heading_err_normal_before","heading_err_normal_after","heading_err_filter_before","heading_err_filter_after","lqr_angle_raw","steer_angle_xerr_contribution","steer_angle_yerr_contribution","steer_angle_headingerr_contribution","real_delta","delta_out","trace_path_x1","trace_path_x2","trace_path_x3","trace_path_x4","trace_path_y1","trace_path_y2","trace_path_y3","trace_path_y4","path_size","InputAngle_start_t","direction_start_time");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, PreTra_lat_error,pos_matched_index,InputAngle_delta_t,direction_init_t,midback,midfront,heading,leftfront,rightfront,leftback,rightback,dist_preview,x_previewpoint,y_previewpoint,R,purepursuitangle,temp1,temp2,temp3,temp4,num_iteration_final,tolerance_final,num_iteration,tolerance,lqr_angle,purepursuit_angle,x_err_raw,y_err_raw,x_err,y_err,heading_err_normal_before,heading_err_normal_after,heading_err_filter_before,heading_err_filter_after,lqr_angle_raw,steer_angle_xerr_contribution,steer_angle_yerr_contribution,steer_angle_headingerr_contribution,real_delta,delta_out,trace_path_x1,trace_path_x2,trace_path_x3,trace_path_x4,trace_path_y1,trace_path_y2,trace_path_y3,trace_path_y4,path_size,InputAngle_start_t,direction_start_time);
} LatcontrolDebugStruct;
REGISTER(LatcontrolDebugStruct,LatcontrolDebugStruct_Func);
