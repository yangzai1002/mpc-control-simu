#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "sPointMsg.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"

typedef short int16;
typedef double float64;
typedef float float32;

enum class eDrivingMode { BRAKING = 0, DRIVERING = 1};
enum class eShiftPosition { PARKING = 0, REVERSE = 1, DRIVER = 3, NEUTRAL = 2 };

struct VehicleLocposPoint
{
	double heading;
	double x;
	double y;
	double s;
};

typedef struct LoncontrolOutput {
	double torque;
	double pressure;
	double target_velocity;
	eDrivingMode driver_mode;
	eShiftPosition shift_position;
} LoncontrolOutput;

typedef struct LoncontrolInput {
	std::vector<sPointMsg> path;
	// sPerception perception_info;
	std::string task_type;
	double acc_desired_a;
	double vehicle_velocity;
	double desired_acc;
	double app_desired_velocity;
	double accelerator;
	double brake;
	int gear;
	unsigned int is_auto_mode;
	double rec_planning_data_time;
	double planning_data_pub_time;
	unsigned int shiftlvlposition;
	unsigned int current_direction;
	unsigned int system_state;
	unsigned int car_fault_level;
	double uisteerangle;
	double sbjspeed;
	double accel;
	double torque_max;
	int taskmode;
	double pitch;
	double yaw;
	bool emergency_stop;
	bool turning_around;
	double task_dis;
	bool task_executing;
	bool not_accurate_stop;
	int fault_level;
	int tick;
	bool is_6x6{false};
	LoncontrolInput() {
		acc_desired_a = 0.0;
		task_type = "";
		emergency_stop = false;
		turning_around = false;
		task_dis = 0.0;
		task_executing = false;
		not_accurate_stop = false;
		fault_level = 0;

	}
} LoncontrolInput;

typedef struct ShiftControlOutput {
	bool is_shift_position;
	eShiftPosition shift_position;
} ShiftControlOutput;

// struct LoncontrolDebugStruct {
// 	double sbj_speed;
// 	double virtual_speed;
// 	double speed_p;
// 	double speed_i;
// 	double target_acel;
// 	double acel_real_raw;
// 	double sbj_acel;
// 	double acel_p;
// 	double acel_i;
// 	double torque;
// 	double pressure;
// 	double temp1;
// 	double temp2;
// 	double temp3;
// 	double temp4;
// 	double path_length;
// 	double point_zero_length;
// 	int emergency_obstacle;
// 	double real_steer_angle;
// 	int is_shift_position;
// 	double preview_distance;
// 	int path_direction;
// 	double pitch;
// 	double collision_obj_x;
// 	double collision_obj_y;
// 	double collision_cell_x;
// 	double collision_cell_y;
// 	double follow_acc_input;
// 	double follow_acc_offset;
// 	double follow_acc_offset_p;
// 	double follow_acc_offset_i;
// 	double follow_acc_output;
// 	int emergency_stop_flag;
// 	LoncontrolDebugStruct() {
// 		sbj_speed = 0;
// 		virtual_speed = 0;
// 		speed_p = 0;
// 		speed_i = 0;
// 		target_acel = 0;
// 		acel_real_raw = 0;
// 		sbj_acel = 0;
// 		acel_p = 0;
// 		acel_i = 0;
// 		torque = 0;
// 		pressure = 0;
// 		temp1 = 0;
// 		temp2 = 0;
// 		temp3 = 0;
// 		temp4 = 0;
// 		point_zero_length = 0;
// 		path_length = 0;
// 		emergency_obstacle = 0;
// 		is_shift_position = 0;
// 		preview_distance = 0;
// 		path_direction = 0;
// 		collision_obj_x = 0;
// 		collision_obj_y = 0;
// 		collision_cell_x = 0;
// 		collision_cell_y = 0;
// 		pitch = 0;
// 		follow_acc_input = 0.0;
// 		follow_acc_offset = 0.0;
// 		follow_acc_offset_p = 0.0;
// 		follow_acc_offset_i = 0.0;
// 		follow_acc_output = 0.0;
// 		emergency_stop_flag = 0;
// 	}
// };

typedef struct UpperControllerConfig
{
	double car_length;
	double car_width;
	double wheel_base_length;
	double front_axle_tofront;
	double left_reduce_ratio;
	double acel_max;
	double acel_min;
	double velocity_lagtime;
	double emergency_brake_lagtime;
	double emergency_brake_accel;
	double wheel_center;
	double kps;
	double kis;
	double integral_upper;
	double integral_lower;
	double dt;
	bool is_use_neutral_gear;
	bool is_use_parking_gear;
	bool shiftposition_pass_neutral_gear;
	bool is_test_virtualspeed;
	bool is_use_path_filter_obs;
	double emergency_threshold;
	double safe_dis_stop;
	double min_preview_dis;
	double k_pitch;
	double end_length_acc_desired;
	double end_length_dis_stop;
	double shiftposition_acc_desired;
	double expand_dis_base_path;
	double expand_dis_base_vehicle;
	bool eps_left_p_right_n;
	//calibration
	double target_torque1;
	double target_torque2;
	double dece_target_pressure;
	int actuator_mode1;
	double acc_time;
	double ret_time;
	double dece_time;
	int ret_enable;
	UpperControllerConfig() {
		car_length = 0.0;
		car_width = 0.0;
		wheel_base_length = 0.0;
		front_axle_tofront = 0.0;
		left_reduce_ratio = 0.0;
		kps = 0.0;
		kis = 0.0;
		integral_upper = 0.0;
		integral_lower = 0.0;
		dt = 0.0;
		acel_max = 0.0;
		acel_min = 0.0;
		is_use_neutral_gear = false;
		is_use_parking_gear = false;
		shiftposition_pass_neutral_gear = false;
		is_test_virtualspeed = false;
		is_use_path_filter_obs = false;
		velocity_lagtime = 0.0;
		emergency_brake_lagtime = 0.0;
		emergency_threshold = 0.0;
		safe_dis_stop = 0;
		min_preview_dis = 0;
		emergency_brake_accel = 0;
		k_pitch = 0;
		wheel_center = 0;
		end_length_acc_desired = 0;
		shiftposition_acc_desired = 0;
		expand_dis_base_path = 0;
		expand_dis_base_vehicle = 0;
		end_length_dis_stop = 0;
		eps_left_p_right_n = true;
		target_torque1 = 0.0;
		target_torque2 = 0.0 ;
		dece_target_pressure = 0.0;
		actuator_mode1 = 0;
		acc_time = 0.0;
		ret_time = 0.0;
		dece_time = 0.0;
		ret_enable = 0;
	}
} UpperControllerConfig;


typedef struct UpperControllerInput
{
	double vehicle_speed;
	double vehicle_acc;
	bool is_shift_position;
	eDrivingMode driver_mode;
	double uisteerangle;
	int current_direction;
	eShiftPosition real_shiftposition;
	unsigned int is_auto_mode;
	double pitch;
	bool emergency_stop;
	bool turning_around;
	double task_dis;
	bool task_executing;
	bool not_accurate_stop;
	UpperControllerInput() {
		vehicle_speed = 0.0;
		vehicle_acc = 0.0;
		is_shift_position = false;
		driver_mode = eDrivingMode::DRIVERING;
		uisteerangle = 0;
		current_direction = 0;
		real_shiftposition = eShiftPosition::NEUTRAL;
		is_auto_mode = 0;
		pitch = 0;
		emergency_stop = false;
		turning_around = false;
		task_dis = 0.0;
		task_executing = false;
		not_accurate_stop = false;
	}
} UpperControllerInput;


typedef struct LowerControllerConfig
{
	double up_switch;
	double low_switch;
	double torque_max;
	double torque_min;
	double pressure_max;
	double pressure_min;
	double min_vehcile_start_torque;
	double accel_kp;
	double accel_ki;
	double accel_interal_upper;
	double accel_interal_lower;
	double k_torque;
	double k_pressure;
	LowerControllerConfig() {
		up_switch = 0.0;
		low_switch = 0.0;
		torque_max = 0.0;
		torque_min = 0.0;
		pressure_max = 0.0;
		pressure_min = 0.0;
		accel_kp = 0;
		accel_ki = 0;
		accel_interal_upper = 0;
		accel_interal_lower = 0;
		k_torque = 0;
		k_pressure = 0;
		min_vehcile_start_torque = 0;
	}
} LowerControllerConfig;


typedef struct LowerControllerInput
{
	double acel_desired;
	double acel_vehicle;
	double vehicle_speed;
	eDrivingMode driver_mode_last;  // todo xy??
	LowerControllerInput() {
		acel_desired = 0.0;
		vehicle_speed = 0.0;
		acel_vehicle = 0.0;
		driver_mode_last = eDrivingMode::DRIVERING;
	}

} LowerControllerInput;


typedef struct LoncontrollerInterfaceConfig {
	// 这个结构体的主要作用就是读取配置参数文件，对控制器参数进行赋值
	// 如果不想移植参数读取库，只有手动对这些参数赋值
	LowerControllerConfig lower_controller_config;
	UpperControllerConfig upper_controller_config;
	std::string car_type{"AION"};

	// 从 "vehicle_interface" 部分加载车辆相关的纵向参数
	bool LoadVechleParamFromModle() {
		// 获取 CustomStack 的单例
		auto ptr = CustomStack::Instance();
		// 定义错误处理变量
		std::string errorcode = "3092001";
		std::string extra_info = "";

		if (!ptr->GetProjectConfigValue("vehicle_interface/car_type", car_type)) {
			extra_info = "vehicle_interface/car_type";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","car_type", car_type);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/torque_max", lower_controller_config.torque_max)) {
			extra_info = "vehicle_interface/torque_max";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","torque_max", lower_controller_config.torque_max);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/torque_min", lower_controller_config.torque_min)) {
			extra_info = "vehicle_interface/torque_min";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","torque_min", lower_controller_config.torque_min);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/pressure_max", lower_controller_config.pressure_max)) {
			extra_info = "vehicle_interface/pressure_max";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","pressure_max", lower_controller_config.pressure_max);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/pressure_min", lower_controller_config.pressure_min)) {
			extra_info = "vehicle_interface/pressure_min";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","pressure_min", lower_controller_config.pressure_min);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/min_driver_torque", lower_controller_config.min_vehcile_start_torque)) {
			extra_info = "vehicle_interface/min_driver_torque";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","min_driver_torque", lower_controller_config.min_vehcile_start_torque);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/car_length", upper_controller_config.car_length)) {
			extra_info = "vehicle_interface/car_length";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","car_length", upper_controller_config.car_length);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/car_width", upper_controller_config.car_width)) {
			extra_info = "vehicle_interface/car_width";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","car_width", upper_controller_config.car_width);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/wheel_base_length", upper_controller_config.wheel_base_length)) {
			extra_info = "vehicle_interface/wheel_base_length";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","wheel_base_length", upper_controller_config.wheel_base_length);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/front_axle_tofront", upper_controller_config.front_axle_tofront)) {
			extra_info = "vehicle_interface/front_axle_tofront";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","front_axle_tofront", upper_controller_config.front_axle_tofront);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/left_reduce_ratio", upper_controller_config.left_reduce_ratio)) {
			extra_info = "vehicle_interface/left_reduce_ratio";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","left_reduce_ratio", upper_controller_config.left_reduce_ratio);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/acel_max", upper_controller_config.acel_max)) {
			extra_info = "vehicle_interface/acel_max";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","acel_max", upper_controller_config.acel_max);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/acel_min", upper_controller_config.acel_min)) {
			extra_info = "vehicle_interface/acel_min";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","acel_min", upper_controller_config.acel_min);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/wheel_center", upper_controller_config.wheel_center)) {
			extra_info = "vehicle_interface/wheel_center";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","wheel_center", upper_controller_config.wheel_center);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/emergency_brake_lagtime", upper_controller_config.emergency_brake_lagtime)) {
			extra_info = "vehicle_interface/emergency_brake_lagtime";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","emergency_brake_lagtime", upper_controller_config.emergency_brake_lagtime);
			return false;
		}
		if (!ptr->GetProjectConfigValue("vehicle_interface/emergency_brake_accel", upper_controller_config.emergency_brake_accel)) {
			extra_info = "vehicle_interface/emergency_brake_accel";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","emergency_brake_accel", upper_controller_config.emergency_brake_accel);
			return false;
		}
        // 所有参数均加载成功
		return true;
	}

    // 从 "control/loncontrol" 部分加载纵向控制算法参数
	bool LoadLonControlParamFromModle() {
		auto ptr = CustomStack::Instance();
		std::string errorcode = "3092001";
		std::string extra_info = "";

		if (!ptr->GetProjectConfigValue("control/loncontrol/up_switch", lower_controller_config.up_switch)) {
			extra_info = "control/loncontrol/up_switch";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","up_switch", lower_controller_config.up_switch);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/low_switch", lower_controller_config.low_switch)) {
			extra_info = "control/loncontrol/low_switch";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","low_switch", lower_controller_config.low_switch);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/accel_kp", lower_controller_config.accel_kp)) {
			extra_info = "control/loncontrol/accel_kp";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","accel_kp", lower_controller_config.accel_kp);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/accel_ki", lower_controller_config.accel_ki)) {
			extra_info = "control/loncontrol/accel_ki";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","accel_ki", lower_controller_config.accel_ki);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/accel_interal_upper", lower_controller_config.accel_interal_upper)) {
			extra_info = "control/loncontrol/accel_interal_upper";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","accel_interal_upper", lower_controller_config.accel_interal_upper);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/accel_interal_lower", lower_controller_config.accel_interal_lower)) {
			extra_info = "control/loncontrol/accel_interal_lower";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","accel_interal_lower", lower_controller_config.accel_interal_lower);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/k_torque", lower_controller_config.k_torque)) {
			extra_info = "control/loncontrol/k_torque";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","k_torque", lower_controller_config.k_torque);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/k_pressure", lower_controller_config.k_pressure)) {
			extra_info = "control/loncontrol/k_pressure";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","k_pressure", lower_controller_config.k_pressure);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/kps", upper_controller_config.kps)) {
			extra_info = "control/loncontrol/kps";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","kps", upper_controller_config.kps);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/kis", upper_controller_config.kis)) {
			extra_info = "control/loncontrol/kis";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","kis", upper_controller_config.kis);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/integral_upper", upper_controller_config.integral_upper)) {
			extra_info = "control/loncontrol/integral_upper";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","integral_upper", upper_controller_config.integral_upper);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/integral_lower", upper_controller_config.integral_lower)) {
			extra_info = "control/loncontrol/integral_lower";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","integral_lower", upper_controller_config.integral_lower);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/dt", upper_controller_config.dt)) {
			extra_info = "control/loncontrol/dt";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","dt", upper_controller_config.dt);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/is_test_virtualspeed", upper_controller_config.is_test_virtualspeed)) {
			extra_info = "control/loncontrol/is_test_virtualspeed";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","is_test_virtualspeed", upper_controller_config.is_test_virtualspeed);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/is_use_neutral_gear", upper_controller_config.is_use_neutral_gear)) {
			extra_info = "control/loncontrol/is_use_neutral_gear";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","is_use_neutral_gear", upper_controller_config.is_use_neutral_gear);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/is_use_parking_gear", upper_controller_config.is_use_parking_gear)) {
			extra_info = "control/loncontrol/is_use_parking_gear";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","is_use_parking_gear", upper_controller_config.is_use_parking_gear);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/shiftposition_pass_neutral_gear", upper_controller_config.shiftposition_pass_neutral_gear)) {
			extra_info = "control/loncontrol/shiftposition_pass_neutral_gear";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","shiftposition_pass_neutral_gear", upper_controller_config.shiftposition_pass_neutral_gear);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/velocity_lagtime", upper_controller_config.velocity_lagtime)) {
			extra_info = "control/loncontrol/velocity_lagtime";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","velocity_lagtime", upper_controller_config.velocity_lagtime);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/emergency_threshold", upper_controller_config.emergency_threshold)) {
			extra_info = "control/loncontrol/emergency_threshold";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","emergency_threshold", upper_controller_config.emergency_threshold);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/safe_dis_stop", upper_controller_config.safe_dis_stop)) {
			extra_info = "control/loncontrol/safe_dis_stop";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","safe_dis_stop", upper_controller_config.safe_dis_stop);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/min_preview_dis", upper_controller_config.min_preview_dis)) {
			extra_info = "control/loncontrol/min_preview_dis";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","min_preview_dis", upper_controller_config.min_preview_dis);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/k_pitch", upper_controller_config.k_pitch)) {
			extra_info = "control/loncontrol/k_pitch";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","k_pitch", upper_controller_config.k_pitch);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/end_length_acc_desired", upper_controller_config.end_length_acc_desired)) {
			extra_info = "control/loncontrol/end_length_acc_desired";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","end_length_acc_desired", upper_controller_config.end_length_acc_desired);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/end_length_dis_stop", upper_controller_config.end_length_dis_stop)) {
			extra_info = "control/loncontrol/end_length_dis_stop";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","end_length_dis_stop", upper_controller_config.end_length_dis_stop);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/shiftposition_acc_desired", upper_controller_config.shiftposition_acc_desired)) {
			extra_info = "control/loncontrol/shiftposition_acc_desired";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","shiftposition_acc_desired", upper_controller_config.shiftposition_acc_desired);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/is_use_path_filter_obs", upper_controller_config.is_use_path_filter_obs)) {
			extra_info = "control/loncontrol/is_use_path_filter_obs";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","is_use_path_filter_obs", upper_controller_config.is_use_path_filter_obs);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/expand_dis_base_path", upper_controller_config.expand_dis_base_path)) {
			extra_info = "control/loncontrol/expand_dis_base_path";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","expand_dis_base_path", upper_controller_config.expand_dis_base_path);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/expand_dis_base_vehicle", upper_controller_config.expand_dis_base_vehicle)) {
			extra_info = "control/loncontrol/expand_dis_base_vehicle";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","expand_dis_base_vehicle", upper_controller_config.expand_dis_base_vehicle);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/eps_left_p_right_n", upper_controller_config.eps_left_p_right_n)) {
			extra_info = "control/loncontrol/eps_left_p_right_n";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","eps_left_p_right_n", upper_controller_config.eps_left_p_right_n);
			return false;
		}

		// lon calibration
		if (!ptr->GetProjectConfigValue("control/loncontrol/target_torque1", upper_controller_config.target_torque1)) {
			extra_info = "control/loncontrol/target_torque1";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","target_torque1", upper_controller_config.target_torque1);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/target_torque2", upper_controller_config.target_torque2)) {
			extra_info = "control/loncontrol/target_torque2";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","target_torque2", upper_controller_config.target_torque2);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/actuator_mode1", upper_controller_config.actuator_mode1)) {
			extra_info = "control/loncontrol/actuator_mode1";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","actuator_mode1", upper_controller_config.actuator_mode1);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/acc_time", upper_controller_config.acc_time)) {
			extra_info = "control/loncontrol/acc_time";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","acc_time", upper_controller_config.acc_time);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/ret_time", upper_controller_config.ret_time)) {
			extra_info = "control/loncontrol/ret_time";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","ret_time", upper_controller_config.ret_time);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/dece_time", upper_controller_config.dece_time)) {
			extra_info = "control/loncontrol/dece_time";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","dece_time", upper_controller_config.dece_time);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/ret_enable", upper_controller_config.ret_enable)) {
			extra_info = "control/loncontrol/ret_enable";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","ret_enable", upper_controller_config.ret_enable);
			return false;
		}
		if (!ptr->GetProjectConfigValue("control/loncontrol/dece_target_pressure", upper_controller_config.dece_target_pressure)) {
			extra_info = "control/loncontrol/dece_target_pressure";
			FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
			ptr->GetConfig("RemoteControl","dece_target_pressure", upper_controller_config.dece_target_pressure);
			return false;
		}
		return true;
	}
} LoncontrollerInterfaceConfig;

