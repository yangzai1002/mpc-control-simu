#pragma once
#include <vector>
#include "path_data_types.h"
#include "../frenet/frenet_data_types.h"
#include "../trajectory/trajectory_data_types.h" // 引入轨迹数据结构

namespace localplanner {
namespace path_planner {

class PathDeciderDP {
public:
    struct Config {
        int s_nodes_num = 40;      
        double s_interval = 2.0;   
        int l_nodes_num = 45;     
        double l_interval = 0.2;  
        
        double weight_obs = 1e8;       
        double weight_ref = 5.0;      
        double weight_dl = 100.0;      
        
        // 🚀 新增：历史轨迹平滑继承代价权重
        double weight_history = 20.0;  
    };

    explicit PathDeciderDP(const Config& config, const VehicleConfig& veh_cfg) 
        : config_(config), veh_cfg_(veh_cfg) {}

    /**
     * @brief 决策主函数 (引入历史轨迹)
     */
    bool Execute(const std::vector<ObstacleSL>& obstacles,
                 const PathPointSL& init_sl,
                 double current_v, 
                 const std::vector<trajectory::TrajectoryPoint>& last_trajectory, // 🚀 传入上一帧轨迹
                 std::vector<PathPointSL>& dp_path) const;

private:
    double CalculateCollisionCost(const PathPointSL& point, 
                                  const std::vector<ObstacleSL>& obstacles) const;

    Config config_;
    VehicleConfig veh_cfg_;
};

} // namespace path_planner
} // namespace localplanner