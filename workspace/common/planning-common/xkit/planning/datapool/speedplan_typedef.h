#ifndef DATAPOOL_INCLUDE_SPEEDPLAN_TYPEDEF_H_
#define DATAPOOL_INCLUDE_SPEEDPLAN_TYPEDEF_H_

#include "xkit/planning/datapool/public_typedef.h"

namespace avos {
namespace planning {

const double MAX_VELOCITY_INCREASE_GRADIENT = 80 / 3.6;
const double MAX_VELOCITY_DECREASE_GRADIENT = 5.0 / 3.6;

enum class eSpeedPlanMode {
  DEFAULT = 0,
  LOW_SPEED,
  MIDDLE_SPEED,
  HIGH_SPEED
};

typedef struct SpeedPlanMode {
  eSpeedPlanMode speedplan_mode;
  std::string speedplan_mode_str;
  SpeedPlanMode() {
    speedplan_mode = eSpeedPlanMode::DEFAULT;
    speedplan_mode_str = "default";
  }
  void SetLowSpeedMode() {
    speedplan_mode = eSpeedPlanMode::LOW_SPEED;
    speedplan_mode_str = "low_speed";
  }
  void SetMiddleSpeedMode() {
    speedplan_mode = eSpeedPlanMode::MIDDLE_SPEED;
    speedplan_mode_str = "middle_speed";
  }
  void SetHighSpeedMode() {
    speedplan_mode = eSpeedPlanMode::HIGH_SPEED;
    speedplan_mode_str = "high_speed";
  }
} SpeedPlanMode;

struct StopType {
  bool object_avoid;
  bool cell_avoid;
  bool fault_stop;
  bool fence_stop;
  bool predicted_ego_stop;
  bool shift_stop;
  bool far_from_start_stop;
  bool far_from_path_stop;
  bool config_read_fail_stop;
  bool emergency_action;
  bool obstacles_near_path;
  bool obstacles_near_ego;
  bool lon_speed_overshoot;
  bool continuous_high_speed;

  double object_lat;
  double object_lon;
  //------------------follow---------------------//
  bool target_invalid_and_path_short_stop;
  bool less_than_min_follow_dis_stop;
  bool follow_stable_stop;
  StopType() {
    Reset();
  }
  void Reset(){
  object_avoid = false;
  cell_avoid = false;
  fault_stop = false;
  fence_stop = false;
  predicted_ego_stop = false;
  shift_stop = false; 
  far_from_start_stop = false; 
  far_from_path_stop = false;  

  config_read_fail_stop = false;
  emergency_action = false;
  obstacles_near_path = false;
  obstacles_near_ego = false;
  lon_speed_overshoot = false;
  continuous_high_speed = false;


  less_than_min_follow_dis_stop = false;
  target_invalid_and_path_short_stop = false;
  follow_stable_stop = false;
  }
};
struct SpeedplanInfo {
  StopType stop_reason;
  int stop_frame;
  bool is_passable;
  bool pathReplanFlag;
  int pathDirection;
  int pathplannerGo;
  double pathplannerSpeedLimit;
  bool business_speed_limit;
  double stop_dis;
  double stop_dis_AC;
  double stop_dis_fs;//added by DZ
  bool is_final_adjust;
  int drive_mode;//0: default, 1: overtaking, 2: meeting
  double guided_speed;//the min velocity is guided to be LARGER than guided_speed
  bool emgency_stop;
  bool obs_emgency_stop;
  double current_velocity;
  double track_error_threshold;
  SpeedPlanMode speedplan_mode;
  PathData path;
  CollisionInfoVec collision_info_vec;
  std::vector<std::string> velocity_limit_reason;
  SpeedplanInfo() {
    is_passable = false;
    pathDirection = 0;
    pathplannerSpeedLimit = 0.5;
    pathplannerGo = 0;
    pathReplanFlag = false;
    velocity_limit_raw = 5.0;
    velocity_limit = 10.0;
    business_speed_limit = false;
    stop_dis = std::numeric_limits<double>::max();
    stop_dis_AC = std::numeric_limits<double>::max();
    stop_dis_fs = std::numeric_limits<double>::max();
    collision_info_vec.clear();
    is_final_adjust = false;
    drive_mode = 0;
    guided_speed = 0.0;
    emgency_stop = false;
    obs_emgency_stop = false;
    current_velocity = 0.0;
    track_error_threshold = 5.0;
  }

  double GetVelocityLimit(){
    return velocity_limit;
  }

  void SetVelocityLimit(const double& velocity, const std::string& reason,
                        const bool& cover = false) {
    double set_velocity = velocity;
    velocity_limit = set_velocity;
    // if (cover) {
    //     velocity_limit = set_velocity;
    // } else {
    //     if (current_velocity - velocity > MAX_VELOCITY_DECREASE_GRADIENT) {
    //         set_velocity = current_velocity - MAX_VELOCITY_DECREASE_GRADIENT;
    //     } else if (velocity - current_velocity > MAX_VELOCITY_INCREASE_GRADIENT) {
    //         set_velocity = current_velocity + MAX_VELOCITY_INCREASE_GRADIENT;
    //     }
    //     velocity_limit = std::min(velocity_limit, set_velocity);
    // }

    velocity_limit_reason.emplace_back(std::to_string(set_velocity) + " ----- " + reason);
  }

  void SetCurrentVelocity(const double& velocity) {
    current_velocity = velocity;
  }

  void SetVelocityLimitRaw(const double& velocity) {
    velocity_limit_raw = velocity;
  }

  double GetVelocityLimitRaw(){
    return velocity_limit_raw;
  }

  void ClearVelocityLimitReasons() {
    velocity_limit_reason.clear();
  }
private:
  double velocity_limit_raw;
  double velocity_limit;
};

}
}

#endif
