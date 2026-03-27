#pragma once
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "xkit/planning/toolbox/pursuit_interface/pursuit_interface.h"
#include "unstruct_refpath_define.h"
#include "xkit/planning/datapool/task_input.h"
#include <cstdio>

using geometry::Site;
using geometry::SiteVec;

namespace avos {
namespace planning {

inline UnstructRefpath::UnstructRefpath() {
  std::cout << "UnstructRefpath" << std::endl;
 Reset();
}

inline UnstructRefpath::~UnstructRefpath() {}

inline void UnstructRefpath::Reset() {
  is_avoid_ = false;
  cur_reverse_ = false;
  local_path_index_ = 0;
  enable_struct_behavior_ = false;
  // raw_path_.reserve(500);
  // local_path_.reserve(500);
  cur_nearest_index_ = 0;
  cur_nearest_origin_index_ = 0;
  raw_path_.clear();
  local_path_.clear();
  sehs_raw_path_.clear();
}

inline int UnstructRefpath::InitPath(std::vector<InnerPointInfo> &raw_path,
                              std::string &err_code) {
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
//  printf("ssssssssssssssssssssssssss\n");
  if (raw_path.empty()) {
    err_code = std::to_string(UNST_INIT_PATH_EMPTY);
    return -1;
  }
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;

  raw_path_.clear();
  local_path_.clear();
  sehs_raw_path_.clear();
  sehs_raw_path_ = raw_path;
  geometry::Site pt;
  for (const auto &p : raw_path) {
    pt.x = p.point.x;
    pt.y = p.point.y;
    pt.angle = p.point.angle;
    pt.xg = p.global_point.xg;
    pt.yg = p.global_point.yg;
    pt.globalangle = p.global_point.global_angle;
    pt.index = p.index;
    pt.origin_index = p.index;
    pt.length = p.length;
    // AERROR << "REFPATH:" << pt.length;
    pt.reverse = p.direction == 0 ? false : true;
    pt.curvature = p.curvature;
    raw_path_.push_back(pt);
    if(pt.index > 0){
      local_path_.push_back(pt);
      continue;
    }//local_path_仅利用平滑后的点
    
  }
  local_path_index_ = 0;
  is_avoid_ = false;
  cur_reverse_ = false;
  cur_nearest_index_ = 0;
  cur_nearest_origin_index_ = 0;
  return 0;
}

inline int UnstructRefpath::InitPathDR(std::vector<InnerPointInfo> &raw_path,
                              std::string &err_code,
                              const LocalizationData &loc_data) {
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
//  printf("ssssssssssssssssssssssssss\n");
  if (raw_path.empty()) {
    err_code = std::to_string(UNST_INIT_PATH_EMPTY);
    return -1;
  }
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;

  raw_path_.clear();
  local_path_.clear();
  sehs_raw_path_.clear();
  sehs_raw_path_ = raw_path;
  geometry::Site pt;
  for (const auto &p : raw_path) {
    pt.x = p.point.x;
    pt.y = p.point.y;
    pt.angle = p.point.angle;
    pt.xg = p.global_point.xg;
    pt.yg = p.global_point.yg;
    pt.globalangle = p.global_point.global_angle;
    pt.index = p.index;
    pt.origin_index = p.index;
    pt.length = p.length;
    // AERROR << "REFPATH:" << pt.length;
    pt.reverse = p.direction == 0 ? false : true;
    pt.curvature = p.curvature;
    raw_path_.push_back(pt);
    if(pt.index > 0){
      local_path_.push_back(pt);
      continue;
    }//local_path_仅利用平滑后的点
    
  }
  local_path_index_ = 0;
  is_avoid_ = false;
  cur_reverse_ = false;
  cur_nearest_index_ = 0;
  cur_nearest_origin_index_ = 0;

  // 根据绝对定位查找最近点并更新 index
  if (!local_path_.empty()) {
    double min_dis = std::numeric_limits<double>::max();
    int nearest_ind = 0;
    auto near_it = local_path_.begin();
    int ind = 0;
    for (auto it = local_path_.begin(); it != local_path_.end(); ++it, ++ind) {
      double dis = std::hypot(it->xg - loc_data.loc_xg_dr, it->yg - loc_data.loc_yg_dr);
      if (dis < min_dis) {
        min_dis = dis;
        nearest_ind = ind;
        near_it = it;
      }
    }
    local_path_index_ = nearest_ind;
    cur_nearest_index_ = near_it->index;
    cur_nearest_origin_index_ = near_it->origin_index;
    cur_reverse_ = near_it->reverse;
    if (cur_nearest_origin_index_ != cur_nearest_index_) {
      is_avoid_ = true;
    } else {
      is_avoid_ = false;
    }
  }
  return 0;
}

//默认参非follw模式下不要传入！ xyk
inline int UnstructRefpath::PathExtraction(const double &speed_limit,
    const LocalizationData &loc_data,
    const std::vector<InnerPointInfo> &raw_path, const CarModel &car_model,
    const bool &is_front, Paths &paths, std::string &err_code, int follow_diff_index, bool follow_avoid_flag) {
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  err_code = "";
  loc_data_ = loc_data;
  // 1.检测定位是否有效
  if (std::fabs(loc_data.xg) < 0.1 && std::fabs(loc_data.yg) < 0.1) {
    err_code += std::to_string(UNST_LOCALIZATION_ERROR);
  }
  // 2.检测原始路径是否有效
  if (-1 == CheckRawValid(raw_path, err_code)) return -1;
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;

  // 3.将原始路径中尾端多余的点添加到localpath中
  AddRawToLocal(raw_path, err_code, follow_diff_index, follow_avoid_flag);
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  bool cur_reverse = cur_reverse_;
  // 4.检测路径是否存在前后向折点
  CheckFrontPtExistTurn(is_front, cur_reverse);
 // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  // 5.提取最近点，获取前后向路径
  GetForwaradAndBackwardPaths(speed_limit, is_front, cur_reverse,
                              paths.front_local_path.path,
                              paths.back_local_path.path, err_code);
   // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  // 6.原始路径转换
  GetRawPath(raw_path, paths.front_local_path_raw.path);
   // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  // 7.生成前向纯跟踪路径
 GetFrontPursuitPath(paths.front_local_path, car_model,
                      paths.front_pursuit_path);
   // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  // 8.在路径太长的情况下，剪断前面的一部分
  PathCutting();
   // std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  return 0;
}

//默认参非follw模式下不要传入！ xyk
inline int UnstructRefpath::PathExtractionDr(const double &speed_limit,
  const LocalizationData &loc_data,
  const std::vector<InnerPointInfo> &raw_path, const CarModel &car_model,
  const bool &is_front, Paths &paths, std::string &err_code) {
err_code = "";
loc_data_ = loc_data;
// 1.检测定位是否有效
if (std::fabs(loc_data.xg) < 0.1 && std::fabs(loc_data.yg) < 0.1) {
  err_code += std::to_string(UNST_LOCALIZATION_ERROR);
}
// 2.检测原始路径是否有效
if (-1 == CheckRawValid(raw_path, err_code)) return -1;

bool cur_reverse = cur_reverse_;
// 4.检测路径是否存在前后向折点
CheckFrontPtExistTurn(is_front, cur_reverse);
// 5.提取最近点，获取前后向路径
GetForwaradAndBackwardPathsDr(speed_limit, is_front, cur_reverse,
                            paths.front_local_path.path,
                            paths.back_local_path.path, err_code);
// 6.原始路径转换
GetRawPath(raw_path, paths.front_local_path_raw.path);
// 7.生成前向纯跟踪路径
GetFrontPursuitPathDr(paths.front_local_path, car_model,
                    paths.front_pursuit_path);
// 8.在路径太长的情况下，剪断前面的一部分
PathCutting();
return 0;
}

inline int UnstructRefpath::WriteCalculatePath(
    const LocalizationData &loc_data,
    const std::vector<InnerPointInfo> &raw_path, const PathData &edit_path,
    std::string &err_code) {
  // todo
  if (-1 == CalculatePathCheck(loc_data, sehs_raw_path_, edit_path, err_code)) {
    return -1;
  }
  // 根据写入路径初始点来获取localpath的写入起点
  std::cout << "local_path_index_prev:" << local_path_index_ << std::endl;
  int back_index = 0;
  int front_index = 0;
  int path_size = local_path_.size();
  if(path_size == 0 && edit_path.path.size() > 0) {
    raw_path_.clear();
    local_path_.clear();
    sehs_raw_path_.clear();
    sehs_raw_path_ = raw_path;
    geometry::Site pt;
    for (const auto &p : edit_path.path) {
      // pt.x = p.x;
      // pt.y = p.y;
      // pt.angle = p.angle;
      // pt.xg = p.xg;
      // pt.yg = p.yg;
      // pt.globalangle = p.globalangle;
      // pt.index = p.index;
      // pt.origin_index = p.index;
      // pt.length = p.length;
      // // AERROR << "REFPATH:" << pt.length;
      // pt.reverse = p.direction == 0 ? false : true;
      // pt.curvature = p.curvature;
      raw_path_.push_back(p);
      // if(p.index > 0){
        local_path_.push_back(p);
        // continue;
      // }//local_path_仅利用平滑后的点
      
    }
    local_path_index_ = 0;
    is_avoid_ = false;
    cur_reverse_ = false;
    cur_nearest_index_ = 0;
    cur_nearest_origin_index_ = 0;
    return 0;
  }
  front_index = local_path_index_ + 30 < path_size ? local_path_index_ + 30
                                                   : path_size - 1;
  back_index = local_path_index_ - 30 >= 0 ? local_path_index_ - 30 : 0;
  if (front_index < back_index) {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_INDEX_ERROR);
    std::cout << "front_index < back_index" << std::endl;
    return -1;
  }
  auto it = local_path_.begin();
  auto near_it = local_path_.begin();
  for (int i = 0; i < back_index; i++) {
    if (std::next(it) == local_path_.end()) {
      break;
    }
    it = std::next(it);
    near_it = std::next(near_it);
  }
  double min_dis = std::numeric_limits<double>::max();
  int nearest_ind = local_path_index_;
  for (int i = back_index; i <= front_index; i++) {
    double dis = std::hypot(it->xg - edit_path.path.front().xg,
                            it->yg - edit_path.path.front().yg);
        // double dis = std::hypot(it->x - edit_path.path.front().x,
        //                     it->y - edit_path.path.front().y);
    if (dis <= min_dis) {
      min_dis = dis;
      nearest_ind = i;
      near_it = it;
    }
    if (std::next(it) == local_path_.end()) {
      break;
    }
    it = std::next(it);
  }
  local_path_index_ = nearest_ind;
  std::cout << "local_path_index_prev:" << local_path_index_ << std::endl;
  // 写入路径的处理
  double last_length = 0.0;
  bool start_avoid = false;
  if (local_path_index_ < 10) {
    start_avoid = true;
    last_length = 0.0;
  } else {
    start_avoid = false;
    last_length = std::prev(near_it)->length;
  }
  std::list<geometry::Site> write_path = edit_path.path;
  // std::cout << "write_path size = :" << write_path.size() << std::endl;

  if(write_path.empty()) {
    std::cout << "write_path is empty!!!!!" << std::endl;
    return -1;
  }
  // std::cout << "write_path size2 = :" << write_path.size() << std::endl;

  geometry::Site back_pt = write_path.front();
  // std::cout << "near_it:" << std::setprecision(15) << near_it->xg << ","
  //           << std::setprecision(15) << near_it->yg << "," << near_it->globalangle
  //           << std::endl;
  // std::cout << "back_pt:" << std::setprecision(15) << back_pt.xg << ","
  //           << std::setprecision(15) << back_pt.yg << "," << back_pt.globalangle
  //           << std::endl;
  // 注意对length的处理
  int write_index = 0;
  for (auto &p : write_path) {
    p.index = edit_path.index;
    // p.origin_index = sehs_raw_path_.front().index +
    //                  write_index *
    //                      (edit_path.index - sehs_raw_path_.front().index) /
    //                      write_path.size();
       last_length += std::hypot(back_pt.xg - p.xg, back_pt.yg - p.yg);
   
    p.length = last_length;
    back_pt = p;
    write_index++;
  }
  //std::cout << "local_path_index_prev:" << local_path_index_ << std::endl;

  int count = -1;
  auto end_near_it = near_it;
  auto end_it = local_path_.begin();
  min_dis = std::numeric_limits<double>::max();
  for( ; end_it != local_path_.end(); ++end_it) {
    ++count;
    // if(count > 800) break;
    double temp_length = std::hypot(end_it->xg - write_path.back().xg, end_it->yg - write_path.back().yg);
   // std::cout <<  "temp_length = " << temp_length << "," << end_it->x << std::endl;
    if(temp_length < min_dis) {
      min_dis = temp_length;
      end_near_it = end_it;
    }
  }
  std::list<geometry::Site> tail_road(end_near_it, local_path_.end());
  //std::cout << "local_path_index_prev:" << local_path_index_ << "," << local_path_.size() << std::endl;

  if (start_avoid) {
    local_path_.clear();
    local_path_index_ = 0;
  } else {
    local_path_.erase(near_it, local_path_.end());
  }
  local_path_.insert(local_path_.end(), write_path.begin(), write_path.end());
 // std::cout << "local_path_index_prev:" << local_path_index_ << "," << local_path_.size() << std::endl;
{
    double tail_length = 0.0;
    if (!tail_road.empty() && !local_path_.empty()) {
      tail_length = std::hypot(local_path_.back().xg - tail_road.front().xg,
                               local_path_.back().yg - tail_road.front().yg);
      double length = tail_road.front().length;
      for (auto& p : tail_road) {
          p.length = p.length - length + local_path_.back().length + tail_length;
      }
    }
   // std::cout << "local_path_index_prev:" << local_path_index_ <<  "," << tail_road.size() << std::endl;
    local_path_.insert(local_path_.end(), tail_road.begin(), tail_road.end());
  }
return 0;
}

inline int UnstructRefpath::CheckRawValid(
    const std::vector<InnerPointInfo> &raw_path, std::string &err_code) {
  if (raw_path.empty()) {
    err_code += std::to_string(UNST_RAW_ECTRACTION_RAW_EMPTY);
    return -1;
  }
  return 0;
}

//默认参非follw模式下不要传入！ xyk
inline int UnstructRefpath::AddRawToLocal(
    const std::vector<InnerPointInfo> &raw_path, std::string &err_code, int follow_index_diff, bool follow_avoid_flag) {
  std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;
  if (local_path_.empty()) {
    std::cout<<"local_path_ is empty" << std::endl;
    geometry::Site pt;
    for (const auto &p : raw_path) {
      if(p.index < 0){break;}//local_path_仅利用平滑后的点
      pt.x = p.point.x;
      pt.y = p.point.y;
      pt.angle = p.point.angle;
      pt.xg = p.global_point.xg;
      pt.yg = p.global_point.yg;
      pt.globalangle = p.global_point.global_angle;
      pt.index = p.index;
      pt.origin_index = p.index;
      pt.length = p.length;
      pt.reverse = p.direction == 0 ? false : true;
      pt.curvature = p.curvature;
      local_path_.push_back(pt);
    }
    local_path_index_ = 0;
    return 0;
  }
  std::cout << __FUNCTION__ << "," << __LINE__ << std::endl;

  auto riter = raw_path.rbegin();
  int real_raw_path_index = -999;
  for(;riter != raw_path.rend(); ++riter){
    if(riter->index > 0){
      real_raw_path_index = riter->index;
      break;
    }
  }
  std::cout<<"local_path_.back().index: "<<local_path_.back().index
       <<", raw_path.back().index: "<<real_raw_path_index << std::endl;
  std::cout<<"follow_index_diff:"<<follow_index_diff << std::endl;
  if((follow_index_diff > 0) && (follow_avoid_flag == true)){
    local_path_.back().index += follow_index_diff;
  }
  if (local_path_.back().index < real_raw_path_index) {
    std::cout <<"Add raw to local" << std::endl;
    std::vector<geometry::Site> insert_path;
    geometry::Site pt;
    for (int i = raw_path.size() - 1; i >= 0; i--) {
      if(raw_path[i].index < 0) continue;//local_path_仅利用平滑后的点
      if(raw_path[i].index <= local_path_.back().index) break;
      pt.x = raw_path[i].point.x;
      pt.y = raw_path[i].point.y;
      pt.angle = raw_path[i].point.angle;
      pt.xg = raw_path[i].global_point.xg;
      pt.yg = raw_path[i].global_point.yg;
      pt.globalangle = raw_path[i].global_point.global_angle;
      pt.index = raw_path[i].index;
      pt.origin_index = raw_path[i].index;
      pt.length = raw_path[i].length;
      pt.reverse = raw_path[i].direction == 0 ? false : true;
      pt.curvature = raw_path[i].curvature;
      insert_path.push_back(pt);
    }
    //added by zz 
    double end_length =
        std::hypot(local_path_.back().xg - insert_path.back().xg,
                   local_path_.back().yg - insert_path.back().yg);
    for (auto &p : insert_path) {
      p.length = p.length - insert_path.back().length +
                 local_path_.back().length + end_length;
    }
    local_path_.insert(local_path_.end(), insert_path.rbegin(),
                       insert_path.rend());
  }

  return 0;
}

inline bool UnstructRefpath::CheckFrontPtExistTurn(const bool &is_front,
                                            bool &cur_reverse) {
  if (!is_front) return false;
  int nearest_it = 0;
  auto it = local_path_.begin();
  for (int i = 0; i < local_path_index_; i++) {
    if (std::next(it) == local_path_.end()) break;
    it = std::next(it);
  }
  //最近路点的方向
  cur_reverse = it->reverse;
  double cur_length = it->length;
  bool exist_turn = false;
  auto turn_it = local_path_.begin();
  for (int i = 0; i < 10; i++) {
    if (std::next(it) == local_path_.end()) break;
    if (std::next(it)->reverse != cur_reverse && it->length - cur_length < 0.3) {
      exist_turn = true;
      turn_it = std::next(it);
      break;
    }
    it = std::next(it);
  }
  if (exist_turn) {
    cur_reverse = turn_it->reverse;
    local_path_.erase(local_path_.begin(), turn_it);
    local_path_index_ = 0;
    return true;
  }
  return false;
}

inline int UnstructRefpath::GetForwaradAndBackwardPaths(const double &speed_limit,
    const bool &is_front, const bool &cur_reverse,
    std::list<geometry::Site> &forward_path,
    std::list<geometry::Site> &backward_path, std::string &err_code) {
  forward_path.clear();
  backward_path.clear();
  if (local_path_.empty()) {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_EMPTY);
    return -1;
  }
  int path_size = local_path_.size();
  if (local_path_index_ < 0 || local_path_index_ >= path_size) {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_INDEX_OUTOF_BOUNDARY);
    return -1;
  }
  int back_index = 0;
  int front_index = 0;
  if (is_front) {
    front_index = local_path_index_ + 100 < path_size ? local_path_index_ + 100
                                                      : path_size - 1;
    back_index = local_path_index_;

  } else {
    front_index = local_path_index_;
    back_index = local_path_index_ - 50 >= 0 ? local_path_index_ - 50 : 0;
  }
  if (front_index < back_index) {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_INDEX_ERROR);
    return -1;
  }
  auto it = local_path_.begin();
  auto near_it = local_path_.begin();
  for (int i = 0; i < back_index; i++) {
    if (std::next(it) == local_path_.end()) {
      break;
    }
    it = std::next(it);
  }
  double min_dis = std::numeric_limits<double>::max();
  int nearest_ind = local_path_index_;
  cur_reverse_ = cur_reverse;
  for (int i = back_index; i <= front_index; i++) {
    double dis = 0.0;
      dis = std::hypot(it->xg - loc_data_.xg, it->yg - loc_data_.yg); 
    if (dis <= min_dis && it->reverse == cur_reverse) {
      min_dis = dis;
      nearest_ind = i;
      cur_reverse_ = it->reverse;
      near_it = it;

    }
    if(it->reverse != cur_reverse) break;
    if (std::next(it) == local_path_.end()) {
      break;
    }
    it = std::next(it);
  }
  local_path_index_ = nearest_ind;

  cur_nearest_index_ = near_it->index;
  cur_nearest_origin_index_ = near_it->origin_index;
  if (cur_nearest_origin_index_ != cur_nearest_index_) {
    is_avoid_ = true;
  } else {
    is_avoid_ = false;
  }

  auto front_it = near_it;
  auto back_it = near_it;
  double max_length_front = 0.0;
  double max_length_back = 0.0;
  std::cout<<"speed_limit"<<speed_limit;
  if(std::fabs(speed_limit) <= 8.35){//0-30km/h
    max_length_front = 100.0;
    max_length_back = 20.0;
  }else if(std::fabs(speed_limit) <= 16.67){//30-60km/h
    max_length_front = 150.0;
    max_length_back = 20.0;
  }else if(std::fabs(speed_limit) <= 22.23){//60-80km/h
    max_length_front = 350.0;
    max_length_back = 20.0;
  }else{
    max_length_front = 350.0;
    max_length_back = 20.0;
  }
  std::cout <<"max_length_front："<<max_length_front << std::endl;
  for (; front_it != local_path_.end(); front_it = std::next(front_it)) {
    if (front_it->length - near_it->length > max_length_front) break;
    forward_path.push_back(*front_it);
  }
  for (; back_it != local_path_.begin(); back_it = std::prev(back_it)) {
    if (near_it->length - back_it->length > max_length_back) break;
    backward_path.push_back(*back_it);
  }
  CoordTransform *coordtransform = CoordTransform::Instance();
  geometry::Site rel_site;
  for (auto &p : forward_path) {
          if (false == coordtransform->GCCS2VCS(loc_data_, p, rel_site)) {
              std::cout << "CoordTransform ERROR" << loc_data_.xg << "," << loc_data_.yg << "," << p.xg
                     << "," << p.yg << std::endl;
          }
          p.x = rel_site.x;
          p.y = rel_site.y;
          p.angle = p.globalangle - loc_data_.yaw;
    
  }
  for (auto &p : backward_path) {
  coordtransform->GCCS2VCS(loc_data_, p, rel_site);
    p.x = rel_site.x;
    p.y = rel_site.y;
    p.angle = p.globalangle - loc_data_.yaw;
    
  
    p.reverse = !p.reverse;
  }
  if (!forward_path.empty()) {
    std::cout << "near_it:" << near_it->length << std::endl;
    std::cout << "it:" << it->length << std::endl;
    std::cout << "start:" << forward_path.front().length << std::endl;
    std::cout << "end:" << forward_path.back().length << std::endl;
  }
  return 0;
}

inline int UnstructRefpath::GetForwaradAndBackwardPathsDr(const double &speed_limit,
                                                          const bool &is_front, const bool &cur_reverse,
                                                          std::list<geometry::Site> &forward_path,
                                                          std::list<geometry::Site> &backward_path, std::string &err_code)
{
  forward_path.clear();
  backward_path.clear();
  if (local_path_.empty())
  {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_EMPTY);
    return -1;
  }
  int path_size = local_path_.size();
  if (local_path_index_ < 0 || local_path_index_ >= path_size)
  {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_INDEX_OUTOF_BOUNDARY);
    return -1;
  }
  int back_index = 0;
  int front_index = 0;
  if (is_front)
  {
    front_index = local_path_index_ + 100 < path_size ? local_path_index_ + 100
                                                      : path_size - 1;
    back_index = local_path_index_;
  }
  else
  {
    front_index = local_path_index_;
    back_index = local_path_index_ - 50 >= 0 ? local_path_index_ - 50 : 0;
  }
  if (front_index < back_index)
  {
    err_code += std::to_string(UNST_GET_PATH_LOCAL_INDEX_ERROR);
    return -1;
  }
  auto it = local_path_.begin();
  auto near_it = local_path_.begin();
  for (int i = 0; i < back_index; i++)
  {
    if (std::next(it) == local_path_.end())
    {
      break;
    }
    it = std::next(it);
  }
  double min_dis = std::numeric_limits<double>::max();
  int nearest_ind = local_path_index_;
  cur_reverse_ = cur_reverse;
  for (int i = back_index; i <= front_index; i++)
  {
    double dis = 0.0;
    dis = std::hypot(it->xg - loc_data_.loc_xg_dr, it->yg - loc_data_.loc_yg_dr);
    if (dis <= min_dis && it->reverse == cur_reverse)
    {
      min_dis = dis;
      nearest_ind = i;
      cur_reverse_ = it->reverse;
      near_it = it;
    }
    if (it->reverse != cur_reverse)
      break;
    if (std::next(it) == local_path_.end())
    {
      break;
    }
    it = std::next(it);
  }
  local_path_index_ = nearest_ind;

  cur_nearest_index_ = near_it->index;
  cur_nearest_origin_index_ = near_it->origin_index;
  if (cur_nearest_origin_index_ != cur_nearest_index_)
  {
    is_avoid_ = true;
  }
  else
  {
    is_avoid_ = false;
  }

  auto front_it = near_it;
  auto back_it = near_it;
  double max_length_front = 0.0;
  double max_length_back = 0.0;
  std::cout << "speed_limit" << speed_limit;
  if (std::fabs(speed_limit) <= 8.35)
  { // 0-30km/h
    max_length_front = 100.0;
    max_length_back = 20.0;
  }
  else if (std::fabs(speed_limit) <= 16.67)
  { // 30-60km/h
    max_length_front = 150.0;
    max_length_back = 20.0;
  }
  else if (std::fabs(speed_limit) <= 22.23)
  { // 60-80km/h
    max_length_front = 350.0;
    max_length_back = 20.0;
  }
  else
  {
    max_length_front = 350.0;
    max_length_back = 20.0;
  }
  std::cout << "max_length_front：" << max_length_front << std::endl;
  for (; front_it != local_path_.end(); front_it = std::next(front_it))
  {
    if (front_it->length - near_it->length > max_length_front)
      break;
    forward_path.push_back(*front_it);
  }
  for (; back_it != local_path_.begin(); back_it = std::prev(back_it))
  {
    if (near_it->length - back_it->length > max_length_back)
      break;
    backward_path.push_back(*back_it);
  }
  CoordTransform *coordtransform = CoordTransform::Instance();
  geometry::Site rel_site;
  for (auto &p : forward_path)
  {
    if (false == coordtransform->DRGCCS2VCS(loc_data_, p, rel_site))
    {
      std::cout << "CoordTransform ERROR" << loc_data_.xg << "," << loc_data_.yg << "," << p.xg
                << "," << p.yg << std::endl;
    }
    p.x = rel_site.x;
    p.y = rel_site.y;
    p.angle = p.globalangle - loc_data_.loc_yaw_dr;
  }
  for (auto &p : backward_path)
  {
    coordtransform->DRGCCS2VCS(loc_data_, p, rel_site);
    p.x = rel_site.x;
    p.y = rel_site.y;
    p.angle = p.globalangle - loc_data_.loc_yaw_dr;

    p.reverse = !p.reverse;
  }
  if (!forward_path.empty())
  {
    std::cout << "near_it:" << near_it->length << std::endl;
    std::cout << "it:" << it->length << std::endl;
    std::cout << "start:" << forward_path.front().length << std::endl;
    std::cout << "end:" << forward_path.back().length << std::endl;
  }
  return 0;
}

inline void UnstructRefpath::PathCutting() {
  if (local_path_index_ > 5000) {
    auto cut_it = local_path_.begin();
    std::advance(cut_it, 4000);
    local_path_.erase(local_path_.begin(), cut_it);
    local_path_index_ -= 4000;
  }
}

inline void UnstructRefpath::GetFrontPursuitPath(const PathData &origin_path,
                                          const CarModel &car_model,
                                          PathData &new_path) {
  geometry::Site self_pos;
  self_pos.xg = loc_data_.xg;
  self_pos.yg = loc_data_.yg;
  self_pos.globalangle = loc_data_.yaw;
  int inter_ind = PursuitInterface::GetPursuitPath(origin_path.path, self_pos,
                                                   car_model, new_path.path);
  new_path.index = inter_ind;
}

inline void UnstructRefpath::GetFrontPursuitPathDr(const PathData &origin_path,
						  const CarModel &car_model,
 						 PathData &new_path) {
	geometry::Site self_pos;
	self_pos.xg = loc_data_.loc_xg_dr;
	self_pos.yg = loc_data_.loc_yg_dr;
	self_pos.globalangle = loc_data_.loc_yaw_dr;
	int inter_ind = PursuitInterface::GetPursuitPath(origin_path.path, self_pos,
        	   car_model, new_path.path);
	new_path.index = inter_ind;
}

inline int UnstructRefpath::CalculatePathCheck(
    const LocalizationData &loc_data,
    const std::vector<InnerPointInfo> &raw_path, const PathData &edit_path,
    std::string &err_code) {
  err_code = "";
  // if (std::fabs(loc_data.xg) < 0.1 && std::fabs(loc_data.yg) < 0.1) {
  //   err_code += std::to_string(UNST_LOCALIZATION_ERROR);
  //   return -1;
  // }
  if (edit_path.path.empty()) {
    err_code += std::to_string(UNST_EDITPATH_EMPTY_ERROR);
    return -1;
  }
  // if (raw_path.empty()) {
  //   err_code += std::to_string(UNST_RAW_ECTRACTION_RAW_EMPTY);
  //   return -1;
  // }

  //当前坐标与避障路径最近距离限制
  double loc_dis = __DBL_MAX__;
  for(auto p : edit_path.path) {
    double temp_dis = std::hypot(p.xg - loc_data_.xg, p.yg - loc_data_.yg);
    if(temp_dis < loc_dis) {
      loc_dis = temp_dis;
    }
  }
  if(loc_dis > 10) {
    std::cout<<"loc_dis too large: "<<loc_dis << std::endl;
    std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<"loc_dis too large: "<<loc_dis<<std::endl;
    return -1;
  }
  //写路起点距离限制
  auto write_path_start = edit_path.path.front();//非空判定已做
  auto dis2loc = std::hypot(write_path_start.xg - loc_data_.xg, write_path_start.yg - loc_data_.yg);
  if(dis2loc > 5.0 && !edit_path.calculation_took_a_long_time){
    std::cout <<"dis2loc too large: "<<dis2loc << std::endl;
    std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<"dis2loc too large: "<<dis2loc<<std::endl;
    return -1;
  }
  // // if (edit_path.index < raw_path.front().index || edit_path.index >
  // // raw_path.back().index) {
  // //   err_code += std::to_string(UNST_EDITPATH_INDEX_ERROR);
  // //   return -1;
  // // }
  if(false && !enable_struct_behavior_){
    for (int i = raw_path.size() - 1; i >= 0; i--) {
      if (raw_path[i].index == edit_path.index) {
        double dis = std::hypot(raw_path[i].global_point.xg - edit_path.path.back().xg,
                                raw_path[i].global_point.yg - edit_path.path.back().yg);
        if (dis < 2.0) {
          break;
        } else {
          std::cout << "dis too large: " << dis << std::endl;
          err_code += std::to_string(UNST_EDITPATH_DIS_ERROR);
          return -1;
        }
      }
    }
  }
  return 0;
}

inline void UnstructRefpath::GetRawPath(const std::vector<InnerPointInfo> &raw_path,
                                 std::list<geometry::Site> &path) {
  path.clear();
  CoordTransform *coordtransform = CoordTransform::Instance();
  geometry::Site pt;
  geometry::Site rel_site;
  for (const auto &p : raw_path) {
    pt.xg = p.global_point.xg;
    pt.yg = p.global_point.yg;
    pt.globalangle = p.global_point.global_angle;
    if (false == coordtransform->GCCS2VCS(loc_data_, pt, rel_site)) {
      std::cout << "CoordTransform ERROR" << loc_data_.xg << "," << loc_data_.yg
             << "," << p.global_point.xg << "," << p.global_point.yg << std::endl;
    }
    pt.x = rel_site.x;
    pt.y = rel_site.y;
    pt.angle = rel_site.angle;
    pt.index = p.index;
    pt.origin_index = p.index;
    pt.length = p.length;
    pt.reverse = p.direction == 0 ? false : true;
    pt.curvature = p.curvature;
    path.push_back(pt);
  }
  return;
}

}  // namespace planning
}  // namespace avos
