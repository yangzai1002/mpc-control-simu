#include "cartesian_frenet_converter.h"
#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm> // 引入 algorithm 用于 std::max / std::min

namespace localplanner {
namespace frenet {

bool CartesianFrenetConverter::cartesian_to_frenet(
    const std::vector<ref_line::ReferencePoint>& ref_line,
    const double x, const double y,
    FrenetPoint& f_point) {
    
    if (ref_line.size() < 2) {
        std::cerr << "[FrenetConverter] Reference line points are too few for conversion!" << std::endl;
        return false;
    }

    // 1. 找到距离目标点最近的离散点索引
    size_t match_index = find_nearest_index(ref_line, x, y);

    // 2. 寻找真实的投影线段（判断是与前一个点还是后一个点构成最佳投影线段）
    size_t next_index = match_index;
    if (match_index == 0) {
        next_index = 1;
    } else if (match_index == ref_line.size() - 1) {
        next_index = match_index;
        match_index = match_index - 1;
    } else {
        // 利用向量点乘判断投影点在哪一侧
        double dx = x - ref_line[match_index].x;
        double dy = y - ref_line[match_index].y;
        
        // 向量：match -> match+1
        double dx1 = ref_line[match_index + 1].x - ref_line[match_index].x;
        double dy1 = ref_line[match_index + 1].y - ref_line[match_index].y;

        if (dx * dx1 + dy * dy1 > 0) {
            next_index = match_index + 1;
        } else {
            next_index = match_index;
            match_index = match_index - 1;
        }
    }

    // 3. 解析几何：计算精确投影
    const auto& p1 = ref_line[match_index];
    const auto& p2 = ref_line[next_index];
    
    double dx_ref = p2.x - p1.x;
    double dy_ref = p2.y - p1.y;
    double segment_len_sq = dx_ref * dx_ref + dy_ref * dy_ref;
    
    // 如果两点重合，直接返回最近点信息
    if (segment_len_sq < 1e-6) {
        f_point.s = p1.s;
        f_point.l = std::hypot(x - p1.x, y - p1.y);
        return true;
    }

    // 计算投影比例 projection_ratio
    double dx_target = x - p1.x;
    double dy_target = y - p1.y;
    double proj = (dx_target * dx_ref + dy_target * dy_ref) / segment_len_sq;
    
    // 限制在段内 [0, 1]
    proj = std::max(0.0, std::min(1.0, proj));

    // 计算精确的投影点坐标
    double proj_x = p1.x + proj * dx_ref;
    double proj_y = p1.y + proj * dy_ref;

    // 4. 赋值 S 和 L
    f_point.s = p1.s + proj * std::hypot(dx_ref, dy_ref);
    
    // 通过叉乘计算横向偏移 L 及正负号 (左正右负)
    double cross_prod = dx_ref * dy_target - dy_ref * dx_target;
    f_point.l = (cross_prod > 0 ? 1.0 : -1.0) * std::hypot(x - proj_x, y - proj_y);

    return true;
}

bool CartesianFrenetConverter::frenet_to_cartesian(
    const std::vector<ref_line::ReferencePoint>& ref_line,
    const FrenetPoint& f_point,
    double& x, double& y) {
    
    if (ref_line.empty()) {
        std::cerr << "[FrenetConverter] Reference line is empty!" << std::endl;
        return false;
    }

    // 简化的反投影逻辑：通过遍历找到 S 最接近的匹配点
    // TODO: 量产代码可在此处替换为二分查找 (std::lower_bound) 优化时间复杂度
    size_t match_index = 0;
    double min_s_diff = std::numeric_limits<double>::max();
    
    for (size_t i = 0; i < ref_line.size(); ++i) {
        double diff = std::abs(ref_line[i].s - f_point.s);
        if (diff < min_s_diff) {
            min_s_diff = diff;
            match_index = i;
        }
    }

    const auto& match_point = ref_line[match_index];
    
    // 根据 L 偏移量，沿着匹配点的法线方向推算 X, Y
    // 假设航向角为 theta，法线方向向量为 (-sin(theta), cos(theta)) (左正右负)
    double cos_theta = std::cos(match_point.theta);
    double sin_theta = std::sin(match_point.theta);

    x = match_point.x - f_point.l * sin_theta;
    y = match_point.y + f_point.l * cos_theta;

    return true;
}

size_t CartesianFrenetConverter::find_nearest_index(
    const std::vector<ref_line::ReferencePoint>& ref_line,
    const double x, const double y) {
    
    size_t min_index = 0;
    double min_dist_sq = std::numeric_limits<double>::max();

    for (size_t i = 0; i < ref_line.size(); ++i) {
        double dx = x - ref_line[i].x;
        double dy = y - ref_line[i].y;
        double dist_sq = dx * dx + dy * dy;

        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            min_index = i;
        }
    }
    return min_index;
}

} // namespace frenet
} // namespace localplanner