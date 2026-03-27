#ifndef SPEEDPLAN_BASE_H_
#define SPEEDPLAN_BASE_H_

// #include "planning_msgs/motiondebug.h"
#include "xkit/planning/toolbox/algorithm_interface/speedplan_header.h"
// using MotionDebug = planning_msgs::motiondebug;

namespace avos {
namespace motionplan {

  struct MotionDebug {
    char avoid;
    char perception_status;
    double speed;
    double ego_speed;
    double speed_limit_raw;
    double speedlimit_curvature;
    double speedlimit_beside;
    double speedlimit_track_error;
    double track_error;
    double curvature;
    int behavior_fsm_state;  // smaller or equal to 0 is slow down to stop, larger than 0 means go
    int fault_level;
    int emergency_flag;
    int path_direction;    // 0 means drive, 1 means reverse
    int shift_pos_match;   // if vehicle shift pos is match with path direction, it is 1, else 0
    int delay_to_start;    // if lat long coupling success, it is 1, else 0
    int deviate_path_flag;   // when car is deviate from path, should stop immediately
    int is_passable;     // determin by decision, 1 means reachable, 0 means unreachable
    int replan_flag;
    int free_drive_replan;
    int path_update_flag;    // when rewrite path in map engine happend, is true
    double safe_dist;
    double preview_dist;
    double roi_type;      // 1:front, 2:beside, 3:near, 0.5:predict front, 1.5: predict beside, 2.5: predict near
    double roi_id;        // obj id after add predict
    double front_s;
    double front_l;
    double beside_s;
    double beside_l;
    double crashed_cell_s;    // crashed with cell at longtitude distance s
    double cell_preview_dist;
    double cell_limit_index;
    double steer_angle;
    double target_angle;    // approximate target steer angle
    double cost_time;
    double temp1;
    double temp2;
    double temp3;
    double objLength;
    double distPreview;
    double follow_thw;
    double follow_headway_dis;
    double follow_path_to_car;
    double follow_desired_acc;
    double follow_desired_dis;
    double follow_deltav;
    double follow_dis_ratio;
    double follow_path_length;
    double follow_blindsScope;
    bool follow_ispath_in_car;
    bool follow_is_end_intersect_car;
    bool receive_per_flag;
    double follow_target_global_angle;
    double follow_target_length;
    double follow_target_width;
    double follow_target_xg;
    double follow_target_yg;
    double follow_target_velocity;
    double objVRelProj2point;
    double fence_angle;
    double collision_dis;
    int estop_flag;
    bool object_avoid_stop;
    bool cell_avoid_stop;
    bool fault_stop;
    std::string behavior_state;
    std::vector<std::string> velocity_limit_reason;
  };

enum class eSpeedPlanType {
  DEFAULT = 0,
  Dummy,
  TRACK,
  FOLLOW,
  ARRIVE
};

typedef struct SpeedPlanType
{
  eSpeedPlanType type;
  std::string type_str;
  SpeedPlanType() {
    type = eSpeedPlanType::DEFAULT;
    type_str = "defualt";
  }

  void SetDummySpeedPlan() {
    type = eSpeedPlanType::TRACK;
    type_str = "Dummy SpeedPlan";
  }
  void SetTrackSpeedPlan() {
    type = eSpeedPlanType::TRACK;
    type_str = "TRACK SpeedPlan";
  }

  void SetFollowSpeedPlan() {
    type = eSpeedPlanType::FOLLOW;
    type_str = "FOLLOW SpeedPlan";
  }

  void SetArriveSpeedPlan() {
    type = eSpeedPlanType::ARRIVE;
    type_str = "ARRIVE SpeedPlan";
  }

} SpeedPlanType;

class SpeedplanBase {
public:
  SpeedplanBase() {};
  ~SpeedplanBase() {};
  
  // template<class T>
  // bool IsGetValue(ModelParam *pmodel_config, const std::string param_name, T& param) {
  //   if (!pmodel_config->GetValue(param_name, &param)) {
  //     std::cout<< "cannot get "<< param_name;
  //     return false;
  //   }
  //   // AINFO<<param_name<<" "<<param;
  //   return true;
  // }

  double NormalizeAngleByRad(const double angle) {
    double a = std::fmod(angle + M_PI, 2.0 * M_PI);
    if (a < 0.0) {
      a += (2.0 * M_PI);
    }
    return a - M_PI;
  }

  double IncludeAngleByRad(const double angleA, const double angleB) {
    // should normalize by rad advance
    if ( std::fabs(angleA) > M_PI || std::fabs(angleB) > M_PI ) {
      // assert(false);
      return 9999.0;
    }
    double includeAngle = angleA - angleB;
    if ( includeAngle > M_PI) {
      includeAngle = 2 * M_PI - includeAngle;
    } else if ( includeAngle < -M_PI) {
      includeAngle = 2 * M_PI + includeAngle;
    }
    return includeAngle;
  }

  double dist_get(double x0, double y0, double x1, double y1)
  {
    double dist_temp = sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
    return dist_temp;
  }

  virtual void PullData() = 0;
  virtual void PushData() = 0;
  // virtual bool LoadConfigFromModel(ModelParam *pmodel_config) {
  //   return true;
  // }
  virtual bool RunSpeedPlan() {
    std::cout << "***********base_speed_plan***********" << std::endl;
    return true;
  }
public:
  MotionDebug motionVars;
  SpeedPlanType SpeedPlan_type_;
  
};

} // namespace motionplan
} // namespace avos

#endif 
