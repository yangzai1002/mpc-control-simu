#pragma once

#include "cartesian_frenet_conversion.h"

namespace avos{
namespace planning{

inline void CartesianFrenetConverter::CartesianToFrenet(std::list<Site>& ref_cartesian_path,
                                                 const std::list<Site>& cartesian_path, 
                                                 std::list<Site>& frenet_path){
    frenet_path.clear();
    match_iterators_.clear();
    frenet_path.resize(cartesian_path.size());
    match_iterators_.reserve(frenet_path.size());

    auto iter_frenet = frenet_path.begin();
    auto match_iteration = ref_cartesian_path.begin();
    for(auto iter_cartesian = cartesian_path.begin(); iter_cartesian != cartesian_path.end(); ++iter_cartesian){
        FindNearestPoint(ref_cartesian_path, iter_cartesian->x, iter_cartesian->y, match_iteration);
        CartesianToFrenet(match_iteration->length, 
                          match_iteration->x, 
                          match_iteration->y, 
                          match_iteration->angle,
                          iter_cartesian->x, iter_cartesian->y,
                          iter_frenet->s, iter_frenet->l);
        iter_frenet = std::next(iter_frenet);
        match_iterators_.emplace_back(match_iteration);
    }
    RemoveSameS(frenet_path);
}

inline void CartesianFrenetConverter::CartesianToFrenet(std::list<Site>& ref_cartesian_path,
                                                 SortedTree& static_obj,
                                                 SortedTree& dynamic_obj,
                                                 SortedTree& static_lidar_cell,
                                                 SortedTree& static_ultrasonic_cell, 
                                                 std::map<double,std::multiset<double>>& frenet_cells){
    for(auto iter = ref_cartesian_path.begin(); iter != ref_cartesian_path.end(); ++iter){
        std::multiset<double> bound;
        bound.insert(-20.0);
        bound.insert(20.0);
        frenet_cells.insert(std::make_pair(iter->length, bound));
    }
    for(auto &obj_cells : static_obj){
        for(auto &obj_cell : obj_cells.second){
            auto iter_refpath = ref_cartesian_path.begin();
            double s,l;
            FindNearestPoint(ref_cartesian_path, obj_cell.x, obj_cell.y, iter_refpath);
            CartesianToFrenet(iter_refpath->length, 
                              iter_refpath->x, 
                              iter_refpath->y, 
                              iter_refpath->angle,
                              obj_cell.x, obj_cell.y,
                              s, l);
            frenet_cells.at(s).insert(l);
        }
    }
    for(auto &obj_cells : dynamic_obj){
        for(auto &obj_cell : obj_cells.second){
            auto iter_refpath = ref_cartesian_path.begin();
            double s,l;
            FindNearestPoint(ref_cartesian_path, obj_cell.x, obj_cell.y, iter_refpath);
            CartesianToFrenet(iter_refpath->length, 
                              iter_refpath->x, 
                              iter_refpath->y, 
                              iter_refpath->angle,
                              obj_cell.x, obj_cell.y,
                              s, l);
            frenet_cells.at(s).insert(l);
        }
    }
    for(auto &obj_cells : static_lidar_cell){
        for(auto &obj_cell : obj_cells.second){
            auto iter_refpath = ref_cartesian_path.begin();
            double s,l;
            FindNearestPoint(ref_cartesian_path, obj_cell.x, obj_cell.y, iter_refpath);
            CartesianToFrenet(iter_refpath->length, 
                              iter_refpath->x, 
                              iter_refpath->y, 
                              iter_refpath->angle,
                              obj_cell.x, obj_cell.y,
                              s, l);
            frenet_cells.at(s).insert(l);
        }
    }
    for(auto &obj_cells : static_ultrasonic_cell){
        for(auto &obj_cell : obj_cells.second){
            auto iter_refpath = ref_cartesian_path.begin();
            double s,l;
            FindNearestPoint(ref_cartesian_path, obj_cell.x, obj_cell.y, iter_refpath);
            CartesianToFrenet(iter_refpath->length, 
                              iter_refpath->x, 
                              iter_refpath->y, 
                              iter_refpath->angle,
                              obj_cell.x, obj_cell.y,
                              s, l);
            frenet_cells.at(s).insert(l);
        }
    }
}

inline void CartesianFrenetConverter::CartesianToFrenet(const double& rs, const double& rx, const double& ry, const double& rtheta,
                                                 const double& x, const double& y, double& s, double& l) {
    const double dx = x - rx;
    const double dy = y - ry;

    const double cos_theta_r = std::cos(rtheta / 57.3);
    const double sin_theta_r = std::sin(rtheta / 57.3);

    const double cross_rd_nd = cos_theta_r * dy - sin_theta_r * dx;
    l = std::copysign(std::sqrt(dx * dx + dy * dy), cross_rd_nd);
    s = rs;
}

inline void CartesianFrenetConverter::FrenetToCartesian(const std::list<Site>& ref_cartesian_path,
                                                 const std::list<Site>& frenet_path, 
                                                 std::list<Site>& cartesian_path){
    cartesian_path.resize(frenet_path.size());
    auto iter_cartesian = cartesian_path.begin();

    int index = 0;
    for(auto iter_frenet = frenet_path.begin(); iter_frenet != frenet_path.end(); ++iter_frenet){
        FrenetToCartesian(match_iterators_.at(index)->length, match_iterators_.at(index)->x,
                          match_iterators_.at(index)->y, match_iterators_.at(index)->angle,
                          iter_frenet->s, iter_frenet->l, iter_cartesian->x, iter_cartesian->y);
        iter_cartesian = std::next(iter_cartesian);
        index++;
    }
}

inline void CartesianFrenetConverter::FrenetToCartesian(const double& rs, const double& rx, const double& ry, const double& rtheta,
                                                 const double& s_condition,
                                                 const double& d_condition, 
                                                 double& x, double& y) {
    if(std::abs(rs - s_condition) > 1.0e-6)
        std::cout<< "The reference point s and s_condition don't match" << std::endl;

    const double cos_theta_r = std::cos(rtheta/57.3);
    const double sin_theta_r = std::sin(rtheta/57.3);

    x = rx - sin_theta_r * d_condition;
    y = ry + cos_theta_r * d_condition;
}

inline void CartesianFrenetConverter::FindNearestPoint(const std::list<Site>& path,
                                                const double& x, const double& y,
                                                std::list<Site>::iterator& min_dis_iter){
    double min_dis = std::numeric_limits<double>::max();
    int count = 0;
    for(auto iter = min_dis_iter; iter != path.end(); ++iter){
        double dis = std::hypot(iter->x - x, iter->y - y);
        if(dis < min_dis){
            min_dis = dis;
            min_dis_iter = iter;
            count = 0;
        }
        else{
            count++;
        }
        if(count > 10) break;
    }
}

inline void CartesianFrenetConverter::RemoveSameS(std::list<Site>& frenet_path) {
    std::unordered_set<double> unique_s;
    auto itrA = frenet_path.begin();
    auto itrB = match_iterators_.begin();

    while (itrA != frenet_path.end()) {
        if (unique_s.count(itrA->s) > 0) {
            itrA = frenet_path.erase(itrA);
            itrB = match_iterators_.erase(itrB);
        } else {
            unique_s.insert(itrA->s);
            ++itrA;
            ++itrB;
        }
    }
}

}
}