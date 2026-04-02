#include "ref_math_utils.h"
#include <cmath>

namespace localplanner {
namespace ref_line {

void RefMath::Densify(const std::vector<RawPoint>& raw, 
                      std::vector<RawPoint>& dense, 
                      const double step) {
    dense.clear();
    if (raw.size() < 2) return;

    for (size_t i = 0; i < raw.size() - 1; ++i) {
        double dx = raw[i+1].x - raw[i].x;
        double dy = raw[i+1].y - raw[i].y;
        double dist = std::hypot(dx, dy); 
        
        int num = static_cast<int>(dist / step); 
        for (int j = 0; j < num; ++j) {
            double ratio = static_cast<double>(j) / num;
            dense.push_back({raw[i].x + dx * ratio, raw[i].y + dy * ratio});
        }
    }
    dense.push_back(raw.back()); // 确保存入最后一个原始点
}

void RefMath::ComputeAttributes(std::vector<ReferencePoint>& path) {
    if (path.size() < 2) return;
    
    // 1. 计算累计弧长 s 和航向角 theta
    path[0].s = 0.0;
    for (size_t i = 1; i < path.size(); ++i) {
        double dx = path[i].x - path[i-1].x;
        double dy = path[i].y - path[i-1].y;
        path[i].s = path[i-1].s + std::hypot(dx, dy);
        path[i].theta = std::atan2(dy, dx); 
    }
    path[0].theta = path[1].theta; // 起点航向角用第二点代替

    // 2. 计算曲率 kappa (通过航向角的变化率计算)
    for (size_t i = 1; i < path.size() - 1; ++i) {
        double ds = path[i+1].s - path[i-1].s;
        double dtheta = path[i+1].theta - path[i-1].theta;
        
        // 关键：处理 atan2 带来的角度突变 (-PI 跃迁到 PI)
        while (dtheta > M_PI)  dtheta -= 2.0 * M_PI;
        while (dtheta < -M_PI) dtheta += 2.0 * M_PI;
        
        path[i].kappa = dtheta / (ds + 1e-6); // kappa = d_theta / d_s，加极小值防除零
    }
    
    // 边界点曲率简单赋值，保证数组完整性
    path[0].kappa = path[1].kappa;
    path.back().kappa = path[path.size()-2].kappa;
}

} // namespace ref_line
} // namespace localplanner