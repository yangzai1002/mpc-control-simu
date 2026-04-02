#pragma once

#include <vector>
#include <Eigen/Dense>
#include <OsqpEigen/OsqpEigen.h>
#include "ref_data_types.h"

namespace localplanner {
namespace ref_line {

/**
 * @brief 基于二次规划 (QP) 的离散点参考线平滑器
 * @details 利用 OSQP 求解器，在保证不超出安全走廊(Boundary)的前提下，最优化路径的平滑度、紧凑度和贴合度
 */
class ReferenceLineSmoother {
public:
    /**
     * @brief 构造函数，初始化平滑器配置参数
     * @param[in] config 包含权重和步长的配置对象
     */
    explicit ReferenceLineSmoother(const SmootherConfig& config);

    /**
     * @brief 平滑算法主入口 (业务流水线)
     * @param[in] input 输入的稀疏原始点集
     * @param[out] output 输出平滑后且计算完几何属性的参考线点集
     * @return 规划求解是否成功
     */
    bool Execute(const std::vector<RawPoint>& input, 
                 std::vector<ReferencePoint>& output);

private:
    /**
     * @brief 核心数学逻辑：构建 QP 矩阵并调用 OSQP 求解
     * @param[in] dense 致密化后的输入点集
     * @param[out] opt 优化后的 (x, y) 坐标集
     * @return OSQP 求解器是否成功收敛
     */
    bool SolveQP(const std::vector<RawPoint>& dense, 
                 std::vector<RawPoint>& opt);

    SmootherConfig config_; ///< 平滑器算法配置
};

} // namespace ref_line
} // namespace localplanner