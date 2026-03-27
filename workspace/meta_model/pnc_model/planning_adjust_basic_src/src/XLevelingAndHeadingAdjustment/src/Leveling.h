/**
 * @file Leveling.h
 * @brief 车辆自动找平功能处理类
 * @details 基于感知地势图筛选平坦区域并生成行驶轨迹
 * @author ljq
 */

#pragma once

#include "msghandle.hpp"
#include "CommonHeader.h"
#include "PlanningTrajectory.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "XLevelingAndHeadingAdjustment.h" 
#include <vector>
#include <unordered_map>
#include <cmath>
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "common_header/geotool.h"
namespace avos {
namespace planning {

struct GridIndex {
    int x_idx;
    int y_idx;

    bool operator==(const GridIndex& other) const {
        return x_idx == other.x_idx && y_idx == other.y_idx;
    }
};

// 哈希函数
struct GridIndexHash {
    std::size_t operator()(const GridIndex& k) const {
        return std::hash<int>()(k.x_idx) ^ (std::hash<int>()(k.y_idx) << 1);
    }
};

// 定义调试数据的 Type 枚举
enum LevelingDebugType {
    DBG_PATCH_RAW_OK    = 10,  // 原始点-坡度合格 (绿)
    DBG_PATCH_SLOPE_BAD = 11,  // 原始点-坡度超标 (红)
    DBG_CANDIDATE_TIER1 = 12,  // 候选-Tier1 (正前方)  建议颜色: 深蓝
    DBG_CANDIDATE_TIER2 = 13,  // 候选-Tier2 (微调)    建议颜色: 蔚蓝
    DBG_CANDIDATE_TIER3 = 14,  // 候选-Tier3 (转向)    建议颜色: 天蓝
    DBG_CANDIDATE_TIER4 = 15,  // 候选-Tier4 (保底)    建议颜色: 青色
    DBG_SPOT_POOR_COV   = 16,  // 不合格 (覆盖率/高差挂了)  建议颜色: 黄色
    DBG_TARGET_FINAL    = 17   // 最终目标  建议颜色: 紫色
};

class Leveling {
public:
    Leveling();
    ~Leveling();

    /**
     * @brief 执行找平逻辑的核心接口
     * @param command 业务指令
     * @param localization_data 车辆当前定位
     * @param semantic_map 感知语义地图（包含地势图）
     * @param trajectory [输出] 规划出的轨迹
     * @return true 成功规划, false 失败（无平坦区域或参数错误）
     */
    bool Process(const ara::adsfi::BusinessCommand& command,
                 const avos::planning::LocalizationData& localization_data,
                 const ara::adsfi::PerceptionStaticEnv& semantic_map,
                 PlanningTrajectory& trajectory,
                 ara::adsfi::PerceptionStaticEnv& terrain_debug);
    void Reset() {
        is_target_locked_ = false;       // 解锁
        unlock_counter_ = 0;             // 清零计数器
        locked_target_ = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // 清空缓存点
        stopped_duration_ = 0.0; // 复位计时器
    }
    //判断是否到达锁定的目标点
    bool IsTargetReached(const avos::planning::LocalizationData& loc_data);
    
private:
    avos::planning::CoordTransform m_transform_;
    // --- 配置参数 ---
    const float VEHICLE_LENGTH = 5.0f;       // 车辆长度 (m)
    const float VEHICLE_WIDTH = 2.8f;        // 车辆宽度 (m)
    const float SLOPE_THRESHOLD_DEG = 15.0f; // 坡度阈值 (度)
    const float TIME_STEP = 0.1f;            // 轨迹点时间间隔 (s)
    const float CRUISE_VELOCITY = 1.0f;      // 巡航速度 (m/s)
    const double DIST_THRESHOLD_REACHED = 0.5;// 到达判定距离 (30cm)
    // --- 辅助结构 ---
    struct FlatPatchNode {
        float x;
        float y;
        float z;
        float slopx; 
        float slopy;
    };

    struct LevelingSearchContext {
        // 输入参数
        double current_x;
        double current_y;
        double current_heading;
        float patch_size;
        int grid_span;              // 搜索窗口边长 (格子数)
        int half_span;              // 窗口半径
        double coverage_threshold;  // 覆盖率阈值
        int total_points_needed;    // 窗口总格子数
        float max_height_diff_threshold; // Z轴最大允许高差
        double maintain_coverage_threshold;       // e.g. 0.60
        float maintain_height_diff_threshold;     // e.g. 0.30

        // 运行统计状态
        int valid_area_count = 0;
        int cov_stats[6] = {0};     // 覆盖率分布统计
        double max_coverage = -1.0; // 全图最高覆盖率
        GridIndex max_cov_idx = {0,0}; // 最高覆盖率对应的索引

        // 最佳候选点缓存 (Tier 1 ~ Tier 4)
        double min_dist[5];         // index 0 unused, 1~4 used
        const FlatPatchNode* best_nodes[5]; // index 0 unused

        // 构造函数初始化
        LevelingSearchContext() {
            std::fill(std::begin(min_dist), std::end(min_dist), std::numeric_limits<double>::max());
            std::fill(std::begin(best_nodes), std::end(best_nodes), nullptr);
        }
    };

private:
    bool is_target_locked_;          // 是否已锁定目标
    FlatPatchNode locked_target_;    // 锁定的目标点信息 (绝对坐标)
    int unlock_counter_;
    double stopped_duration_ = 0.0;  // 记录满足停止条件的累计时间 (秒)
    const int MAX_UNLOCK_FRAMES = 100; // 连续 10 帧失败才解锁

    /**
     * @brief 初始化 Debug 消息头
     */
    void InitDebugInfo(const ara::adsfi::PerceptionStaticEnv& map_in, ara::adsfi::PerceptionStaticEnv& debug_out);

    /**
     * @brief 计算搜索上下文参数 (阈值、窗口尺寸等)
     */
    void ComputeSearchContext(LevelingSearchContext& ctx);

    /**
     * @brief 核心决策 (状态机：复核锁定 -> 全局搜索)
     * @return true=找到目标, false=失败
     */
    bool DetermineTargetPoint(const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map,
                              const avos::planning::LocalizationData& loc_data,
                              const LevelingSearchContext& ctx,
                              double& out_x, double& out_y, double& out_z,
                              ara::adsfi::PerceptionStaticEnv& debug_out);

    /**
     * @brief 记录最终目标点的调试信息 (含坐标转换和日志)
     */
    void RecordFinalTargetDebug(const avos::planning::LocalizationData& loc_data,
                                double target_x, double target_y, double target_z,
                                ara::adsfi::PerceptionStaticEnv& debug_out);


    /**
     * @brief 构建平坦点的高效查询索引
     * @param map_data 感知地图数据
     * @param patch_size 单个格子的物理尺寸
     * @param flat_nodes_map [输出] 存储平坦点的哈希表
     */
    void BuildFlatGridMap(const ara::adsfi::PerceptionStaticEnv& map_data, const avos::planning::LocalizationData& localization_data,
        const float patch_size, std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map, ara::adsfi::PerceptionStaticEnv& terrain_debug);

    /**
     * @brief 搜索最佳平坦区域中心
     * @param flat_nodes_map 平坦点哈希表
     * @param current_x 车辆当前X
     * @param current_y 车辆当前Y
     * @param patch_size 格子尺寸
     * @param target_x [输出] 目标点X
     * @param target_y [输出] 目标点Y
     * @param target_z [输出] 目标点Z
     * @return 是否找到有效区域
     */
    bool FindBestFlatSpot(const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map,
                        const avos::planning::LocalizationData& loc_data,
                        const LevelingSearchContext& ctx,
                        double& target_x, double& target_y, double& target_z,
                        ara::adsfi::PerceptionStaticEnv& terrain_debug);

    /**
     * @brief 生成点对点直线轨迹
     */
    void GenerateTrajectory(const avos::planning::LocalizationData& start_loc,
                            double target_x, double target_y, double target_z,
                            PlanningTrajectory& trajectory);
    // 计算指定区域的覆盖率
    double CalculateRegionCoverage(const GridIndex& center_idx, 
                                   const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& map,
                                   const LevelingSearchContext& ctx, float& out_z_diff);

    // 计算候选点的几何信息和 Tier 等级
    void ComputeCandidateTier(const FlatPatchNode& node, 
                              const LevelingSearchContext& ctx,
                              int& out_tier, double& out_dist_sq);

    // 填充并记录调试信息 (Debug Info)
    void AddDebugInfo(const FlatPatchNode& node, const double coverage, const float z_diff, const int tier, 
                            const LevelingSearchContext& ctx, ara::adsfi::PerceptionStaticEnv& terrain_debug);

    // 打印
    void PrintFailureAnalysis(const LevelingSearchContext& ctx,
                              const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& map);
    /**
     * @brief 复核当前锁定的目标点是否依然有效
     * @details 检查锁定点在当前最新的地图数据中，覆盖率和高差是否依然满足要求
     */
    bool VerifyLockedTarget(const std::unordered_map<GridIndex, FlatPatchNode, GridIndexHash>& flat_nodes_map,
                            const LevelingSearchContext& ctx, // 复用上下文配置
                            double& current_coverage,         // [输出] 当前最新的覆盖率
                            float& current_z_diff);           // [输出] 当前最新的高差
};

} // namespace planning
} // namespace avos
