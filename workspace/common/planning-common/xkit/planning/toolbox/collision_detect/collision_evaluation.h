#ifndef COMMON_TOOBOX_COLLISION_EVALUATION_INCLUDE_COLLISION_DETECT_H_
#define COMMON_TOOBOX_COLLISION_EVALUATION_INCLUDE_COLLISION_DETECT_H_
#include <vector>
#include "xkit/planning/toolbox/collision_detect/collision_detect.h"
#include "xkit/planning/datapool/data_pool.h"
#include "log/log.h"
#include "ara/adsfi/impl_type_msgpoint3dvec.h"

using namespace avos::planning;
// using namespace avos::dwa;

class ObjectCollisionEvaluation
{
public:
  ObjectCollisionEvaluation() {};
  ~ObjectCollisionEvaluation() {};

  /*
  check_result<记录碰撞obj等级，无碰撞-1)， 碰撞位置>
  */
  template <class T>
  static int WeakConstrainObjectCollisionEvaluation(const T &point,
                                                    const avos::planning::CarModel &car_model,
                                                    std::vector<std::pair<int, SortedTree>>& obs_map,
                                                    std::vector<std::pair<int, ara::adsfi::MsgPoint3D>>& check_result) 
  {
    int rtvalue = -1;
    if(obs_map.empty()) {
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" obs_map is empty!"<<std::endl;
      return rtvalue;
    }
    // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" get weak_constrain_objmap with size: "<<obs_map.size()<<std::endl;
    check_result.clear();

    for(std::vector<std::pair<int, SortedTree>>::iterator iter = obs_map.begin();
        iter != obs_map.end(); ++iter){
      ara::adsfi::MsgPoint3D collision_point;
      // auto start_time = ros::Time::now().toSec();
      if(ObjectCollisionDetect::CollisionDetection<T>(point, iter->second, car_model, -1, collision_point, 0)){
        check_result.emplace_back(std::make_pair(iter->first, collision_point));//order - point
      }else{
        check_result.emplace_back(std::make_pair(-1, collision_point));
      }
    }
    if(check_result.size() == obs_map.size()){
      rtvalue = 0;
    }else{
      std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" ERROR LOGIC"<<std::endl;
    }
    return rtvalue;
  }

  /*
  check_result<（对应obj的等级，不应有-1）， (碰撞总次数，无碰撞0)>
  delta_dis：检测间隔长度，若负值则逐点检测
  valid_dis：检测路径长度限制
  */
  template <class T, class ContainerT>
  static int RoadWeakConstrainObjectCollisionEvaluation(const ContainerT &point_vec,
                                                        const avos::planning::CarModel &car_model,
                                                        const bool& is_in_thread,
                                                        std::unordered_map<int, int>& check_result,
                                                        double delta_dis = -1.0, 
                                                        double valid_dis = numeric_limits<double>::max()) 
  {
    int rtvalue = -1; 
    if (point_vec.size() < 2) {
      std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" Invalid point_vec size: "<<point_vec.size()<<std::endl;
      return rtvalue;
    }

    std::map<int, SortedTree> origin_obs_map;
    auto DP = avos::planning::DataPool::Instance();
    if(is_in_thread){
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" is_in_thread check."<<std::endl;
      if(eMissionType::ARRIVE == DP->GetSwapComputeDateRef().mission_type) {
        origin_obs_map = DP->GetSwapComputeDateRef().arrive_cognition_info.unstruct_env_info.dynamic_info.weak_constrain_objmap;
      }
      else if(eMissionType::FOLLOW == DP->GetSwapComputeDateRef().mission_type) {
        origin_obs_map = DP->GetSwapComputeDateRef().follow_cognition_info.unstruct_env_info.dynamic_info.weak_constrain_objmap;
      }
      else if(eMissionType::TRACK == DP->GetSwapComputeDateRef().mission_type) {
        origin_obs_map = DP->GetSwapComputeDateRef().track_cognition_info.unstruct_env_info.dynamic_info.weak_constrain_objmap;
      }
      else {
        AERROR << "EMPTY OBS MAP";
      }
    }else{
      if(eMissionType::ARRIVE == DP->GetMainDataPtr()->task_content.mission_type) {
        origin_obs_map = DP->GetMainDataPtr()->arrive_cognition_info.unstruct_env_info.dynamic_info.weak_constrain_objmap;
      }
      else if(eMissionType::FOLLOW == DP->GetMainDataPtr()->task_content.mission_type) {
        origin_obs_map = DP->GetMainDataPtr()->follow_cognition_info.unstruct_env_info.dynamic_info.weak_constrain_objmap;
      }
      else if(eMissionType::TRACK == DP->GetMainDataPtr()->task_content.mission_type) {
        origin_obs_map = DP->GetMainDataPtr()->track_cognition_info.unstruct_env_info.dynamic_info.weak_constrain_objmap;
      }
      else {
        AERROR << "EMPTY OBS MAP";
      }
    }

    if(origin_obs_map.empty()){
      return rtvalue;
    }
    
    std::vector<std::pair<int, SortedTree>> obs_map;
    for(const auto& o : origin_obs_map){
      obs_map.push_back(o);
    }

    check_result.clear();
    double dis = 0.0, temp_delta_dis = 0.0;
    auto it = point_vec.begin();
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      double temp_dis = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp_dis;
      temp_delta_dis += temp_dis;
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" dis: "<<dis<<", temp_delta_dis: "<<temp_delta_dis<<std::endl;
      if (dis > valid_dis) {break;}
      if (temp_delta_dis < delta_dis) {continue;}
      
      temp_delta_dis = 0;
      std::vector<std::pair<int, ara::adsfi::MsgPoint3D>> single_check_result_vec;
      int rt = ObjectCollisionEvaluation::WeakConstrainObjectCollisionEvaluation<Site>(
                                           *it, car_model, obs_map, single_check_result_vec);

      if (-1 != rt) {
        for(size_t i = 0; i < single_check_result_vec.size(); ++i){
          auto single_check_result = single_check_result_vec.at(i);
          if(-1 != single_check_result.first){
            rtvalue = 0;
            check_result[single_check_result.first] += 1;
          }
        }
      }else{
        // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" WeakConstrainObjectCollisionEvaluation fail"<<std::endl;
        return rtvalue;
      }
    }

    return 0;
  }


};

#endif  // COMMON_TOOBOX_COLLISION_DETECT_INCLUDE_COLLISION_DETECT_H_