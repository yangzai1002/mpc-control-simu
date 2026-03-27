#ifndef DATAPOOL_INCLUDE_COMMON_CONFIG_H_
#define DATAPOOL_INCLUDE_COMMON_CONFIG_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
// #include "param_reader.h"
// #include "base_log/log.h"
#include "xkit/planning/toolbox/geometry/circular_point.h"




namespace avos {
namespace planning {

enum class eFaultHandleStatus {
  NORMAL = 0, 
  SLOW_DOWN,     
  SLOW_STOP,       
  EMERGENCY_STOP,  
};

//车型参数
struct CarModel {
  double length;
  double car_width;
  double length_wheelbase;
  double front_axle_tofront;
  double wheel_center;
  double min_turning_radius;
  double max_speed; //暂时默认120
  double max_accelaration;
  double max_braking_value;
  double min_drive_speed; //暂时不加
  double slide_accelaration;//测出来的
  double eps_transmission_ratio;
  double right_max_steerangle;
  double front_over_hang;
  double back_over_hang;
  double half_wheel;
  double target_min_turn_radius;
  int trafficability;//cm
  int negative_trafficability;//cm（负值）
  double turning_around_min_radius;
  std::vector<geometry::CircularPoint> model_circles;
  CarModel() {
    Reset();
  }
  void Reset() {
    length = 0.0;
    car_width = 0.0;
    length_wheelbase = 0.0;
    front_axle_tofront = 0.0;
    wheel_center = 0.0;
    min_turning_radius = 0.0;
    max_speed = 0.0;
    max_accelaration = 0.0;
    max_braking_value = 0.0;
    min_drive_speed = 0.0;
    slide_accelaration = 0.0;
    eps_transmission_ratio = 0.0;
    right_max_steerangle = 0.0;
    front_over_hang = 1.2;
    back_over_hang = 0.3;
    half_wheel = 0.5;
    trafficability = 20;
    negative_trafficability = -20;
    target_min_turn_radius = 88.0;
    turning_around_min_radius = 5.0;
    model_circles.clear();
  }
  void Set(double frontoverhang, double backoverhang, double halfwheel) {
    front_over_hang = frontoverhang;
    back_over_hang = backoverhang;
    half_wheel = halfwheel;
  }
  // bool LoadVeicleParamsFromModel(ModelParam *pmodel_config) {
  //   if (pmodel_config->GetValue("car_length", &length) == false) {
  //     std::cout << "cannot get car_length" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("car_width", &car_width) == false) {
  //     std::cout << "cannot get car_width" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("wheel_base_length", &length_wheelbase) == false) {
  //     std::cout << "cannot get length_wheel_base" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("front_axle_tofront", &front_axle_tofront) == false) {
  //     std::cout << "cannot get front_axle_tofront" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("wheel_center", &wheel_center) == false)  {std:
  //     std::cout << "cannot get wheel_center" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("min_turn_radius", &min_turning_radius) == false) {
  //     std::cout << "cannot get min_turn_radius" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("acel_max", &max_accelaration) == false) {
  //     std::cout << "cannot get acel_max" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("emergency_brake_accel", &max_braking_value) == false) {
  //     std::cout << "cannot get emergency_brake_accel" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("right_reduce_ratio", &eps_transmission_ratio) == false) {
  //     std::cout << "cannot get right_reduce_ratio" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("right_max_steerangle", &right_max_steerangle) == false) {
  //     std::cout << "cannot get right_max_steerangle" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("trafficability", &trafficability) == false) {//cm
  //     std::cout << "cannot get trafficability" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("negative_trafficability", &negative_trafficability) == false) {//cm
  //     std::cout << "cannot get negative_trafficability" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("target_min_turn_radius", &target_min_turn_radius) == false) {
  //     std::cout << "cannot get target_min_turn_radius" << std::endl;
  //     return false;
  //   }
  //   right_max_steerangle = 500;
  //   max_speed = 120.0 / 3.6;

  //   front_over_hang = length_wheelbase + front_axle_tofront;
  //   back_over_hang = length - front_over_hang;
  //   half_wheel = car_width / 2.0;

  //   double voronoi_model_corner_x, voronoi_model_corner_y, voronoi_model_delta_x, voronoi_model_delta_y, voronoi_model_circle_radius;
  //   int voronoi_model_index_x, voronoi_model_index_y;
  //   if (pmodel_config->GetValue("voronoi_model_corner_x", &voronoi_model_corner_x) == false) {
  //     std::cout << "cannot get voronoi_model_corner_x" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("voronoi_model_corner_y", &voronoi_model_corner_y) == false) {
  //     std::cout << "cannot get voronoi_model_corner_y" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("voronoi_model_delta_x", &voronoi_model_delta_x) == false) {
  //     std::cout << "cannot get voronoi_model_delta_x" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("voronoi_model_delta_y", &voronoi_model_delta_y) == false) {
  //     std::cout << "cannot get voronoi_model_delta_y" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("voronoi_model_circle_radius", &voronoi_model_circle_radius) == false) {
  //     std::cout << "cannot get voronoi_model_circle_radius" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("voronoi_model_index_x", &voronoi_model_index_x) == false) {
  //     std::cout << "cannot get voronoi_model_index_x" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("voronoi_model_index_y", &voronoi_model_index_y) == false) {
  //     std::cout << "cannot get voronoi_model_index_y" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("turning_around_min_radius", &turning_around_min_radius) == false) {
  //     std::cout << "cannot get turning_around_min_radius" << std::endl;
  //     return false;
  //   }
  //   CalculateModelCircles(voronoi_model_corner_x, voronoi_model_corner_y, voronoi_model_delta_x, 
  //     voronoi_model_delta_y, voronoi_model_circle_radius, voronoi_model_index_x, voronoi_model_index_y);
  //   return true;
  // }
  void CalculateModelCircles(const double& corner_x, const double& corner_y,const double& delta_x, 
    const double& delta_y, const double& radius, const int& num_x, const int& num_y) {
    model_circles.clear();
    geometry::CircularPoint car_circle;
    for (int index_x = 0; index_x < num_x; ++index_x) {
      for (int index_y = 0; index_y < num_y; ++index_y) {
        car_circle.Reset();
        car_circle.position.x = corner_x + index_x * delta_x;
        car_circle.position.y = corner_y + index_y * delta_y;
        car_circle.radius = radius;
        model_circles.push_back(car_circle);
      }
    }
    // AINFO<<"car model:"<<front_over_hang<<","<<back_over_hang<<","<<half_wheel;
    // for(int i=0; i<model_circles.size();i++){
    //   AINFO<<"model_circles:"<<model_circles.at(i).position.x<<","<<model_circles.at(i).position.y<<","<<model_circles.at(i).radius;
    // }


  }
};

//认知层配置参数
struct CognitionConfig {
  bool enable_acurracy_control;
  double set_scale_x;
  double set_scale_xb;
  double set_scale_y;
  double speed_threshold;
  double high_speed_threshold;
  double result_path_dis;
  double front_path_dis;
  int semantic_obj_type_1_priority, semantic_obj_type_2_priority, 
      semantic_obj_type_3_priority, semantic_obj_type_4_priority, 
      semantic_obj_type_5_priority, semantic_obj_type_6_priority, 
      semantic_obj_type_7_priority, semantic_obj_type_8_priority;
  int semantic_obj_positive_height_priority_1, semantic_obj_positive_height_priority_2, 
      semantic_obj_positive_height_priority_3,
      semantic_obj_negative_height_priority_1, semantic_obj_negative_height_priority_2, 
      semantic_obj_negative_height_priority_3;
  int semantic_obj_type_1_height;//不可通行属性的基础上的高度阈值，属性不可通行并且大于该高度才认为不可通行
  bool enable_struct_behavior;
  double track_diff_test; //位置误差测试
  double track_diff_angle_test; //角度误差
  bool enable_voronoi_detect_flag;
  bool enable_time_matching_flag;
  double emgency_brake_back_area, emgency_brake_front_area, emgency_brake_left_area, emgency_brake_right_area, emgency_safe_distance;
  bool perception_enable;
  bool shift_base_on_calibrated_path;//平移基于的参考路径（true：使用calibrated_path, false: front_local_raw）
  double car_width_expan;
  bool high_config;
  bool weak_localization;
  double refpath_offset;
  int fence_disappear_frame;
  bool config_pedestrian_avoid;
  double shift_dis;
  int perception_status; // 0--Perception & SemanticMap on; 1--Perception off; 2--SemanticMap off; 3--Perception & SemanticMap off;
  double emergency_stop_safe_dis;
  double lon_1;
  double lon_2;
  double lat_1;
  double lat_2;
  double P0_speed;
  double P1_speed;
  double P2_speed;
  double safe_radius;

  CognitionConfig() {
    enable_acurracy_control = false;
    set_scale_x = 100.0;
    set_scale_xb = -45.0;
    set_scale_y = 45.0;
    speed_threshold = 0.5; // m/s
    high_speed_threshold = 10.0; // m/s
    result_path_dis = 70.0;
    front_path_dis = 70.0;
    enable_struct_behavior = false;
    track_diff_test = 0.0;
    track_diff_angle_test = 0.0;
    enable_voronoi_detect_flag = false;
    enable_time_matching_flag = true;
    emgency_brake_back_area = -10.0;
    emgency_brake_front_area = 20.0;
    emgency_brake_left_area = 5.0;
    emgency_brake_right_area = -5.0;
    emgency_safe_distance = 5.0;
    perception_enable = false;
    shift_base_on_calibrated_path = false;
    car_width_expan = 0.0;
    high_config = false;
    weak_localization = false;
    fence_disappear_frame = 0;
    refpath_offset = 0.0;
    config_pedestrian_avoid = false;
    shift_dis = 2.0;
    perception_status = 0;
    emergency_stop_safe_dis = 3.0;
    lon_1 = 10.0;
    lon_2 = 30.0;
    lat_1 = 2.0;
    lat_2 = 3.0;
    P0_speed = 1.0;
    P1_speed = 2.0;
    P2_speed = 3.0;
    safe_radius = 5.0;
  }
  bool IsInArea(double x, double y) {
    if (x > set_scale_xb && x < 100.0 && fabs(y) < set_scale_y)
      return true;
    return false;
  }
  bool IsCarInArea(double x, double y) {
    if (x > set_scale_xb && x < 100.0 && fabs(y) < set_scale_y)
      return true;
    return false;
  }
  // bool LoadCognitionConfigParamsFromModel(ModelParam *pmodel_config) {
  //   //不同类型障碍物对应的约束等级
  //   if (pmodel_config->GetValue("semantic_obj_type_1_height", &semantic_obj_type_1_height) == false) {
  //     std::cout << "cannot get semantic_obj_type_1_height" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("fence_disappear_frame", &fence_disappear_frame) == false) {
  //     std::cout << "cannot get fence_disappear_frame" << std::endl;
  //     return false;
  //   }    
  //   if (pmodel_config->GetValue("semantic_obj_type_1_priority", &semantic_obj_type_1_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_1_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_2_priority", &semantic_obj_type_2_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_2_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_3_priority", &semantic_obj_type_3_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_3_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_4_priority", &semantic_obj_type_4_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_4_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_5_priority", &semantic_obj_type_5_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_5_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_6_priority", &semantic_obj_type_6_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_6_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_7_priority", &semantic_obj_type_7_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_7_priority" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_type_8_priority", &semantic_obj_type_8_priority) == false) {
  //     std::cout << "cannot get semantic_obj_type_8_priority" << std::endl;
  //     return false;
  //   }

  //   //trafficability基础上，不同约束对应的相对高度（正值）
  //   if (pmodel_config->GetValue("semantic_obj_positive_height_priority_1", &semantic_obj_positive_height_priority_1) == false) {
  //     std::cout << "cannot get semantic_obj_positive_height_priority_1" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_positive_height_priority_2", &semantic_obj_positive_height_priority_2) == false) {
  //     std::cout << "cannot get semantic_obj_positive_height_priority_2" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_positive_height_priority_3", &semantic_obj_positive_height_priority_3) == false) {
  //     std::cout << "cannot get semantic_obj_positive_height_priority_3" << std::endl;
  //     return false;
  //   }
  //   // if (pmodel_config->GetValue("semantic_obj_positive_height_priority_4", &semantic_obj_positive_height_priority_4) == false) {
  //   //   std::cout << "cannot get semantic_obj_positive_height_priority_4" << std::endl;
  //   //   return false;
  //   // }

  //   //negative_trafficability基础上，不同约束对应的相对高度（负值）
  //   if (pmodel_config->GetValue("semantic_obj_negative_height_priority_1", &semantic_obj_negative_height_priority_1) == false) {
  //     std::cout << "cannot get semantic_obj_negative_height_priority_1" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_negative_height_priority_2", &semantic_obj_negative_height_priority_2) == false) {
  //     std::cout << "cannot get semantic_obj_negative_height_priority_2" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("semantic_obj_negative_height_priority_3", &semantic_obj_negative_height_priority_3) == false) {
  //     std::cout << "cannot get semantic_obj_negative_height_priority_3" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("enable_struct_behavior", &enable_struct_behavior) == false) {
  //     std::cout << "cannot get enable_struct_behavior" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("enable_voronoi_detect_flag", &enable_voronoi_detect_flag) == false) {
  //     std::cout << "cannot get enable_voronoi_detect_flag" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("enable_time_matching_flag", &enable_time_matching_flag) == false) {
  //     std::cout << "cannot get enable_time_matching_flag" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("track_diff_test", &track_diff_test) == false) {
  //       std::cout << "cannot get track_diff_test" << std::endl;
  //       return false;
  //   }
  //   if (pmodel_config->GetValue("track_diff_angle_test", &track_diff_angle_test) == false) {
  //       std::cout << "cannot get track_diff_angle_test" << std::endl;
  //       return false;
  //   }


  //   //安全急停cell点的范围
  //   if (pmodel_config->GetValue("emgency_brake_back_area", &emgency_brake_back_area) == false) {
  //     std::cout << "cannot get emgency_brake_back_area" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("emgency_brake_front_area", &emgency_brake_front_area) == false) {
  //     std::cout << "cannot get emgency_brake_front_area" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("emgency_brake_left_area", &emgency_brake_left_area) == false) {
  //     std::cout << "cannot get emgency_brake_left_area" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("emgency_brake_right_area", &emgency_brake_right_area) == false) {
  //     std::cout << "cannot get emgency_brake_right_area" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("emgency_safe_distance", &emgency_safe_distance) == false) {
  //     std::cout << "cannot get emgency_safe_distance" << std::endl;
  //     return false;
  //   }

  //   if (pmodel_config->GetValue("perception_enable", &perception_enable) == false) {
  //     std::cout << "cannot get perception_enable" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("shift_base_on_calibrated_path", &shift_base_on_calibrated_path) == false) {
  //     std::cout << "cannot get shift_base_on_calibrated_path" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("car_width_expan", &car_width_expan) == false) {
  //     std::cout << "cannot get car_width_expan" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("high_config", &high_config) == false) {
  //     std::cout << "cannot get hight_config" << std::endl;
  //     return false;
  //   }
  //       if (pmodel_config->GetValue("weak_localization", &weak_localization) == false) {
  //     std::cout << "cannot get weak_localization" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("refpath_offset", &refpath_offset) == false) {
  //     std::cout << "cannot get refpath_offset" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("config_pedestrian_avoid", &config_pedestrian_avoid) == false) {
  //     std::cout << "cannot get config_pedestrian_avoid" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("shift_dis", &shift_dis) == false) {
  //     std::cout << "cannot get shift_dis" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("perception_status", &perception_status) == false) {
  //     std::cout << "cannot get perception_status" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("emergency_stop_safe_dis", &emergency_stop_safe_dis) == false) {
  //     std::cout << "cannot get emergency_stop_safe_dis" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("lon_1", &lon_1) == false) {
  //     std::cout << "cannot get lon_1" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("lon_2", &lon_2) == false) {
  //     std::cout << "cannot get lon_2" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("lat_1", &lat_1) == false) {
  //     std::cout << "cannot get lat_1" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("lat_2", &lat_2) == false) {
  //     std::cout << "cannot get lon_2" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("P0_speed", &P0_speed) == false) {
  //     std::cout << "cannot get P0_speed" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("P1_speed", &P1_speed) == false) {
  //     std::cout << "cannot get P1_speed" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("P2_speed", &P2_speed) == false) {
  //     std::cout << "cannot get P2_speed" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("safe_radius", &safe_radius) == false) {
  //     std::cout << "cannot get safe_radius" << std::endl;
  //     return false;
  //   }
  //   // if (pmodel_config->GetValue("semantic_obj_negative_height_priority_4", &semantic_obj_negative_height_priority_4) == false) {
  //   //   std::cout << "cannot get semantic_obj_negative_height_priority_4" << std::endl;
  //   //   return false;
  //   // }
  //   return true;
  // }
};

//行为约束
struct BehaviorConfig {
  int avoid;
  bool enable_unstruct_avoid;
  bool enable_unstruct_back_follow;
  int unstruct_avoid_level;

  int forward_stay_still_times_setting;
  int back_stay_still_times_setting;
  int back_stay_still_failed_times_setting;
  int back_following_times_setting;
  int time_for_dwa_setting;
  int time_for_ha_setting;
  double curvature_threshold;
  double slow_dis_threshold;
  double deviation_dis_err;
  double deviation_angle_err;
  double detect_height_threshold;
  double back_follow_dis_threshold;
  //follow params
  double follow_dp_min_turning_radius;
  double follow_dp_min_lon_bias;
  double follow_dp_max_lon_bias;
  double follow_dp_lon_sample_velocity_times;//纵向采样距离相对速度的倍数
  double follow_dp_max_lat_bias;//横向采样最大限制
  double follow_dp_min_lat_bias;//横向采样最低限制
  //通过二次函数形式设置速度与横向采样间隔的关系，同时受上述边界值限制
  //lat_bias = a*(v)^2 + b
  double follow_dp_lat_sample_2nd_order_param;// -1/12 二阶系数a
  double follow_dp_lat_sample_1st_order_param;// 61/12 一阶系数b
  int follow_collision_check_slice_num;//开始忽略碰撞检测的层数
  int follow_forwardfollowing_calling_interval;//forwardfollowing状态下调用DP的间隔
  int follow_slowdown_calling_interval;//slowdown状态下调用DP的间隔
  int follow_staystill_calling_interval;//staystill状态下调用HA*的间隔
  bool follow_enable_refpath_connection;//是否允许使用参考路径作为采样（尽量还原参考路径）
  int follow_lat_sample_half_goal_num;//DP横向采样时所包含的采样点个数（仅_lat_goal_type == eGenerateLatGoalType::DYNAMIC_NUM生效）
  //follow frenet
  double follow_frenet_max_curvature;
  double follow_frenet_max_road_width;
  double follow_frenet_min_road_width;
  double follow_frenet_road_width_step;
  double follow_frenet_sample_time;
  double follow_frenet_sample_time_stepwidth;
  double follow_frenet_sample_dis_stepwidth;
  int follow_frenet_refpath_fit_order;
  double follow_frenet_jerk_weight;
  double follow_frenet_time_weight;
  double follow_frenet_distance_weight;
  double follow_frenet_lat_weight;
  double follow_frenet_lon_weight;
  bool follow_frenet_using_connect_line;
  //track params
  double ignore_collision_distance;//障碍物干涉路径的距离大于此值时不认为发生碰撞
  double track_dp_min_turning_radius;
  double track_dp_min_lon_bias;
  double track_dp_max_lon_bias;
  double track_dp_lon_sample_velocity_times;//纵向采样距离相对速度的倍数
  double track_dp_max_lat_bias;//横向采样最大限制
  double track_dp_min_lat_bias;//横向采样最低限制
  double track_dp_expect_avoid_dis;//期望避障距离
  double track_negtive_obs_expan;//循迹避障负障碍物膨胀系数
  double track_state_switch_collision_dis;//循迹状态切换碰撞距离

  bool track_casadi_optimize_enable;//启用casadi局部路径优化
  

  double arrive_collision_weight_param;//arrive 碰撞与路径长度费用权重
  double arrive_refpath_match_weight_param;//arrive 参考路径匹配费用权重
  double arrive_lastpath_match_weight_param;//arrive 前帧路径匹配费用权重
  double arrive_voronoi_dis_weight_param;//arrive voronoi 图权重
  double arrive_window_cost_threshold;//arrive窗口跳变阈值
  double arrive_semantic_edge_radius;//arrive边缘检测半径
  double arrive_min_feasible_dis;//arrive最小可通行距离。
  double arrive_guide_min_dis;//引导点距离车辆的最短距离
  double arrive_min_sample_radius;//arrive最小转弯。
  double arrive_max_sample_len;//arrive最大采样长度
  double arrive_max_circle_count;//采样路径最大长度占采样圆的圈数
  double arrive_ignore_edge_len;//kiwi车没有补盲雷达，需要车身两侧忽略盲区
  int arrive_semanticmap_resolution;//arrive语义地图分辨率(栅格边长/cm)

  //通过二次函数形式设置速度与横向采样间隔的关系，同时受上述边界值限制
  //lat_bias = a*(v)^2 + b
  double track_dp_lat_sample_2nd_order_param;// -1/12 二阶系数a
  double track_dp_lat_sample_1st_order_param;// 61/12 一阶系数b
  int track_collision_check_slice_num;//开始忽略碰撞检测的层数
  int track_forwardfollowing_calling_interval;//forwardfollowing状态下调用DP的间隔
  int track_slowdown_calling_interval;//slowdown状态下调用DP的间隔
  int track_staystill_calling_interval;//staystill状态下调用HA*的间隔
  bool track_enable_refpath_connection;//是否允许使用参考路径作为采样（尽量还原参考路径）
  int track_lat_sample_half_goal_num;//DP横向采样时所包含的采样点个数（仅_lat_goal_type == eGenerateLatGoalType::DYNAMIC_NUM生效）
  bool track_enable_following_periodically_dp;//是否允许forwarfollowing状态下的DP
  bool track_enable_round_trip;//是否启用生成往返循迹路径的逻辑
  bool track_enable_curvature_variable;//使能dp避障最大曲率根据速度变化
  std::string track_enable_sub_high_precision;//使能亚高精循迹
  bool track_enable_reverse_avoid;//使能倒车避障
  bool track_enable_pivot_steering;//使能原地转向
  double track_avoid_dis;//避障距离
double track_collision_weight_param;
double track_refpath_match_weight_param;
double track_lastpath_match_weight_param;
double track_voronoi_dis_weight_param;
double track_window_cost_threshold;
double track_semantic_edge_radius;
double track_min_feasible_dis;
double track_guide_min_dis;
double track_min_sample_radius;
double track_max_sample_len;
double track_max_circle_count;
int track_semanticmap_resolution;
double track_tentacle_count;
double track_path_length_to_end_task;
double track_dp_car_expan_speed_co;


  //add fence
  bool enable_fence;
  double height_bline; //盲区
  bool follow_enable_dr;
  bool follow_use_init_heading;
  //用于混合A*调姿态
  double hybridastar_turning_radius;
  int hybridastar_grid_front_width;
  int hybridastar_grid_back_width;
  int hybridastar_grid_half_height;
  int hybridastar_grid_headings;
  double hybridastar_cell_size;
  double hybridastar_min_road_width;
  double hybridastar_pre_ref_length;
  int rectified_path_point_num;
  bool hybridastar_enable_bezier_shot;
  double pose_adjust_lat_limit;
  double pose_adjust_lon_limit;
  double pose_position_error_threshold;
  double pose_roll_error_threshold;
  double pose_pitch_error_threshold;
  double pose_yaw_error_threshold;
  
  //用于路径处理类
  double path_dheading_threshold; //路径航向跳变检查阈值
  double dist_to_sharp_threshold; //路径折点前后的距离
  double bezier_safe_dist;

  //用于脱困算法
  double steering_radian_;
  int steering_discrete_num_;
  int segment_length_discrete_num_min;
  int segment_length_discrete_num_max;
  double dist_to_carbox_threshold_for_introuble;
  double dist_to_carbox_threshold_for_isfree;
  double time_threshold_for_in_trouble;
  double reserved_path_length_for_splicing;

  bool remote_enable_dr;
  double remote_desired_speed;

  //调头
  bool track_enable_turning_max;

  bool QP_enable;
  double QP_car2obs_dist;

  BehaviorConfig() {
    avoid = 1;
    enable_unstruct_avoid = true;
    enable_unstruct_back_follow = true;
    unstruct_avoid_level = 1;

    forward_stay_still_times_setting = 30;
    back_stay_still_times_setting = 1200000;
    back_stay_still_failed_times_setting = 1200;
    back_following_times_setting = 150;
    time_for_dwa_setting = 5;
    time_for_ha_setting = 50;
    curvature_threshold = 1.0 / 1.2;
    slow_dis_threshold = 1.7;
    deviation_dis_err = 0.4;
    deviation_angle_err = 90.0;
    detect_height_threshold = 0.2;
    back_follow_dis_threshold = 3.0;
    //follow params
    follow_dp_min_turning_radius = 4.0;
    follow_dp_min_lon_bias = 3.0;
    follow_dp_max_lon_bias = 10.0;
    follow_dp_lon_sample_velocity_times = 2.0;//纵向采样距离相对速度的倍数
    follow_dp_max_lat_bias = 5.0;//横向采样最大限制
    follow_dp_min_lat_bias = 1.0;//横向采样最低限制
    track_negtive_obs_expan = -1.0;//负数表示不膨胀
    //通过二次函数形式设置速度与横向采样间隔的关系，同时受上述边界值限制
    follow_dp_lat_sample_2nd_order_param = -0.083;// 1/12 二阶系数a
    follow_dp_lat_sample_1st_order_param = 5.083;// 61/12 一阶系数b
    follow_collision_check_slice_num = 4;
    follow_forwardfollowing_calling_interval = 30;
    follow_slowdown_calling_interval = 30;
    follow_staystill_calling_interval = 30;
    follow_enable_refpath_connection = false;
    follow_lat_sample_half_goal_num = 10;

    // follow_frenet_max_curvature = 0.2;
    // follow_frenet_max_road_width = -10.0;
    // follow_frenet_min_road_width = 10.0;
    // follow_frenet_road_width_step = 0.5;
    // follow_frenet_sample_time = 2.0;
    // follow_frenet_sample_time_stepwidth = 0.1;
    // follow_frenet_sample_dis_stepwidth = 0.1;
    // follow_frenet_refpath_fit_order = 3;
    // follow_frenet_jerk_weight = 0.3;
    // follow_frenet_time_weight = 0.0;
    // follow_frenet_distance_weight = 0.7;
    // follow_frenet_lat_weight = 1.0;
    // follow_frenet_lon_weight = 0.0;
    follow_frenet_max_curvature = 0.0;
    follow_frenet_max_road_width = 0.0;
    follow_frenet_min_road_width = 0.0;
    follow_frenet_road_width_step = 0.0;
    follow_frenet_sample_time = 0.0;
    follow_frenet_sample_time_stepwidth = 0.0;
    follow_frenet_sample_dis_stepwidth = 0.0;
    follow_frenet_refpath_fit_order = 0;
    follow_frenet_jerk_weight = 0.0;
    follow_frenet_time_weight = 0.0;
    follow_frenet_distance_weight = 0.0;
    follow_frenet_lat_weight = 0.0;
    follow_frenet_lon_weight = 0.0;
    follow_frenet_using_connect_line = false;
    //track params
    ignore_collision_distance = 60.0;
    track_dp_min_turning_radius = 10.0;
    track_dp_expect_avoid_dis = 1.0;
    track_dp_min_lon_bias = 3.0;
    track_dp_max_lon_bias = 10.0;
    track_dp_lon_sample_velocity_times = 4.0;//纵向采样距离相对速度的倍数
    track_dp_max_lat_bias = 5.0;//横向采样最大限制
    track_dp_min_lat_bias = 1.0;//横向采样最低限制
    track_state_switch_collision_dis = 7.5;

    track_casadi_optimize_enable = false;
    //arrive params
    arrive_collision_weight_param = 1.0;
    arrive_refpath_match_weight_param = 0.5;
    arrive_lastpath_match_weight_param = 0.5;
    arrive_voronoi_dis_weight_param = 0.5;
    arrive_guide_min_dis = 20.0;
    arrive_min_sample_radius = 1.0;//arrive最小转弯。
    arrive_max_sample_len = 20.0;//arrive最大采样长度
    arrive_ignore_edge_len = 2.0;
    arrive_max_circle_count = 0.5;//采样路径最大长度占采样圆的圈数
    //通过二次函数形式设置速度与横向采样间隔的关系，同时受上述边界值限制
    track_dp_lat_sample_2nd_order_param = -0.083;// 1/12 二阶系数a
    track_dp_lat_sample_1st_order_param = 5.083;// 61/12 一阶系数b
    track_collision_check_slice_num = 4;
    track_forwardfollowing_calling_interval = 10;
    track_slowdown_calling_interval = 10;
    track_staystill_calling_interval = 10;
    track_enable_refpath_connection = true;
    track_lat_sample_half_goal_num = 10;
    track_enable_following_periodically_dp = true;
    track_enable_round_trip = false;
    track_enable_curvature_variable = false;
    track_enable_sub_high_precision = "none";
    track_enable_reverse_avoid = true;
    track_enable_pivot_steering = false;
    track_avoid_dis = 0.5;
    //track params
    track_collision_weight_param = 1.0;
    track_refpath_match_weight_param = 0.5;
    track_lastpath_match_weight_param = 0.5;
    track_voronoi_dis_weight_param = 0.5;
    track_guide_min_dis = 20.0;
    track_min_sample_radius = 1.0;
    track_max_sample_len = 20.0;
    track_max_circle_count = 0.5;
    track_dp_car_expan_speed_co = 0.04;
    //add fence
    enable_fence = false;
    height_bline = 0.0;
    follow_enable_dr = false;
    follow_use_init_heading = true;
    hybridastar_turning_radius = 0.0;
    hybridastar_grid_front_width = 0;
    hybridastar_grid_back_width = 0;
    hybridastar_grid_half_height = 0;
    hybridastar_grid_headings = 0;
    hybridastar_cell_size = 0.0;
    hybridastar_min_road_width = 0.0;
    hybridastar_pre_ref_length = 0.0;
    rectified_path_point_num = 20;
    hybridastar_enable_bezier_shot = false;
    pose_adjust_lat_limit = 5.0;
    pose_adjust_lon_limit = 40.0;
    pose_position_error_threshold = 1.0;
    pose_roll_error_threshold = 5.0;
    pose_pitch_error_threshold = 5.0;
    pose_yaw_error_threshold = 10.0;

    //用于路径处理类
    path_dheading_threshold = 5.0; //路径航向跳变检查阈值（°）
    dist_to_sharp_threshold = 5.0; //路径折点前后的距离（m）
    bezier_safe_dist = 10.0;

    //用于脱困算法
    steering_radian_ = 0.13;
    steering_discrete_num_ = 1;
    segment_length_discrete_num_min = 40;
    segment_length_discrete_num_max = 80;
    dist_to_carbox_threshold_for_introuble = 0.5;
    dist_to_carbox_threshold_for_isfree = 1.0;
    time_threshold_for_in_trouble = 15.0;
    reserved_path_length_for_splicing = 5.0;

    //遥控
    remote_enable_dr = false;
    remote_desired_speed = 0.0;

    //调头
    track_enable_turning_max = false;

    //QP
    QP_enable = false;
    QP_car2obs_dist = 1.0;
  }
//   bool LoadBehviorConfigParamsFromModel(ModelParam *pmodel_config) {
//     if (pmodel_config->GetValue("avoid", &avoid) == false) {
//         std::cout << "cannot get avoid" << std::endl;
//         return false;
//     }

//     if (pmodel_config->GetValue("enable_fence", &enable_fence) == false) {
//         std::cout << "cannot get enble_fence" << std::endl;
//         return false;
//     }

//     if (pmodel_config->GetValue("enable_unstruct_back_follow", &enable_unstruct_back_follow) == false) {
//       std::cout << "cannot get enable_unstruct_back_follow" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("enable_unstruct_avoid", &enable_unstruct_avoid) == false) {
//       std::cout << "cannot get enable_unstruct_avoid" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("unstruct_avoid_level", &unstruct_avoid_level) == false) {
//       std::cout << "cannot get unstruct_avoid_level" << std::endl;
//       return false;
//     }

//     //follow DP params
//     if (pmodel_config->GetValue("follow_dp_min_turning_radius", &follow_dp_min_turning_radius) == false) {
//       std::cout << "cannot get follow_dp_min_turning_radius" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_dp_min_lon_bias", &follow_dp_min_lon_bias) == false) {
//       std::cout << "cannot get follow_dp_min_lon_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_dp_max_lon_bias", &follow_dp_max_lon_bias) == false) {
//       std::cout << "cannot get follow_dp_max_lon_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_dp_lon_sample_velocity_times", &follow_dp_lon_sample_velocity_times) == false) {
//       std::cout << "cannot get follow_dp_lon_sample_velocity_times" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_dp_max_lat_bias", &follow_dp_max_lat_bias) == false) {
//       std::cout << "cannot get follow_dp_max_lat_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_dp_min_lat_bias", &follow_dp_min_lat_bias) == false) {
//       std::cout << "cannot get follow_dp_min_lat_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_negtive_obs_expan", &track_negtive_obs_expan) == false) {
//       std::cout << "cannot get track_negtive_obs_expan" << std::endl;
//       return false;
//     }    
//     if (pmodel_config->GetValue("follow_dp_lat_sample_2nd_order_param", &follow_dp_lat_sample_2nd_order_param) == false) {
//       std::cout << "cannot get follow_dp_lat_sample_2nd_order_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_dp_lat_sample_1st_order_param", &follow_dp_lat_sample_1st_order_param) == false) {
//       std::cout << "cannot get follow_dp_lat_sample_1st_order_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_forwardfollowing_calling_interval", &follow_forwardfollowing_calling_interval) == false) {
//       std::cout << "cannot get follow_forwardfollowing_calling_interval" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_collision_check_slice_num", &follow_collision_check_slice_num) == false) {
//       std::cout << "cannot get follow_collision_check_slice_num" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_slowdown_calling_interval", &follow_slowdown_calling_interval) == false) {
//       std::cout << "cannot get follow_slowdown_calling_interval" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_staystill_calling_interval", &follow_staystill_calling_interval) == false) {
//       std::cout << "cannot get follow_staystill_calling_interval" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_enable_refpath_connection", &follow_enable_refpath_connection) == false) {
//       std::cout << "cannot get follow_enable_refpath_connection" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_lat_sample_half_goal_num", &follow_lat_sample_half_goal_num) == false) {
//       std::cout << "cannot get follow_lat_sample_half_goal_num" << std::endl;
//       return false;
//     }


//     if (pmodel_config->GetValue("follow_frenet_max_curvature", &follow_frenet_max_curvature) == false) {
//       std::cout << "cannot get follow_frenet_max_curvature" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_max_road_width", &follow_frenet_max_road_width) == false) {
//       std::cout << "cannot get follow_frenet_max_road_width" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_min_road_width", &follow_frenet_min_road_width) == false) {
//       std::cout << "cannot get follow_frenet_min_road_width" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_road_width_step", &follow_frenet_road_width_step) == false) {
//       std::cout << "cannot get follow_frenet_road_width_step" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_sample_time", &follow_frenet_sample_time) == false) {
//       std::cout << "cannot get follow_frenet_sample_time" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_sample_time_stepwidth", &follow_frenet_sample_time_stepwidth) == false) {
//       std::cout << "cannot get follow_frenet_sample_time_stepwidth" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_sample_dis_stepwidth", &follow_frenet_sample_dis_stepwidth) == false) {
//       std::cout << "cannot get follow_frenet_sample_dis_stepwidth" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_refpath_fit_order", &follow_frenet_refpath_fit_order) == false) {
//       std::cout << "cannot get follow_frenet_refpath_fit_order" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_jerk_weight", &follow_frenet_jerk_weight) == false) {
//       std::cout << "cannot get follow_frenet_jerk_weight" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_time_weight", &follow_frenet_time_weight) == false) {
//       std::cout << "cannot get follow_frenet_time_weight" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_distance_weight", &follow_frenet_distance_weight) == false) {
//       std::cout << "cannot get follow_frenet_distance_weight" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_lat_weight", &follow_frenet_lat_weight) == false) {
//       std::cout << "cannot get follow_frenet_lat_weight" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_lon_weight", &follow_frenet_lon_weight) == false) {
//       std::cout << "cannot get follow_frenet_lon_weight" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("follow_frenet_using_connect_line", &follow_frenet_using_connect_line) == false) {
//       std::cout << "cannot get follow_frenet_using_connect_line" << std::endl;
//       return false;
//     }
//     //track DP params
//     if (pmodel_config->GetValue("track_dp_ignore_collision_distance", &ignore_collision_distance) == false) {
//       std::cout << "cannot get track_dp_ignore_collision_distance" << std::endl;
//       return false;
//     }    
//     if (pmodel_config->GetValue("track_dp_min_turning_radius", &track_dp_min_turning_radius) == false) {
//       std::cout << "cannot get track_dp_min_turning_radius" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_expect_avoid_dis", &track_dp_expect_avoid_dis) == false) {
//       std::cout << "cannot get track_dp_expect_avoid_dis" << std::endl;
//       return false;
//     }    
//     if (pmodel_config->GetValue("track_dp_min_lon_bias", &track_dp_min_lon_bias) == false) {
//       std::cout << "cannot get track_dp_min_lon_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_max_lon_bias", &track_dp_max_lon_bias) == false) {
//       std::cout << "cannot get track_dp_max_lon_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_lon_sample_velocity_times", &track_dp_lon_sample_velocity_times) == false) {
//       std::cout << "cannot get track_dp_lon_sample_velocity_times" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_max_lat_bias", &track_dp_max_lat_bias) == false) {
//       std::cout << "cannot get track_dp_max_lat_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_state_switch_collision_dis", &track_state_switch_collision_dis) == false) {
//       std::cout << "cannot get track_state_switch_collision_dis" << std::endl;
//       return false;
//     }        
//     if (pmodel_config->GetValue("track_dp_min_lat_bias", &track_dp_min_lat_bias) == false) {
//       std::cout << "cannot get track_dp_min_lat_bias" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_lat_sample_2nd_order_param", &track_dp_lat_sample_2nd_order_param) == false) {
//       std::cout << "cannot get track_dp_lat_sample_2nd_order_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_lat_sample_1st_order_param", &track_dp_lat_sample_1st_order_param) == false) {
//       std::cout << "cannot get track_dp_lat_sample_1st_order_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_forwardfollowing_calling_interval", &track_forwardfollowing_calling_interval) == false) {
//       std::cout << "cannot get track_forwardfollowing_calling_interval" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_collision_check_slice_num", &track_collision_check_slice_num) == false) {
//       std::cout << "cannot get track_collision_check_slice_num" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_slowdown_calling_interval", &track_slowdown_calling_interval) == false) {
//       std::cout << "cannot get track_slowdown_calling_interval" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_staystill_calling_interval", &track_staystill_calling_interval) == false) {
//       std::cout << "cannot get track_staystill_calling_interval" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_enable_refpath_connection", &track_enable_refpath_connection) == false) {
//       std::cout << "cannot get track_enable_refpath_connection" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_lat_sample_half_goal_num", &track_lat_sample_half_goal_num) == false) {
//       std::cout << "cannot get track_lat_sample_half_goal_num" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_enable_following_periodically_dp", &track_enable_following_periodically_dp) == false) {
//       std::cout << "cannot get track_enable_following_periodically_dp" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_enable_round_trip", &track_enable_round_trip) == false) {
//       std::cout << "cannot get track_enable_round_trip" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("track_enable_curvature_variable", &track_enable_curvature_variable) == false) {
//       std::cout << "cannot get track_enable_curvature_variable" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("track_casadi_optimize_enable", &track_casadi_optimize_enable) == false) {
//       std::cout << "cannot get track_casadi_optimize_enable" << std::endl;
//       return false;
//     }     
//     if (pmodel_config->GetValue("track_enable_sub_high_precision", &track_enable_sub_high_precision) == false) {
//       std::cout << "cannot get track_enable_sub_high_precision" << std::endl;
//       return false;
//     }    
//     if (pmodel_config->GetValue("track_enable_reverse_avoid", &track_enable_reverse_avoid) == false) {
//       std::cout << "cannot get track_enable_reverse_avoid" << std::endl;
//       return false;
//     }      
//     if (pmodel_config->GetValue("track_enable_pivot_steering", &track_enable_pivot_steering) == false) {
//       std::cout << "cannot get track_enable_pivot_steering" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("track_avoid_dis", &track_avoid_dis) == false) {
//       std::cout << "cannot get track_avoid_dis" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_dp_car_expan_speed_co", &track_dp_car_expan_speed_co) == false) {
//       std::cout << "cannot get track_dp_car_expan_speed_co" << std::endl;
//       return false;
//     }
//     //arrive path choose param
//     if (pmodel_config->GetValue("arrive_collision_weight_param", &arrive_collision_weight_param) == false) {
//       std::cout << "cannot get arrive_collision_weight_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("arrive_refpath_match_weight_param", &arrive_refpath_match_weight_param) == false) {
//       std::cout << "cannot get arrive_refpath_match_weight_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("arrive_lastpath_match_weight_param", &arrive_lastpath_match_weight_param) == false) {
//       std::cout << "cannot get arrive_lastpath_match_weight_param" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("arrive_voronoi_dis_weight_param", &arrive_voronoi_dis_weight_param) == false) {
//       std::cout << "cannot get arrive_voronoi_dis_weight_param" << std::endl;
//       return false;
//     }     
//     if (pmodel_config->GetValue("arrive_window_cost_threshold", &arrive_window_cost_threshold) == false) {
//       std::cout << "cannot get arrive_window_cost_threshold" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("arrive_min_feasible_dis", &arrive_min_feasible_dis) == false) {
//       std::cout << "cannot get arrive_min_feasible_dis" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("arrive_semanticmap_resolution", &arrive_semanticmap_resolution) == false) {
//       std::cout << "cannot get arrive_semanticmap_resolution" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("arrive_semantic_edge_radius", &arrive_semantic_edge_radius) == false) {
//       std::cout << "cannot get arrive_semantic_edge_radius" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("arrive_guide_min_dis", &arrive_guide_min_dis) == false) {
//       std::cout << "cannot get arrive_guide_min_dis" << std::endl;
//       return false;
//     }    
//     if (pmodel_config->GetValue("arrive_min_sample_radius", &arrive_min_sample_radius) == false) {
//       std::cout << "cannot get arrive_min_sample_radius" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("arrive_max_sample_len", &arrive_max_sample_len) == false) {
//       std::cout << "cannot get arrive_max_sample_len" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("arrive_ignore_edge_len", &arrive_ignore_edge_len) == false) {
//       std::cout << "cannot get arrive_ignore_edge_len" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("arrive_max_circle_count", &arrive_max_circle_count) == false) {
//       std::cout << "cannot get arrive_max_circle_count" << std::endl;
//       return false;
//     }     
//     if (pmodel_config->GetValue("height_bline", &height_bline) == false) {
//       std::cout << "cannot get height_bline" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("hybridastar_turning_radius", &hybridastar_turning_radius) == false) {
//       std::cout << "cannot get hybridastar_turning_radius" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("hybridastar_grid_front_width", &hybridastar_grid_front_width) == false) {
//       std::cout << "cannot get hybridastar_grid_front_width" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("hybridastar_grid_back_width", &hybridastar_grid_back_width) == false) {
//       std::cout << "cannot get hybridastar_grid_back_width" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("hybridastar_grid_half_height", &hybridastar_grid_half_height) == false) {
//       std::cout << "cannot get hybridastar_grid_half_height" << std::endl;
//       return false;
//     }      
//     if (pmodel_config->GetValue("hybridastar_grid_headings", &hybridastar_grid_headings) == false) {
//       std::cout << "cannot get hybridastar_grid_headings" << std::endl;
//       return false;
//     }      
//     if (pmodel_config->GetValue("hybridastar_cell_size", &hybridastar_cell_size) == false) {
//       std::cout << "cannot get hybridastar_cell_size" << std::endl;
//       return false;
//     }      
//     if (pmodel_config->GetValue("hybridastar_min_road_width", &hybridastar_min_road_width) == false) {
//       std::cout << "cannot get hybridastar_min_road_width" << std::endl;
//       return false;
//     }         
//     if (pmodel_config->GetValue("hybridastar_pre_ref_length", &hybridastar_pre_ref_length) == false) {
//       std::cout << "cannot get hybridastar_pre_ref_length" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("rectified_path_point_num", &rectified_path_point_num) == false) {
//       std::cout << "cannot get rectified_path_point_num" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("hybridastar_enable_bezier_shot", &hybridastar_enable_bezier_shot) == false) {
//       std::cout << "cannot get hybridastar_enable_bezier_shot" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("pose_adjust_lat_limit", &pose_adjust_lat_limit) == false) {
//       std::cout << "cannot get pose_adjust_lat_limit" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("pose_adjust_lon_limit", &pose_adjust_lon_limit) == false) {
//       std::cout << "cannot get pose_adjust_lon_limit" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("pose_position_error_threshold", &pose_position_error_threshold) == false) {
//       std::cout << "cannot get pose_position_error_threshold" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("pose_yaw_error_threshold", &pose_yaw_error_threshold) == false) {
//       std::cout << "cannot get pose_yaw_error_threshold" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("pose_roll_error_threshold", &pose_roll_error_threshold) == false) {
//       std::cout << "cannot get pose_roll_error_threshold" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("pose_pitch_error_threshold", &pose_pitch_error_threshold) == false) {
//       std::cout << "cannot get pose_pitch_error_threshold" << std::endl;
//       return false;
//     }
// //track path choose param
//     if (pmodel_config->GetValue("track_collision_weight_param", &track_collision_weight_param) == false) {
//       std::cout << "cannot get track_collision_weight_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_refpath_match_weight_param", &track_refpath_match_weight_param) == false) {
//       std::cout << "cannot get track_refpath_match_weight_param" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_lastpath_match_weight_param", &track_lastpath_match_weight_param) == false) {
//       std::cout << "cannot get track_lastpath_match_weight_param" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("track_voronoi_dis_weight_param", &track_voronoi_dis_weight_param) == false) {
//       std::cout << "cannot get track_voronoi_dis_weight_param" << std::endl;
//       return false;
//     }     
//     if (pmodel_config->GetValue("track_window_cost_threshold", &track_window_cost_threshold) == false) {
//       std::cout << "cannot get track_window_cost_threshold" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("track_min_feasible_dis", &track_min_feasible_dis) == false) {
//       std::cout << "cannot get track_min_feasible_dis" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_semanticmap_resolution", &track_semanticmap_resolution) == false) {
//       std::cout << "cannot get track_semanticmap_resolution" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("track_semantic_edge_radius", &track_semantic_edge_radius) == false) {
//       std::cout << "cannot get track_semantic_edge_radius" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("track_guide_min_dis", &track_guide_min_dis) == false) {
//       std::cout << "cannot get track_guide_min_dis" << std::endl;
//       return false;
//     }    
//     if (pmodel_config->GetValue("track_min_sample_radius", &track_min_sample_radius) == false) {
//       std::cout << "cannot get track_min_sample_radius" << std::endl;
//       return false;
//     }  
//     if (pmodel_config->GetValue("track_max_sample_len", &track_max_sample_len) == false) {
//       std::cout << "cannot get track_max_sample_len" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("track_max_circle_count", &track_max_circle_count) == false) {
//       std::cout << "cannot get track_max_circle_count" << std::endl;
//       return false;
//     }   
//     if (pmodel_config->GetValue("height_bline", &height_bline) == false) {
//       std::cout << "cannot get height_bline" << std::endl;
//       return false;
//     } 
//     if (pmodel_config->GetValue("track_path_length_to_end_task", &track_path_length_to_end_task) == false) {
//       std::cout << "cannot get track_path_length_to_end_task" << std::endl;
//       return false;
//     } 

    
//     if (pmodel_config->GetValue("follow_enable_dr", &follow_enable_dr) == false) {
//       std::cout << "cannot get follow_enable_dr" << std::endl;
//       return false;
//     }
        
//     if (pmodel_config->GetValue("follow_use_init_heading", &follow_use_init_heading) == false) {
//       std::cout << "cannot get follow_use_init_heading" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("path_dheading_threshold", &path_dheading_threshold) == false) {
//       std::cout << "cannot get path_dheading_threshold" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("dist_to_sharp_threshold", &dist_to_sharp_threshold) == false) {
//       std::cout << "cannot get dist_to_sharp_threshold" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("bezier_safe_dist", &bezier_safe_dist) == false) {
//       std::cout << "cannot get bezier_safe_dist" << std::endl;
//       return false;
//     }
//     //用于脱困
//     if (pmodel_config->GetValue("steering_radian_", &steering_radian_) == false) {
//       std::cout << "cannot get steering_radian_" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("steering_discrete_num_", &steering_discrete_num_) == false) {
//       std::cout << "cannot get steering_discrete_num_" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("segment_length_discrete_num_min", &segment_length_discrete_num_min) == false) {
//       std::cout << "cannot get segment_length_discrete_num_min" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("segment_length_discrete_num_max", &segment_length_discrete_num_max) == false) {
//       std::cout << "cannot get segment_length_discrete_num_max" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("dist_to_carbox_threshold_for_introuble", &dist_to_carbox_threshold_for_introuble) == false) {
//       std::cout << "cannot get dist_to_carbox_threshold_for_introuble" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("dist_to_carbox_threshold_for_isfree", &dist_to_carbox_threshold_for_isfree) == false) {
//       std::cout << "cannot get dist_to_carbox_threshold_for_isfree" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("time_threshold_for_in_trouble", &time_threshold_for_in_trouble) == false) {
//       std::cout << "cannot get time_threshold_for_in_trouble" << std::endl;
//       return false;
//     }
//     if (pmodel_config->GetValue("reserved_path_length_for_splicing", &reserved_path_length_for_splicing) == false) {
//       std::cout << "cannot get reserved_path_length_for_splicing" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("remote_enable_dr", &remote_enable_dr) == false) {
//       std::cout << "cannot get remote_enable_dr" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("remote_desired_speed", &remote_desired_speed) == false) {
//       std::cout << "cannot get remote_desired_speed" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("track_enable_turning_max", &track_enable_turning_max) == false) {
//       std::cout << "cannot get track_enable_turning_max" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("QP_enable", &QP_enable) == false) {
//       std::cout << "cannot get QP_enable" << std::endl;
//       return false;
//     }

//     if (pmodel_config->GetValue("QP_car2obs_dist", &QP_car2obs_dist) == false) {
//       std::cout << "cannot get QP_car2obs_dist" << std::endl;
//       return false;
//     }

//     return true;
//   }
};

//非结构化路径规划算法
struct PathplanConfig {
  bool enable_unstruct_dwa;
  bool enable_ss_plan;
  bool enable_dynamic_plan;
  bool enable_hybrid_astar;

  PathplanConfig() {
    enable_unstruct_dwa = true;
    enable_ss_plan = true;
    enable_dynamic_plan = true;
    enable_hybrid_astar = true;
  }
};

//速度规划配置
struct SpeedplanConfig {
  double braking_dis;
  double speed;
  double speed_attenuation_coefficient;
  SpeedplanConfig() {
    braking_dis = 0.0;
    speed = 30.0;
    speed_attenuation_coefficient = 1.0;
  }
  // bool LoadSpeedplanConfigParamsFromModel(ModelParam *pmodel_config) {
  //   if (pmodel_config->GetValue("speed", &speed) == false) {
  //     std::cout << "cannot get speed" << std::endl;
  //     return false;
  //   }
  //   if (pmodel_config->GetValue("speed_attenuation_coefficient", &speed_attenuation_coefficient) == false) {
  //     std::cout << "cannot get speed_attenuation_coefficient" << std::endl;
  //     return false;
  //   }
  //   return true;
  // }
};

typedef struct ConfigInfo {
  CarModel car_model;
  CognitionConfig cognition_config;
  BehaviorConfig behavior_config;
  BehaviorConfig history_behavior_config;
  PathplanConfig pathplan_config;
  SpeedplanConfig speedplan_config;
  time_t last_modified_time_of_planning_config;
  time_t last_modified_time_of_vehicle_interface_config;
  // std::string planning_config_path;
  // std::string vehicle_interface_config_path;
  bool need_reload_config = false;

  ConfigInfo& operator=(const ConfigInfo& other) {
        // 检查自赋值
        if (this != &other) {
            car_model = other.car_model;
            cognition_config = other.cognition_config;
            behavior_config = other.behavior_config;
            // history_behavior_config = other.history_behavior_config;
    // std::cout << "ssssssssssssssssssssssss5" << std::endl;
            pathplan_config = other.pathplan_config;
            speedplan_config = other.speedplan_config;
            last_modified_time_of_planning_config = other.last_modified_time_of_planning_config;
            last_modified_time_of_vehicle_interface_config = other.last_modified_time_of_vehicle_interface_config;
            need_reload_config = other.need_reload_config;
        }
        return *this;
    }
} ConfigInfo;


//故障处理信息
struct FaultHandleInfo{
  eFaultHandleStatus falut_handle_status;
  double fault_velocity_limit;
  FaultHandleInfo() {
    Reset();
  }
  void Reset(){
    falut_handle_status = eFaultHandleStatus::NORMAL;
    fault_velocity_limit = 20.0;
  }

};


}
}

#endif
