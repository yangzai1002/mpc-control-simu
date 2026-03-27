#ifndef MPC_TRAJECTORY_HPP
#define MPC_TRAJECTORY_HPP

#include <vector>
#include <iostream>

namespace mpc_lateral_controller
{

class MPCTrajectoryPoint
{
public:
    double x;
    double y;
    double z;
    double yaw;
    double vx;
    double k;
    double smooth_k;
    double relative_time;
};

//
class MPCTrajectory
{
public:
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<double> yaw;    //yaw pose
    std::vector<double> vx;    //vx velocity
    std::vector<double> k;    //k曲率
    std::vector<double> smooth_k;       //k smoothed-curvature
    std::vector<double> relative_time;    //relative_time duration time from start point

    void push_back(const double &xp, const double &yp, const double &zp,
                   const double &yawp, const double &vxp, const double &kp,
                   const double &smooth_kp, const double &tp);

    void push_back(const MPCTrajectoryPoint &p);

    MPCTrajectoryPoint back();

    void clear();

    size_t size() const;

    inline bool empty() const
    {
        return size() == 0;
    }

};

}    // namespace

#endif  // MPC_TRAJECTORY_HPP
