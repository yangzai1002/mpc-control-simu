#ifndef DATAPOOL_INCLUDE_ARRIVE_BEHAVIOR_TYPEDEF_H_
#define DATAPOOL_INCLUDE_ARRIVE_BEHAVIOR_TYPEDEF_H_

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

enum class eArrivingBehaviorStatus {
  DEFAULT = 0,
  IDLE,
  FORWARDFOLLOWING,
  SLOW_DOWN,
  STAY_STILL,
  ENDTASK,
  BEHAVIOR_ERROR
};


typedef struct ArrivingBehaviorStatus {
  eArrivingBehaviorStatus status;
  std::string status_str;
  ArrivingBehaviorStatus() {
    status = eArrivingBehaviorStatus::DEFAULT;
    status_str = "default";
  }

  void Reset() {
    status = eArrivingBehaviorStatus::DEFAULT;
    status_str = "default";
  }
  void SetIdle() {
    status = eArrivingBehaviorStatus::IDLE;
    status_str = "behavior idle";
  }

  void SetForwardFollowing() {
    status = eArrivingBehaviorStatus::FORWARDFOLLOWING;
    status_str = "ForwardFollowing";
  }

  void SetSlowDown() {
    status = eArrivingBehaviorStatus::SLOW_DOWN;
    status_str = "slow down";
  }

  void SetStayStill() {
    status = eArrivingBehaviorStatus::STAY_STILL;
    status_str = "stay still";
  }

  void SetEndTask() {
    status = eArrivingBehaviorStatus::ENDTASK;
    status_str = "end task";
  }
} ArrivingBehaviorStatus;


typedef struct ArrivingBehaviorFSMInfo {
  ArrivingBehaviorStatus behavior_status;
  BehaviorBusyStatus behavior_busy_status;
  BehaviorExecuteStatus forward_following_status;
  BehaviorExecuteStatus slow_down_status;
  StatisticsTimes time_for_dp_avoid;
  StatisticsTimes time_for_hybrid_atar;
  StatisticsTimes staystill_time;
  StatisticsTimes time_for_sehs;
  StatisticsTimes time_for_casadi_optimize;
  BehaviorExecuteStatus astar_result_status;

  double last_xg;
  double last_yg; 

  bool suspend_refpath_generation;
  int suspend_refpath_generation_interval;//loop num
  bool manual_trigger_refpath_generation;

  ArrivingBehaviorFSMInfo() {
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
    time_for_casadi_optimize.SetStatisticsTimes(30);
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
  bool IsHybridAstarSuccess() {
    return (astar_result_status.status == eBehaviorExecuteStatus::EXE_SUCCESS);
  }

  bool IsNeedCallCasadiOptimize() {
    return time_for_casadi_optimize.IsTimesUp();
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
} ArrivingBehaviorFSMInfo;

typedef struct ArrivingBehaviorFSMParam {
  ArrivingBehaviorFSMInfo behavior_fsm_info;
  VehicleStatus vehicle_status;
  UnstructDynamicInfo dynamic_info;
  UnstructSemanticInfo semantic_info;
  LocalizationData localization_data;
//   MapEngineData mapengine_data;
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

  ArrivingBehaviorFSMParam()
  {
    is_release_set = false;
  }

} ArrivingBehaviorFSMParam;

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_BEHAVIOR_TYPEDEF_H_
