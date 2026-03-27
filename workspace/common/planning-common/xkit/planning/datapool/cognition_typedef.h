#ifndef DATAPOOL_INCLUDE_COGNITION_TYPEDEF_H_
#define DATAPOOL_INCLUDE_COGNITION_TYPEDEF_H_

#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/task_input.h"
#include "xkit/planning/toolbox/geometry/dwa_header.h"
#include "xkit/planning/toolbox/geometry/geoheader.h"
#include "xkit/planning/toolbox/geometry/point2D.h"
#include "common_math/vec2d.h"
#include "common_math/box2d.h"

namespace avos {
namespace planning {

//围栏结构体
struct FenceType {
    double xg;
    double yg;
    FenceType() {
        xg = 0.0;
        yg = 0.0;
    }
    FenceType(double xg_, double yg_) : xg(xg_), yg(yg_) {
    }
};
struct GpsType {
    double lon;
    double lat;
    GpsType() {
        lon = 0.0;
        lat = 0.0;
    }
    GpsType(double _lon, double _lat) : lon(_lon), lat(_lat) {
    }
};
struct SourceDataCheck {
    double loc_time;
    double loc_delta_time;
    double loc_system_time;
    double perception_time;
    double perception_delta_time;
    double perception_system_time;
    double semantic_time;
    double semantic_delta_time;
    double semantic_system_time;
    SourceDataCheck() {
        loc_time = 0.0;
        loc_delta_time = 0.0;
        loc_system_time = 0.0;
        perception_time = 0.0;
        perception_delta_time = 0.0;
        perception_system_time = 0.0;
        semantic_time = 0.0;
        semantic_delta_time = 0.0;
        semantic_system_time = 0.0;                 
    }
};

//风险状态
enum class eCarRiskStatus {
  FREE = 0,
  LIGHTLY_RISKY = 1,
  EXTREMELY_RISKY = 2
};

//发生碰撞的cell点
struct CollisionCell {
  int id;
  int type;
  double x;
  double y;
  double z;
  double height;
  double speed;
  CollisionCell() {
    Reset();
  }
  void Reset() {
    id = -1;
    type = -1;
    x = 0.0;
    y = 0.0;
    z = 0.0;
    height = 2.0;
    speed =0.0;
  }
};

//碰撞信息
struct CollisionInfo {
  //发生碰撞的障碍物cell信息
  CollisionCell cell_info;
  //发生碰撞点的路点信息
  geometry::Site path_pt_info;
  //碰撞次数
  StatisticsDetailTimes collid_times;
  float collision_max_obj_speed;
  //发生碰撞的obj信息
  Object collision_obj;
  CollisionInfo() {
    Reset();
  }
  void Reset() {
    cell_info.Reset();
    path_pt_info.Reset();
    path_pt_info.index = 9999;
    path_pt_info.length = 999.0;
    collid_times.SetStatisticsTimes(10, 0.8);
    collision_max_obj_speed = std::numeric_limits<float>::max();
    collision_obj.Reset();
  }
};

//路径的静态信息
struct UnstructStaticInfo {
  PathData front_local_path;
  PathData front_pursuit_path;
  PathData back_local_path;
  PathData front_raw_path;
  PathData result_path;
  PathData result_pursuit_path;
  UnstructStaticInfo() {
    Reset();
  }
  void Reset() {
    // front_local_path.Reset();
    // front_pursuit_path.Reset();
    // back_local_path.Reset();
    // front_raw_path.Reset();
    // result_path.Reset();
    // result_pursuit_path.Reset();
  }
};

//动态环境信息
struct UnstructDynamicInfo {
  SortedTree dynamic_obj;//objcell velocity larger than
  SortedTree static_obj;//objcell 
  SortedTree static_lidar_cell;//static cell
  SortedTree static_ultrasonic_cell;
  SortedTree predicted_obj;
  SortedTree expansion_obj;
  SortedTree edge_obj;

  std::vector<std::pair<Object, SortedTree>> cartype_obj_vec;
  std::map<int, SortedTree> weak_constrain_objmap; //语义地图解析结果
  UnstructDynamicInfo() {
    Reset();
  }
  void Reset() {
    dynamic_obj.clear();
    static_obj.clear();
    static_lidar_cell.clear();
    static_ultrasonic_cell.clear();
    expansion_obj.clear();
    cartype_obj_vec.clear();
    weak_constrain_objmap.clear();
    edge_obj.clear();
  }
};

//非结构化语义信息
struct UnstructSemanticInfo {
  eCarRiskStatus curent_car_risk;
  double slow_down_dis;
  CollisionInfo result_dynamic_obj_collision_info;
  CollisionInfo result_static_obj_collision_info;
  CollisionInfo result_static_lidar_cell_collision_info;
  CollisionInfo result_follow_target_collision_info;
  CollisionInfo result_static_ultra_cell_collision_info;
  CollisionInfo result_info;
  CollisionInfo front_dynamic_obj_collision_info;
  CollisionInfo front_static_obj_collision_info;
  CollisionInfo front_static_lidar_cell_collision_info;
  CollisionInfo front_static_ultra_cell_collision_info;
  CollisionInfo front_info;
  CollisionInfo input_path_dynamic_obj_collision_info;
  CollisionInfo input_path_static_obj_collision_info;
  CollisionInfo input_path_static_lidar_cell_collision_info;
  CollisionInfo input_path_static_ultra_cell_collision_info;
  CollisionInfo input_path_info;
  StatisticsDetailTimes path_resultcollision_times;
  StatisticsDetailTimes path_frontcollision_times;
  StatisticsDetailTimes path_negitivecollision_times;
  StatisticsDetailTimes path_emergency_times;
  StatisticsDetailTimes input_path_uncollision_times;
  StatisticsDetailTimes moved_path_collision_times;
  StatisticsTimes time_for_abnormal_endding;
  UnstructSemanticInfo() {
    Reset();  
  }
  void Reset() {
    curent_car_risk = eCarRiskStatus::FREE;
    slow_down_dis = 9999.9;
    result_dynamic_obj_collision_info.Reset();
    result_static_obj_collision_info.Reset();
    result_static_lidar_cell_collision_info.Reset();
    result_follow_target_collision_info.Reset();
    result_static_ultra_cell_collision_info.Reset();
    result_info.Reset();
    front_dynamic_obj_collision_info.Reset();
    front_static_obj_collision_info.Reset();
    front_static_lidar_cell_collision_info.Reset();
    front_static_ultra_cell_collision_info.Reset();
    front_info.Reset();
    path_resultcollision_times.SetStatisticsTimes(10, 0.8, 0.3);
    path_negitivecollision_times.SetStatisticsTimes(10, 0.4, 0.8);
    path_frontcollision_times.SetStatisticsTimes(10, 0.8, 0.8);
    path_emergency_times.SetStatisticsTimes(10, 0.8);
    input_path_uncollision_times.SetStatisticsTimes(20, 0.8);
    moved_path_collision_times.SetStatisticsTimes(30, 0.8);
    time_for_abnormal_endding.SetStatisticsTimes(200000);
  }
  void UpdateNewResultCollision() {
    path_resultcollision_times.NewPositiveData();
  }
  void UpdateNonResultCollision() {
    path_resultcollision_times.NewNegativeData();
  }
  bool IsPathCollisionTimeUp() {
    return path_resultcollision_times.IsTimesUp();
  }
  void ResetResultCollision() {
    path_resultcollision_times.Reset();
  }
  void UpdateNewFrontCollision() {
    path_frontcollision_times.NewPositiveData();
  }
  void UpdateNonFrontCollision() {
    path_frontcollision_times.NewNegativeData();
  } 
  bool IsPathNoCollisionTimeUp() {
    return path_frontcollision_times.IsNoTimesUp();
  }
  void ResetFrontCollision() {
    path_frontcollision_times.Reset();
  }
  void UpdateNonInoutPathCollision() {
    input_path_uncollision_times.NewPositiveData();
  }
  void UpdateNewInputPathCollision() {
    input_path_uncollision_times.NewNegativeData();
  }
  bool IsInputPathUnCollisionTimeUp() {
    return input_path_uncollision_times.IsTimesUp();
  }
  void ResetInputPathCollision() {
    input_path_uncollision_times.Reset();
  }
  void UpdateNewMovedPathCollision() {
    moved_path_collision_times.NewPositiveData();
  }
  void UpdateNonMovedPathCollision() {
    moved_path_collision_times.NewNegativeData();
  }
  bool IsMovedPathCollisionTimeUp() {
    return moved_path_collision_times.IsTimesUp();
  }
  void ResetMovedPathCollision() {
    moved_path_collision_times.Reset();
  }
  void UpdateNewNegtiveCollision() {
    path_negitivecollision_times.NewPositiveData();
  }
  void UpdateNonNegtiveCollision() {
    path_negitivecollision_times.NewNegativeData();
  }
  bool IsNegtiveCollisionTimeUp() {
    return path_negitivecollision_times.IsTimesUp();
  }
  void ResetNegtiveCollision() {
    path_negitivecollision_times.Reset();
  }  
  void UpdateNewEmergency() {
    path_emergency_times.NewPositiveData();
  }
  void UpdateNonEmergency() {
    path_emergency_times.NewNegativeData();
  }
  bool IsEmergencyTimeUp() {
    return path_emergency_times.IsTimesUp();
  }
  void ResetEmergency() {
    path_emergency_times.Reset();
  }
  void ResetAllTimes(){
   ResetResultCollision();
   ResetFrontCollision();
   ResetEmergency();
  }
};

//非结构化环境信息
struct UnstructEnv {
  //路径相关信息
  UnstructStaticInfo static_info;
  //动态环境相关信息
  UnstructDynamicInfo dynamic_info;
  //语义信息
  UnstructSemanticInfo semantic_info;
  //护栏信息
  std::vector<geometry::Point2D> fence_info;

  std::vector<SemanticCell> passable_cells;

  bool obs_near_path;
  
  int _stay_count;//the stopping time(count)
  float _frequency;
  double fence_angle;
  int fence_disappear_frame;
  UnstructEnv() {
    Reset();
  }
  void Reset() {
    static_info.Reset();
    dynamic_info.Reset();
    semantic_info.Reset();
    fence_info.clear();
    _stay_count = 0;
    _frequency = 10.0;
    fence_angle = 0.0;
    fence_disappear_frame = 0.0;
  }
  void SetNodeFrequency(const float& _f){
    if(_f > 0.0){
      _frequency = _f;
    }
  }
  void IncreaseStayedTime(const int& _t = 1){
    _stay_count += _t;
    if(_stay_count > 86400*_frequency){//24h
      _stay_count = 86400*_frequency;
    }
  }
  void ResetStayedTime(const int& _t = 0){
    _stay_count = (_t < 0) ? 0 : _t;
    if(_stay_count > 86400*_frequency){//24h
      _stay_count = 86400*_frequency;
    }
  }
  float HasStayedSecond(){//s
    return (_stay_count / _frequency);
  }
};

//跟随认知信息
typedef struct FollowCognitionInfo {
  UnstructEnv unstruct_env_info;
} FollowCognitionInfo;

//自主通行认知信息
typedef struct ArriveCognitionInfo {
  UnstructEnv unstruct_env_info;
} ArriveCognitionInfo;

//寻迹认知信息
typedef struct TrackCognitionInfo {
  UnstructEnv unstruct_env_info;
} TrackCognitionInfo;

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_COGNITION_TYPEDEF_H_
