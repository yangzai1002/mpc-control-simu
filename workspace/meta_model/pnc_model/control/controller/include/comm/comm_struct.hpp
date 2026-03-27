#ifndef COMM_STRUCT_H
#define COMM_STRUCT_H

#include <cmath>
#include <iostream>
#include "core/types.h"
#include "planning/ego_trajectory.h"
#include "mpc_lateral_controller/mpc_trajectory.hpp"
#include <optional>
#include <chrono>  // 必须加这一行

using namespace Adsfi;
using namespace mpc_lateral_controller;
//#include "rs_driver/common/rs_log.hpp"
#define RS_ERROR   std::cout << "\033[1m\033[31m"  // bold red
#define RS_WARNING std::cout << "\033[1m\033[33m"  // bold yellow
#define RS_INFO    std::cout << "\033[1m\033[32m"  // bold green
#define RS_INFOL   std::cout << "\033[32m"         // green
#define RS_DEBUG   std::cout << "\033[1m\033[36m"  // bold cyan
#define RS_REND    "\033[0m" << std::endl

#define RS_TITLE   std::cout << "\033[1m\033[35m"  // bold magenta
#define RS_MSG     std::cout << "\033[1m\033[37m"  // bold white
namespace commStruct
{

#define STEERING_RATIO 25.713

enum GearCMD
{
    NONE,    //0-无效
    PARK,    //1-停车
    REVERSE,    //2-倒车
    NEUTRAL,    //3-空挡
    DRIVE,    //4-前进
    ECONOMY,    //5-节能
    SPORT,    //6-运动
    LOW,    //7-低速
    HIGH    //8-高速
};

typedef struct AckermannLateralCommand
{
    HafTime stamp;
    float steering_tire_angle;
    float steering_tire_rotation_rate;
    bool IsSteerConverged;
} AckermannLateralCommand;

typedef struct LongitudinalCommand
{
    HafTime stamp;
    float speed;
    float acceleration;
    float jerk;
    float throttle;
    float brake;
} LongitudinalCommand;

typedef struct AckermannControlCommand
{
    HafTime stamp;
    AckermannLateralCommand lateral;
    LongitudinalCommand longitudinal;
} AckermannControlCommand;

typedef struct SteeringReport
{
    HafTime stamp;
    float steering_tire_angle;
} SteeringReport;

typedef struct VehicleInfo
{
    double wheelbase;
    double max_steer_angle_rad;
    std::string vehicle_model_type;
    double mass_fl, mass_fr, mass_rl, mass_rr, cf, cr;
} VehicleInfo;

typedef struct TrajectoryPoint
{
    Adsfi::HafPose point;
    double speed;
} TrajectoryPoint;

typedef struct Trajectory
{
    std::vector<TrajectoryPoint> trajectory;
} Trajectory;

inline double normalizeRadian(const double rad, const double min_rad = -M_PI);

template<class Point1, class Point2>
double calcDistance2d(const Point1 &point1, const Point2 &point2);

template<class Point1, class Point2>
double calcSquaredDistance2d(const Point1 &point1, const Point2 &point2);

double calcDistance2dHaf(const HafWayPoint &point1, const HafWayPoint &point2);

double calcDistance3d(const MPCTrajectory &trajectory,
                      const size_t idx1, const size_t idx2);

double calcCurvature(const Adsfi::Point3d &p1,
                     const Adsfi::Point3d &p2,
                     const Adsfi::Point3d &p3);

Adsfi::HafQuaternion createQuaternionFromYaw(const double yaw);

// geometry_msgs::Vector3 createTranslation(const double x, const double y,
//                                          const double z);

// Adsfi::HafPose calcOffsetPose(const Adsfi::HafPose &p, const double x,
//                                    const double y, const double z,
//                                    const double yaw);

double calcYawDeviation(const Adsfi::HafPose &base_pose,
                        const Adsfi::HafPose &target_pose);

bool calcMPCTrajectoryTime(MPCTrajectory &traj);

MPCTrajectory convertToMPCTrajectory(
        const HafEgoTrajectory &input);

Trajectory convertToAutowareTrajectory(
        const MPCTrajectory &traj);

size_t findNearestIndex(const Trajectory &points,
                        const Adsfi::HafPose &pose, double max_dist =
                                std::numeric_limits<double>::max(),
                        double max_yaw = std::numeric_limits<double>::max());

size_t findFirstNearestIndexWithSoftConstraints(
        const Trajectory &points, const Adsfi::HafPose &pose,
        const double dist_threshold = std::numeric_limits<double>::max(),
        const double yaw_threshold = std::numeric_limits<double>::max());

template<class Pose1, class Pose2>
bool isDrivingForward(const Pose1 &src_pose, const Pose2 &dst_pose);

std::optional<bool> isDrivingForward(const commStruct::Trajectory &points);

double calcAzimuthAngle(const Adsfi::Point3d &p_from,
                        const Adsfi::Point3d &p_to);

double getYaw(const Adsfi::HafQuaternion& orientation);

}

#endif // COMM_STRUCT_H
