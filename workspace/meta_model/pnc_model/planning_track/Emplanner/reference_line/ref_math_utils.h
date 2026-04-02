#pragma once

#include "ref_data_types.h"
#include <vector>

namespace localplanner {
namespace ref_line {

/**
 * @brief 参考线模块专用数学工具类
 * @details 提供线性插值致密化和几何属性 (s, theta, kappa) 计算的静态方法
 */
class RefMath {
public:
    RefMath() = delete; // 纯工具类，禁止实例化

    /**
     * @brief 线性插值致密化
     * @details 将稀疏的点阵按指定步长加密，以便进行更精确的二次规划和平滑
     * @param[in] raw 原始稀疏点集
     * @param[out] dense 输出的致密化点集
     * @param[in] step 插值步长 (m)
     */
    static void Densify(const std::vector<RawPoint>& raw, 
                        std::vector<RawPoint>& dense, 
                        const double step);

    /**
     * @brief 计算路径的几何运动学属性
     * @details 根据平滑后的 (x, y) 坐标推算 s, theta, kappa
     * @param[in,out] path 待计算的参考线点集（传入时需已包含合法的 x, y）
     */
    static void ComputeAttributes(std::vector<ReferencePoint>& path);
};

} // namespace ref_line
} // namespace localplanner