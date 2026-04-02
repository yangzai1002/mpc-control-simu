#pragma once
#include <vector>
#include <iostream>
#include <limits>
#include <chrono>
#include <unistd.h>
#include <deque>
#include <string>
#include "publisher.h"
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include "location/location.h"
#include "planning/ego_trajectory.h"
#include "control/body_report.h"
#include "control/chassis_report.h"
#include "core/types.h"
#include "mpc_utils.h"
#include "mpc_trajectory.hpp"
#include "lowpass_filter.h"
#include "amathutils_lib/amathutils.hpp"
#include "vehicle_model/vehicle_model_bicycle_kinematics.h"
#include "vehicle_model/vehicle_model_bicycle_dynamics.h"
#include "vehicle_model/vehicle_model_bicycle_kinematics_no_delay.h"
#include "qp_solver/qp_solver_unconstr.h"
#include "qp_solver/qp_solver_unconstr_fast.h"
#include "qp_solver/qp_solver_qpoases.h"
#include "pid_longitudinal_controller/pid_longitudinal_controller.hpp"
#include "trajectory_follower_base/input_data.hpp"
#include "trajectory_follower_base/lateral_controller_base.hpp"
#include <fstream>

using namespace Adsfi;
using namespace trajectory_follower;
/** 
 * @class MPC-based waypoints follower class
 * @brief calculate control command to follow reference waypoints
 */
namespace MpcFollower{

class MPCFollowerController:public LateralControllerBase
{
	private:
	    /*输入变量*/
		HafBodyReport m_bodyReport;
		HafChassisReport m_chassisReport;
		HafEgoTrajectory m_egoTrajectory;
		Adsfi::HafPose m_current_pose;
		Adsfi::HafTwist m_current_twist;
		//

		MPCTrajectory ref_traj_;                                   //!< @brief reference trajectory to be followed
		Butterworth2dFilter lpf_steering_cmd_;                     //!< @brief lowpass filter for steering command
		Butterworth2dFilter lpf_lateral_error_;                    //!< @brief lowpass filter for lateral error to calculate derivatie
		Butterworth2dFilter lpf_yaw_error_;                        //!< @brief lowpass filter for heading error to calculate derivatie
		Butterworth2dFilter lpf_vel_cmd_ ;

		std::shared_ptr<VehicleModelInterface> vehicle_model_ptr_; //!< @brief vehicle model for MPC
		std::string vehicle_model_type_;                           //!< @brief vehicle model type for MPC
		std::shared_ptr<QPSolverInterface> qpsolver_ptr_;          //!< @brief qp solver for MPC
		std::string output_interface_;                             //!< @brief output command type
		std::deque<double> input_buffer_;                          //!< @brief control input (mpc_output) buffer for delay time conpemsation


		/* parameters for control*/
		double ctrl_period_;              //!< @brief control frequency [s]
		double steering_lpf_cutoff_hz_;   //!< @brief cutoff frequency of lowpass filter for steering command [Hz]
		double admisible_position_error_; //!< @brief stop MPC calculation when lateral error is large than this value [m]
		double admisible_yaw_error_deg_;  //!< @brief stop MPC calculation when heading error is large than this value [deg]
		double steer_lim_deg_;            //!< @brief steering command limit [rad]
		double wheelbase_;                //!< @brief vehicle wheelbase length [m] to convert steering angle to angular velocity
		ulong  ctrl_cycle = 0;
		double desired_velocity;

		/* parameters for path smoothing */
		bool enable_path_smoothing_;     //< @brief flag for path smoothing
		bool enable_yaw_recalculation_;  //< @brief flag for recalculation of yaw angle after resampling
		int path_filter_moving_ave_num_; //< @brief param of moving average filter for path smoothing
		int path_smoothing_times_;       //< @brief number of times of applying path smoothing filter
		int curvature_smoothing_num_;    //< @brief point-to-point index distance used in curvature calculation
		double traj_resample_dist_;      //< @brief path resampling interval [m]
		bool show_debug_info_;      //!< @brief flag to display debug info

		struct MPCParam
		{
			int prediction_horizon;                         //< @brief prediction horizon step
			double prediction_sampling_time;                //< @brief prediction horizon period
			double weight_lat_error;                        //< @brief lateral error weight in matrix Q
			double weight_heading_error;                    //< @brief heading error weight in matrix Q
			double weight_heading_error_squared_vel_coeff;  //< @brief heading error * velocity weight in matrix Q
			double weight_steering_input;                   //< @brief steering error weight in matrix R
			double weight_steering_input_squared_vel_coeff; //< @brief steering error * velocity weight in matrix R
			double weight_lat_jerk;                         //< @brief lateral jerk weight in matrix R
			double weight_terminal_lat_error;               //< @brief terminal lateral error weight in matrix Q
			double weight_terminal_heading_error;           //< @brief terminal heading error weight in matrix Q
			double zero_ff_steer_deg;                       //< @brief threshold that feed-forward angle becomes zero
			double delay_compensation_time;                //< @brief delay time for steering input to be compensated
		};

		MPCParam mpc_param_; // for mpc design parameter
		double steer_cmd_prev_;     //< @brief steering command calculated in previous period
		double lateral_error_prev_; //< @brief previous lateral error for derivative
		double yaw_error_prev_;     //< @brief previous lateral error for derivative

		mdc::visual::Publisher mpc_predicted_trajPub = mdc::visual::Publisher::Advertise
				<mdc::visual::Marker>(ara::core::String("mpc_predicted_traj"));

		mdc::visual::Publisher mpc_origin_trajPub = mdc::visual::Publisher::Advertise
		<mdc::visual::Marker>(ara::core::String("mpc_origin_traj"));

		mdc::visual::Publisher mpc_current_pos_Pub = mdc::visual::Publisher::Advertise
						<mdc::visual::Marker>(ara::core::String("mpc_current_pos"));
        ofstream ofs_log;
	public:
		MPCFollowerController(std::string params_path);
		void MpcUpdateTraj(HafEgoTrajectory curTrajectory);
		bool MpcUpdate(double& vel_cmd,double& acc_cmd,double& steer_cmd,double& steer_vel_cmd);
		bool calculateMPC(double &vel_cmd, double &acc_cmd, double &steer_cmd, double &steer_vel_cmd);
		void convertTrajToMarker(MPCTrajectory debug_mpc_predicted_traj,mdc::visual::Marker& marker,
		float r,float g,float b);
		void convertPosToMarkerAndPublish(const Adsfi::HafPose vehicle_pose,
		                                      std::string ns, double r, double g, double b, double z);
		LateralOutput run(const InputData &input_data) override;
		bool isReady([[maybe_unused]] const InputData &input_data) override;
};
}