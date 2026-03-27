#ifndef DATAPOOL_INCLUDE_DEBUG_MESSAGE_H_
#define DATAPOOL_INCLUDE_DEBUG_MESSAGE_H_

#include <iostream>
#include <string>

namespace avos {
namespace planning {

typedef struct DebugStateMsg {
  std::string str;
  int value;
  void SetDebugStateMsg(std::string _str, int _value = 0) {
    str = str + _str + " ";
    value = _value;
  }
  void Reset() {
    str = "";
    value = 0;
  }
} DebugStateMsg;

typedef struct DebugSemanticInfo {
  int pursuit_index;
  int obs_id;
  int obs_type;
  double obs_x;
  double obs_y;
  double obs_z;
  double obs_height;
  double path_x;
  double path_y;
  double path_angle;
  double path_index;
  double path_length;
  std::string collid_count;
  void Reset() {
    pursuit_index = -1;
    obs_id = -1;
    obs_type = -1;
    obs_x = 0.0;
    obs_y = 0.0;
    obs_z = 0.0;
    obs_height = 2.0;
    path_x = 0.0;
    path_y = 0.0;
    path_angle = 0.0;
    path_index = 999;
    path_length = 999.0;
    collid_count = " ";
  }
} DebugSemanticInfo;

typedef struct DebugUnsCollidMsg {
  DebugSemanticInfo front_collision_info; 
  DebugSemanticInfo result_collision_info;
  double slow_down_dis;
} DebugUnsCollidMsg;

typedef struct DebugRefpathMsg
{
  int local_path_index;
  int cut_path_index;
  bool is_avoid;
  bool cur_reverse;
  int cur_nearest_index;
  int cur_nearest_origin_index;
  DebugRefpathMsg() {
    Reset();
  }
  void Reset() {
    local_path_index = 0;
    cut_path_index = 0;
    is_avoid = false;
    cur_reverse = false;
    cur_nearest_index = 0;
    cur_nearest_origin_index = 0;
  }
} DebugRefpathMsg;

typedef struct DebugCostMsg
{
  double parent_x;
  double parent_y;
  double current_x;
  double current_y;
  double cost_sum;
  double length_cost;
  double balance_cost;
  double last_cost;
  double center_cost;
  double no_circular_penalty;
  double curvature_cost;
  double middle_cost;
  double approximate_cost;
  DebugCostMsg() {
    Reset();
  }
  void Reset() {
    parent_x = 0;
    parent_y = 0;
    current_x = 0;
    current_y = 0;
    cost_sum = 0;
    length_cost = 0;
    balance_cost = 0;
    last_cost = 0;
    center_cost = 0;
    no_circular_penalty = 0;
    curvature_cost = 0;
    middle_cost = 0;
    approximate_cost = 0;
  }
} DebugCostMsg;

typedef struct DebugDWAMsg
{
  std::string DWA_info;
  std::string lat_goals_info;
  std::string lon_goals_info;
  float search_path_size;
  float link_path_size;
  float search_model_halfwheel_left;
  float search_model_halfwheel_right;
  bool if_force_avoid;
  bool if_dwa_success;
  bool if_lateral_goal_reached;
  bool if_longitudinal_goal_reached;
  bool if_link_success;
  int lateral_goals_num;
  int longitudinal_goals_num;
  int refpath_size;
  int dwapath_size;
  int linkpath_size;
  int allpath_size;

  void Reset()
  {
    DWA_info = "No news...";
    lat_goals_info = "Not assigned...";
    lon_goals_info = "Not assigned...";
    search_path_size = -1.0;
    link_path_size = -1.0;
    search_model_halfwheel_left = -1.0;
    search_model_halfwheel_right = -1.0;
    if_force_avoid = false;
    if_dwa_success = false;
    if_lateral_goal_reached = false;
    if_longitudinal_goal_reached = false;
    if_link_success = false;
    lateral_goals_num = -1;
    longitudinal_goals_num = -1;
    refpath_size = -1;
    dwapath_size = -1;
    linkpath_size = -1;
    allpath_size = -1;
  }

} DebugDWAMsg;

typedef struct DebugASTARMsg {
  std::string goalinfo;
  std::string startinfo;
  std::string type;
  std::string MTR_info;
  std::string allowance_info;
  std::string stayinfo;
  bool goalvalid;
  bool startvalid;
  std::vector<int> loop;
  int RS_size;
  int SEHS_size;
  int allpath_size;

  void Reset()
  {
    goalinfo = "No news...";
    startinfo = "No news...";
    type = "No news...";
    MTR_info = "No news...";
    allowance_info = "No news...";
    stayinfo = "";
    goalvalid = false;
    startvalid = false;
    loop.clear();
    RS_size = -1;
    SEHS_size = -1;
    allpath_size = -1;
  }

} DebugASTARMsg;

typedef struct DebugDriveModeMsg {
  std::string drive_mode;
  int obj_id;
  float obj_dis_rel;
  float slow_down_dis;
  float guided_speed;
  float velocity_limit;
  std::string info;

  void Reset()
  {
    drive_mode = "";
    obj_id = -1;
    obj_dis_rel = 0.0;
    slow_down_dis = 0.0;
    guided_speed = 0.0;
    velocity_limit = -1.0;
    info = "";
  }

} DebugDriveModeMsg;


typedef struct MotionPlanDebugMsg {
  float first_point_v;
  float ego_speed;
  float cruise_speed_max;
  float first_point_kappa;
  float first_point_a;
  int following_object_id;
  float following_object_v;
  float following_object_dis;
  bool is_new_path;
  bool is_fallback_trajectory;
  bool is_replan;
  int replan_reason;
  int yield_object_id;
  float yield_object_v;
  float yield_object_current_s;
  float yield_object_collision_min_s;
  float yield_object_collision_min_t;

  MotionPlanDebugMsg() {
    Reset();
  }

  void Reset()
  {
    first_point_v = -1.0;
    ego_speed = -1.0;
    cruise_speed_max = -1.0;
    first_point_kappa = 0.0;
    first_point_a = 0.0;
    following_object_id = -1;
    following_object_v = -1.0;
    following_object_dis = -1.0;
    is_new_path = false;
    is_fallback_trajectory = false;
    is_replan = false;
    replan_reason = 0;
    yield_object_id = -1;
    yield_object_v = -1.0;
    yield_object_current_s = -1.0;
    yield_object_collision_min_s = -1.0;
    yield_object_collision_min_t = -1.0;
  }

} MotionPlanDebugMsg;

typedef struct DebugPlanningMsg {
  DebugStateMsg main_stream_msg;
  DebugStateMsg task_fsm_condition;
  DebugStateMsg task_fsm_state;
  DebugStateMsg cognition_msg;
  DebugStateMsg business_msg;
  DebugStateMsg pathplan_msg;
  DebugStateMsg behavior_fsm_condition;
  DebugStateMsg behavior_fsm_state;
  DebugStateMsg thread_state;

  DebugUnsCollidMsg uns_collid_msg;
  DebugRefpathMsg refpath_msg;
  DebugDWAMsg DWA_message;
  DebugASTARMsg ASTAR_message;
  DebugDriveModeMsg drive_mode_message;
  MotionPlanDebugMsg motionplan_debug;

  DebugPlanningMsg() {
    Reset();
  }
  void Reset() {
    main_stream_msg.Reset();
    task_fsm_condition.Reset();
    task_fsm_state.Reset();
    cognition_msg.Reset();
    business_msg.Reset();
    pathplan_msg.Reset();
    behavior_fsm_condition.Reset();
    behavior_fsm_state.Reset();
    thread_state.Reset();
    drive_mode_message.Reset();
    motionplan_debug.Reset();
  }
  // task fsm condition
  void SetTaskIsNewEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskIsNewEvent " + info + "\n";
    task_fsm_condition.value = 1;
  }
  void SetTaskExecutableEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskExecutableEvent " + info + "\n";
    task_fsm_condition.value = 1;
  }
  void SetTaskUnexecutableEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskUnexecutableEvent " + info + "\n";
    task_fsm_condition.value = 2;
  }
  void SetTaskExecuteOverEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskExecuteOverEvent " + info + "\n";
    task_fsm_condition.value = 3;
  }
  void SetTaskExecuteFaultEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskExecuteFaultEvent " + info + "\n";
    task_fsm_condition.value = 4;
  }
  void SetTaskOverEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskOverEvent " + info + "\n";
    task_fsm_condition.value = 5;
  }
  void SetTaskAbortedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskAbortedEvent " + info + "\n";
    task_fsm_condition.value = 6;
  }
  void SetTaskSuspendedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskSuspendedEvent " + info + "\n";
    task_fsm_condition.value = 7;
  }
  void SetTaskContinuedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetTaskContinuedEvent " + info + "\n";
    task_fsm_condition.value = 8;
  }
  void SetSyetemExceptionEvent(const std::string info = "") {
    task_fsm_condition.str = "SetSyetemExceptionEvent " + info + "\n";
    task_fsm_condition.value = 9;
  }
  void SetSyetemNormalEvent(const std::string info = "") {
    task_fsm_condition.str = "SetSyetemNormalEvent " + info + "\n";
    task_fsm_condition.value = 10;
  }
  void SetBtnEmergencyPressedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetBtnEmergencyPressedEvent " + info + "\n";
    task_fsm_condition.value = 11;
  }
  void SetBtnEmergencyUnpressedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetBtnEmergencyUnpressedEvent " + info + "\n";
    task_fsm_condition.value = 12;
  }
  void SetRemoteCtrlConnectedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetRemoteCtrlConnectedEvent " + info + "\n";
    task_fsm_condition.value = 13;
  }
  void SetRemoteCtrlUnconnectedEvent(const std::string info = "") {
    task_fsm_condition.str = "SetRemoteCtrlUnconnectedEvent " + info + "\n";
    task_fsm_condition.value = 14;
  }
  void SetEndTaskHandleOverEvent(const std::string info = "") {
    task_fsm_condition.str = "SetEndTaskHandleOverEvent " + info + "\n";
    task_fsm_condition.value = 15;
  }

  // task state func
  void SetIdle2AnalyseTaskFunc() {
    task_fsm_state.str = "Idle2AnalyseTaskFunc";
    task_fsm_state.value = 1;
  }
  void SetIdleTaskFunc() {
    task_fsm_state.str = "IdleTaskFunc";
    task_fsm_state.value = 2;
  }
  void SetAnalyseTaskFunc() {
    task_fsm_state.str = "AnalyseTaskFunc";
    task_fsm_state.value = 3;
  }
  void SetExecuteTaskFunc() {
    task_fsm_state.str = "ExecuteTaskFunc";
    task_fsm_state.value = 4;
  }
  void SetEndTaskFunc() {
    task_fsm_state.str = "EndTaskFunc";
    task_fsm_state.value = 5;
  }
  void SetExceptionalTaskFunc() {
    task_fsm_state.str = "ExceptionalTaskFunc";
    task_fsm_state.value = 6;
  }
  void SetSuspendTaskFunc() {
    task_fsm_state.str = "SuspendTaskFunc";
    task_fsm_state.value = 7;
  }
  void SetEmergencyBtnHandleFunc() {
    task_fsm_state.str = "EmergencyBtnHandleFunc";
    task_fsm_state.value = 8;
  }
  void SetRemoteControlFunc() {
    task_fsm_state.str = "RemoteControlFunc";
    task_fsm_state.value = 9;
  }
  void SetSuspend2AnalyseTaskFunc() {
    task_fsm_state.str = "Suspend2AnalyseTaskFunc";
    task_fsm_state.value = 10;
  }
  void SetRemote2AnalyseTaskFunc() {
    task_fsm_state.str = "Remote2AnalyseTaskFunc";
    task_fsm_state.value = 11;
  }

  // behavior condition
  void SetPathNoCollisionEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "PathNoCollision " + info + "\n";
    behavior_fsm_condition.value = 1;
  }
  void SetCurvatureFaultEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "CurvatureFault " + info + "\n";
    behavior_fsm_condition.value = 2;
  }
  void SetDeviationPathEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "DeviationPath " + info + "\n";
    behavior_fsm_condition.value = 3;
  }
  void SetPathCollisionEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "PathCollision " + info + "\n";
    behavior_fsm_condition.value = 4;
  }
  void SetAebEmergencyEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "AebEmergency " + info + "\n";
    behavior_fsm_condition.value = 5;
  }
  void SetSlowDownFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "SlowDownFinish " + info + "\n";
    behavior_fsm_condition.value = 6;
  }
  void SetNarrowTurnFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "NarrowTurnFinish " + info + "\n";
    behavior_fsm_condition.value = 7;
  }
  void SetNarrowTurnFaultEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "NarrowTurnFault " + info + "\n";
    behavior_fsm_condition.value = 8;
  }
  void SetForceAvoidFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "ForceAvoidFinish " + info + "\n";
    behavior_fsm_condition.value = 9;
  }
  void SetForceAvoidFaultEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "ForceAvoidFault " + info + "\n";
    behavior_fsm_condition.value = 10;
  }
  void SetDWAResultSuccessEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "DWAResultSuccess " + info + "\n";
    behavior_fsm_condition.value = 11;
  }
  void SetHybridAstarSuccessEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "HybridAstarSuccess " + info + "\n";
    behavior_fsm_condition.value = 12;
  }
  void SetForwardStayTimeOutEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "ForwardStayTimeOut " + info + "\n";
    behavior_fsm_condition.value = 13;
  }
  void SetBackStayTimeOutEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "BackStayTimeOut " + info + "\n";
    behavior_fsm_condition.value = 14;
  }
  void SetBackFollowingFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "BackFollowingFinish " + info + "\n";
    behavior_fsm_condition.value = 15;
  }
  void SetForwardAvoidFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "ForwardAvoidFinish " + info + "\n";
    behavior_fsm_condition.value = 16;
  }
  void SetBackAvoidFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "BackAvoidFinish " + info + "\n";
    behavior_fsm_condition.value = 17;
  }
  void SetYieldFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "YieldFinish " + info + "\n";
    behavior_fsm_condition.value = 18;
  }
  void SetBehaviorErrorFinishEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "BehaviorErrorFinish " + info + "\n";
    behavior_fsm_condition.value = 19;
  }
  void SetBehaviorResetEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "BehaviorReset " + info + "\n";
    behavior_fsm_condition.value = 20;
  }
  void SetNoThingToDoEvent(const std::string info = "") {
    behavior_fsm_condition.str = behavior_fsm_condition.str + "NoThingToDo " + info + "\n";
    behavior_fsm_condition.value = 21;
  }

  // behavior state function
  void SetIdleFunc(const std::string &info = "") {
    behavior_fsm_state.str = "IdleFunc:" + info;
    behavior_fsm_state.value = 1;
  }
  void SetForwardFollowingFunc(const std::string &info = "") {
    behavior_fsm_state.str = "ForwardFollowingFunc:" + info;
    behavior_fsm_state.value = 2;
  }
  void SetNarrowTurnFunc(const std::string &info = "") {
    behavior_fsm_state.str = "NarrowTurnFunc:" + info;
    behavior_fsm_state.value = 3;
  }
  void SetForceAvoidFunc(const std::string &info = "") {
    behavior_fsm_state.str = "ForceAvoidFunc:" + info;
    behavior_fsm_state.value = 4;
  }
  void SetSlowDownFunc(const std::string &info = "") {
    behavior_fsm_state.str = "SlowDownFunc:" + info;
    behavior_fsm_state.value = 5;
  }
  void SetBackStayStillFunc(const std::string &info = "") {
    behavior_fsm_state.str = "BackStayStillFunc:" + info;
    behavior_fsm_state.value = 6;
  }
  void SetForwardStayStillFunc(const std::string &info = "") {
    behavior_fsm_state.str = "ForwardStayStillFunc:" + info;
    behavior_fsm_state.value = 7;
  }
  void SetForwardAvoidFunc(const std::string &info = "") {
    behavior_fsm_state.str = "ForwardAvoidFunc:" + info;
    behavior_fsm_state.value = 8;
  }
  void SetBackFollowingFunc(const std::string &info = "") {
    behavior_fsm_state.str = "BackFollowingFunc:" + info;
    behavior_fsm_state.value = 9;
  }
  void SetBackAvoidFunc(const std::string &info = "") {
    behavior_fsm_state.str = "BackAvoidFunc:" + info;
    behavior_fsm_state.value = 10;
  }
  void SetBehaviorErrorFunc(const std::string &info = "") {
    behavior_fsm_state.str = "BehaviorErrorFunc:" + info;
    behavior_fsm_state.value = 11;
  }
  void SetBehaviorEndTaskFunc(const std::string &info = "") {
    behavior_fsm_state.str = "EndTaskFunc:" + info;
    behavior_fsm_state.value = 12;
  }

} DebugPlanningMsg;

} // namespace planning
} // namespace avos

#endif
