/**
 * @file Leveling.cpp
 * @brief 车辆自动找平功能实现
 * @author ljq
 */

#include "Leveling.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "ap_log/ap_log_interface.h"
#include "log/log.h"
#include "base_control/adsfi_struct_adapter.h"

namespace avos {
namespace planning {

Leveling::Leveling() : is_target_locked_(false), unlock_counter_(0) {
    locked_target_.x = 0.0f;
    locked_target_.y = 0.0f;
    locked_target_.z = 0.0f;
}

Leveling::~Leveling() {}

bool Leveling::Process(const ara::adsfi::BusinessCommand& command,
                       const avos::planning::LocalizationData& localization_data,
                       const ara::adsfi::PerceptionStaticEnv& semantic_map,
                       PlanningTrajectory& trajectory,
                       ara::adsfi::PerceptionStaticEnv& terrain_debug) {
    
    // 1. 初始化输出数据
    trajectory.trajectory.clear();
    trajectory.header.time = localization_data.time;
    
    // 初始化 Debug 头信息
    InitDebugInfo(semantic_map, terrain_debug);

    AINFO_IF(FLAGS_enable_log) << "[Leveling] 开始执行找平逻辑, 接收到地图Patch数量: " << semantic_map.terrain_patches.size();

    // 2. 基础校验
    if (semantic_map.terrain_patches.empty()) {
        AERROR_IF(FLAGS_enable_log) << "[Leveling] 失败: 地图数据为空";
        is_target_locked_ = false; // 强制解锁
        return false;
    }

    // 3. 构建空间索引
    std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash> flat_nodes_map;
    BuildFlatGridMap(semantic_map, localization_data, 0.6f, flat_nodes_map, terrain_debug);

    if (flat_nodes_map.empty()) {
        AWARN_IF(FLAGS_enable_log) << "[Leveling] 失败: 构建哈希表后为空";
        is_target_locked_ = false; // 强制解锁
        return false;
    }

    // 4. 统一计算和配置参数
    LevelingSearchContext ctx;
    ComputeSearchContext(ctx);
    ctx.current_x = localization_data.xg;
    ctx.current_y = localization_data.yg;
    ctx.current_heading = localization_data.yaw;

    // 5. 核心决策：确定目标点
    double target_x = 0.0;
    double target_y = 0.0;
    double target_z = 0.0;
    
    // 状态机逻辑入口
    bool found = DetermineTargetPoint(flat_nodes_map, localization_data, ctx, 
                                      target_x, target_y, target_z, terrain_debug);

    if (!found) {
        return false; 
    }

    // 6. 记录最终调试信息
    RecordFinalTargetDebug(localization_data, target_x, target_y, target_z, terrain_debug);

    // 7. 生成规划轨迹
    GenerateTrajectory(localization_data, target_x, target_y, target_z, trajectory);

    return true;
}

void Leveling::BuildFlatGridMap(const ara::adsfi::PerceptionStaticEnv& map_data, const avos::planning::LocalizationData& localization_data,
        const float patch_size, std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map, ara::adsfi::PerceptionStaticEnv& terrain_debug) {
    
    // 预留空间
    flat_nodes_map.reserve(map_data.terrain_patches.size() / 2);

    // 当前车辆的绝对位姿
    avos::vectormap::PointGCCS vehicle_pose;
    vehicle_pose.xg = localization_data.xg;
    vehicle_pose.yg = localization_data.yg;
    vehicle_pose.angle = localization_data.yaw;

    // --- 调试统计变量 ---
    int total_pts = map_data.terrain_patches.size();
    int pass_all = 0; // 双向都合格
    int pass_x = 0;   // 仅X合格
    int pass_y = 0;   // 仅Y合格
    
    // 坡度区间统计: [0-5), [5-10), [10-15), [15+)
    int slope_dist[4] = {0, 0, 0, 0}; 
    // -------------------------

    AINFO_IF(FLAGS_enable_log) << "[Leveling Debug] >>> 开始遍历原始地图点 (总数: " << total_pts << ") <<<"
          << " SLOPE_THRESHOLD_DEG = " << SLOPE_THRESHOLD_DEG;

    for (const auto& patch : map_data.terrain_patches) {
        // Debug 数据直接使用原始 VCS 数据
        ara::adsfi::PerceptionTerrainPatch debug_patch = patch; 
        
        // 算法逻辑需要的绝对坐标转换
        avos::vectormap::PointVCS patch_center_vcs;
        avos::vectormap::PointGCCS patch_center_global;
        patch_center_vcs.x = patch.patch_center.x;
        patch_center_vcs.y = patch.patch_center.y;
        patch_center_vcs.angle = 0.0; 
        m_transform_.VCS2GCCS(vehicle_pose, patch_center_vcs, patch_center_global);

        // --- 详细数据收集 ---
        float abs_sx = std::abs(patch.slopx);
        float abs_sy = std::abs(patch.slopy);
        float max_s = std::max(abs_sx, abs_sy);
        
        // 统计分布区间
        if (max_s < 5.0) slope_dist[0]++;
        else if (max_s < 10.0) slope_dist[1]++;
        else if (max_s < 15.0) slope_dist[2]++;
        else slope_dist[3]++;

        // 筛选逻辑
        bool is_flat_x = std::abs(patch.slopx) < SLOPE_THRESHOLD_DEG;
        bool is_flat_y = std::abs(patch.slopy) < SLOPE_THRESHOLD_DEG;

        if (is_flat_x) pass_x++;
        if (is_flat_y) pass_y++;
        if (is_flat_x && is_flat_y) pass_all++; 

        if (is_flat_x && is_flat_y) {
            // 记录合格点 (Type 10)
            debug_patch.type = DBG_PATCH_RAW_OK;
            terrain_debug.terrain_patches.push_back(debug_patch);

            // 入库
            int idx_x = static_cast<int>(std::round(patch_center_global.xg / patch_size));
            int idx_y = static_cast<int>(std::round(patch_center_global.yg / patch_size));
            
            FlatPatchNode node;
            node.x = patch_center_global.xg;
            node.y = patch_center_global.yg;
            node.z = patch.patch_center.z;
            node.slopx = patch.slopx;
            node.slopy = patch.slopy;
            
            flat_nodes_map[{idx_x, idx_y}] = node;

        } else {
            // 记录坡度超标点 (Type 11)
            debug_patch.type = DBG_PATCH_SLOPE_BAD;
            terrain_debug.terrain_patches.push_back(debug_patch);
        }
    }

    // --- 打印最终统计结果 ---
    float ratio_all = (total_pts > 0) ? (float)pass_all / total_pts * 100.0f : 0.0f;
    
    AINFO_IF(FLAGS_enable_log) << "\n================ [Leveling 斜率分析报告] ================\n"
          << "总点数: " << total_pts << "\n"
          << "最终合格数: " << pass_all << " (合格率: " << ratio_all << "%)\n"
          << "-------------------------------------------------------\n"
          << "X轴合格数: " << pass_x << " (不合格: " << total_pts - pass_x << ")\n"
          << "Y轴合格数: " << pass_y << " (不合格: " << total_pts - pass_y << ")\n"
          << "-------------------------------------------------------\n"
          << "坡度分布 (Max(|x|,|y|)):\n"
          << "  0°~ 5°: " << slope_dist[0] << "\n"
          << "  5°~10°: " << slope_dist[1] << "\n"
          << " 10°~15°: " << slope_dist[2] << "\n"
          << "   > 15°: " << slope_dist[3] << "\n"
          << "=======================================================";
}

bool Leveling::FindBestFlatSpot(const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map,
                                const avos::planning::LocalizationData& loc_data,
                                const LevelingSearchContext& ctx,
                                double& target_x, double& target_y, double& target_z,
                                ara::adsfi::PerceptionStaticEnv& terrain_debug) {

    LevelingSearchContext local_ctx = ctx;
    AINFO_IF(FLAGS_enable_log) << "[Leveling Debug] 搜索参数: GridSpan=" << local_ctx.grid_span 
          << ", Threshold=" << (int)(local_ctx.coverage_threshold * 100) << "%"
          << ", Z_Diff_Limit=" << local_ctx.max_height_diff_threshold;

    // 2. 遍历所有潜在点
    for (const auto& kv : flat_nodes_map) {
        // Step A: 计算覆盖率 + Z轴高差
        float z_height_diff = 0.0f;
        double coverage = CalculateRegionCoverage(kv.first, flat_nodes_map, local_ctx, z_height_diff);

        // Step B: 几何计算与 Tier 分级
        int tier = 4;
        double dist_sq = 0.0;
        ComputeCandidateTier(kv.second, local_ctx, tier, dist_sq);

        // Step C: 统计与记录状态
        if (coverage >= 0.90) local_ctx.cov_stats[0]++;
        else if (coverage >= 0.80) local_ctx.cov_stats[1]++;
        else if (coverage >= 0.70) local_ctx.cov_stats[2]++;
        else if (coverage >= 0.60) local_ctx.cov_stats[3]++;
        else if (coverage >= 0.50) local_ctx.cov_stats[4]++;
        else local_ctx.cov_stats[5]++;

        if (coverage > local_ctx.max_coverage) {
            local_ctx.max_coverage = coverage;
            local_ctx.max_cov_idx = kv.first;
        }

        // Step D: 填充 Debug 信息
        AddDebugInfo(kv.second, coverage, z_height_diff, tier, local_ctx, terrain_debug);

        // Step E: 择优 (覆盖率达标时)
        bool is_safe_flat = (coverage >= local_ctx.coverage_threshold) && 
                            (z_height_diff <= local_ctx.max_height_diff_threshold);

        if (is_safe_flat) {
            local_ctx.valid_area_count++;
            // 更新该 Tier 下的最佳点
            if (dist_sq < local_ctx.min_dist[tier]) {
                local_ctx.min_dist[tier] = dist_sq;
                local_ctx.best_nodes[tier] = &kv.second;
            }
        } else {
            // 如果覆盖率够但高差不够，可以在 PrintFailureAnalysis 里统计这种情况
        }
    }

    // 3. 打印分析
    PrintFailureAnalysis(local_ctx, flat_nodes_map);

    // 4. 最终决策
    const FlatPatchNode* final_target = nullptr;
    if (local_ctx.best_nodes[1]) final_target = local_ctx.best_nodes[1];      // Tier 1 优先
    else if (local_ctx.best_nodes[2]) final_target = local_ctx.best_nodes[2]; // Tier 2 次之
    else if (local_ctx.best_nodes[3]) final_target = local_ctx.best_nodes[3]; // Tier 3
    else if (local_ctx.best_nodes[4]) final_target = local_ctx.best_nodes[4]; // Tier 4 保底

    if (final_target != nullptr) {
        target_x = final_target->x;
        target_y = final_target->y;
        target_z = final_target->z;
        AINFO_IF(FLAGS_enable_log) << "[Leveling] 成功锁定目标, Tier=" << (local_ctx.best_nodes[1] ? 1 : (local_ctx.best_nodes[2] ? 2 : (local_ctx.best_nodes[3] ? 3 : 4)));
        return true;
    }

    return false;
}

/**
 * @brief 计算区域覆盖率，区域平整度(Z-Range)
 * @param center 区域中心索引
 * @param map 哈希表
 * @param ctx 搜索上下文
 * @param out_z_diff [输出] 计算出的该区域 Z 轴最大高差
 * @return 覆盖率 (0.0 ~ 1.0)
 */
double Leveling::CalculateRegionCoverage(const GridIndex& center,
                                         const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& map,
                                         const LevelingSearchContext& ctx,
                                         float& out_z_diff) {
    
    int start_x = center.x_idx - ctx.half_span;
    int end_x = start_x + ctx.grid_span;
    int start_y = center.y_idx - ctx.half_span;
    int end_y = start_y + ctx.grid_span;

    int valid_count = 0;
    
    // 初始化 Z 轴极值
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::lowest();

    for (int i = start_x; i < end_x; ++i) {
        for (int j = start_y; j < end_y; ++j) {
            auto it = map.find({i, j});
            if (it != map.end()) {
                valid_count++;
                
                // 统计 Z 值
                float z = it->second.z;
                if (z < min_z) min_z = z;
                if (z > max_z) max_z = z;
            }
        }
    }

    // 计算高差 (如果没有有效点，高差为0)
    if (valid_count > 0) {
        out_z_diff = max_z - min_z;
    } else {
        out_z_diff = 0.0f;
    }

    return static_cast<double>(valid_count) / ctx.total_points_needed;
}

//计算 Tier 和距离
void Leveling::ComputeCandidateTier(const FlatPatchNode& node, 
                                    const LevelingSearchContext& ctx,
                                    int& out_tier, double& out_dist_sq) {
    double dx = node.x - ctx.current_x;
    double dy = node.y - ctx.current_y;
    out_dist_sq = dx * dx + dy * dy;

    double target_angle_rad = std::atan2(dy, dx);
    double target_angle_deg = target_angle_rad * 180.0 / M_PI;

    double rel_angle = target_angle_deg - ctx.current_heading;
    while (rel_angle > 180.0) rel_angle -= 360.0;
    while (rel_angle < -180.0) rel_angle += 360.0;
    double abs_angle = std::abs(rel_angle);

    // Tier 1: 正前方 +/- 10度
    // Tier 2: 前方 +/- 30度
    // Tier 3: 前方 +/- 90度
    // Tier 4: 其他 (侧后方)
    if (abs_angle <= 10.0) out_tier = 1;
    else if (abs_angle <= 30.0) out_tier = 2;
    else if (abs_angle <= 90.0) out_tier = 3;
    else out_tier = 4;
}

//填充 Debug 信息
void Leveling::AddDebugInfo(const FlatPatchNode& node, const double coverage, const float z_diff, const int tier, 
                            const LevelingSearchContext& ctx, ara::adsfi::PerceptionStaticEnv& terrain_debug) {
    
    // 构造车辆位姿用于坐标转换
    avos::vectormap::PointGCCS vehicle_pose;
    vehicle_pose.xg = ctx.current_x;
    vehicle_pose.yg = ctx.current_y;
    vehicle_pose.angle = ctx.current_heading;

    avos::vectormap::PointGCCS node_gccs;
    node_gccs.xg = node.x;
    node_gccs.yg = node.y;
    node_gccs.angle = 0.0;
    
    avos::vectormap::PointVCS node_vcs;
    m_transform_.GCCS2VCS(vehicle_pose, node_gccs, node_vcs);

    ara::adsfi::PerceptionTerrainPatch debug_patch;
    debug_patch.patch_center.x = node_vcs.x;
    debug_patch.patch_center.y = node_vcs.y;
    debug_patch.patch_center.z = node.z;

    // 复用字段存储可视化信息
    debug_patch.slopx = static_cast<float>(coverage);
    debug_patch.slopy = z_diff;

    // 根据是否合格以及 Tier 等级，赋予不同的 Type
    bool is_safe_flat = (coverage >= ctx.coverage_threshold) && 
                        (z_diff <= ctx.max_height_diff_threshold);

    if (is_safe_flat) {
        switch (tier) {
            case 1: debug_patch.type = DBG_CANDIDATE_TIER1; break; // 12
            case 2: debug_patch.type = DBG_CANDIDATE_TIER2; break; // 13
            case 3: debug_patch.type = DBG_CANDIDATE_TIER3; break; // 14
            default: debug_patch.type = DBG_CANDIDATE_TIER4; break; // 15
        }
    } else {
        debug_patch.type = DBG_SPOT_POOR_COV; // 16
    }
    
    terrain_debug.terrain_patches.push_back(debug_patch);
}

//打印
void Leveling::PrintFailureAnalysis(const LevelingSearchContext& ctx,
                                    const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& map) {
    
    int total = map.size();
    if (total == 0) return;

    // 1. 打印覆盖率分布
    float ratio_90 = (float)ctx.cov_stats[0] / total * 100.0f;
    
    AINFO_IF(FLAGS_enable_log) << "\n================ [Leveling 覆盖率分布报告] ================\n"
          << "总遍历点数: " << total << "\n"
          << "当前阈值: " << (int)(ctx.coverage_threshold * 100) << "% (合格数: " << ctx.valid_area_count << ")\n"
          << "最高覆盖率: " << (ctx.max_coverage * 100) << "%\n"
          << "-------------------------------------------------------\n"
          << " >  90% : " << ctx.cov_stats[0] << " 个 (" << ratio_90 << "%)\n"
          << "80%~90% : " << ctx.cov_stats[1] << " 个\n"
          << "70%~80% : " << ctx.cov_stats[2] << " 个\n"
          << "60%~70% : " << ctx.cov_stats[3] << " 个\n"
          << "50%~60% : " << ctx.cov_stats[4] << " 个\n"
          << " <  50% : " << ctx.cov_stats[5] << " 个\n"
          << "=======================================================";

    // 2. 如果失败，打印最佳失败案例
    if (ctx.valid_area_count == 0) {
        AWARN_IF(FLAGS_enable_log) << ">>> [Leveling 深度排查] 未找到满足要求的区域。";

        // 计算中心点 VCS 坐标
        double center_vcs_x = 0.0, center_vcs_y = 0.0;
        auto it = map.find(ctx.max_cov_idx);
        if (it != map.end()) {
            avos::vectormap::PointGCCS pg; 
            pg.xg = it->second.x; pg.yg = it->second.y; pg.angle = 0.0;
            avos::vectormap::PointGCCS v_pose;
            v_pose.xg = ctx.current_x; v_pose.yg = ctx.current_y; v_pose.angle = ctx.current_heading;
            avos::vectormap::PointVCS pv;
            m_transform_.GCCS2VCS(v_pose, pg, pv);
            center_vcs_x = pv.x; center_vcs_y = pv.y;
        }

        AWARN_IF(FLAGS_enable_log) << "最佳失败案例矩阵 (O=有平地, .=空洞)\n"
              << "  -> Center_VCS(x=" << center_vcs_x << "m, "
              << "y=" << center_vcs_y << "m) [Idx:" << ctx.max_cov_idx.x_idx << "," << ctx.max_cov_idx.y_idx << "]:";

        int start_x = ctx.max_cov_idx.x_idx - ctx.half_span;
        int end_x = start_x + ctx.grid_span;
        int start_y = ctx.max_cov_idx.y_idx - ctx.half_span;
        int end_y = start_y + ctx.grid_span;

        // 逐行打印矩阵
        for (int j = end_y - 1; j >= start_y; --j) {
            std::stringstream ss_row;
            ss_row << "Row " << std::setw(4) << j << ": "; 
            for (int i = start_x; i < end_x; ++i) {
                if (map.find({i, j}) != map.end()) ss_row << "O ";
                else ss_row << ". ";
            }
            AWARN_IF(FLAGS_enable_log) << ss_row.str();
        }
    }
}

void Leveling::GenerateTrajectory(const avos::planning::LocalizationData& start_loc,
                                  double target_x, double target_y, double target_z,
                                  PlanningTrajectory& trajectory) {
    
    // 1. 向量计算
    double dx = target_x - start_loc.xg;
    double dy = target_y - start_loc.yg;
    double total_dist = std::hypot(dx, dy);
    double target_heading_rad = std::atan2(dy, dx); 
    double target_heading_deg = target_heading_rad * 180.0 / M_PI; 

    // 安全保护：防止定位为0导致生成几百公里的轨迹卡死程序
    if (total_dist > 50.0) { 
        AERROR_IF(FLAGS_enable_log) << "[Leveling] 异常: 目标点距离过远 (" << total_dist << "m)，疑似定位漂移或未初始化，取消规划。";
        return; 
    }

    if (total_dist < std::max(0.0,(DIST_THRESHOLD_REACHED - 0.1))) { 
         AINFO_IF(FLAGS_enable_log) << "[Leveling] 距离极近 (" << total_dist << "m < " << std::max(0.0,(DIST_THRESHOLD_REACHED - 0.1)) << "m)，停止轨迹生成。";
         return; 
    }

    // --- 2. 梯形速度规划参数 ---
    const float MAX_SPEED = 1.0f;       // 最大限制速度 (m/s)
    const float ACC = 0.5f;             // 加速度 (m/s^2)
    const float DEC = 0.5f;             // 减速度 (m/s^2)
    
    // 初始速度：取当前车速，但在0和MAX_SPEED之间钳位
    float start_v = std::max(0.0f, std::min(static_cast<float>(start_loc.velocity), MAX_SPEED));
    float end_v = 0.0f; 

    // 计算加速段需要的距离
    float dist_acc = (MAX_SPEED * MAX_SPEED - start_v * start_v) / (2 * ACC);
    // 计算减速段需要的距离
    float dist_dec = (MAX_SPEED * MAX_SPEED - end_v * end_v) / (2 * DEC);

    float peak_v = MAX_SPEED; // 实际能达到的峰值速度

    // 如果距离不够加速到最大速度再减速
    if (dist_acc + dist_dec > total_dist) {
        // 重新计算峰值速度 v_peak
        // s_total = (vp^2 - v0^2)/2a + vp^2/2d
        // 2*a*d*s + d*v0^2 = (d+a)*vp^2
        float numerator = 2 * ACC * DEC * total_dist + DEC * start_v * start_v;
        float denominator = ACC + DEC;
        peak_v = std::sqrt(numerator / denominator);

        // 更新各段距离
        dist_acc = (peak_v * peak_v - start_v * start_v) / (2 * ACC);
        dist_dec = total_dist - dist_acc;
        
        AINFO_IF(FLAGS_enable_log) << "[Leveling] 距离较短，执行三角形速度规划，峰值速度: " << peak_v << " m/s";
    } else {
        // 梯形规划，中间有匀速段
        // dist_acc 和 dist_dec 保持不变
        AINFO_IF(FLAGS_enable_log) << "[Leveling] 执行梯形速度规划，加速距: " << dist_acc << ", 减速距: " << dist_dec;
    }

    // --- 3. 循环生成点 ---
    float current_s = 0.0f;
    float current_t = 0.0f;
    float current_v = start_v;

    avos::vectormap::PointGCCS vehicle_pose;
    vehicle_pose.xg = start_loc.xg;
    vehicle_pose.yg = start_loc.yg;
    vehicle_pose.angle = start_loc.yaw;

    // 使用距离作为循环条件，防止死循环
    while (current_s < total_dist) {
        PlanningTrajectoryPoint point;
        point.Clear();

        // 计算下一时刻的预估速度
        float next_v = current_v;

        // 状态机：加速 -> 匀速 -> 减速
        if (current_s < dist_acc) {
            // 加速段
            next_v = current_v + ACC * TIME_STEP;
            if (next_v > peak_v) next_v = peak_v;
        } 
        else if (current_s >= (total_dist - dist_dec)) {
            // 减速段
            float s_remain = total_dist - current_s;
            // 简单的匀减速预测
            next_v = current_v - DEC * TIME_STEP;
            if (next_v < 0.0f) next_v = 0.0f;
        } 
        else {
            // 匀速段
            next_v = peak_v;
        }

        // 更新位移 (使用梯形面积法计算一步的距离)
        float ds = (current_v + next_v) * 0.5f * TIME_STEP;
        
        // 边界保护：不要超调
        if (current_s + ds > total_dist) {
            ds = total_dist - current_s;
            next_v = 0.0f; 
        }

        current_s += ds;
        current_t += TIME_STEP;
        current_v = next_v;

        // --- 填充点 ---
        double ratio = (total_dist > 0.001) ? (current_s / total_dist) : 1.0;
        
        point.xg = start_loc.xg + dx * ratio;
        point.yg = start_loc.yg + dy * ratio;
        point.angleglobal = static_cast<float>(target_heading_deg);
        point.velocity = current_v;
        point.t = current_t;
        point.type = 0; 

        // 坐标转换
        avos::vectormap::PointGCCS curr_point_global;
        avos::vectormap::PointVCS curr_point_vcs;
        curr_point_global.xg = point.xg;
        curr_point_global.yg = point.yg;
        curr_point_global.angle = point.angleglobal;
        m_transform_.GCCS2VCS(vehicle_pose, curr_point_global, curr_point_vcs);

        point.x = curr_point_vcs.x;
        point.y = curr_point_vcs.y;
        
        double rel_angle = curr_point_vcs.angle;
        while (rel_angle > 180.0) rel_angle -= 360.0;
        while (rel_angle < -180.0) rel_angle += 360.0;
        point.angle = static_cast<float>(rel_angle);

        trajectory.trajectory.push_back(point);

        // 终止条件：到达终点或速度极小且接近终点
        if (current_s >= total_dist || (std::abs(total_dist - current_s) < 0.05 && current_v < 0.01)) {
            break;
        }
    }

    // 强保底：确保最后一个点完全静止
    if (!trajectory.trajectory.empty()) {
        trajectory.trajectory.back().velocity = 0.0f;
        trajectory.trajectory.back().a = 0.0f;
    }

    // [调试] 打印轨迹点的速度分布，排查是否因为速度过小导致车辆不动
    if (!trajectory.trajectory.empty()) {
        std::stringstream ss_vel;
        ss_vel << "[Leveling 速度诊断] 总距离: " << total_dist 
               << "m, 峰值速度(PeakV): " << peak_v 
               << "m/s, 起始速度(StartV): " << start_v 
               << "m/s\n>>> 逐点速度(m/s): ";
        
        // 遍历打印每个点的速度
        for (size_t i = 0; i < trajectory.trajectory.size(); ++i) {
            ss_vel << trajectory.trajectory[i].velocity;
            if (i < trajectory.trajectory.size() - 1) {
                ss_vel << ", ";
            }
        }
        
        // 输出到日志
        AINFO_IF(FLAGS_enable_log) << ss_vel.str();
    } else {
        AWARN_IF(FLAGS_enable_log) << "[Leveling 速度诊断] 轨迹为空，未生成任何点！";
    }
}

void Leveling::InitDebugInfo(const ara::adsfi::PerceptionStaticEnv& map_in, ara::adsfi::PerceptionStaticEnv& debug_out) {
    debug_out.header = map_in.header;
    debug_out.header.data_name = "Leveling_Debug_Info"; 
    debug_out.cell_size = map_in.terrain_patch_size;
    debug_out.terrain_patch_size = map_in.terrain_patch_size;
    debug_out.terrain_patches.clear(); 
    debug_out.terrain_patches.reserve(map_in.terrain_patches.size() * 2); 
}

void Leveling::ComputeSearchContext(LevelingSearchContext& ctx) {
    // 统一配置参数
    ctx.patch_size = 0.6f; 
    ctx.coverage_threshold = 0.70; 
    ctx.max_height_diff_threshold = 0.20f; // 20cm
    ctx.maintain_coverage_threshold = 0.50; 
    ctx.maintain_height_diff_threshold = 0.35f;

    // 根据车身尺寸计算窗口
    float max_dim = std::max(VEHICLE_LENGTH, VEHICLE_WIDTH);
    ctx.grid_span = static_cast<int>(std::ceil(max_dim / ctx.patch_size));
    
    // 强制奇数，保证中心对齐
    if (ctx.grid_span % 2 == 0) ctx.grid_span += 1;
    
    ctx.half_span = ctx.grid_span / 2;
    ctx.total_points_needed = ctx.grid_span * ctx.grid_span;
}

bool Leveling::DetermineTargetPoint(const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map,
                                    const avos::planning::LocalizationData& loc_data,
                                    const LevelingSearchContext& ctx,
                                    double& out_x, double& out_y, double& out_z,
                                    ara::adsfi::PerceptionStaticEnv& debug_out) {
    bool found = false;

    // --- 分支 A: 尝试复核已锁定的目标 ---
    if (is_target_locked_) {
        //仿真时要打开
        // for (const auto& kv : flat_nodes_map) {
        //     float z_diff_temp = 0.0f;
        //     double cov_temp = CalculateRegionCoverage(kv.first, flat_nodes_map, ctx, z_diff_temp);
            
        //     int tier_temp = 4;
        //     double dist_sq_temp = 0.0;
        //     ComputeCandidateTier(kv.second, ctx, tier_temp, dist_sq_temp);
        //     AddDebugInfo(kv.second, cov_temp, z_diff_temp, tier_temp, ctx, debug_out);
        // }

        double current_cov = 0.0;
        float current_z_diff = 0.0;
        
        bool check_pass = VerifyLockedTarget(flat_nodes_map, ctx, current_cov, current_z_diff);

        int current_tier = 4; 
        double dummy_dist = 0.0;
        
        // 计算锁定点的最新状态
        ComputeCandidateTier(locked_target_, ctx, current_tier, dummy_dist);
        // 锁定点
        AddDebugInfo(locked_target_, current_cov, current_z_diff, current_tier, ctx, debug_out);

        if (check_pass) {
            // 复核通过
            AINFO_IF(FLAGS_enable_log) << "[Leveling] 维持锁定(稳): Cov=" << (int)(current_cov*100) 
                  << "%, Z_Diff=" << current_z_diff;
            
            out_x = locked_target_.x;
            out_y = locked_target_.y;
            out_z = locked_target_.z;
            found = true;
            unlock_counter_ = 0; 
        } else {
            // 复核失败 -> 进入容错计数
            unlock_counter_++;
            AWARN_IF(FLAGS_enable_log) << "[Leveling] 锁定点数据异常! 计数器: " << unlock_counter_ << "/" << MAX_UNLOCK_FRAMES
                  << " (Cov=" << current_cov << ", Z=" << current_z_diff << ")";

            if (unlock_counter_ <= MAX_UNLOCK_FRAMES) {
                out_x = locked_target_.x;
                out_y = locked_target_.y;
                out_z = locked_target_.z;
                found = true;
            } else {
                // 解锁
                AERROR_IF(FLAGS_enable_log) << "[Leveling] 锁定点连续失效，触发重搜!";
                is_target_locked_ = false; 
                unlock_counter_ = 0;
            }
        }
    }

    // --- 分支 B: 全局重新搜索 ---
    if (!is_target_locked_) {
        found = FindBestFlatSpot(flat_nodes_map, loc_data, ctx,
         out_x, out_y, out_z, debug_out);
        
        if (found) {
            // 建立新锁定
            is_target_locked_ = true;
            locked_target_.x = out_x;
            locked_target_.y = out_y;
            locked_target_.z = out_z;
            unlock_counter_ = 0;// 新锁定，计数器清零
            AINFO_IF(FLAGS_enable_log) << "[Leveling] 建立新锁定 -> (" << out_x << ", " << out_y << ")";
        }
    }

    return found;
}

void Leveling::RecordFinalTargetDebug(const avos::planning::LocalizationData& loc_data,
                                      double target_x, double target_y, double target_z,
                                      ara::adsfi::PerceptionStaticEnv& debug_out) {
    // 1. 坐标转换 GCCS -> VCS
    avos::vectormap::PointGCCS target_global;
    target_global.xg = target_x;
    target_global.yg = target_y;
    target_global.angle = 0.0; 

    avos::vectormap::PointGCCS vehicle_pose;
    vehicle_pose.xg = loc_data.xg;
    vehicle_pose.yg = loc_data.yg;
    vehicle_pose.angle = loc_data.yaw;

    avos::vectormap::PointVCS target_vcs;
    m_transform_.GCCS2VCS(vehicle_pose, target_global, target_vcs);

    // 2. 填充 Type 17
    ara::adsfi::PerceptionTerrainPatch final_patch;
    final_patch.patch_center.x = target_vcs.x;
    final_patch.patch_center.y = target_vcs.y;
    final_patch.patch_center.z = target_z;
    final_patch.type = DBG_TARGET_FINAL;
    final_patch.slopx = 0.0; 
    final_patch.slopy = 0.0;
    
    debug_out.terrain_patches.push_back(final_patch);

    // 3. 打印日志
    AINFO_IF(FLAGS_enable_log) 
          << "[Leveling] 锁定目标点: "
          << "  -> [UTM] x:" << target_x << ", y:" << target_y 
          << "  -> [VCS] x:" << target_vcs.x << ", y:" << target_vcs.y 
          << " (Dist: " << std::hypot(target_vcs.x, target_vcs.y) << "m)";
}

bool Leveling::VerifyLockedTarget(const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map,
                                  const LevelingSearchContext& ctx,
                                  double& current_coverage,
                                  float& current_z_diff) {
    // 将绝对坐标转回 GridIndex
    int idx_x = static_cast<int>(std::round(locked_target_.x / ctx.patch_size));
    int idx_y = static_cast<int>(std::round(locked_target_.y / ctx.patch_size));
    GridIndex target_idx = {idx_x, idx_y};

    // 1.检查目标中心点是否还在哈希表中？
    // 如果不在，说明感知这一帧根本没输出这个位置的数据（可能是盲区，也可能是被剔除了）
    bool center_exist = (flat_nodes_map.find(target_idx) != flat_nodes_map.end());

    // 2.复用计算函数获取核心指标
    current_coverage = CalculateRegionCoverage(target_idx, flat_nodes_map, ctx, current_z_diff);

    // 3.计算判断结果
    bool is_cov_pass = (current_coverage >= ctx.maintain_coverage_threshold);
    bool is_z_pass = (current_z_diff <= ctx.maintain_height_diff_threshold);

    // 打印诊断日志
    if (!is_cov_pass || !is_z_pass) {
        // 计算目标点距离当前车辆的距离，用于判断是否是因为"开太近导致进入盲区"
        double dist_to_car = std::hypot(locked_target_.x - ctx.current_x, locked_target_.y - ctx.current_y);

        AWARN_IF(FLAGS_enable_log) << "[Leveling Check] 锁定点复核失败! 详情如下:"
              << "\n  > 失败原因: " << (!is_cov_pass ? "[覆盖率不足] " : "") << (!is_z_pass ? "[高差过大(不平)] " : "")
              << "\n  > 当前数据: 覆盖率=" << current_coverage << " (阈值" << ctx.maintain_coverage_threshold << ")"
              << ", 高差=" << current_z_diff << " (阈值" << ctx.maintain_height_diff_threshold << ")"
              << "\n  > 地图状态: 中心点Key" << (center_exist ? "存在" : "丢失(感知未输出该点)")
              << "\n  > 相对距离: " << dist_to_car << "米";
    }

    return is_cov_pass && is_z_pass;
}

bool Leveling::IsTargetReached(const avos::planning::LocalizationData& loc_data) {
    // 1. 如果没有锁定目标，肯定不算到达
    if (!is_target_locked_) {
        stopped_duration_ = 0.0; // 安全复位
        return false;
    }

    // 2. 计算当前位置与锁定目标的欧氏距离
    double dx = loc_data.xg - locked_target_.x;
    double dy = loc_data.yg - locked_target_.y;
    double dist = std::hypot(dx, dy);

    // 3. 判定阈值配置
    const double SPEED_THRESHOLD = 0.05;  // 0.05m/s (5cm/s) 以下认为停稳，比0.1更严格
    const double TIME_THRESHOLD = 2.0;    // 必须持续停稳 2.0 秒才允许切换

    // 4. 判断单帧状态
    bool is_close_enough = (dist < DIST_THRESHOLD_REACHED);
    bool is_stopped = (std::abs(loc_data.velocity) < SPEED_THRESHOLD);

    // 5. 时间窗确认逻辑
    const double delta_t = 0.1;

    if (is_close_enough && is_stopped) {
        stopped_duration_ += delta_t; // 累加停稳时间
        
        AINFO_IF(FLAGS_enable_log) << "[Leveling] 到位确认中,持续时长: " << stopped_duration_ << "s / " << TIME_THRESHOLD << "s";

        if (stopped_duration_ >= TIME_THRESHOLD) {
            // 只有当持续时间满足要求，才返回 true
            return true;
        }
    } else {
        // 一旦任何一帧不满足，计时器立即清零
        stopped_duration_ = 0.0;
    }

    return false;
}
} // namespace planning
} // namespace avos
