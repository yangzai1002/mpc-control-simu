#pragma once

#include <vector>
#include "frenet_data_types.h"
#include "../reference_line/ref_data_types.h"

namespace localplanner {
namespace frenet {

/**
 * @brief 笛卡尔坐标系 (Cartesian) 与 Frenet 坐标系双向转换器
 * @details 提供静态方法，依赖于外部传入的平滑参考线进行坐标投影计算
 */
class CartesianFrenetConverter {
public:
    CartesianFrenetConverter() = delete; // 纯工具类，禁止实例化

    /**
     * @brief 将真实世界坐标 (X, Y) 转换为 Frenet 坐标 (S, L)
     * @param[in] ref_line 平滑且带有 s, theta 属性的参考线点集
     * @param[in] x 目标点的真实 X 坐标
     * @param[in] y 目标点的真实 Y 坐标
     * @param[out] f_point 输出的 Frenet 坐标点
     * @return 转换是否成功 (若参考线点数不足则返回 false)
     */
    static bool cartesian_to_frenet(const std::vector<ref_line::ReferencePoint>& ref_line,
                                    const double x, const double y,
                                    FrenetPoint& f_point);

    /**
     * @brief 将 Frenet 坐标 (S, L) 转换为真实世界坐标 (X, Y)
     * @param[in] ref_line 平滑且带有 s, theta 属性的参考线点集
     * @param[in] f_point 待转换的 Frenet 坐标点
     * @param[out] x 输出的笛卡尔 X 坐标
     * @param[out] y 输出的笛卡尔 Y 坐标
     * @return 转换是否成功
     */
    static bool frenet_to_cartesian(const std::vector<ref_line::ReferencePoint>& ref_line,
                                    const FrenetPoint& f_point,
                                    double& x, double& y);

private:
    /**
     * @brief 内部辅助函数：寻找参考线上距离目标点最近的离散点索引
     * @param[in] ref_line 参考线点集
     * @param[in] x 目标 X 坐标
     * @param[in] y 目标 Y 坐标
     * @return 最近点的数组索引
     */
    static size_t find_nearest_index(const std::vector<ref_line::ReferencePoint>& ref_line,
                                     const double x, const double y);
};

} // namespace frenet
} // namespace localplanner