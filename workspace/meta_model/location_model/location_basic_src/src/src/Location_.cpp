#include "location_.h"

#include <cmath>
#include <cstdint>

namespace {

bool PoseIsZero(const ara::adsfi::MsgHafPoseWithCovariance& pose)
{
    constexpr double kEps = 1e-9;
    return std::fabs(pose.pose.position.x) < kEps &&
           std::fabs(pose.pose.position.y) < kEps &&
           std::fabs(pose.pose.position.z) < kEps &&
           std::fabs(pose.pose.orientation.x) < kEps &&
           std::fabs(pose.pose.orientation.y) < kEps &&
           std::fabs(pose.pose.orientation.z) < kEps;
}

} // namespace

void Location_(const ara::adsfi::MsgHafLocation& dr_loc,
               const ara::adsfi::MsgHafLocation& tpimu_loc,
               ara::adsfi::MsgHafLocation& out_loc)
{
    // 主定位字段全部来自 TPIMU，DR 位姿填入 dr_pose
    out_loc = tpimu_loc;

    const auto& dr_src = PoseIsZero(dr_loc.dr_pose) ? dr_loc.pose : dr_loc.dr_pose;
    out_loc.dr_pose = dr_src;

}
