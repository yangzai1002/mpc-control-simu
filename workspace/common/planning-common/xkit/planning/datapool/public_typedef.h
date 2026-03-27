#ifndef DATAPOOL_INCLUDE_PUBLIC_TYPEDEF_H_
#define DATAPOOL_INCLUDE_PUBLIC_TYPEDEF_H_
#include <map>
#include <list>
#include <queue>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <unordered_map>
#include "ara/adsfi/impl_type_msgpoint3dvec.h"
// #include "node_time.h"
#include "xkit/planning/toolbox/geometry/geoheader.h"
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/geometry/point2D.h"
#include "geotool.h"


namespace avos {
namespace planning {

#define MAINSTREAM_RESUME_TIMES 5
#define TASK_RESUME_TIME  5
#define LOCPOS_JUMP_RESUME_TIMES  5
#define MAPENGINE_RESUME_TIMES  5

#define MAX_STATIC_CELL_SIZE 2000
#define MAX_OBJ_CELL_SIZE 500
#define MAX_OBJ_SIZE 100
#define MAX_LIGHTS_SIZE 50

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;

using CollisionInfoVec = std::vector<std::tuple<int, ara::adsfi::MsgPoint3D>>;

enum ePlanningThread {
  COMPUTE_THREAD_ID = 0,
  ADJUST_THREAD_ID = 1,
  COGNITION_THREAD_ID = 2,
  BEHAVIORPLAN_THREAD_ID = 3,
  PATHPLAN_THREAD_ID = 4,
  SPEEDPLAN_THREAD_ID = 5,
  REFPATH_THREAD_ID = 6,
  DP_PERIODICALLY_THREAD_ID = 7,
  FRENET_PLANNING_THREAD_ID = 8,
  VORONOI_THREAD_ID = 9,
  POSE_ADJUST_THREAD_ID = 10,
  MAIN_VORONOI_THREAD_ID = 11,
  MAIN_DIS_OF_FENCE_T0_CAR_ID = 12,
  DR_THREAD_ID=13,
  TENTACLE_THREAD_ID=14,
  WEAL_LOC_COGNITION_ID=15,
  CASADI_OPTIMIZE_ID=16,
  GET_OUT_OF_TROUBLE_ID = 17,
  TURN_AROUND_ID = 18,
  PASS_THROUGH_ID = 19,
  HYBRID_ASTAR_ID = 20
};

enum class eComputeThreadType {
  DEFAULT = 0,    /*!< default is zero */
  PLANNER_DWA_AVOID,
  PLANNER_DWA_FORCE_AVOID,
  PLANNER_ASTAR_AVOID,
  PLANNER_DP_AVOID,
  PLANNER_DP_PERIODICALLY_AVOID,
  PLANNER_ASTAR_AVOID_FINAL,
  PLANNER_ASTAR_ADJUST,
  PLANNER_SEHS_AVOID,
  PLANNER_FRENET_PLANNING,
  PLANNER_VORONOI_THREAD_ID,
  PLANNER_POSE_ADJUST_THREAD_ID,
  PLANNER_MAIN_VORONOI_THREAD,
  PLANNER_MAIN_DIS_OF_FENCE_T0_CAR,
  PLANNER_DR_THREAD,
  PLANNER_TENTACLE_THREAD,
  PLANNER_WEAL_LOC_COGNITION_THREAD,
  PLANNER_CASADI_OPTIMIZE,
  PLANNER_GET_OUT_OF_TROUBLE,
  PLANNER_TURN_AROUND,
  PLANNER_PASS_THROUGH,
  PLANNER_HYBRID_ASTAR
};

enum class eComputeResult {
  OTHER = -2,
  ERROR = -1,
  SUCCESS = 0,
  BUSY = 1
};

typedef struct ComputeThreadType {
  eComputeThreadType type;
  std::string str_type;
  ComputeThreadType() {
    type = eComputeThreadType::DEFAULT;
    str_type = "DEFAULT";
  }
  void Reset() {
    type = eComputeThreadType::DEFAULT;
    str_type = "DEFAULT";
  }

  void SetPlannerDwaAvoid() {
    type = eComputeThreadType::PLANNER_DWA_AVOID;
    str_type = "dwa avoid";
  }

  void SetPlannerDwaForceAvoid() {
    type = eComputeThreadType::PLANNER_DWA_FORCE_AVOID;
    str_type = "dwa force avoid";
  }

  void SetPlannerAstarAvoid() {
    type = eComputeThreadType::PLANNER_ASTAR_AVOID;
    str_type = "astar avoid";
  }

  void SetPlannerSEHSAvoid() {
    type = eComputeThreadType::PLANNER_SEHS_AVOID;
    str_type = "sehs avoid";
  }

  void SetPlannerDPAvoid() {
    type = eComputeThreadType::PLANNER_DP_AVOID;
    str_type = "DP avoid";
  }

  void SetPlannerDPPeriodicallyAvoid() {
    type = eComputeThreadType::PLANNER_DP_PERIODICALLY_AVOID;
    str_type = "DP Periodically avoid";
  }

  void SetPlannerHybridAstarPoseAdjust() {
    type = eComputeThreadType::PLANNER_POSE_ADJUST_THREAD_ID;
    str_type = "Hybrid A star pose adjust";
  }

  void SetPlannerGetOutOfTrouble() {
    type = eComputeThreadType::PLANNER_GET_OUT_OF_TROUBLE;
    str_type = "Get out of trouble";
  }

  void SetPlannerVoronoiDiagram() {
    type = eComputeThreadType::PLANNER_VORONOI_THREAD_ID;
    str_type = "Voronoi diagram generate";
  }  

  void SetPlannerAstarAvoidFinal() {
    type = eComputeThreadType::PLANNER_ASTAR_AVOID_FINAL;
    str_type = "astar avoid final";
  }

  void SetPlannerAstarAdjust() {
    type = eComputeThreadType::PLANNER_ASTAR_ADJUST;
    str_type = "astar adjust";
  }

  void SetPlannerFrenetAvoid() {
    type = eComputeThreadType::PLANNER_FRENET_PLANNING;
    str_type = "Frenet path planning avoid";
  }

  void SetPlannerDRAvoid() {
    type = eComputeThreadType::PLANNER_DR_THREAD;
    str_type = "Frenet path planning avoid";
  }
  
  void SetPlannerTentacleGenerateAvoid() {
    type = eComputeThreadType::PLANNER_TENTACLE_THREAD;
    str_type = "Tentacle path planning avoid";
  }
  void SetVoronoiProcess() {
    type = eComputeThreadType::PLANNER_MAIN_VORONOI_THREAD;
    str_type = "main voronoi";
  }

    void SetDisOfFence() {
    type = eComputeThreadType::PLANNER_MAIN_DIS_OF_FENCE_T0_CAR;
    str_type = "main dis of fence";
  }

  void SetWeakLocCognition() {
    type = eComputeThreadType::PLANNER_WEAL_LOC_COGNITION_THREAD;
    str_type = "weak loc cognition";
  }
  void SetPlannerCasadiOptimize() {
    type = eComputeThreadType::PLANNER_CASADI_OPTIMIZE;
    str_type = "casadi optimize";
  }  

  void SetPlannerTurnAround() {
    type = eComputeThreadType::PLANNER_TURN_AROUND;
    str_type = "Turn Around";
  }

  void SetPlannerPassThrough() {
    type = eComputeThreadType::PLANNER_PASS_THROUGH;
    str_type = "Pass Through";
  }

  void SetPlannerHybridAstar() {
    type = eComputeThreadType::PLANNER_HYBRID_ASTAR;
    str_type = "Hybrid Astar";
  }

} ComputeThreadType;

enum class eSystemStatus {
  DEFAULT = 0,
  FAULT_LEVEL_ONE = 1,
  FAULT_LEVEL_TWO = 2
};

enum class eExceptionHandleRes {
  DEFAULT = 0,
  EXECUTABLE,
  UNEXECUTABLE,
  PERSERVE
};

typedef struct NodeFaultMsg
{
  std::string info;
  int level;
  int is_preserve;
  int fault_type;
} NodeFaultMsg;

typedef struct ExceptionAnalyseResult {
  eExceptionHandleRes status;
  std::vector<NodeFaultMsg>  node_fault_vec;
  std::string info;
  ExceptionAnalyseResult() {
    status = eExceptionHandleRes::DEFAULT;
    node_fault_vec.reserve(10);
  }

  void ClearAllNodeFaultVec() {
    node_fault_vec.clear();
  }

  void AddNodeFault(std::string _info, int _level, int _fault_type, int _is_preserve = 1) {
    for (size_t i = 0; i < node_fault_vec.size(); i++) {
      if (node_fault_vec[i].fault_type == _fault_type) {
        return ;
      }
    }
    NodeFaultMsg node_fault_msg;
    node_fault_msg.info = _info;
    node_fault_msg.level = _level;
    node_fault_msg.fault_type = _fault_type;
    node_fault_msg.is_preserve = _is_preserve;
    node_fault_vec.push_back(node_fault_msg);
  }

  bool IsFaultExist() {
    return node_fault_vec.size() > 0;
  }
  void print() {
    for (size_t i = 0; i < node_fault_vec.size(); i++) {
      // AINFO << "node_fault_vec: " << node_fault_vec[i].info << "," <<  node_fault_vec[i].level;
    }
  }

  void RemoveParticular(int fault_level) {
    auto it = node_fault_vec.begin();
    for (; it != node_fault_vec.end(); ) {
      if (it->fault_type == fault_level ) {
        it = node_fault_vec.erase(it);
      } else {
        ++it;
      }
    }
  }

  void ClearNonPreserve() {
    auto it = node_fault_vec.begin();
    for (; it != node_fault_vec.end(); ) {
      if (it->is_preserve == 0 ) {
        it = node_fault_vec.erase(it);
      } else {
        ++it;
      }

    }
  }
} ExceptionAnalyseResult;

struct FaultInfo {
    int error_code;
    std::string error_msg;
    int fault_level;
    
    int Convert16To10(const std::string& data) {
      int result = std::stoi(data, nullptr, 16);
      return result;
    }
};

//定位故障
struct ModuleFault {
    double timestamp_sec;
    std::string module_name;
    std::unordered_map<int, std::vector<FaultInfo>> fault_info; //故障等级， 故障信息
    ModuleFault() {
        timestamp_sec = 0.0;
        module_name = "planning";
        fault_info.clear();
    } 
};

struct ComputedInfo {
  std::string algorithm;
  eComputeResult result;
  double time_cost;
  ComputedInfo() {
    Reset();
  }
  void Reset() {
    algorithm = "none";
    result = eComputeResult::OTHER;
    time_cost = 0.0;
  }
};

/**
* @brief define unstruct path data struct , for globle path  , refrence path , local path
*/
struct PathData {
  ComputedInfo computed_info;
  double time_stamp; //@pqg add for struct map /*!< the time for get the path data, the unit is second*/
  bool is_new;  
  int path_property;  /*!<  1 local  2 DR  default : -1 */
  std::string path_type;  /*!<  1 for output   "welt"or"normal" */
  int index;
  //todo
  std::list<geometry::Site> path;    /*!< define the points in path data*/
  double acc_a;
  float left_restrict;
  float right_restrict;

  int current_id;  //@pqg add 当前参考线编号
  int target_id;   //目标参考线编号
  std::vector<std::string> lane_ids; 
  bool is_blocked ;        //规划轨迹是否堵塞
  bool calculation_took_a_long_time;//路径生成可能花费了较长时间

  PathData() {
    time_stamp = 0.0;
    is_new = false;
    acc_a = 0.0;
    path_property = -1;
    index = -999;
    path_type = "normal";
    path.clear();
    left_restrict = std::numeric_limits<double>::max();
    right_restrict = std::numeric_limits<double>::max();
    is_blocked = false;
    calculation_took_a_long_time = false;
  }

  ~PathData() {
    time_stamp = 0.0;
    is_new = false;
    path_property = -1;
    index = -999;
    path_type = "normal";
    path.clear();
    left_restrict = std::numeric_limits<double>::max();
    right_restrict = std::numeric_limits<double>::max();
    is_blocked = false;
    calculation_took_a_long_time = false;
  }
  void Reset() {
    computed_info.Reset();
    time_stamp = 0.0;
    is_new = false;
    path_property = -1;
    index = -999;
    path.clear();
    path_type = "normal";
    left_restrict = std::numeric_limits<double>::max();
    right_restrict = std::numeric_limits<double>::max();
    is_blocked = false;
    calculation_took_a_long_time = false;
  }
  void clear() {
    path.clear();
    left_restrict = std::numeric_limits<double>::max();
    right_restrict = std::numeric_limits<double>::max();
  }
  int size(){
    return path.size();
  }
  // void operator=(const std::vector<geometry::Site> temp_path) {
  //   path.clear();
  //   path.insert(path.end(), temp_path.begin(), temp_path.end());
  // }

  bool operator==(PathData& a) {
    if (a.path.size() != path.size())
      return false;
    auto origin = path.begin();
    auto target = a.path.begin();
    for (size_t i = 0; i < path.size(); i++) {
      if ((*origin) == (*target)) {
        origin = std::next(origin, 1);
        target = std::next(target, 1);
      } else {
        return false;
      }
    }
    left_restrict = a.left_restrict;
    right_restrict = a.right_restrict;
    return true;
  }


  double GetRestLength() {
    double sum_dis = 0.0;
    for (auto it = path.begin(); std::next(it, 1) != path.end() &&
         it != path.end(); ++it) {
      sum_dis += std::hypot(std::next(it, 1)->x - it->x,
                            std::next(it, 1)->y - it->y);
    }
    return sum_dis;
  }

  geometry::Site GetIndex(size_t i) {
    if (i >= path.size()) {
      return *(path.begin());
    }
    size_t count  = 0;
    for (auto &p : path) {
      if (i == count) return p;
      count++;
    }
    return *(path.begin());
  }

  void SetPathProfile(const geometry::Site &ego_pose) {
    if (path.empty())
      return;

    avos::vectormap::PointGCCS gccs, ego{ego_pose.xg, ego_pose.yg, ego_pose.globalangle};
    auto geotool = std::make_shared<avos::vectormap::GeoTool>();
    
    double sum_dis = 0.0;
    auto it = path.begin();
    auto last_p = it;
    for (; next(it) != path.end(); it++) {
      if (next(it)->reverse != it->reverse && it != path.begin()) {
        it->angle = prev(it)->angle;
      } else {
        it->angle = std::atan2((next(it)->y - it->y),
                                  (next(it)->x - it->x)) *
                      180.0 / M_PI;
        if (it->reverse) {
          it->angle += 180.0;
        }
      }

      if (it->angle > 180.0)
        it->angle -= 360.0;
      if (it->angle < -180.0)
        it->angle += 360.0;

      avos::vectormap::PointVCS vcs{it->x, it->y, it->angle};
      geotool->VCS2GCCS(ego, vcs, gccs);
      sum_dis += std::hypot(it->x - last_p->x, it->y - last_p->y);
      it->xg = gccs.xg;
      it->yg = gccs.yg;
      it->globalangle = gccs.angle;
      it->length = sum_dis;

      last_p = it;
    }

    avos::vectormap::PointVCS vcs{it->x, it->y, it->angle};
    geotool->VCS2GCCS(ego, vcs, gccs);
    sum_dis += std::hypot(it->x - last_p->x, it->y - last_p->y);
    it->angle = last_p->angle;
    it->xg = gccs.xg;
    it->yg = gccs.yg;
    it->globalangle = gccs.angle;
    it->length = sum_dis;
  }

};

typedef struct StatisticsDetailTimes
{
  size_t setting_times;
  double setting_rate;
  double setting_negated_rate;
  double now_rate;
  double negated_rate;
  std::list<uint8> data_detail;
  StatisticsDetailTimes() {
    setting_times = 0;
    setting_rate = 0.0;
  }
  void Reset() {
    data_detail.clear();
  }

  bool SetStatisticsTimes(int times, double rate, double _negated_rate = 0.9) {
    data_detail.clear();
    if (times <= 0) return false;
    if (rate > 1 || rate <= 0) return false;
    setting_times = times;
    setting_rate = rate;
    setting_negated_rate = _negated_rate;
    return true;
  }

  void NewPositiveData() {
    data_detail.push_back(1);
    if (data_detail.size() > setting_times) {
      data_detail.erase(data_detail.begin());
    }
  }
  void NewNegativeData() {
    data_detail.push_back(0);
    if (data_detail.size() > setting_times) {
      data_detail.erase(data_detail.begin());
    }
  }

  std::string ToString() {
    int count = 0, negtive_count = 0;
    for (auto &p : data_detail) {
      if (!p) negtive_count++;
      else count++;
    }
    negated_rate = (double)(negtive_count) / (double )setting_times;
    now_rate = (double)(count) / (double )setting_times;
    return (std::to_string(count) + "," +
            std::to_string(negtive_count) + "," +
            std::to_string(now_rate) + "," +
            std::to_string(negated_rate));
  }

  bool IsNoTimesUp() {
    if (data_detail.size() != setting_times) return false;
    int count = 0;
    for (auto &p : data_detail) {
      if (!p) count++;
    }
    negated_rate = (double)(count) / (double )setting_times;
    if (negated_rate > setting_negated_rate) {
      return true;
    }
    return false;
  }

  bool IsTimesUp() {
    if (data_detail.size() != setting_times) return false;
    int count = 0;
    for (auto &p : data_detail) {
      if (p) count++;
    }
    now_rate = (double)(count) / (double )setting_times;
    if (now_rate > setting_rate) {
      return true;
    }
    return false;
  }

  int PositiveCount()
  {
    int totalcount = 0;
    for (const auto& c : data_detail)
    {
      totalcount += c;
    }
    return totalcount;
  }
  int NegtiveCount()
  {
    int totalcount = 0;
    for (const auto& c : data_detail)
    {
      totalcount += c;
    }
    return (setting_times - totalcount);
  }
} StatisticsDetailTimes;

typedef struct StatisticsTimes {
  int setting_times;
  int current_times;
  StatisticsTimes() {
    setting_times = 0;
    current_times = 0;
  }
  void Reset() {
    current_times = 0;
  }

  void SetStatisticsTimes(int times) {
    Reset();
    setting_times = times;
  }

  bool NewTimes() {
    current_times++;
    if(current_times > 999999) {current_times = 999999;}
    if (current_times > setting_times) {
      return true;
    }
    return false;
  }

  bool IsTimesUp() {
    if (current_times > setting_times) {
      return true;
    }
    return false;
  }

  float StayedTime() {
    return current_times * 0.1 / 60.0; //min
  }

  std::string ToString() {

    return (std::to_string(current_times) + "," +
            std::to_string(setting_times));
  }
} StatisticsTimes;


struct Paths {
  PathData front_local_path;
  PathData back_local_path;
  PathData front_pursuit_path;
  PathData front_local_path_raw;  /*!< for the reference path data */
  PathData front_local_path_raw_moved;
  PathData front_local_path_raw_calibrated;
  PathData sketchy_path;
  PathData calculate_path;
  PathData result_path;
  PathData result_pursuit_path;
  PathData motion_path;
  PathData dr_path;

  PathData follow_drag_path;
  PathData follow_traj_path;

  Paths() {
    Reset();
  }
  void Reset() {
    follow_drag_path.Reset();
    follow_traj_path.Reset();
    front_local_path.Reset();
    back_local_path.Reset();
    front_pursuit_path.Reset();
    front_local_path_raw.Reset();
    front_local_path_raw_moved.Reset();
    front_local_path_raw_calibrated.Reset();
    sketchy_path.Reset();
    calculate_path.Reset();
    result_path.Reset();
    motion_path.Reset();
    dr_path.Reset();
    result_pursuit_path.Reset();
    // last_map_engine_ref.clear();
  }
};

} 
}

#endif // DATAPOOL_INCLUDE_PUBLIC_TYPEDEF_H_
