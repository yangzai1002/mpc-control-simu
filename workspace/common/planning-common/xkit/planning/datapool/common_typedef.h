#ifndef DATAPOOL_INCLUDE_COMMON_TYPEDEF_H_
#define DATAPOOL_INCLUDE_COMMON_TYPEDEF_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <cmath>
#include <limits>
#include <mutex>
#include <thread>
#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/task_input.h"
#include "xkit/planning/datapool/vehicle_status.h"
#include "xkit/planning/datapool/task_typedef.h"
#include "xkit/planning/toolbox/geometry/point2D.h"
#include "xkit/planning/datapool/cognition_typedef.h"

#include "xkit/planning/datapool/arrive_behavior_typedef.h"
#include "xkit/planning/datapool/follow_behavior_typedef.h"
#include "xkit/planning/datapool/track_behavior_typedef.h"

#include "xkit/planning/datapool/speedplan_typedef.h"
#include "xkit/planning/datapool/common_config.h"
#include "PlanningStatus.h"
// #include "common/base/time/include/node_time.h"
// #include "common/geotool/geotool.h"
// #include "src/algorithm/methods/astar/interface/include/astar_avoid.h"
// #include "src/algorithm/methods/track_algorithm/astar/interface/include/astar_avoid.h"
// #include "src/algorithm/methods/astar/interface/include/astar_adjust.h"
// #include "src/execution/cognition/cognition_base/include/cognition_base.h"
// #include "src/execution/behaviorplan/behavior_base/behavior_context_base.h"
// #include "src/execution/cognition/dummy_cognition/include/dummy_cognition.h"
// #include "src/execution/cognition/follow_cognition/include/follow_cognition.h"
// #include "src/execution/cognition/track_cognition/include/track_cognition.h"
// #include "src/execution/cognition/arrive_cognition/include/arrive_cognition.h"
// #include "src/execution/speedplan/speedplan_base/include/speedplan_header.h"
// #include "src/execution/speedplan/speedplan_base/include/speedplan_base.h"
// #include "common/mapcheck/collisionmodel/collision_checking/collisionchecking.h"
#include "control_flags.h"
#include "error_code.h"



namespace avos {
namespace planning {


typedef struct PlanningMainData {
  /***********INPUT***************/
  Perception perception;  // input from  perception
  PerceptionFusionBuffer perception_fusion_buffer;
  PerceptionSemanticMapBuffer perception_semantic_map_buffer;
  PerceptionTargetObjBuffer perception_target_obj_buffer;
  // PerceptionBuffer perception_buffer;
  // PerceptionBuffer perception_target_buffer;
  Localization localization;
  LocalizationBuffer localization_buffer;

  LocalizationDr localization_dr;
  LocalizationDrBuffer localization_dr_buffer;
  TaskContent task_content;  // input from  app
  FaultDecisionCmd fault_decision_cmd;
  // VehicleDriveStatus drive_status; // input from  task topic
  VehicleStatus vehicle_status;  // input from  can topic
  // PredictionData prediction_data;  // input from  prediction
  /***********INPUT***************/
  StopType stop_reason;
  /***********OUTPUT***************/
  //任务是否执行成功
  eTaskExeResult task_exe_result;
  PathData motion_path; // 交给motion来定义数据结构
  DebugPlanningMsg debug_planning_msg;
  // PredictionData prediction_bk_data;
  // path
  ModuleFault module_fault_;
  /***********OUTPUT***************/

  //对所有故障进行处理信息
  FaultHandleInfo fault_handle_info;


  //规划内部向外发生的故障码及等级
  std::vector<std::string> planning_fault_level_code;
  // std::vector<std::string> planning_fault_codes;
  std::vector<ErrorCode> planning_fault_codes;

  ConfigInfo config_info;
  TaskFSMInfo task_fsm_info;
  // int slowdown_dp_failed_count = 0;
  //认知
  FollowCognitionInfo follow_cognition_info;
  ArriveCognitionInfo arrive_cognition_info;
  TrackCognitionInfo track_cognition_info;
  ArrivingBehaviorFSMInfo arriving_behavior_fsm_info;
  FollowingBehaviorFSMInfo following_behavior_fsm_info;
  TrackingBehaviorFSMInfo tracking_behavior_fsm_info;
  SpeedplanInfo speedplan_info;
  SourceDataCheck data_check_info;
  PlanningStatus planning_status;
  int avoid_flag = 0;
  bool pedestrian_avoid = false;// 行人避障标志 PEDESTRIAN
  bool receive_per_flag = false;
  bool obs_near_path = false;
  double rail_length_offset = 0;
  double refpath_fence_offset = 0.0;
  double to_app_rail_length_offset = 0.0;
  double dis_to_lane = 0.0;
  int follow_refpath_index_diff = 0;
  bool run_real_time = false;
  bool weak_localization = false;
  bool track_enable_dr = false;
  std::vector<DebugCostMsg> dwa_costs;
  std::vector<geometry::Site> arrive_costs;
  std::vector<geometry::Site> track_costs;
  std::vector<geometry::Site> dwa_sample_points;

  std::mutex m_arrive_tentacle_mutex;
  std::mutex m_arrive_voronoi_mutex;
  std::mutex m_track_tentacle_mutex;
  std::mutex m_track_voronoi_mutex;
  std::mutex m_guide_point_mutex;
  std::mutex m_cloud_points_mutex;
  std::mutex m_final_cloud_points_mutex;
  std::mutex m_dr_cloud_points_mutex;
  std::mutex m_dr_final_cloud_points_mutex;
  std::mutex m_voronoi_diagram_info_mutex;
  std::mutex m_casadi_bound_mutex;
  std::mutex m_casadi_obstacles_mutex;
  std::mutex m_casadi_paths_mutex;
  std::mutex m_casadi_key_pose_mutex;

  std::vector<std::vector<geometry::Site>> tentacle_paths;

  std::vector<geometry::Site> casadi_bound;
  std::vector<std::vector<geometry::Site>> casadi_obstacles;
  std::vector<std::vector<geometry::Site>> casadi_paths;
  std::vector<std::vector<geometry::Site>> casadi_key_pose;
  std::vector<double> voronoi_diagram;
  std::mutex m_voronoi_mutex;
  
  VoronoiDiagramInfo voronoi_diagram_info;
  std::mutex m_semantic_path_mutex;
    std::mutex m_smooth_semantic_path_mutex;
    std::mutex m_all_semantic_path_mutex;
    std::mutex m_semantic_edge_down_sample_mutex;
    std::mutex m_semantic_edge_mutex;
    std::mutex m_last_passable_cell_mutex;

  std::vector<geometry::Site> semantic_path;
  std::vector<geometry::Site> smooth_semantic_path;
  std::vector<geometry::Point2D> semantic_edge_down_sample;
  std::vector<geometry::Point2D> semantic_edge;
  std::vector<std::vector<geometry::Site>> all_semantic_path;
  geometry::Site last_passable_cell;
  
  // std::shared_ptr<BusinessBase> business_ptr;
  // std::shared_ptr<BusinessBase> last_business_ptr;
  // std::shared_ptr<AlgorithmBase> algorithm_ptr;
  // std::shared_ptr<avos::planning::CognitionBase> cognition_ptr_;
  // std::shared_ptr<BehaviorContextBase> behavior_ptr_;
  // std::shared_ptr<avos::motionplan::SpeedplanBase> speed_plan_ptr_;

  std::string business_str;
  std::string last_business_str;
  std::string algorithm_str;
  std::string cognition_str;
  std::string behavior_str;
  std::string speed_plan_str;

  //提路模块计算的路径　前向路径，反向路径，前向纯跟踪路径(避障时需要)，前向原始路径
  Paths paths;
  //从当前前向路劲提取出来的路径
  PathData result_path;
  PathData dr_path;
  //当前路径的纯跟踪路径
  PathData result_pursuit_path;
  //计算的避障路径
  PathData calculate_path;
  //task input path
  PathData input_refpath;
  //当前计算的线程
  std::vector<ComputeThreadType> compute_thread_type;
  //TODO 不清楚
  // CollisionChecking unstcognition_model;
  ControlFlags control_flags;

  int turning_flag;
  //------------------------remote_ctrl-------------------//
  bool remote_ctl_flag = false;
  bool last_remote_ctl_flag = false;
  double remote_steering_angle_ = 0;
  int remote_gear = 0;
  bool is_idle_remote_ = false;
  int remote_ctl_mode = 0; //0: 关闭, 1: 直接控制, 2: 规划控制-预警不停障, 3: 规划控制-预警停障, 4: 规划控制-避障
  bool remote_ctl_stop = false;


  int turning;
  // int get_avoid_result;
  double start_xg;
  double start_yg;
  //引导点
  geometry::Site guide_point;
  //调头目标点
  // geometry::Site turn_around_target;
  //停车帧数
  // int stop_frame = 0;
  //准备匹配的点
  std::vector<geometry::Site> cloud_points;
  std::vector<geometry::Site> final_cloud_points;
  //dr
  std::vector<geometry::Site> dr_cloud_points;
  std::vector<geometry::Site> dr_final_cloud_points;
  bool turn_around = false;
  bool PathHeadingCheck = true;
  double kTrackErrorUp;
  // bool path_has_been_extended = false;
  double speed_limit;

  //脱困
  int stop_duration;
  int stay_still_for_a_long_time;
  bool except_shift_position_cause_stop = false;
  bool get_out_of_trouble_now = false;
  bool isfree = false;

  double dp_ignore_collision_distance;

    //turn_around temp
  bool turn_around_path_get_ready = true;
  int turn_around_path_interval_count = 100;

  // SingleTrajectory trajectory_data;
  // TaskContent task_content;
  // CarStatus car_status;
  // TaskFSMInfo task_fsm_info;

  // bool is_loc_valid;  // for test

  // std::shared_ptr<BusinessBase> business_ptr;
  // std::shared_ptr<BusinessBase> last_business_ptr;

  PlanningMainData() {
    // is_loc_valid = false;    
    // get_avoid_result = 0;
    planning_fault_level_code.clear();
    planning_fault_codes.clear();
    compute_thread_type.resize(30);
  }

  void ResetAll() {

  }
  // void SetVoronoiDiagramInfo(const VoronoiDiagramInfo &_voronoi_diagram_info)
  // {
  //     std::unique_lock<std::mutex> locker(m_voronoi_mutex);
  //     voronoi_diagram_info = _voronoi_diagram_info;
  // }
  // void GetVoronoiDiagramInfo(VoronoiDiagramInfo &_voronoi_diagram_info)
  // {
  //     std::unique_lock<std::mutex> locker(m_voronoi_mutex);
  //     _voronoi_diagram_info = voronoi_diagram_info;
  // }

    void SetLastPassableCell(const geometry::Site &_last_passable_cell)
  {
      std::unique_lock<std::mutex> locker(m_last_passable_cell_mutex);
      last_passable_cell = _last_passable_cell;
  }
  void GetLastPassableCell( geometry::Site &_last_passable_cell)
  {
      std::unique_lock<std::mutex> locker(m_last_passable_cell_mutex);
      _last_passable_cell = last_passable_cell;
  }

  void SetGuidePoint(const geometry::Site &_guide_point)
  {
      std::unique_lock<std::mutex> locker(m_guide_point_mutex);
      guide_point = _guide_point;
  }
  void GetGuidePoint( geometry::Site &_guide_point)
  {
      std::unique_lock<std::mutex> locker(m_guide_point_mutex);
      _guide_point = guide_point;
  }

    void SetSemanticEdgeDownSample(const std::vector<geometry::Point2D> &_semantic_edge_down_sample)
  {
      std::unique_lock<std::mutex> locker(m_semantic_edge_down_sample_mutex);
      semantic_edge_down_sample = _semantic_edge_down_sample;
  }
  void GetSemanticEdgeDownSample( std::vector<geometry::Point2D> &_semantic_edge_down_sample)
  {
      std::unique_lock<std::mutex> locker(m_semantic_edge_down_sample_mutex);
      _semantic_edge_down_sample = semantic_edge_down_sample;
  }

      void SetSemanticEdge(const std::vector<geometry::Point2D> &_semantic_edge)
  {
      std::unique_lock<std::mutex> locker(m_semantic_edge_mutex);
      semantic_edge = _semantic_edge;
  }
  void GetSemanticEdge( std::vector<geometry::Point2D> &_semantic_edge)
  {
      std::unique_lock<std::mutex> locker(m_semantic_edge_mutex);
      _semantic_edge = semantic_edge;
  }

    void SetAllSemanticPath(const std::vector<std::vector<geometry::Site>> &_all_semantic_path)
  {
      std::unique_lock<std::mutex> locker(m_all_semantic_path_mutex);
      all_semantic_path = _all_semantic_path;
  }
  void GetAllSemanticPath( std::vector<std::vector<geometry::Site>> &_all_semantic_path)
  {
      std::unique_lock<std::mutex> locker(m_all_semantic_path_mutex);
      _all_semantic_path = all_semantic_path;
  }

    void SetSemanticPath(const std::vector<geometry::Site> & _semantic_path)
  {
      std::unique_lock<std::mutex> locker(m_semantic_path_mutex);
      semantic_path = _semantic_path;
  }
  void GetSemanticPath( std::vector<geometry::Site> &_semantic_path)
  {
      std::unique_lock<std::mutex> locker(m_semantic_path_mutex);
      _semantic_path = semantic_path;
  }
  void SetSmoothSemanticPath(const std::vector<geometry::Site> & _smooth_semantic_path)
  {
      std::unique_lock<std::mutex> locker(m_smooth_semantic_path_mutex);
      smooth_semantic_path = _smooth_semantic_path;
  }
  void GetSmoothSemanticPath( std::vector<geometry::Site> &_smooth_semantic_path)
  {
      std::unique_lock<std::mutex> locker(m_smooth_semantic_path_mutex);
      _smooth_semantic_path = smooth_semantic_path;
  }
  

    void SetCloudPoints(const std::vector<geometry::Site> &_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_cloud_points_mutex);
      cloud_points = _cloud_points;
  }
  void GetCloudPoints( std::vector<geometry::Site> &_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_cloud_points_mutex);
      _cloud_points = cloud_points;
  }

      void SetFinalCloudPoints(const std::vector<geometry::Site> &_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_final_cloud_points_mutex);
      final_cloud_points = _cloud_points;
  }
  void GetFinalCloudPoints( std::vector<geometry::Site> &_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_final_cloud_points_mutex);
      _cloud_points = final_cloud_points;
  }

        void SetDrCloudPoints(const std::vector<geometry::Site> &_dr_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_dr_cloud_points_mutex);
      dr_cloud_points = _dr_cloud_points;
  }
  void GetDrCloudPoints( std::vector<geometry::Site> &_dr_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_dr_cloud_points_mutex);
      _dr_cloud_points = dr_cloud_points;
  }

        void SetDrFinalCloudPoints(const std::vector<geometry::Site> &_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_dr_final_cloud_points_mutex);
      dr_final_cloud_points = _cloud_points;
  }
  void GetDrFinalCloudPoints( std::vector<geometry::Site> &_cloud_points)
  {
    //   std::unique_lock<std::mutex> locker(m_dr_final_cloud_points_mutex);
      _cloud_points = dr_final_cloud_points;
  }

  void SetTentaclePath(const std::vector<std::vector<geometry::Site>> &arrive_tentacle)
  {
      std::unique_lock<std::mutex> locker(m_arrive_tentacle_mutex);
      tentacle_paths = arrive_tentacle;
  }
  void GetTentaclePath(std::vector<std::vector<geometry::Site>> &arrive_tentacle)
  {
      std::unique_lock<std::mutex> locker(m_arrive_tentacle_mutex);
      arrive_tentacle = tentacle_paths;
  }


  void SetCasadiBound(const std::vector<geometry::Site> &_casadi_bound)
  {
      std::unique_lock<std::mutex> locker(m_casadi_bound_mutex);
      casadi_bound = _casadi_bound;
  }
  void GetCasadiBound( std::vector<geometry::Site> &_casadi_bound)
  {
      std::unique_lock<std::mutex> locker(m_casadi_bound_mutex);
      _casadi_bound = casadi_bound;
  }
  void SetCasadiObstacles(const std::vector<std::vector<geometry::Site>> &_casadi_obstacles)
  {
      std::unique_lock<std::mutex> locker(m_casadi_obstacles_mutex);
      casadi_obstacles = _casadi_obstacles;
  }
  void GetCasadiObstacles(std::vector<std::vector<geometry::Site>> &_casadi_obstacles)
  {
      std::unique_lock<std::mutex> locker(m_casadi_obstacles_mutex);
      _casadi_obstacles = casadi_obstacles;
  }
  void SetCasadiPaths(const std::vector<std::vector<geometry::Site>> &_casadi_paths)
  {
      std::unique_lock<std::mutex> locker(m_casadi_paths_mutex);
      casadi_paths = _casadi_paths;
  }
  void GetCasadiPaths(std::vector<std::vector<geometry::Site>> &_casadi_paths)
  {
      std::unique_lock<std::mutex> locker(m_casadi_paths_mutex);
      _casadi_paths = casadi_paths;
  }

  void SetCasadiKeyPose(const std::vector<std::vector<geometry::Site>> &_casadi_key_pose)
  {
      std::unique_lock<std::mutex> locker(m_casadi_key_pose_mutex);
      casadi_key_pose = _casadi_key_pose;
  }
  void GetCasadiKeyPose(std::vector<std::vector<geometry::Site>> &_casadi_key_pose)
  {
      std::unique_lock<std::mutex> locker(m_casadi_key_pose_mutex);
      _casadi_key_pose = casadi_key_pose;
  }

  void SetArriveVoronoi(const std::vector<double> &arrive_voronoi)
  {
      std::unique_lock<std::mutex> locker(m_arrive_voronoi_mutex);
      voronoi_diagram = arrive_voronoi;
  }
  void GetArriveVoronoi(std::vector<double> &arrive_voronoi)
  {
      std::unique_lock<std::mutex> locker(m_arrive_voronoi_mutex);
      arrive_voronoi = voronoi_diagram;
  }  
  void SetTrackVoronoi(const std::vector<double> &arrive_voronoi)
  {
      std::unique_lock<std::mutex> locker(m_track_voronoi_mutex);
      voronoi_diagram = arrive_voronoi;
  }
  void GetTrackVoronoi(std::vector<double> &track_voronoi)
  {
      std::unique_lock<std::mutex> locker(m_track_voronoi_mutex);
      track_voronoi = voronoi_diagram;
  } 
  void SetVoronoiDiagramInfo(const VoronoiDiagramInfo &_voronoi_diagram_info)
  {
      std::unique_lock<std::mutex> locker(m_voronoi_diagram_info_mutex);
      voronoi_diagram_info = _voronoi_diagram_info;
  }
  void GetVoronoiDiagramInfo( VoronoiDiagramInfo &_voronoi_diagram_info)
  {
      std::unique_lock<std::mutex> locker(m_voronoi_diagram_info_mutex);
      _voronoi_diagram_info = voronoi_diagram_info;
  }  

} PlanningMainData;

typedef struct PlanningSwapComputeData {
  eMissionType mission_type;
  LocalizationData localization_data;
  LocalizationDataDr localization_dr_data;
  Perception perception;
  Paths paths;
  FollowCognitionInfo follow_cognition_info;
  ArriveCognitionInfo arrive_cognition_info;
  TrackCognitionInfo track_cognition_info;
  PathData result_path;
  PathData dr_path;
  PathData casadi_optimize_path;
  std::vector<geometry::Site> casadi_bound;
  std::vector<std::vector<geometry::Site>> casadi_obstacles;
  std::vector<std::vector<geometry::Site>> casadi_paths; 
  std::vector<std::vector<geometry::Site>> casadi_key_pose; 
  CarModel car_model;
  // CollisionChecking unstcognition_model;
  ConfigInfo config_info;
  bool is_final_adjust;
  VehicleStatus vehicle_status; 
  DebugDWAMsg DWA_message;
  DebugASTARMsg ASTAR_message;
  // std::shared_ptr<track_algorithm::astar_interface::AstarAvoid> astar_avoid_ptr;
  // std::shared_ptr<avos::astar_interface::AstarAdjust> astar_adjust_ptr;
  geometry::SiteVec goals_status_vec;
  int sehs_rtvalue;
  geometry::Site guide_point;
  std::vector<geometry::Site> semantic_path;
  std::vector<geometry::Site> smooth_semantic_path;
  TrackingBehaviorStatus current_behavior_status;
  int turning_flag;

  //------------------------remote_ctrl-------------------//
  double remote_steering_angle_ = 0;
  double remote_acc_ = 0;
  double remote_gear = 0;


  std::vector<InnerPointInfo> result_refpath, input_refpath;
  PlanningSwapComputeData() {
    // astar_avoid_ptr = std::make_shared<track_algorithm::astar_interface::AstarAvoid>();
    // astar_adjust_ptr = std::make_shared<avos::astar_interface::AstarAdjust>();
    is_final_adjust = false;
    sehs_rtvalue = -1;
    result_refpath.clear();
    input_refpath.clear();
  }
} PlanningSwapComputeData;

typedef struct PlanningSwapCognitionData {
  int bak_type;
} PlanningSwapCognitionData;
typedef struct PlanningSwapBehaviorData {
  int bak_type;
} PlanningSwapBehaviorData;
typedef struct PlanningSwapPathplanData {
  int bak_type;
} PlanningSwapPathplanData;

} // namespace planning 
} // namespace avos

#endif // DATAPOOL_INCLUDE_COMMON_TYPEDEF_H_
