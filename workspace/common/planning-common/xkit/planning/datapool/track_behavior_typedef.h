#ifndef DATAPOOL_INCLUDE_TRACK_BEHAVIOR_TYPEDEF_H_
#define DATAPOOL_INCLUDE_TRACK_BEHAVIOR_TYPEDEF_H_

#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/cognition_typedef.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/common_config.h"
#include "xkit/planning/datapool/speedplan_typedef.h"
#include "xkit/planning/datapool/base_behavior_typedef.h"
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/algorithm_interface/algorithm_base.h"
#include "xkit/planning/datapool/planning_info.h"


namespace avos {
namespace planning {

enum class eTrackingBehaviorStatus {
  DEFAULT = 0,
  IDLE,
  FORWARDFOLLOWING,
  SLOW_DOWN,
  STAY_STILL,
  ENDTASK,
  BEHAVIOR_ERROR,
  DEPEND_FENCE,
  POSE_ADJUST,
  GET_OUT_OF_TROUBLE,
  RUNREALTIME,
  CASADI_OPTIMIZE,
  TURN_AROUND,
  REMOTE_PLAN,
  PASS_THROUGH
};


typedef struct TrackingBehaviorStatus {
  eTrackingBehaviorStatus status;
  std::string status_str;
  TrackingBehaviorStatus() {
    status = eTrackingBehaviorStatus::DEFAULT;
    status_str = "default";
  }

  void Reset() {
    status = eTrackingBehaviorStatus::DEFAULT;
    status_str = "default";
  }
  void SetIdle() {
    status = eTrackingBehaviorStatus::IDLE;
    status_str = "behavior idle";
  }

  void SetForwardFollowing() {
    status = eTrackingBehaviorStatus::FORWARDFOLLOWING;
    status_str = "ForwardFollowing";
  }

  void SetSlowDown() {
    status = eTrackingBehaviorStatus::SLOW_DOWN;
    status_str = "slow down";
  }

  void SetStayStill() {
    status = eTrackingBehaviorStatus::STAY_STILL;
    status_str = "stay still";
  }

  void SetEndTask() {
    status = eTrackingBehaviorStatus::ENDTASK;
    status_str = "end task";
  }

  void SetDependFence() {
    status = eTrackingBehaviorStatus::DEPEND_FENCE;
    status_str = "depend fence planning";
  }

  void SetPoseAdjust() {
    status = eTrackingBehaviorStatus::POSE_ADJUST;
    status_str = "Pose ADdjust planning";
  }
  void SetGetOutOfTrouble() {
    status = eTrackingBehaviorStatus::GET_OUT_OF_TROUBLE;
    status_str = "Get out of Trouble planning";
  }
  void SetRunRealTime() {
    status = eTrackingBehaviorStatus::RUNREALTIME;
    status_str = "run real time";
  }
  void SetCasadiOptimize() {
    status = eTrackingBehaviorStatus::CASADI_OPTIMIZE;
    status_str = "casadi local optimize";
  } 
  void SetTurnAround() {
    status = eTrackingBehaviorStatus::TURN_AROUND;
    status_str = "Turn Around planning";
  } 
  void SetRemotePlan() {
    status = eTrackingBehaviorStatus::REMOTE_PLAN;
    status_str = "Remote Control Planning";
  }
  void SetPassThrough() {
    status = eTrackingBehaviorStatus::PASS_THROUGH;
    status_str = "Pass Through planning";
  } 
} TrackingBehaviorStatus;


typedef struct TrackingBehaviorFSMInfo {
  TrackingBehaviorStatus behavior_status;
  BehaviorBusyStatus behavior_busy_status;
  BehaviorExecuteStatus forward_following_status;
  BehaviorExecuteStatus slow_down_status;
  StatisticsTimes time_for_dp_avoid;
  StatisticsTimes time_for_hybrid_atar;
  StatisticsTimes staystill_time;
  StatisticsTimes time_for_sehs;
  StatisticsTimes time_for_hastar_adjust;
  StatisticsTimes time_for_hastar_avoid;
  StatisticsTimes time_for_casadi_optimize;
  BehaviorExecuteStatus astar_result_status;

  double last_xg;
  double last_yg; 
  int dp_avoid_result;//dp避障状态：0-无避障，1-避障成功，-1-避障失败

  bool suspend_refpath_generation;
  int suspend_refpath_generation_interval;//loop num
  bool manual_trigger_refpath_generation;
  bool hybrid_astar_adjust_success;
  bool avoid_success;

  TrackingBehaviorFSMInfo() {
    Reset();
  }

  void Reset() {
    behavior_status.Reset();
    behavior_busy_status.Reset();
    last_xg = 0.0;
    last_yg = 0.0;
    time_for_dp_avoid.SetStatisticsTimes(10);
    time_for_hybrid_atar.SetStatisticsTimes(20);
    staystill_time.SetStatisticsTimes(1200);//2min
    time_for_sehs.SetStatisticsTimes(10);
    time_for_hastar_adjust.SetStatisticsTimes(10);
    time_for_casadi_optimize.SetStatisticsTimes(30);
    time_for_hastar_avoid.SetStatisticsTimes(5);
    suspend_refpath_generation = false;
    manual_trigger_refpath_generation = false;
    suspend_refpath_generation_interval = 20;
    dp_avoid_result = 0;
    hybrid_astar_adjust_success = false;
    avoid_success = false;
  }

  bool IsNeedCallDpAvoid() {
    return time_for_dp_avoid.IsTimesUp();
  }
  bool IsNeedCallHybridAstar() {
    if (time_for_hybrid_atar.IsTimesUp()) return true;
    return false;
  }
  bool IsNeedCallSEHSAvoid() {
    return time_for_sehs.IsTimesUp();
  }
  bool IsNeedCallPostAdjust() {
    return time_for_hastar_adjust.IsTimesUp();
  }
  bool IsNeedCallCasadiOptimize() {
    return time_for_casadi_optimize.IsTimesUp();
  }  
  bool IsNeedCallHastarAvoid() {
    return time_for_hastar_avoid.IsTimesUp();
  }  
  bool IsHybridAstarSuccess() {
    return (astar_result_status.status == eBehaviorExecuteStatus::EXE_SUCCESS);
  }
  void ResetHybridAstar() {
    astar_result_status.Reset();
  }

  void SetSuspendFlag(){
    suspend_refpath_generation = true;
  }
  void ResetSuspendFlag(){
    suspend_refpath_generation = false;
  }
  void SetManualTriggerFlag(){
    manual_trigger_refpath_generation = true;
  }
  void ResetManualTriggerFlag(){
    manual_trigger_refpath_generation = false;
  }
} TrackingBehaviorFSMInfo;

typedef struct TrackingBehaviorFSMParam {
  TrackingBehaviorFSMInfo behavior_fsm_info;
  VehicleStatus vehicle_status;
  // UnstructDynamicInfo dynamic_info;
  // UnstructSemanticInfo semantic_info;
  UnstructEnv unstruct_env_info;
  LocalizationData localization_data;
  // MapEngineData mapengine_data;
  ConfigInfo config_info;
  PlanningInfo planning_info;
  ExceptionAnalyseResult exception_analyse_result;

  std::shared_ptr<AlgorithmBase> algorithm_ptr;

  PathData last_result_path;
  PathData result_path;
  // PathData calculate_path;
  PathData dr_path;
  Paths paths;

  DebugPlanningMsg debug_planning_msg;

  // geometry::Site turn_around_target;
  // int stop_frame;

  bool turn_around;
  bool is_release_set;
  bool enable_struct_behavior;//SZ
  bool turn_around_path_get_ready;
  int turn_around_path_interval_count = 100;
  int pose_adjust_path_interval_count = 100;
  bool hastar_cal_overtime = false;

  TrackingBehaviorFSMParam()
  {
    is_release_set = false;
    enable_struct_behavior = false;
  }

} TrackingBehaviorFSMParam;

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_BEHAVIOR_TYPEDEF_H_
