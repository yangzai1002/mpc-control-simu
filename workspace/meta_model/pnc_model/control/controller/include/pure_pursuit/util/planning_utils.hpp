#ifndef PLANNING_UTILS_HPP
#define PLANNING_UTILS_HPP

#define EIGEN_MPL2_ONLY

#include "interpolate.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include "core/types.h"
#include <chrono>  
#include <publisher.h>
#include "mpc_lateral_controller/mpc_utils.h"

typedef struct TrajectoryPoint
{
    double time_from_start;
    Adsfi::HafPose pose;
    float longitudinal_velocity_mps;
    float lateral_velocity_mps;
    float acceleration_mps2;
    float heading_rate_rps;
    float front_wheel_angle_rad;
    float rear_wheel_angle_rad;
} TrajectoryPoint;


typedef struct Trajectory
{
    Adsfi::HafHeader header;
    std::vector<TrajectoryPoint> points;
} Trajectory;


namespace pure_pursuit
{

namespace planning_utils
{

constexpr double ERROR = 1e-6;

double calcArcLengthFromWayPoint(const Trajectory &input_path,
                                 const size_t src_idx,
                                 const size_t dst_idx);

double calcCurvature(const Adsfi::Point3d &target,
                     const Adsfi::HafPose &curr_pose);


double calcDistance2D(const Adsfi::Point3d &p,
                      const Adsfi::Point3d &q);


double calcDistSquared2D(const Adsfi::Point3d &p,
                         const Adsfi::Point3d &q);


double calcStopDistanceWithConstantJerk(const double &v_init, const double &j);

double calcLateralError2D(const Adsfi::Point3d &a_start,
                          const Adsfi::Point3d &a_end, const Adsfi::Point3d &b);

double calcRadius(const Adsfi::Point3d &target,
                  const Adsfi::HafPose &current_pose);

double convertCurvatureToSteeringAngle(double wheel_base, double kappa);


std::vector<Adsfi::HafPose> extractPoses(const Trajectory &motions);


std::pair<bool, int32_t> findClosestIdxWithDistAngThr(
        const std::vector<Adsfi::HafPose> &poses,
        const Adsfi::HafPose &current_pose,
        const double th_dist = 2.0,
        const double th_yaw = M_PI_2);


int8_t getLaneDirection(const std::vector<Adsfi::HafPose> &poses,
                        double th_dist = 0.5);


bool isDirectionForward(const Adsfi::HafPose &prev,
                        const Adsfi::HafPose &next);

bool isDirectionForward(const std::vector<Adsfi::HafPose> &poses);


template<typename T>
bool isInPolygon(const std::vector<T> &polygon, const T &point)
{
    if (polygon.size() < 3)
    {
        return false;
    }

    bool in_poly = false;
    double x1, x2, y1, y2;

    uint32_t nr_poly_points = polygon.size();
    double xold = polygon.at(nr_poly_points - 1).x();
    double yold = polygon.at(nr_poly_points - 1).y();

    for (const auto &poly_p : polygon)
    {
        double xnew = poly_p.x();
        double ynew = poly_p.y();

        if (xnew > xold)
        {
            x1 = xold;
            x2 = xnew;
            y1 = yold;
            y2 = ynew;
        }
        else
        {
            x1 = xnew;
            x2 = xold;
            y1 = ynew;
            y2 = yold;
        }

        if ((xnew < point.x()) == (point.x() <= xold)
                && (point.y() - y1) * (x2 - x1) < (y2 - y1) * (point.x() - x1))
        {
            in_poly = !in_poly;
        }


        xold = xnew;
        yold = ynew;
    }

    return in_poly;
}


template<>
bool isInPolygon(const std::vector<Adsfi::Point3d> &polygon,
                 const Adsfi::Point3d &point);


double kmph2mps(const double velocity_kmph);

double mps2kmph(const double velocity_mps);


double normalizeEulerAngle(const double euler);

Adsfi::Point3d transformToAbsoluteCoordinate2D(
        const Adsfi::Point3d &point,
        const Adsfi::HafPose &current_pose);


Adsfi::Point3d transformToRelativeCoordinate2D(
        const Adsfi::Point3d &point,
        const Adsfi::HafPose &current_pose);

Adsfi::HafQuaternion getQuaternionFromYaw(const double _yaw);


std::vector<TrajectoryPoint> convertToTrajectoryPointArray(
        const Trajectory &trajectory);


Trajectory resampleTrajectory(const Trajectory &input_trajectory,
                              const std::vector<double> &resampled_arclength,
                              const bool use_akima_spline_for_xy = false,
                              const bool use_lerp_for_z = true,
                              const bool use_zero_order_hold_for_twist = true);

bool validate_points_duplication(const std::vector<TrajectoryPoint> &points);


bool validate_arguments(const std::vector<TrajectoryPoint> &input_points,
                        const std::vector<double> &resampling_intervals);

bool validate_resampling_range(const std::vector<TrajectoryPoint> &points,
                               const std::vector<double> &resampling_intervals);


Trajectory convertToTrajectory(const std::vector<TrajectoryPoint> &trajectory,
                               const Adsfi::HafHeader &header = Adsfi::HafHeader 
                               { });


void validateNonEmpty(const std::vector<TrajectoryPoint> &points);


std::vector<TrajectoryPoint> removeOverlapPoints(
        const std::vector<TrajectoryPoint> &points, const size_t start_idx = 0);


size_t findNearestSegmentIndex(const std::vector<TrajectoryPoint> &points,
                               const Adsfi::Point3d &point);


double calcLongitudinalOffsetToSegment(
        const std::vector<TrajectoryPoint> &points, const size_t seg_idx,
        const Adsfi::Point3d &p_target,
        const bool throw_exception = false);


size_t findNearestIndexPoint(const std::vector<TrajectoryPoint> &points,
                             const Adsfi::Point3d &point);


std::optional<size_t> findNearestIndex(
        const std::vector<TrajectoryPoint> &points,
        const Adsfi::HafPose &pose, const double max_dist =
                std::numeric_limits<double>::max(), const double max_yaw =
                        std::numeric_limits<double>::max());


double calcArcLength(const std::vector<TrajectoryPoint> &points);


double calcLateralOffset(const std::vector<TrajectoryPoint> &points,
                         const Adsfi::Point3d &p_target,
                         const bool throw_exception = false);


double calcSignedArcLength(const std::vector<TrajectoryPoint> &points,
                           const size_t src_idx, const size_t dst_idx);


std::vector<Adsfi::Point3d> resamplePointVector(
        const std::vector<Adsfi::Point3d> &points,
        const std::vector<double> &resampled_arclength,
        const bool use_akima_spline_for_xy = false, const bool use_lerp_for_z =
                true);

std::vector<Adsfi::HafPose> resamplePoseVector(
        const std::vector<Adsfi::HafPose> &points,
        const std::vector<double> &resampled_arclength,
        const bool use_akima_spline_for_xy = false,
        const bool use_lerp_for_z = true);

double calcElevationAngle(const Adsfi::Point3d &p_from,
                          const Adsfi::Point3d &p_to);

double calcAzimuthAngle(const Adsfi::Point3d &p_from,
                        const Adsfi::Point3d &p_to);

void setOrientation(const Adsfi::HafQuaternion &orientation,
                    Adsfi::HafPose &p);


void insertOrientation(std::vector<Adsfi::HafPose> &points,
                       const bool is_driving_forward);


mdc::visual::Times getCurrentTimeVis();


}  // namespace planning_utils
}  // namespace pure_pursuit

#endif  // PLANNING_UTILS_HPP
