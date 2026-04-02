#pragma once

#include <vector>

namespace localplanner {
namespace ref_line {

/**
 * @brief 原始输入点数据结构
 * @details 用于接收上游（如全局路由规划）传来的稀疏离散路径点
 */
struct RawPoint {
    double x = 0.0; ///< 笛卡尔坐标系 X (m)
    double y = 0.0; ///< 笛卡尔坐标系 Y (m)
};

/**
 * @brief 平滑参考线点数据结构
 * @details 包含下游控制（LQR/MPC）及 Frenet 坐标转换所需的所有运动学和几何属性
 */
struct ReferencePoint {
    double x = 0.0;      ///< 优化后的 X (m)
    double y = 0.0;      ///< 优化后的 Y (m)
    double s = 0.0;      ///< 累计物理弧长 (m)，即 Frenet 坐标系的 S 轴
    double theta = 0.0;  ///< 航向角 (rad)，范围 [-PI, PI]，与 X 轴夹角
    double kappa = 0.0;  ///< 曲率 (1/m)，反映路径弯曲程度
};

/**
 * @brief 平滑器算法权重与物理约束配置
 * @details 调参核心：通过调节权重来平衡轨迹的“顺滑度”和“精确度”
 */
struct SmootherConfig {
    double weight_ref = 1.0;     ///< 贴合度权重：数值越大，平滑后的线越靠近原始点
    double weight_len = 10.0;    ///< 紧凑度权重：类似于拉紧橡皮筋，让点间距更均匀
    double weight_cur = 500.0;   ///< 平滑度权重：最关键参数，越大则轨迹越丝滑，方向盘越柔和
    double boundary = 0.5;       ///< 偏移约束：允许平滑点偏离原始点的最大物理距离 (m)
    double sample_step = 0.5;    ///< 采样步长：预处理时将稀疏点加密到该间距 (m)
};

} // namespace ref_line
} // namespace localplanner