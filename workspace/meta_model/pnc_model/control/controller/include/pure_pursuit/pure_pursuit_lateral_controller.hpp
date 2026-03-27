#ifndef PURE_PURSUIT_LATERAL_CONTROLLER_H
#define PURE_PURSUIT_LATERAL_CONTROLLER_H


#include <publisher.h>
#include "comm/comm_struct.hpp"
#include "comm/yaml_reader.hpp"
#include "trajectory_follower_base/lateral_controller_base.hpp"
#include "pure_pursuit.hpp"
#include "mpc_lateral_controller/mpc_utils.h"
#include "util/marker_helper.hpp"

//
using trajectory_follower::InputData;
using trajectory_follower::LateralControllerBase;
using trajectory_follower::LateralOutput;


namespace pure_pursuit
{


//pure_pursuit输出
struct PpOutput
{
    double curvature;
    double velocity;
};


//PP用到的一些参数
struct Param
{
    // 全局参数
    double wheel_base;
    double max_tire_steering_angle;  // [rad]

    //控制周期
    double ctrl_period;

    // 算法参数
    double lookahead_distance_ratio; //
    double ld_velocity_ratio;
    double ld_lateral_error_ratio;
    double ld_curvature_ratio;
    double min_lookahead_distance;
    double max_lookahead_distance;

    // 倒车档下最小预瞄距离
    double reverse_min_lookahead_distance;
    double converged_steer_rad_;
    double prediction_ds;

    // 预测轨迹的总距离
    double prediction_distance_length;
    double resampling_ds;
    double curvature_calculation_distance;
    double long_ld_lateral_error_threshold;

    bool enable_path_smoothing;

    int path_filter_moving_ave_num;
};



struct DebugData
{
   Adsfi::Point3d next_target;
};



//PP横向控制器
class PurePursuitLateralController: public LateralControllerBase
{

public:
    explicit PurePursuitLateralController(std::string params_path);


private:
    uint64_t m_seq = 0;

    std::vector<TrajectoryPoint> output_tp_array_;
    Trajectory trajectory_resampled_;
    Adsfi::HafEgoTrajectory haf_trajectory;
    Trajectory trajectory_;
   // nav_msgs::Odometry current_odometry_;

    commStruct::SteeringReport current_steering_;
    commStruct::AckermannLateralCommand prev_cmd_;

    mdc::visual::Publisher m_pub_predicted_traj =
            mdc::visual::Publisher::Advertise<mdc::visual::Path>(
                    ara::core::String("purePursuit_predicted_traj"));

    mdc::visual::Publisher m_pub_Resample_traj =
            mdc::visual::Publisher::Advertise<mdc::visual::Path>(
                    ara::core::String("purePursuit_Resample_traj"));

    mdc::visual::Publisher pure_debug_Pub = mdc::visual::Publisher::Advertise<
            mdc::visual::PoseStamped>(ara::core::String("debugvalue"));

            
    mdc::visual::Publisher m_pub_current_pose = mdc::visual::Publisher::Advertise<
            mdc::visual::PoseStamped>(ara::core::String("debugvalue"));

    mdc::visual::Publisher pursuit_current_pos_Pub = mdc::visual::Publisher::Advertise
                                        <mdc::visual::Marker>(ara::core::String("pursuit_current_pos"));

    mdc::visual::Publisher pursuit_debug_Pub = mdc::visual::Publisher::Advertise
        <mdc::visual::MarkerArray>(ara::core::String("pursuit_debug_marker"));

    void publishCurrentPose(const Adsfi::HafPose vehicle_pose,
                                      std::string ns, double r, double g, double b, double z);

    void publishPredictedTraj(const Trajectory &predicted_traj);
    void publishResampleTraj(const Trajectory &trajectory_resampled_);
    

    void onTrajectory(const Trajectory msg);

    //void onCurrentOdometry(const nav_msgs::Odometry msg);

    void setResampledTrajectory();

    Adsfi::HafPose current_pose_;
    Adsfi::HafTwist current_twist_;

    std::vector<Adsfi::Point3d> generateTrajectoryCircle(
            const Adsfi::Point3d &target,
            const Adsfi::HafPose &current_pose);

    mdc::visual::Marker createNextTargetMarker(
            const Adsfi::Point3d & next_target);


    mdc::visual::Marker createTrajectoryCircleMarker(
            const Adsfi::Point3d & target, const Adsfi::HafPose & current_pose);


    void publishDebugMarker();


    bool isReady([[maybe_unused]] const InputData &input_data) override;

    LateralOutput run(const InputData &input_data) override;

    commStruct::AckermannLateralCommand generateCtrlCmdMsg(
            const double target_curvature);


    Param param_{ };


    std::unique_ptr<PurePursuit> pure_pursuit_;

    boost::optional<PpOutput> calcTargetCurvature(bool is_control_output,
            Adsfi::HafPose pose);


    TrajectoryPoint calcNextPose(const double ds, TrajectoryPoint &point,
                                 commStruct::AckermannLateralCommand cmd) const;

    boost::optional<Trajectory> generatePredictedTrajectory();

    commStruct::AckermannLateralCommand generateOutputControlCmd();

    bool calcIsSteerConverged(const commStruct::AckermannLateralCommand &cmd);


    double calcLookaheadDistance(const double lateral_error,
                                 const double curvature, const double velocity, const double min_ld,
                                 const bool is_control_cmd);


    double calcCurvature(const size_t closest_idx);

    //
    void averageFilterTrajectory(Trajectory &u);


    Trajectory ConvertToLocalTrajectory(const HafEgoTrajectory &input);

    DebugData debug_data_;

};


}  // namespace

#endif  //PURE_PURSUIT_LATERAL_CONTROLLER_H
