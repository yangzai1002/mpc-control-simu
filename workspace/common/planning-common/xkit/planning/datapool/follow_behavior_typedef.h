#ifndef DATAPOOL_INCLUDE_FOLLOWING_BEHAVIOR_TYPEDEF_H_
#define DATAPOOL_INCLUDE_FOLLOWING_BEHAVIOR_TYPEDEF_H_

#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/cognition_typedef.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/common_config.h"
#include "xkit/planning/datapool/speedplan_typedef.h"
#include "xkit/planning/datapool/base_behavior_typedef.h"
#include "xkit/planning/toolbox/algorithm_interface/algorithm_base.h"


namespace avos {
namespace planning {

enum class eFollowingBehaviorStatus {
  DEFAULT = 0,
  IDLE,
  FORWARDFOLLOWING,
  SLOW_DOWN,
  STAY_STILL,
  ENDTASK,
  BEHAVIOR_ERROR
};


typedef struct FollowingBehaviorStatus {
  eFollowingBehaviorStatus status;
  std::string status_str;
  FollowingBehaviorStatus() {
    status = eFollowingBehaviorStatus::DEFAULT;
    status_str = "default";
  }

  void Reset() {
    status = eFollowingBehaviorStatus::DEFAULT;
    status_str = "default";
  }
  void SetIdle() {
    status = eFollowingBehaviorStatus::IDLE;
    status_str = "behavior idle";
  }

  void SetForwardFollowing() {
    status = eFollowingBehaviorStatus::FORWARDFOLLOWING;
    status_str = "ForwardFollowing";
  }

  void SetSlowDown() {
    status = eFollowingBehaviorStatus::SLOW_DOWN;
    status_str = "slow down";
  }

  void SetStayStill() {
    status = eFollowingBehaviorStatus::STAY_STILL;
    status_str = "stay still";
  }

  void SetEndTask() {
    status = eFollowingBehaviorStatus::ENDTASK;
    status_str = "end task";
  }
} FollowingBehaviorStatus;


typedef struct FollowingBehaviorFSMInfo {
  FollowingBehaviorStatus behavior_status;
  BehaviorBusyStatus behavior_busy_status;
  BehaviorExecuteStatus forward_following_status;
  BehaviorExecuteStatus slow_down_status;
  StatisticsTimes time_for_dp_avoid;
  StatisticsTimes time_for_hybrid_atar;
  StatisticsTimes time_for_forward_frenet;
  StatisticsTimes time_for_slowdown_frenet;
  StatisticsTimes time_for_staystill_frenet;
  StatisticsTimes staystill_time;
  StatisticsTimes time_for_sehs;
  BehaviorExecuteStatus astar_result_status;

  double last_xg;
  double last_yg; 

  bool suspend_refpath_generation;
  int suspend_refpath_generation_interval;//loop num
  bool manual_trigger_refpath_generation;

  FollowingBehaviorFSMInfo() {
    Reset();
  }

  void Reset() {
    behavior_status.Reset();
    behavior_busy_status.Reset();
    last_xg = 0.0;
    last_yg = 0.0;
    time_for_dp_avoid.SetStatisticsTimes(20);
    time_for_hybrid_atar.SetStatisticsTimes(20);
    staystill_time.SetStatisticsTimes(1200);//2min
    time_for_sehs.SetStatisticsTimes(20);
    time_for_forward_frenet.SetStatisticsTimes(20);
    time_for_slowdown_frenet.SetStatisticsTimes(20);
    time_for_staystill_frenet.SetStatisticsTimes(20);
    suspend_refpath_generation = false;
    manual_trigger_refpath_generation = false;
    suspend_refpath_generation_interval = 20;
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
  bool IsNeedCallForwardFrenetAvoid() {
    return time_for_forward_frenet.IsTimesUp();
  }
  bool IsNeedCallSlowdownFrenetAvoid() {
    return time_for_slowdown_frenet.IsTimesUp();
  }
  bool IsNeedCallStaystillFrenetAvoid() {
    return time_for_staystill_frenet.IsTimesUp();
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
} FollowingBehaviorFSMInfo;

typedef struct FollowingBehaviorFSMParam {
  FollowingBehaviorFSMInfo behavior_fsm_info;
  VehicleStatus vehicle_status;
  // VehicleDriveStatus drive_status;
  UnstructDynamicInfo dynamic_info;
  UnstructSemanticInfo semantic_info;
  LocalizationData localization_data;
  // MapEngineData mapengine_data;
  BehaviorConfig behavior_config;
  SpeedplanInfo speedplan_info;
  ExceptionAnalyseResult exception_analyse_result;

  std::shared_ptr<AlgorithmBase> algorithm_ptr;

  PathData last_result_path;
  PathData result_path;
  PathData calculate_path;
  Paths paths;

  DebugPlanningMsg debug_planning_msg;

  bool is_release_set;

  FollowingBehaviorFSMParam()
  {
    is_release_set = false;
  }

} FollowingBehaviorFSMParam;

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_BEHAVIOR_TYPEDEF_H_
