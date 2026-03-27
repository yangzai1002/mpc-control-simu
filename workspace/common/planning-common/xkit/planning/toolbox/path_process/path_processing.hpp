#pragma once

#include "xkit/planning/toolbox/coordtransform/coordtransform.h"

namespace trackalgorithm {
inline void PathProcessing::SupplementPath(SiteList &path){
    avos::planning::DataPool* DP = avos::planning::DataPool::Instance();
    Site ego;
    ego.xg = DP->GetMainDataRef().localization.localization_data.xg;
    ego.yg = DP->GetMainDataRef().localization.localization_data.yg;
    ego.globalangle = DP->GetMainDataRef().localization.localization_data.yaw;
    Site last_p = path.front();
    double sum_dis = 0.0;
    avos::planning::CoordTransform *coordtransform = avos::planning::CoordTransform::Instance();
    auto iter = path.begin();
    
    for (; next(iter) != path.end(); iter++) {
        if (next(iter)->reverse != iter->reverse && iter != path.begin()) {
            iter->angle = prev(iter)->angle;
        } else {
            iter->angle = std::atan2((next(iter)->y - iter->y), (next(iter)->x - iter->x)) * 180.0 / M_PI;
            if (iter->reverse) {
                iter->angle += 180.0;
            }
        }
        if(iter->angle > 180.0) iter->angle -= 360.0;
        if(iter->angle < -180.0) iter->angle += 360.0;    

        Site gccs, p_gccs;
        p_gccs = *iter;
        // AINFO<<"ego:"<<ego.xg<<","<<ego.yg<<","<<ego.globalangle;
        // AINFO<<"pathpoint:"<<iter->x<<","<<iter->y<<","<<iter->angle;
        coordtransform->VCS2GCCS(ego, *iter, gccs);
        // AINFO<<"gccs:"<<gccs.xg<<","<<gccs.yg<<","<<gccs.globalangle;
        iter->xg = gccs.xg;
        iter->yg = gccs.yg;
        iter->globalangle = gccs.globalangle;
        sum_dis += std::hypot(iter->x - last_p.x, iter->y - last_p.y);      
        iter->length = sum_dis;
        last_p = *iter;
    }

    //最后一个点单独处理
    iter->angle = last_p.angle;
    Site gccs, p_gccs;
    p_gccs = *iter;
    coordtransform->VCS2GCCS(ego, *iter, gccs);
    iter->xg = gccs.xg;
    iter->yg = gccs.yg;
    iter->globalangle = gccs.globalangle;
    sum_dis += std::hypot(iter->x - last_p.x, iter->y - last_p.y);      
    iter->length = sum_dis;
}

inline bool PathProcessing::HeadingCheck(SiteList &path){
    Init();
    if(path.empty()) return false;//斟酌一下
    auto iter_path = path.begin();
    for(; std::next(iter_path)!=path.end() && iter_path!=path.end(); ++iter_path){
      auto iter_path_next = std::next(iter_path);
      if(fabs(NormalizeAngle(iter_path_next->angle - iter_path->angle)) > path_dheading_threshold_){
        path_sharp_iters_.emplace_back(iter_path);
      }
    }

    //*******
    //去除距离路径前段太近的异常点
    if(!path_sharp_iters_.empty()){
        double dist = 0.0;
        auto first_sharp_iter = path_sharp_iters_.front();
        for(; std::prev(first_sharp_iter)!=path.begin() && first_sharp_iter!=path.begin(); --first_sharp_iter){
            auto iter_first_sharp_prev = std::prev(first_sharp_iter);
            dist += std::hypot(iter_first_sharp_prev->x - first_sharp_iter->x, iter_first_sharp_prev->y - first_sharp_iter->y);
            if(dist > dist_to_sharp_threshold_) break;
        }
        if(dist < dist_to_sharp_threshold_){
            path_sharp_iters_.erase(path_sharp_iters_.begin());
        }
    }
    //去除距离路径后端太近的异常点
    if(!path_sharp_iters_.empty()){
        double dist = 0.0;
        auto last_sharp_iter = path_sharp_iters_.back();
        for(; std::next(last_sharp_iter)!=path.end() && last_sharp_iter!=path.end(); ++last_sharp_iter){
            auto iter_last_sharp_next = std::next(last_sharp_iter);
            dist += std::hypot(iter_last_sharp_next->x - last_sharp_iter->x, iter_last_sharp_next->y - last_sharp_iter->y);
            if(dist > dist_to_sharp_threshold_) break;
        }
        if(dist < dist_to_sharp_threshold_){
            path_sharp_iters_.pop_back();
        }
    }

    //去除距离第一个异常点太远的其他异常点
    while (path_sharp_iters_.size() >= 2) {
        auto first_sharp = path_sharp_iters_.front();
        auto last_sharp = path_sharp_iters_.back();
        double dist_avoid_too_long = 0.0;
        for(;first_sharp != last_sharp; ++first_sharp){
            auto first_sharp_next = std::next(first_sharp);
            dist_avoid_too_long += std::hypot(first_sharp_next->x - first_sharp->x, first_sharp_next->y - first_sharp->y);
            if(dist_avoid_too_long > bezier_safe_dist_){
                path_sharp_iters_.pop_back();
                break;
            }
        }
        if(first_sharp == last_sharp) break;
    }
    //
    
    //*******
    if(path_sharp_iters_.empty()) return true;
    else return false;
}

inline bool PathProcessing::BezierSmooth(SiteList &path){
    Init();
    if(path.empty()) return false;//斟酌一下
    if(HeadingCheck(path)) return false;//斟酌一下
    auto iter_start = path_sharp_iters_.front();
    auto iter_end = path_sharp_iters_.back();//暂时不用
    // auto iter_end = path_sharp_iters_.front();
    //找到异常点前一定距离的点
    double dist_to_sharp = 0.0;
    for(; std::prev(iter_start)!=path.begin() && iter_start!=path.begin(); --iter_start){
      auto iter_start_prev = std::prev(iter_start);
      dist_to_sharp += std::hypot(iter_start_prev->x - iter_start->x, iter_start_prev->y - iter_start->y);
      if(dist_to_sharp >= dist_to_sharp_threshold_)
          break;
    }
    //找到异常点后一定距离的点
    dist_to_sharp = 0.0;
    for(; std::next(iter_end)!=path.end() && iter_end!=path.end(); ++iter_end){
      auto iter_end_next = std::next(iter_end);
      dist_to_sharp += std::hypot(iter_end_next->x - iter_end->x, iter_end_next->y - iter_end->y);
      if(dist_to_sharp >= dist_to_sharp_threshold_)
          break;
    }
    //生成贝塞尔曲线并转换为SiteList
    auto bezier_ptr = std::make_shared<Bezier>();
    SiteVec partial_path_smoothed_vec;
    SiteList partial_path_smoothed_list;
    if(-1 == bezier_ptr->GenerateBezierCurve(*iter_start, *iter_end, partial_path_smoothed_vec)){
        std::cout << "GenerateBezierCurve failed" << std::endl;
        return false;//斟酌一下
    }
    auto iter = partial_path_smoothed_vec.begin();
    while(std::next(iter) != partial_path_smoothed_vec.end()){
        partial_path_smoothed_list.emplace_back(*iter);
        ++iter;
    }
    //替换拼接
    path.erase(std::next(iter_start), std::prev(iter_end));
    path.splice(std::next(iter_start), partial_path_smoothed_list, next(partial_path_smoothed_list.begin()), partial_path_smoothed_list.end());
    //插值
    auto start_iter = path.begin();
    auto end_iter = std::next(start_iter);
    while(end_iter != path.end())
    {
        LinearInterpolation(start_iter,end_iter, 0.1, path);
        start_iter = end_iter;
        end_iter = std::next(start_iter);
    }
    //补全信息
    SupplementPath(path);
    // std::cout << "after supplementpath: " <<  std::endl;
    // for(const auto &p: path){
    //     std::cout << p.x << "\t" << p.y << "\t" << p.angle << "\t" << p.xg << "\t" << p.yg << "\t" << p.globalangle << "\t" << p.length << std::endl;
    // }
    return true;
}

inline bool PathProcessing::LinearInterpolation(const SiteList::iterator& start_iter,
                                         SiteList::iterator& end_iter, const double resolution,
                                         SiteList& result_path) {
    if (resolution < 0.1 || resolution > 5) return false;
    double length = std::hypot(start_iter->x - end_iter->x, start_iter->y - end_iter->y);
    if (length < 1.6*resolution) {
        return true;
    }
    int count = static_cast<int>((length+resolution/2) / resolution);
    if(count < 2) return false;
    double detal_x = (end_iter->x - start_iter->x) / static_cast<double>(count);
    double detal_y = (end_iter->y - start_iter->y) / static_cast<double>(count);
    double detal_angle = NormalizeAngle(end_iter->angle - start_iter->angle) / static_cast<double>(count);
    SiteList insert_path;
    insert_path.clear();
    Site position;
    auto iter_current = end_iter;
    for (int index = 1; index < count; index++) {
        position.x = start_iter->x + detal_x * index;
        position.y = start_iter->y + detal_y * index;
        position.angle = NormalizeAngle(start_iter->angle + detal_angle * index);
        position.reverse = start_iter->reverse;
        insert_path.push_back(position);
    }
    result_path.insert(end_iter,insert_path.begin(),insert_path.end());
    return true;
}

inline void PathProcessing::Init(){
    avos::planning::DataPool* DP = avos::planning::DataPool::Instance();
    path_dheading_threshold_ = DP->GetMainDataRef().config_info.behavior_config.path_dheading_threshold;
    dist_to_sharp_threshold_ = DP->GetMainDataRef().config_info.behavior_config.dist_to_sharp_threshold;
    bezier_safe_dist_ = DP->GetMainDataRef().config_info.behavior_config.bezier_safe_dist;
}

inline bool PathProcessing::LinearInterpolation(SiteList &path, const double& resolution){
    auto start_iter = path.begin();
    auto end_iter = std::next(start_iter);
    while(end_iter != path.end())
    {
        LinearInterpolation(start_iter,end_iter, resolution, path);
        start_iter = end_iter;
        end_iter = std::next(start_iter);
    }
}

inline void PathProcessing::ThinningPath(SiteList& path, size_t num) {
    if (path.empty())
        return;

    SiteList t_path;
    int i = 0;
    for (auto it = path.begin(); std::next(it) != path.end(); it++) {
        if (i++ % num == 0) {
            t_path.emplace_back(*it);
        }
    }
    t_path.emplace_back(path.back());
    path = t_path;
}

inline SiteList PathProcessing::ExtendPath(const SiteList& path, const double& extensionDistance,
                                    const int& direction, const double& angle,
                                    const double& resolution) {
    SiteList extendedPath = path;

    if (path.empty()) {
        return extendedPath;
    }

    int numPoints = static_cast<int>(extensionDistance / resolution);
    if (numPoints <= 0) {
        return extendedPath;
    }

    Site s;
    if (direction == -1) {
        const Site& startPoint = path.front();
        double heading = std::atan2((++path.begin())->y - path.begin()->y,
                                    (++path.begin())->x - path.begin()->x);
        for (int i = 0; i < numPoints; ++i) {
            s.x = startPoint.x - (i + 1) * resolution * std::cos(heading);
            s.y = startPoint.y - (i + 1) * resolution * std::sin(heading);
            s.length = startPoint.length - (i + 1) * resolution;
            s.angle = NormalizeAngle(heading * 180.0 / M_PI);
            extendedPath.emplace_front(s);
        }
    } else if (direction == 1) {
        const Site& endPoint = path.back();
        auto last = std::prev(path.end(), 2);
        double heading = std::atan2(endPoint.y - last->y, endPoint.x - last->x);
        for (int i = 0; i < numPoints; ++i) {
            s.x = endPoint.x + (i + 1) * resolution * std::cos(heading);
            s.y = endPoint.y + (i + 1) * resolution * std::sin(heading);
            s.length = endPoint.length + (i + 1) * resolution;
            s.angle = NormalizeAngle(heading * 180.0 / M_PI);
            extendedPath.emplace_back(s);
        }
    } else if (direction == 0) {
        const Site& endPoint = path.back();
        for (int i = 0; i < numPoints; ++i) {
            s.x = endPoint.x + (i + 1) * resolution * std::cos(angle * M_PI / 180);
            s.y = endPoint.y + (i + 1) * resolution * std::sin(angle * M_PI / 180);
            s.length = endPoint.length + (i + 1) * resolution;
            s.angle = NormalizeAngle(angle);
            extendedPath.emplace_back(s);
        }
    }

    return extendedPath;
}

inline std::pair<SiteList::iterator, double>
PathProcessing::FindNearestPoint(const double& x, const double& y, SiteList& path) {
    if (path.empty()) {
        throw std::runtime_error("Path is empty");
    }
    
    double minDistance = std::numeric_limits<double>::max();
    SiteList::iterator nearestPointIter;

    for (auto it = path.begin(); it != path.end(); ++it) {
        double dist = hypot(x - it->x, y - it->y);
        if (dist < minDistance) {
            minDistance = dist;
            nearestPointIter = it;
        }
    }

    return std::make_pair(nearestPointIter, minDistance);
}

inline double PathProcessing::NormalizeAngle(const double &x) {
    double v = fmod(x, 360);
    if (v < -180) {
        v += 360;
    } else if (v > 180) {
        v -= 360;
    }
    return v;
}

}
