
#include "pure_pursuit/pure_pursuit.hpp"


namespace pure_pursuit
{

bool PurePursuit::isDataReady()
{
    if (!curr_wps_ptr_)
    {
        return false;
    }

    if (!curr_pose_ptr_)
    {
        return false;
    }

    return true;
}



std::pair<bool, double> PurePursuit::run()
{
    if (!isDataReady())
    {
        return std::make_pair(false, std::numeric_limits<double>::quiet_NaN());
    }


    auto closest_pair = planning_utils::findClosestIdxWithDistAngThr(
                                                                      *curr_wps_ptr_,   //
                                                                      *curr_pose_ptr_,   //
                                                                       closest_thr_dist_,  //距离阈值
                                                                       closest_thr_ang_ ); //角度阈值

    if (!closest_pair.first)
    {
        HAF_LOG_INFO << "<"<< __FILE__ << "> "
                     << " cannot find closest_pair. "
                     << " | curr_bool: " << closest_pair.first
                     << ", closest_idx: " << closest_pair.second;

        return std::make_pair(false, std::numeric_limits<double>::quiet_NaN());
    }else
    {
        // HAF_LOG_INFO << "<"<< __FILE__ << "> "
        //              << " find closest_pair. "
        //              << " | curr_bool: " << closest_pair.first
        //              << ", closest_idx: " << closest_pair.second;
    }

    int32_t next_wp_idx = findNextPointIdx(closest_pair.second);

    if (next_wp_idx == -1)
    {
        HAF_LOG_INFO << "<" << __FILE__ << "> " << " Lost next waypoint";
        return std::make_pair(false, std::numeric_limits<double>::quiet_NaN());
    }


    loc_next_wp_ = curr_wps_ptr_->at(next_wp_idx).position;

    Adsfi::Point3d next_tgt_pos;


    if (next_wp_idx == 0)
    {
        next_tgt_pos = curr_wps_ptr_->at(next_wp_idx).position;
    }
    else
    {
        std::pair<bool, Adsfi::Point3d> lerp_pair = lerpNextTarget(
                next_wp_idx);

        if (!lerp_pair.first)
        {
            HAF_LOG_INFO << "<" << __FILE__ << "> " << "lost target!";

            return std::make_pair(false,
                    std::numeric_limits<double>::quiet_NaN());
        }

        next_tgt_pos = lerp_pair.second;
    }

    

    loc_next_tgt_ = next_tgt_pos;

    // 计算曲率1/R, pp算法中自车转过的定圆
    double kappa = planning_utils::calcCurvature(next_tgt_pos, *curr_pose_ptr_);

    return std::make_pair(true, kappa);
}

std::pair<bool, Adsfi::Point3d> PurePursuit::lerpNextTarget(
        int32_t next_wp_idx)
{
    constexpr double ERROR2 = 1e-5;

    const Adsfi::Point3d &vec_end =
            curr_wps_ptr_->at(next_wp_idx).position;

    const Adsfi::Point3d &vec_start =
            curr_wps_ptr_->at(next_wp_idx - 1).position;

    const Adsfi::HafPose &curr_pose = *curr_pose_ptr_;

    Eigen::Vector3d vec_a((vec_end.x - vec_start.x),
                          (vec_end.y - vec_start.y),
                          (vec_end.z - vec_start.z));

    if (vec_a.norm() < ERROR2)
    {
        HAF_LOG_INFO << "<"<< __FILE__ << "> " << ", waypoint interval is almost 0";

        return std::make_pair(false, Adsfi::Point3d());
    }

    const double lateral_error = planning_utils::calcLateralError2D(vec_start,
                                                                    vec_end,
                                                                    curr_pose.position);
   // HAF_LOG_INFO << "[lat_error]: " << lateral_error;

    if (fabs(lateral_error) > lookahead_distance_)
    {

        HAF_LOG_INFO << "<"<< __FILE__ << "> "
                     << ", lateral error is larger than lookahead distance "
                     << " | (lat_error: " << lateral_error
                     << ", lookahead_dis: " << lookahead_distance_ << " )";

        return std::make_pair(false, Adsfi::Point3d());
    }

    // 根据横向距离计算垂直于路径线的垂足位置, 返回该位置作为插值结果
    Eigen::Vector2d uva2d(vec_a.x(), vec_a.y());

    uva2d.normalize();

    Eigen::Rotation2Dd rot =
            (lateral_error > 0) ?
                    Eigen::Rotation2Dd(-M_PI / 2.0) :
                    Eigen::Rotation2Dd(M_PI / 2.0);

    Eigen::Vector2d uva2d_rot = rot * uva2d;

    Adsfi::Point3d h;
    h.x = curr_pose.position.x + fabs(lateral_error) * uva2d_rot.x();
    h.y = curr_pose.position.y + fabs(lateral_error) * uva2d_rot.y();
    h.z = curr_pose.position.z;


    // 一个交点
    if (fabs(fabs(lateral_error) - lookahead_distance_) < ERROR2)
    {
        // 横向误差和预瞄距离之差的绝对值小于error2, 则表示垂足点就是目标点
        return std::make_pair(true, h);
    }
    else
    {
        // 两个交点
        const double s = sqrt( pow(lookahead_distance_, 2) - pow(lateral_error, 2));

        Adsfi::Point3d res;
        res.x = h.x + s * uva2d.x();
        res.y = h.y + s * uva2d.y();
        res.z = curr_pose.position.z;

        return std::make_pair(true, res);
    }

}


int32_t PurePursuit::findNextPointIdx(int32_t search_start_idx)
{
    if (curr_wps_ptr_->size() < 3 || search_start_idx == -1)
    {
        return -1;
    }

    for (int32_t i = search_start_idx; i < (int32_t) curr_wps_ptr_->size(); i++)
    {
        if (i == ((int32_t) curr_wps_ptr_->size() - 1))
        {
            return i;
        }

        const auto gld = planning_utils::getLaneDirection(*curr_wps_ptr_, 0.05);
        if (gld == 0)
        {
            auto ret = planning_utils::transformToRelativeCoordinate2D(
                    curr_wps_ptr_->at(i).position,
                    *curr_pose_ptr_);

            if (ret.x < 0)
            {
                continue;
            }

        }
        else if (gld == 1)
        {
            auto ret = planning_utils::transformToRelativeCoordinate2D(
                    curr_wps_ptr_->at(i).position,
                    *curr_pose_ptr_);

            if (ret.x > 0)
            {
                continue;
            }
        }
        else
        {
            return -1;
        }

        const Adsfi::Point3d &curr_motion_point = curr_wps_ptr_->at(i).position;
        const Adsfi::Point3d &curr_pose_point = curr_pose_ptr_->position;

        const double ds = planning_utils::calcDistSquared2D(curr_motion_point,
                                                            curr_pose_point);
        if (ds > std::pow(lookahead_distance_, 2))
        {
            return i;
        }

    }

    HAF_LOG_INFO << "<"<< __FILE__ << "> " << "  $$$$ Lost waypoint! ";

    return -1;
}



void PurePursuit::setCurrentPose(const Adsfi::HafPose &msg)
{
    curr_pose_ptr_ = std::make_shared<Adsfi::HafPose>();
    *curr_pose_ptr_ = msg;
}


void PurePursuit::setWaypoints(const std::vector<Adsfi::HafPose> &msg)
{
    curr_wps_ptr_ = std::make_shared<std::vector<Adsfi::HafPose>>();
    *curr_wps_ptr_ = msg;
}


}  // namespace pure_pursuit
