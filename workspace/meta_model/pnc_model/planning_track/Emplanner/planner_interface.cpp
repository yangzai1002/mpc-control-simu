#include "planner_interface.h"
#include "frenet/cartesian_frenet_converter.h"
#include "reference_line/smoother.h"
#include <algorithm>
#include <cmath>

namespace localplanner {
namespace interface {

PlannerInterface::PlannerInterface() {
    planner_manager_ = std::make_unique<PlannerManager>();
}

bool PlannerInterface::RunPlanner(const Location& loc,
                                  const std::vector<GlobalPathPoint>& global_path,
                                  const std::vector<double>& occupancy_source,
                                  double vmax,
                                  std::vector<PathPoint>& out_trajectory) {
    out_trajectory.clear();
    if (global_path.empty()) {
        std::cerr << "[PlannerInterface] ERROR: Global path is empty." << std::endl;
        return false;
    }

    // 1. 坐标系降维：计算自车当前的 UTM 坐标并作为局部原点 (Anchor)
    double anchor_utm_x = 0.0, anchor_utm_y = 0.0;
   // LatLon2XY(loc.latitude, loc.longitude, anchor_utm_x, anchor_utm_y);
    anchor_utm_x = loc.longitude;
    anchor_utm_y = loc.latitude;
    //loc.heading = loc.heading * M_PI / 180;

    std::vector<ref_line::RawPoint> full_local_path;
    full_local_path.reserve(global_path.size());
    for (const auto& pt : global_path) {
        double utm_x = 0.0, utm_y = 0.0;
        //LatLon2XY(pt.latitude, pt.longitude, utm_x, utm_y);
        full_local_path.push_back({pt.longitude - anchor_utm_x, pt.latitude - anchor_utm_y});
        //full_local_path.push_back({pt.longitude, pt.latitude});
    }


    // for(auto point:full_local_path)
    // {
    //     std::cout <<"xp:"<<point.x <<"yp:"<<point.y<<std::endl;
    // }


    if (full_local_path.size() < 2) return false;

    // 2. 基于真实物理弧长 (S) 的路径重采样与滑动窗口截取
    std::vector<double> accumulated_s(full_local_path.size(), 0.0);
    for (size_t i = 1; i < full_local_path.size(); ++i) {
        double dx = full_local_path[i].x - full_local_path[i-1].x;
        double dy = full_local_path[i].y - full_local_path[i-1].y;
        accumulated_s[i] = accumulated_s[i-1] + std::hypot(dx, dy);
    }

    size_t match_idx = 0;
    double min_dist_sq = std::numeric_limits<double>::max();
    for (size_t i = 0; i < full_local_path.size(); ++i) {
        double dist_sq = full_local_path[i].x * full_local_path[i].x + 
                         full_local_path[i].y * full_local_path[i].y;
        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            match_idx = i;
        }
    }
    double ego_s = accumulated_s[match_idx]; 

    // 设定滑动截断边界：向后 30m，向前 150m
    double start_s = std::max(0.0, ego_s - 30.0);
    double end_s = std::min(accumulated_s.back(), ego_s + 150.0);
    
    // 线性插值重采样，统一采样步长
    const double sample_step = 2.0; 
    double aligned_start_s = std::floor(start_s / sample_step) * sample_step;

    std::vector<ref_line::RawPoint> local_raw_path;
    for (double target_s = aligned_start_s; target_s <= end_s; target_s += sample_step) {
        if (target_s >= accumulated_s.back() - 1e-4) {
            local_raw_path.push_back(full_local_path.back());
            continue;
        }

        auto it = std::lower_bound(accumulated_s.begin(), accumulated_s.end(), target_s);
        if (it == accumulated_s.begin()) {
            local_raw_path.push_back(full_local_path.front());
            continue;
        }

        size_t next_idx = std::distance(accumulated_s.begin(), it);
        size_t prev_idx = next_idx - 1;

        double segment_length = accumulated_s[next_idx] - accumulated_s[prev_idx];
        double ratio = (segment_length > 1e-6) ? (target_s - accumulated_s[prev_idx]) / segment_length : 0.0;

        double interp_x = full_local_path[prev_idx].x + ratio * (full_local_path[next_idx].x - full_local_path[prev_idx].x);
        double interp_y = full_local_path[prev_idx].y + ratio * (full_local_path[next_idx].y - full_local_path[prev_idx].y);
        
        local_raw_path.push_back({interp_x, interp_y});
    }

    if (local_raw_path.size() < 3) {
        std::cerr << "[PlannerInterface] WARNING: Valid route segment is too short." << std::endl;
        return false;
    }

    // 3. 提取并平滑局部参考线
    ref_line::ReferenceLineSmoother smoother(ref_line::SmootherConfig{});
    std::vector<ref_line::ReferencePoint> smooth_ref;
    if (!smoother.Execute(local_raw_path, smooth_ref)) {
        std::cerr << "[PlannerInterface] ERROR: Local Reference Line Smoothing Failed." << std::endl;
        return false;
    }

    // 4. 处理栅格地图，提取并聚类障碍物
    auto obstacles_sl = ProcessOccupancyMap(occupancy_source, smooth_ref);

    // 5. 状态同步：计算车辆初始 SL 状态和运动学状态
    std::array<double, 3> current_path_state = {0.0, 0.0, 0.0};  
    std::array<double, 3> current_speed_state = {0.0, 0.0, 0.0}; 

    double vehicle_local_x = 0.0;
    double vehicle_local_y = 0.0;
    double vehicle_math_yaw = ConvertHeadingToMathYaw(loc.heading);

    if (!smooth_ref.empty()) {
        frenet::FrenetPoint sl_pt;
        frenet::CartesianFrenetConverter::cartesian_to_frenet(smooth_ref, vehicle_local_x, vehicle_local_y, sl_pt);

        double ref_theta = 0.0;
        for (const auto& pt : smooth_ref) {
            if (pt.s >= sl_pt.s) {
                ref_theta = pt.theta;
                break;
            }
        }

        double delta_theta = vehicle_math_yaw - ref_theta;
        while (delta_theta > M_PI) delta_theta -= 2.0 * M_PI;
        while (delta_theta < -M_PI) delta_theta += 2.0 * M_PI;
        
        const double max_yaw_error = 60.0 * M_PI / 180.0; 
        delta_theta = std::max(-max_yaw_error, std::min(max_yaw_error, delta_theta));
        double real_dl = std::tan(delta_theta);

        current_path_state = {sl_pt.s, sl_pt.l, real_dl};
        current_speed_state = {sl_pt.s, loc.veloity, loc.acc};
    }

    // 6. 调用核心规控管家执行计算
    std::vector<trajectory::TrajectoryPoint> internal_trajectory;
    bool success = planner_manager_->RunOnce(local_raw_path, obstacles_sl, current_path_state, current_speed_state, vmax, internal_trajectory);

    // 7. 轨迹后处理：将局部坐标还原为全局 UTM 坐标
    out_trajectory.reserve(internal_trajectory.size());
    for (const auto& tp : internal_trajectory) {
        PathPoint pt;
        pt.x = tp.x + anchor_utm_x;
        pt.y = tp.y + anchor_utm_y;
        pt.v = tp.v;
        pt.a = tp.a;
        pt.theta = tp.theta; 
        pt.kappa = tp.kappa;
        pt.l = tp.l;                     
        pt.left_bound = tp.left_bound;   
        pt.right_bound = tp.right_bound; 
        out_trajectory.push_back(pt);
    }

    return success;
}

double PlannerInterface::ConvertHeadingToMathYaw(double vehicle_heading) const {
    double math_yaw = (90.0 - vehicle_heading) * M_PI / 180.0;
    while (math_yaw > M_PI) math_yaw -= 2.0 * M_PI;
    while (math_yaw < -M_PI) math_yaw += 2.0 * M_PI;
    return math_yaw;
}

std::vector<path_planner::ObstacleSL> PlannerInterface::ProcessOccupancyMap(
    const std::vector<double>& occupancy_source,
    const std::vector<ref_line::ReferencePoint>& ref_line) const {
    
    map_processor::GridMapConfig map_cfg;
    map_processor::ObstacleExtractor extractor(map_processor::ObstacleExtractor::ExtractConfig{});
    std::vector<frenet::FrenetPoint> frenet_obs = extractor.Extract(occupancy_source, map_cfg, ref_line);

    std::vector<path_planner::ObstacleSL> obstacles_sl;
    if (frenet_obs.empty()) return obstacles_sl;

    // 障碍物 BFS 连通域聚类
    std::vector<bool> visited(frenet_obs.size(), false);
    const double CLUSTER_EPSILON_SQ = 0.6 * 0.6; 
    const double OBS_EXPAND_MARGIN = 0.2; 

    for (size_t i = 0; i < frenet_obs.size(); ++i) {
        if (visited[i]) continue;

        double min_s = frenet_obs[i].s, max_s = frenet_obs[i].s;
        double min_l = frenet_obs[i].l, max_l = frenet_obs[i].l;

        std::vector<size_t> queue;
        queue.push_back(i);
        visited[i] = true;

        size_t head = 0;
        while (head < queue.size()) {
            size_t curr = queue[head++];
            const auto& pt = frenet_obs[curr];

            min_s = std::min(min_s, pt.s);
            max_s = std::max(max_s, pt.s);
            min_l = std::min(min_l, pt.l); 
            max_l = std::max(max_l, pt.l); 

            for (size_t j = 0; j < frenet_obs.size(); ++j) {
                if (!visited[j]) {
                    double ds = frenet_obs[j].s - pt.s;
                    double dl = frenet_obs[j].l - pt.l;
                    if (ds * ds + dl * dl <= CLUSTER_EPSILON_SQ) {
                        visited[j] = true;
                        queue.push_back(j);
                    }
                }
            }
        }

        path_planner::ObstacleSL obs_box;
        obs_box.start_s = min_s - OBS_EXPAND_MARGIN;
        obs_box.end_s   = max_s + OBS_EXPAND_MARGIN;
        obs_box.left_l  = max_l + OBS_EXPAND_MARGIN;  
        obs_box.right_l = min_l - OBS_EXPAND_MARGIN;  

        if ((obs_box.end_s - obs_box.start_s > 0.3) || (obs_box.left_l - obs_box.right_l > 0.3)) {
            obstacles_sl.push_back(obs_box);
        }
    }

    return obstacles_sl;
}

double PlannerInterface::Deg2Rad(double deg) const { return deg * M_PI / 180.0; }
double PlannerInterface::Rad2Deg(double rad) const { return rad * 180.0 / M_PI; }

int PlannerInterface::CalcZone(double lon) const {
    return static_cast<int>((lon + 180.0) / 6.0) + 1;
}

void PlannerInterface::LatLon2XY(double lat, double lon, double& x, double& y) {
    int zone = CalcZone(lon);
    cached_zone_ = zone;
    is_northern_hemisphere_ = (lat >= 0.0);

    double a = WGS84_A;
    double f = WGS84_F;
    double b = a * (1 - f);
    double e = std::sqrt(1 - (b * b) / (a * a));
    double e2 = e * e;
    double e_prime_2 = e2 / (1 - e2);

    double lat_rad = Deg2Rad(lat);
    double lon_rad = Deg2Rad(lon);
    double lon_0 = Deg2Rad((zone - 1) * 6 - 180 + 3);

    double N = a / std::sqrt(1 - e2 * std::sin(lat_rad) * std::sin(lat_rad));
    double T = std::tan(lat_rad) * std::tan(lat_rad);
    double C = e_prime_2 * std::cos(lat_rad) * std::cos(lat_rad);
    double A = (lon_rad - lon_0) * std::cos(lat_rad);

    double M = a * ((1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256) * lat_rad
                  - (3 * e2 / 8 + 3 * e2 * e2 / 32 + 45 * e2 * e2 * e2 / 1024) * std::sin(2 * lat_rad)
                  + (15 * e2 * e2 / 256 + 45 * e2 * e2 * e2 / 1024) * std::sin(4 * lat_rad)
                  - (35 * e2 * e2 * e2 / 3072) * std::sin(6 * lat_rad));

    double utm_x = UTM_K0 * N * (A + (1 - T + C) * A * A * A / 6
                    + (5 - 18 * T + T * T + 72 * C - 58 * e_prime_2) * std::pow(A, 5) / 120)
                    + 500000.0;

    double utm_y = UTM_K0 * (M + N * std::tan(lat_rad) * (A * A / 2
                    + (5 - T + 9 * C + 4 * C * C) * std::pow(A, 4) / 24
                    + (61 - 58 * T + T * T + 600 * C - 330 * e_prime_2) * std::pow(A, 6) / 720));

    if (!is_northern_hemisphere_) utm_y += 10000000.0;

    x = utm_x;
    y = utm_y;
}

void PlannerInterface::XY2LatLon(double x, double y, double& lat, double& lon) {
    int zone = cached_zone_;
    double a = WGS84_A;
    double f = WGS84_F;
    double b = a * (1 - f);
    double e = std::sqrt(1 - (b * b) / (a * a));
    double e2 = e * e;
    double e_prime_2 = e2 / (1 - e2);

    double x_val = x - 500000.0;
    double y_val = is_northern_hemisphere_ ? y : y - 10000000.0;

    double M = y_val / UTM_K0;
    double mu = M / (a * (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256));

    double e1 = (1 - std::sqrt(1 - e2)) / (1 + std::sqrt(1 - e2));
    double phi1 = mu + (3 * e1 / 2 - 27 * std::pow(e1, 3) / 32) * std::sin(2 * mu)
                  + (21 * e1 * e1 / 16 - 55 * std::pow(e1, 4) / 32) * std::sin(4 * mu)
                  + (151 * std::pow(e1, 3) / 96) * std::sin(6 * mu);

    double N1 = a / std::sqrt(1 - e2 * std::sin(phi1) * std::sin(phi1));
    double T1 = std::tan(phi1) * std::tan(phi1);
    double C1 = e_prime_2 * std::cos(phi1) * std::cos(phi1);
    double R1 = a * (1 - e2) / std::pow(1 - e2 * std::sin(phi1) * std::sin(phi1), 1.5);
    double D = x_val / (N1 * UTM_K0);

    double lat_rad = phi1 - (N1 * std::tan(phi1) / R1) * (D * D / 2
                     - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e_prime_2) * std::pow(D, 4) / 24
                     + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e_prime_2 - 3 * C1 * C1) * std::pow(D, 6) / 720);

    double lon_0 = Deg2Rad((zone - 1) * 6 - 180 + 3);
    double lon_rad = lon_0 + (D - (1 + 2 * T1 + C1) * std::pow(D, 3) / 6
                     + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e_prime_2 + 24 * T1 * T1) * std::pow(D, 5) / 120) / std::cos(phi1);

    lat = Rad2Deg(lat_rad);
    lon = Rad2Deg(lon_rad);
}

} // namespace interface
} // namespace localplanner