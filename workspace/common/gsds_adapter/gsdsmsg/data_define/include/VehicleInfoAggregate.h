#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "FaultData.h"
/*
车辆状态关键信息汇总
*/

typedef struct VehicleInfoAggregate{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned char	vehicle_drive_mode;	//底盘车辆模式反馈
	unsigned char	vehicle_shift_position;	//底盘档位信息反馈
	float	vehicle_speed;	//底盘车辆速度反馈
	int	vehicle_fault_code;	//底盘故障码反馈
	unsigned char	vehicle_remote_en_state;	//底盘单手遥控使能状态反馈
	FaultData	sys_fault_level;	//自主故障信息
	int	business_mode;	//当前任务类型 0:none 1:track  2:arrive  3:follow
	int	business_task_avoid_mode;	//当前任务避障属性：0--停障  1--避障  2--越障  3-忽略障碍物模式
	int	business_patrol_type;	//当前任务循迹类型：0-TurnAround，1-Reverse，2-Oneway，3-GlobalOneway，4-GlobalRawplan
	int	business_patrol_loop;	//当前任务循迹循环：0-不循环，1-循环
	double	business_max_speed;	//当前任务行驶最大速度
	int	business_command;	//当前任务状态 0:stop 1:start 2:pause
	int	business_follow_mode;	//当前任务跟随模式：0牵引1轨迹
	double	business_follow_min_dis;	//当前任务跟随最小距离
	int	business_estop_flag;	//急停标志位 0-关闭 1-开启
	int	auto_direction;	//规划轨迹的方向，0为前进，1为倒车
	float	auto_acc_desired;	//规划期望加速度，单位m/s^2
	int	auto_risk_level;	//规划危险等级，0：无，1：低，2：中，3：高（AEB）
	float	target_angle;	//控制目标方向盘转角
	float	target_speed;	//控制目标速度
	unsigned char	target_shift_position;	//控制目标档位
	double	target_current_limit_speed_rc;	//当前遥控限速值 km/h
	double	target_current_set_speed_rc;	//当前遥控定速值 km/h
	double	target_current_set_angle_rc;	//当前遥控定向的目标航向 degrees
	double	loc_utc_time;	//utc 时间
	char	loc_gps_status;	//GPS状态位 0:无定位;  1:单点定位定  2:伪距差分  4:固定解  5:浮点解
	int	loc_init_status;	//定位初始化状态
	int	loc_sate_num;	//卫星数
	double	loc_heading;	//航向角
	double	loc_lat;	//纬度[-180,180]
	double	loc_lon;	//经度[-180,180]
	double	loc_absolute_height;	//绝对水平高度(m)
	int	remotectrl_flag;	//遥控模式 含义【1-基础遥控、2-定速、3-定向、4-既定速也定向】
	int	remotectrll_tick;	//心跳 【0~99】
	int	remotectrl_speed_limit;	//限速 【5~60】
	int	remotectrl_gear;	//档位 含义【0-P、 1-R、 2-N,、3-D】
	int	remotectrl_set_speed;	//遥控定速值
	int	remotectrl_security_assist_enabled;	//遥控安全预警是否启用：0-关闭、1-开启
	double	remotectrl_steering_wheel;	//转向 范围【-32767~32767 左正右负，不区分档位】
	double	remotectrl_accelerator;	//加速 含义【0~32767】
	double	remotectrl_brake;	//制动 含义【0~32767】
	int	remotectrl_estop_flag;	//紧急制动 含义【0-关闭、1-开启】

	VehicleInfoAggregate(){
		key_hash_list.resize(46);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1906330616));
		key_hash_list.push_back(msghandle::type::fix_int32(2119102544));
		key_hash_list.push_back(msghandle::type::fix_int32(895118226));
		key_hash_list.push_back(msghandle::type::fix_int32(38421909));
		key_hash_list.push_back(msghandle::type::fix_int32(1506847247));
		key_hash_list.push_back(msghandle::type::fix_int32(1233733790));
		key_hash_list.push_back(msghandle::type::fix_int32(720323291));
		key_hash_list.push_back(msghandle::type::fix_int32(102772011));
		key_hash_list.push_back(msghandle::type::fix_int32(674124990));
		key_hash_list.push_back(msghandle::type::fix_int32(1134505413));
		key_hash_list.push_back(msghandle::type::fix_int32(1781969328));
		key_hash_list.push_back(msghandle::type::fix_int32(1216800454));
		key_hash_list.push_back(msghandle::type::fix_int32(382881188));
		key_hash_list.push_back(msghandle::type::fix_int32(956790221));
		key_hash_list.push_back(msghandle::type::fix_int32(509553031));
		key_hash_list.push_back(msghandle::type::fix_int32(2062700951));
		key_hash_list.push_back(msghandle::type::fix_int32(659838525));
		key_hash_list.push_back(msghandle::type::fix_int32(427365532));
		key_hash_list.push_back(msghandle::type::fix_int32(143650111));
		key_hash_list.push_back(msghandle::type::fix_int32(1402701017));
		key_hash_list.push_back(msghandle::type::fix_int32(1321626114));
		key_hash_list.push_back(msghandle::type::fix_int32(2026958680));
		key_hash_list.push_back(msghandle::type::fix_int32(908969920));
		key_hash_list.push_back(msghandle::type::fix_int32(1319833805));
		key_hash_list.push_back(msghandle::type::fix_int32(1462316989));
		key_hash_list.push_back(msghandle::type::fix_int32(284249035));
		key_hash_list.push_back(msghandle::type::fix_int32(402214054));
		key_hash_list.push_back(msghandle::type::fix_int32(410788589));
		key_hash_list.push_back(msghandle::type::fix_int32(1079353361));
		key_hash_list.push_back(msghandle::type::fix_int32(2005784734));
		key_hash_list.push_back(msghandle::type::fix_int32(6236416));
		key_hash_list.push_back(msghandle::type::fix_int32(1025898835));
		key_hash_list.push_back(msghandle::type::fix_int32(891455714));
		key_hash_list.push_back(msghandle::type::fix_int32(1895068500));
		key_hash_list.push_back(msghandle::type::fix_int32(1729269370));
		key_hash_list.push_back(msghandle::type::fix_int32(413910195));
		key_hash_list.push_back(msghandle::type::fix_int32(1778924955));
		key_hash_list.push_back(msghandle::type::fix_int32(53622079));
		key_hash_list.push_back(msghandle::type::fix_int32(815642046));
		key_hash_list.push_back(msghandle::type::fix_int32(1366181119));
		key_hash_list.push_back(msghandle::type::fix_int32(1192034112));
		key_hash_list.push_back(msghandle::type::fix_int32(1807112979));
		Clear();
	}

void Clear(){
	vehicle_drive_mode={};
	vehicle_shift_position={};
	vehicle_speed={};
	vehicle_fault_code={};
	vehicle_remote_en_state={};
	sys_fault_level={};
	business_mode={};
	business_task_avoid_mode={};
	business_patrol_type={};
	business_patrol_loop={};
	business_max_speed={};
	business_command={};
	business_follow_mode={};
	business_follow_min_dis={};
	business_estop_flag={};
	auto_direction={};
	auto_acc_desired={};
	auto_risk_level={};
	target_angle={};
	target_speed={};
	target_shift_position={};
	target_current_limit_speed_rc={};
	target_current_set_speed_rc={};
	target_current_set_angle_rc={};
	loc_utc_time={};
	loc_gps_status={};
	loc_init_status={};
	loc_sate_num={};
	loc_heading={};
	loc_lat={};
	loc_lon={};
	loc_absolute_height={};
	remotectrl_flag={};
	remotectrll_tick={};
	remotectrl_speed_limit={};
	remotectrl_gear={};
	remotectrl_set_speed={};
	remotectrl_security_assist_enabled={};
	remotectrl_steering_wheel={};
	remotectrl_accelerator={};
	remotectrl_brake={};
	remotectrl_estop_flag={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,vehicle_drive_mode,vehicle_shift_position,vehicle_speed,vehicle_fault_code,vehicle_remote_en_state,sys_fault_level,business_mode,business_task_avoid_mode,business_patrol_type,business_patrol_loop,business_max_speed,business_command,business_follow_mode,business_follow_min_dis,business_estop_flag,auto_direction,auto_acc_desired,auto_risk_level,target_angle,target_speed,target_shift_position,target_current_limit_speed_rc,target_current_set_speed_rc,target_current_set_angle_rc,loc_utc_time,loc_gps_status,loc_init_status,loc_sate_num,loc_heading,loc_lat,loc_lon,loc_absolute_height,remotectrl_flag,remotectrll_tick,remotectrl_speed_limit,remotectrl_gear,remotectrl_set_speed,remotectrl_security_assist_enabled,remotectrl_steering_wheel,remotectrl_accelerator,remotectrl_brake,remotectrl_estop_flag);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","vehicle_drive_mode","vehicle_shift_position","vehicle_speed","vehicle_fault_code","vehicle_remote_en_state","sys_fault_level","business_mode","business_task_avoid_mode","business_patrol_type","business_patrol_loop","business_max_speed","business_command","business_follow_mode","business_follow_min_dis","business_estop_flag","auto_direction","auto_acc_desired","auto_risk_level","target_angle","target_speed","target_shift_position","target_current_limit_speed_rc","target_current_set_speed_rc","target_current_set_angle_rc","loc_utc_time","loc_gps_status","loc_init_status","loc_sate_num","loc_heading","loc_lat","loc_lon","loc_absolute_height","remotectrl_flag","remotectrll_tick","remotectrl_speed_limit","remotectrl_gear","remotectrl_set_speed","remotectrl_security_assist_enabled","remotectrl_steering_wheel","remotectrl_accelerator","remotectrl_brake","remotectrl_estop_flag");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,vehicle_drive_mode,vehicle_shift_position,vehicle_speed,vehicle_fault_code,vehicle_remote_en_state,sys_fault_level,business_mode,business_task_avoid_mode,business_patrol_type,business_patrol_loop,business_max_speed,business_command,business_follow_mode,business_follow_min_dis,business_estop_flag,auto_direction,auto_acc_desired,auto_risk_level,target_angle,target_speed,target_shift_position,target_current_limit_speed_rc,target_current_set_speed_rc,target_current_set_angle_rc,loc_utc_time,loc_gps_status,loc_init_status,loc_sate_num,loc_heading,loc_lat,loc_lon,loc_absolute_height,remotectrl_flag,remotectrll_tick,remotectrl_speed_limit,remotectrl_gear,remotectrl_set_speed,remotectrl_security_assist_enabled,remotectrl_steering_wheel,remotectrl_accelerator,remotectrl_brake,remotectrl_estop_flag);
} VehicleInfoAggregate;
REGISTER(VehicleInfoAggregate,VehicleInfoAggregate_Func);
