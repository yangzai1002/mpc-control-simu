
#ifndef PURE_PURSUIT_H
#define PURE_PURSUIT_H

#include "pure_pursuit/util/planning_utils.hpp"
#include "core/logger.h"
using namespace Adsfi;

namespace pure_pursuit
{

class PurePursuit
{

public:

    PurePursuit() :
            lookahead_distance_(0.0), closest_thr_dist_(5.0), closest_thr_ang_(
                    M_PI/2 )
    {
    }

    ~PurePursuit() = default;

    void setCurrentPose(const Adsfi::HafPose &msg);

    void setWaypoints(const std::vector<Adsfi::HafPose> &msg);

    void setLookaheadDistance(double ld)
    {
        lookahead_distance_ = ld;
    }


    void setClosestThreshold(double closest_thr_dist, double closest_thr_ang)
    {
        closest_thr_dist_ = closest_thr_dist;
        closest_thr_ang_ = closest_thr_ang;
    }


    Adsfi::Point3d getLocationOfNextWaypoint() const
    {
        return loc_next_wp_;
    }

    Adsfi::Point3d getLocationOfNextTarget() const
    {
        return loc_next_tgt_;
    }

    bool isDataReady();

    std::pair<bool, double> run();

    // 预瞄距离, 最近距离, 最近角度
    double lookahead_distance_, closest_thr_dist_, closest_thr_ang_;

private:
    Adsfi::Point3d loc_next_wp_;
    Adsfi::Point3d loc_next_tgt_;

    std::shared_ptr<std::vector<Adsfi::HafPose>> curr_wps_ptr_;
    std::shared_ptr<Adsfi::HafPose> curr_pose_ptr_;

    int32_t findNextPointIdx(int32_t search_start_idx);
    std::pair<bool,Adsfi::Point3d> lerpNextTarget(int32_t next_wp_idx);

};

}  // namespace

#endif  // PURE_PURSUIT_H
