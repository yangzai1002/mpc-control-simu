#pragma once

#include <Eigen/Dense>
#include <cstdint>
#include <string>
// #include "yaml_io/yaml_io.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <memory>
#include "base_control/lat/latmath.h"
#include "log/log.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
#include "ara/adsfi/impl_type_msghaflocation.h"

static inline double TimeToSec(const ara::adsfi::MsgHafTime& t) {
    return static_cast<double>(t.sec) + static_cast<double>(t.nsec) * 1e-9;
}
static inline double TimeToSec(double t) {
    return t;
}
static inline ara::adsfi::MsgHafTime DoubleToHafTime(double time_sec) {
    ara::adsfi::MsgHafTime t;
    t.sec = static_cast<uint32_t>(time_sec);
    t.nsec = static_cast<uint32_t>((time_sec - static_cast<double>(t.sec)) * 1e9);
    return t;
}

enum class eLatShiftPosition { PARKING = 0, REVERSE = 1, DRIVER = 3, NEUTRAL = 2 };

struct sPath
{
	Eigen::VectorXd x;
	Eigen::VectorXd y;
	Eigen::VectorXd angle;
	Eigen::VectorXd distance;
	Eigen::VectorXd delta;
	Eigen::VectorXd velocity;
	Eigen::VectorXd curvature;
	Eigen::VectorXd xg;
	Eigen::VectorXd yg;
	Eigen::VectorXd angleglobal;

	void FillDistance() {
		int amount_smooth_path_point = x.size();
		double dist_sum = 0;
		distance(0) = 0;
		for (int i = 1; i < amount_smooth_path_point; ++i) {
			dist_sum += std::hypot(x(i - 1) - x(i), y(i - 1) - y(i));
			distance(i) = dist_sum;
		}
	}
	void PathOptimizer() {
		int amount_original_path_point = x.size();
		int smooth_window = 8;

		if (amount_original_path_point <= 2 * smooth_window) {
			return;
		}
		double xpointsum = 0;
		double ypointsum = 0;
		double roaddirectionmin = 0;
		double roaddirectionmax = 0;
		double roaddirectionsum = 0;
		for (int ismooth = 0; ismooth < amount_original_path_point; ++ismooth) {
			if (ismooth <= smooth_window) {
				xpointsum = 0;
				ypointsum = 0;
				for (int i = 0; i <= 2 * ismooth; ++i) {
					xpointsum = xpointsum + x(i);
					ypointsum = ypointsum + y(i);
				}
				x(ismooth) = xpointsum / (2 * ismooth + 1);
				y(ismooth) = ypointsum / (2 * ismooth + 1);
				angle(ismooth) = angle(ismooth);
				velocity(ismooth) = velocity(ismooth);//
				continue;
			} else if (ismooth < amount_original_path_point - smooth_window) {
				xpointsum = 0;
				ypointsum = 0;
				for (int i = 0; i <= 2 * smooth_window; ++i) {
					xpointsum = xpointsum + x(ismooth + smooth_window - i);
					ypointsum = ypointsum + y(ismooth + smooth_window - i);
				}
				x(ismooth) = xpointsum / (2 * smooth_window + 1);
				y(ismooth) = ypointsum / (2 * smooth_window + 1);

				roaddirectionmin = angle(ismooth);
				roaddirectionmax = angle(ismooth);
				for (int i = 0; i <= 2 * smooth_window; ++i) {
					if (angle[ismooth + smooth_window - i] < roaddirectionmin) {
						roaddirectionmin = angle[ismooth + smooth_window - i];
					}
					if (angle[ismooth + smooth_window - i] > roaddirectionmax) {
						roaddirectionmax = angle[ismooth + smooth_window - i];
					}
				}

				if ((roaddirectionmax - roaddirectionmin) < 180) {
					roaddirectionsum = 0;
					for (int i = 0; i <= 2 * smooth_window; ++i) {
						roaddirectionsum = roaddirectionsum + angle(ismooth + smooth_window - i);
					}
					angle(ismooth) = roaddirectionsum / (2 * smooth_window + 1);
				}
				velocity(ismooth) = velocity(ismooth);//
			} else {

			}
			distance(ismooth) = 0;
		}
		FillDistance();
	}
};

struct sCarModel
{
	double front_length;
	double rear_length;
	double base_length;
	double width;
};

struct sPoint
{
	double x;
	double y;
	double angle;
};
struct sCarCoord
{
	double x_car[4];
	double y_car[4];
};

struct sVehState
{
	double car_speed;
	double wheel_angle;
};

struct sTrackError
{
	double lateral_err;
	double heading_err;
};

struct sEpsParam
{
	double eps_angle;
	double eps_torque;
};

struct sParamPurePursuit
{
	double k_preview;
	double d_preview;
	double k_near_preview;
};

struct sParamStanley
{
	double kp_lateral_error;
};

typedef struct VehicleParam {
	eLatShiftPosition realshiftposition;
	unsigned int current_drive_mode;
	unsigned int control_switch_state;
	int system_state;
	double longiaccel;
	double sbjspeed;
	double uisteerangle;

	double car_wheel_base;
	double rear_mid_x;
	double rear_mid_y;
	double car_width;
	double car_length;

	VehicleParam() {
		current_drive_mode = 0;
		control_switch_state = 0;
		car_wheel_base = 2.9;
		rear_mid_x = 0.0;
		rear_mid_y = 0.0;
		car_width = 2.25;
		car_length = 4.8;
		longiaccel = 0.0;
		sbjspeed = 0.0;
		realshiftposition = eLatShiftPosition::NEUTRAL;
		system_state = 0;
	}
} VehicleParam;

struct LatcontrolOutput
{
	double targetangle;
	double targetcurvature;
	double target_angle_ratio; //-1 ~ 1.0
	bool ratio_is_valid;
	int32_t targettorque;
	double target_right_vel;
	double target_left_vel;
};

struct LatErrorsStruct
{
	double P_CS_lat_err;//最近路点坐标系下车辆的横向偏差(路点坐标系)
	double V_CS_lat_err;//车辆坐标系下的最近路点横向偏差(车辆坐标系)
	double P_transf_V_lat_err;//修正后理论最近路点的横向偏差(车辆坐标系)
	double nearest_point_heading_err;//最近点的航向偏差 ref_heaing - real_heading
	double rectified_heading_err;//修正后的航向偏差
	double average_heading_err;//根据位置匹配点计算前后一段距离之间的平均航向偏差
	double heading_err_rate;//航向偏差微分得到
	double average_heading_err_rate;//根据平均航向偏差计算得到的航向偏差变化率
	double yaw_rate_err;//参考航向变化率-实际的横摆角速度
	double rectified_lat_v_err;//修正后的横向速度偏差
	double rectified_lat_a_err;//修正后的横向加速度偏差
	double adj_point_delt_lat_s;//位置匹配点附近两点横向位置的变化量
	double adj_point_delt_lat_v;//位置匹配点附近两点横向速度的变化量
	double adj_point_delt_lat_a;//位置匹配点附近两点横向加速度的变化量
	double adj_point_delt_t;////位置匹配点附近两点时间的变化量
	LatErrorsStruct() {
		P_CS_lat_err = 0.0;
		V_CS_lat_err = 0.0;
		P_transf_V_lat_err = 0.0;
		nearest_point_heading_err = 0.0;
		rectified_heading_err = 0.0;
		average_heading_err = 0.0;
		heading_err_rate = 0.0;
		average_heading_err_rate = 0.0;
		yaw_rate_err = 0.0;
		rectified_lat_v_err = 0.0;
		rectified_lat_a_err = 0.0;
		adj_point_delt_lat_s = 0.0;
		adj_point_delt_lat_v = 0.0;
		adj_point_delt_lat_a = 0.0;
		adj_point_delt_t = 0.0;
	}
};

// typedef struct LatcontrolDebugStruct {
// 	double midback;
// 	double midfront;
// 	double heading;
// 	double leftfront;
// 	double rightfront;
// 	double leftback;
// 	double rightback;
// 	double dist_preview;
// 	double x_previewpoint;
// 	double y_previewpoint;
// 	double R;
// 	double purepursuitangle;
// 	double temp1;
// 	double temp2;
// 	double temp3;
// 	double temp4;
// 	double num_iteration_final;
// 	double tolerance_final;
// 	double num_iteration;
// 	double tolerance;
// 	double lqr_angle;
// 	double purepursuit_angle;
// 	double x_err_raw;
// 	double y_err_raw;
// 	double x_err;
// 	double y_err;
// 	double heading_err_normal_before;
// 	double heading_err_normal_after;
// 	double heading_err_filter_before;
// 	double heading_err_filter_after;
// 	double lqr_angle_raw;
// 	double steer_angle_xerr_contribution;
// 	double steer_angle_yerr_contribution;
// 	double steer_angle_headingerr_contribution;

// 	// std::vector<LatControlSampler >controlsampler;
// 	// std::vector<std::vector<std::vector<LatStateSampler>>>statesampler;
// 	// std::vector<StatesamplerCost>mincost;
// 	double real_delta;
// 	double delta_out;
// 	double trace_path_x1;
// 	double trace_path_x2;
// 	double trace_path_x3;
// 	double trace_path_x4;
// 	double trace_path_y1;
// 	double trace_path_y2;
// 	double trace_path_y3;
// 	double trace_path_y4;

// 	LatcontrolDebugStruct() {

// 		midback = 0;
// 		midfront = 0;
// 		heading = 0;
// 		leftfront = 0;
// 		rightfront = 0;
// 		leftback = 0;
// 		rightback = 0;
// 		dist_preview = 0;
// 		x_previewpoint = 0;
// 		y_previewpoint = 0;
// 		R = 0;
// 		purepursuitangle = 0;
// 		temp1 = 0;
// 		temp2 = 0;
// 		temp3 = 0;
// 		temp4 = 0;
// 		// controlsampler.clear();
// 		// statesampler.clear();
// 		// mincost.clear();
// 		real_delta = 0.0;
// 		delta_out = 0.0;
// 		trace_path_x1 = 0.0;
// 		trace_path_x2 = 0.0;
// 		trace_path_x3 = 0.0;
// 		trace_path_x4 = 0.0;
// 		trace_path_y1 = 0;
// 		trace_path_y2 = 0;
// 		trace_path_y3 = 0;
// 		trace_path_y4 = 0;

// 	}
// } LatcontrolDebugStruct;

typedef struct LatcontrolParam
{
	double k_preview;
	double d_preview;
	double k_near_preview;
	double kp_lateral_error;
	double right_max_steerangle;
	double left_max_steerangle;
	double eps_speed;
	double wheel_center;
	double wheel_base_length;
	double front_axle_tofront;
	double vehicle_length;
	double vehicle_width;
	double left_reduce_ratio;
	double right_reduce_ratio;
	double A_MAX;
	int steer_direction;
	double k_control_pl;
	double k_control_pl_rate;
	double k_control_delta;
	double k_control_delta_rate;
	double k_state_pl;
	double k_state_pl_rate;
	double k_state_delta;
	double k_state_delta_rate;
	double control_step;//控制采样步长
	int control_point_num;//控制采样总点数
	double state_level_dis;//状态采样层的间隔
	int state_point_num;//状态采样每一层点的数量
	double state_point_dis;//状态采样点的间隔
	double arcangle_turn;
	double arcangle_turn_preview_dist;
	double arcangle_turn_preview_dist_second;
	double DT;
	bool dec_ecx_turn;
	bool arcangle_mean_enable;
	double arcangle_turn_second;
	double k_bigturn;
	double lqr_eps;
	int lqr_max_iteration;
	double q_1;
	double q_2;
	double q_3;
	double r_1;
	double r_2;
	double delta_t;
	double turning_around_min_radius;

} LatcontrolParam;

typedef struct LatcontrolInput
{
	sPath traject_path;
	sCarCoord vehicle_coordinate;
	sVehState vehicle_state;
	int reverse_flag;
	int road_source;
	int taskmode;
	double loc_xg;
	double loc_yg;
	double loc_yaw;
	double loc_yaw_rate;
	double virtual_speed;
	LatcontrolInput() {
		reverse_flag = 0;
		road_source = 0;
		loc_yaw_rate = 0.0;
	}


} LatcontrolInput;

typedef struct LatcontrollerInterfaceConfig
{
	std::string latctrl_type;
	std::string car_type;

	LatcontrolParam latcontrol_param;
	LatcontrollerInterfaceConfig() {
		latctrl_type = "";
	}

	bool LoadVechleParamFromModle() {
        // 获取 CustomStack 的单例，用于读取参数
		auto ptr = CustomStack::Instance();

		// std::string usr_config_path = CONFIG_PATH;
		// std::cout << "usr_config_path = " << usr_config_path << std::endl;
		// ptr->LoadModuleConfig(usr_config_path);

		// char *pathvar = nullptr;
		// pathvar = getenv("PROJ_CFG_PATH");
		// std::cout << "getenv(PROJ_CFG_PATH )" << std::string(pathvar) << std::endl;
		// if (pathvar == nullptr)
		// {
		// 	std::cout << "please set PROJ_CFG_PATH" << std::endl;
		// 	exit(0);
		// }
		// std::string prefix = std::string(pathvar);
		// ptr->LoadAllProjectConfig(prefix);

        std::string errorcode = "3092001"; // 错误码
        std::string extra_info = "";


        
		// --- 开始逐一加载原 vehicle_interface 中的参数 ---
        if (!ptr->GetProjectConfigValue("vehicle_interface/right_max_steerangle", latcontrol_param.right_max_steerangle)) {
            extra_info = "vehicle_interface/right_max_steerangle";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","right_max_steerangle", latcontrol_param.right_max_steerangle);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/left_max_steerangle", latcontrol_param.left_max_steerangle)) {
            extra_info = "vehicle_interface/left_max_steerangle";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","left_max_steerangle", latcontrol_param.left_max_steerangle);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/eps_speed", latcontrol_param.eps_speed)) {
            extra_info = "vehicle_interface/eps_speed";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","eps_speed", latcontrol_param.eps_speed);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/wheel_center", latcontrol_param.wheel_center)) {
            extra_info = "vehicle_interface/wheel_center";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","wheel_center", latcontrol_param.wheel_center);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/wheel_base_length", latcontrol_param.wheel_base_length)) {
            extra_info = "vehicle_interface/wheel_base_length";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","wheel_base_length", latcontrol_param.wheel_base_length);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/left_reduce_ratio", latcontrol_param.left_reduce_ratio)) {
            extra_info = "vehicle_interface/left_reduce_ratio";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","left_reduce_ratio", latcontrol_param.left_reduce_ratio);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/right_reduce_ratio", latcontrol_param.right_reduce_ratio)) {
            extra_info = "vehicle_interface/right_reduce_ratio";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","right_reduce_ratio", latcontrol_param.right_reduce_ratio);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/front_axle_tofront", latcontrol_param.front_axle_tofront)) {
            extra_info = "vehicle_interface/front_axle_tofront";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","front_axle_tofront", latcontrol_param.front_axle_tofront);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/car_length", latcontrol_param.vehicle_length)) {
            extra_info = "vehicle_interface/car_length";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","car_length", latcontrol_param.vehicle_length);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/car_width", latcontrol_param.vehicle_width)) {
            extra_info = "vehicle_interface/car_width";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","car_width", latcontrol_param.vehicle_width);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/turning_around_min_radius", latcontrol_param.turning_around_min_radius)) {
            extra_info = "vehicle_interface/turning_around_min_radius";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","turning_around_min_radius", latcontrol_param.turning_around_min_radius);
            return false;
        }
        if (!ptr->GetProjectConfigValue("vehicle_interface/car_type", car_type)) {
            extra_info = "vehicle_interface/car_type";
            FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
            ptr->GetConfig("RemoteControl","car_type", car_type);
            return false;
        }
		return true;
	}

	bool LoadLatControlParamFromModle() {
	    // 获取 CustomStack 的单例
	    auto ptr = CustomStack::Instance();
	    // 定义错误处理变量
	    std::string errorcode = "3092001";
	    std::string extra_info = "";

	    // --- 开始逐一加载原 control 中的所有参数 ---
	    if (!ptr->GetProjectConfigValue("control/latcontrol/latctrl_type", latctrl_type)) {
	        extra_info = "control/latcontrol/latctrl_type";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","latctrl_type", latctrl_type);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_preview", latcontrol_param.k_preview)) {
	        extra_info = "control/latcontrol/k_preview";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_preview", latcontrol_param.k_preview);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/d_preview", latcontrol_param.d_preview)) {
	        extra_info = "control/latcontrol/d_preview";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","d_preview", latcontrol_param.d_preview);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_near_preview", latcontrol_param.k_near_preview)) {
	        extra_info = "control/latcontrol/k_near_preview";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_near_preview", latcontrol_param.k_near_preview);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/kp_lateral_error", latcontrol_param.kp_lateral_error)) {
	        extra_info = "control/latcontrol/kp_lateral_error";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","kp_lateral_error", latcontrol_param.kp_lateral_error);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/A_MAX", latcontrol_param.A_MAX)) {
	        extra_info = "control/latcontrol/A_MAX";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","A_MAX", latcontrol_param.A_MAX);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/steer_direction", latcontrol_param.steer_direction)) {
	        extra_info = "control/latcontrol/steer_direction";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","steer_direction", latcontrol_param.steer_direction);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/arcangle_turn", latcontrol_param.arcangle_turn)) {
	        extra_info = "control/latcontrol/arcangle_turn";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","arcangle_turn", latcontrol_param.arcangle_turn);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/arcangle_turn_second", latcontrol_param.arcangle_turn_second)) {
	        extra_info = "control/latcontrol/arcangle_turn_second";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","arcangle_turn_second", latcontrol_param.arcangle_turn_second);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/arcangle_turn_preview_dist", latcontrol_param.arcangle_turn_preview_dist)) {
	        extra_info = "control/latcontrol/arcangle_turn_preview_dist";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","arcangle_turn_preview_dist", latcontrol_param.arcangle_turn_preview_dist);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/arcangle_turn_preview_dist_second", latcontrol_param.arcangle_turn_preview_dist_second)) {
	        extra_info = "control/latcontrol/arcangle_turn_preview_dist_second";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","arcangle_turn_preview_dist_second", latcontrol_param.arcangle_turn_preview_dist_second);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_control_pl", latcontrol_param.k_control_pl)) {
	        extra_info = "control/latcontrol/k_control_pl";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_control_pl", latcontrol_param.k_control_pl);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_control_pl_rate", latcontrol_param.k_control_pl_rate)) {
	        extra_info = "control/latcontrol/k_control_pl_rate";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_control_pl_rate", latcontrol_param.k_control_pl_rate);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_control_delta", latcontrol_param.k_control_delta)) {
	        extra_info = "control/latcontrol/k_control_delta";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_control_delta", latcontrol_param.k_control_delta);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_control_delta_rate", latcontrol_param.k_control_delta_rate)) {
	        extra_info = "control/latcontrol/k_control_delta_rate";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_control_delta_rate", latcontrol_param.k_control_delta_rate);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_state_pl", latcontrol_param.k_state_pl)) {
	        extra_info = "control/latcontrol/k_state_pl";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_state_pl", latcontrol_param.k_state_pl);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_state_pl_rate", latcontrol_param.k_state_pl_rate)) {
	        extra_info = "control/latcontrol/k_state_pl_rate";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_state_pl_rate", latcontrol_param.k_state_pl_rate);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_state_delta", latcontrol_param.k_state_delta)) {
	        extra_info = "control/latcontrol/k_state_delta";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_state_delta", latcontrol_param.k_state_delta);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_state_delta_rate", latcontrol_param.k_state_delta_rate)) {
	        extra_info = "control/latcontrol/k_state_delta_rate";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_state_delta_rate", latcontrol_param.k_state_delta_rate);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/control_step", latcontrol_param.control_step)) {
	        extra_info = "control/latcontrol/control_step";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","control_step", latcontrol_param.control_step);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/control_point_num", latcontrol_param.control_point_num)) {
	        extra_info = "control/latcontrol/control_point_num";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","control_point_num", latcontrol_param.control_point_num);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/state_level_dis", latcontrol_param.state_level_dis)) {
	        extra_info = "control/latcontrol/state_level_dis";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","state_level_dis", latcontrol_param.state_level_dis);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/state_point_num", latcontrol_param.state_point_num)) {
	        extra_info = "control/latcontrol/state_point_num";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","state_point_num", latcontrol_param.state_point_num);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/state_point_dis", latcontrol_param.state_point_dis)) {
	        extra_info = "control/latcontrol/state_point_dis";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","state_point_dis", latcontrol_param.state_point_dis);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/DT", latcontrol_param.DT)) {
	        extra_info = "control/latcontrol/DT";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","DT", latcontrol_param.DT);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/dec_ecx_turn", latcontrol_param.dec_ecx_turn)) {
	        extra_info = "control/latcontrol/dec_ecx_turn";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","dec_ecx_turn", latcontrol_param.dec_ecx_turn);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/arcangle_mean_enable", latcontrol_param.arcangle_mean_enable)) {
	        extra_info = "control/latcontrol/arcangle_mean_enable";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","arcangle_mean_enable", latcontrol_param.arcangle_mean_enable);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/k_bigturn", latcontrol_param.k_bigturn)) {
	        extra_info = "control/latcontrol/k_bigturn";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","k_bigturn", latcontrol_param.k_bigturn);
	        return false;
	    }

	    // LQR 参数
	    if (!ptr->GetProjectConfigValue("control/latcontrol/lqr_eps", latcontrol_param.lqr_eps)) {
	        extra_info = "control/latcontrol/lqr_eps";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","lqr_eps", latcontrol_param.lqr_eps);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/lqr_max_iteration", latcontrol_param.lqr_max_iteration)) {
	        extra_info = "control/latcontrol/lqr_max_iteration";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","lqr_max_iteration", latcontrol_param.lqr_max_iteration);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/q_1", latcontrol_param.q_1)) {
	        extra_info = "control/latcontrol/q_1";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","q_1", latcontrol_param.q_1);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/q_2", latcontrol_param.q_2)) {
	        extra_info = "control/latcontrol/q_2";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","q_2", latcontrol_param.q_2);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/q_3", latcontrol_param.q_3)) {
	        extra_info = "control/latcontrol/q_3";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","q_3", latcontrol_param.q_3);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/r_1", latcontrol_param.r_1)) {
	        extra_info = "control/latcontrol/r_1";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","r_1", latcontrol_param.r_1);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/r_2", latcontrol_param.r_2)) {
	        extra_info = "control/latcontrol/r_2";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","r_2", latcontrol_param.r_2);
	        return false;
	    }
	    if (!ptr->GetProjectConfigValue("control/latcontrol/delta_t", latcontrol_param.delta_t)) {
	        extra_info = "control/latcontrol/delta_t";
	        FaultHandle::FaultApi::Instance()->SetFaultCode(errorcode, extra_info);
	        ptr->GetConfig("RemoteControl","delta_t", latcontrol_param.delta_t);
	        return false;
	    }

	    return true;
	}
} LatcontrollerInterfaceConfig;
