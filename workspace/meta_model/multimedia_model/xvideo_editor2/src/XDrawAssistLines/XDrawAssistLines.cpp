#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>

#include "DatetimeUtil.hpp"
#include "VideoParameter.h"
#include "XImagePathPlan.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "protocol/cusermdc610funcinterface.h"
#include "shm_util/zero_copy_shm_mem.h"
#include "time_cost.hpp"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080

#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

#define MAX_UNCHANGED 90

// ============================================================================
// 配置缓存结构（优化1: 避免重复读取配置）
// ============================================================================
struct CameraConfigCache {
    bool initialized = false;

    // 前视相机参数
    double front_x = 0.0;
    double front_left = 0.0;
    double front_right = 0.0;
    double front_height = 0.0;

    // 后视相机参数
    double back_x = 0.0;
    double back_left = 0.0;
    double back_right = 0.0;
    double back_height = 0.0;

    // 速度自适应参数
    float base_width = 1.2f;       // 基础边界宽度
    float min_speed_factor = 0.8f; // 低速时的宽度因子
    float max_speed_factor = 1.5f; // 高速时的宽度因子
    float max_velocity = 15.0f;    // 最大速度 (m/s)
};

static CameraConfigCache g_camera_config;

// ============================================================================
// 配置初始化函数（仅在首次调用时执行）
// ============================================================================
bool initCameraConfigOnce(const CameraParam& cam_param) {
    if (g_camera_config.initialized) {
        return true;
    }

    // 从已加载的 camera_param 复制配置
    g_camera_config.front_x = cam_param.front_x;
    g_camera_config.front_left = cam_param.front_left;
    g_camera_config.front_right = cam_param.front_right;
    g_camera_config.front_height = cam_param.front_height;

    g_camera_config.back_x = cam_param.back_x;
    g_camera_config.back_left = cam_param.back_left;
    g_camera_config.back_right = cam_param.back_right;
    g_camera_config.back_height = cam_param.back_height;

    g_camera_config.initialized = true;

    ApInfo("video_editor") << "Camera config cache initialized: "
                           << "front_x=" << g_camera_config.front_x << ", "
                           << "back_x=" << g_camera_config.back_x;

    return true;
}

// ============================================================================
// 辅助函数：速度分组（保持原有逻辑）
// ============================================================================
static VelocityGroup classifyVelocity(float v) {
    if (fabs(v) <= 0.001) {
        return VelocityGroup::STOP;
    } else if (v < 0) {
        return VelocityGroup::BACKWARD;
    } else if (v >= UP_SPEED) {
        return VelocityGroup::HIGH;
    } else if (v < UP_SPEED / 2) {
        return VelocityGroup::LOW;
    } else if (v < UP_SPEED) {
        return VelocityGroup::MID;
    }
    return VelocityGroup::STOP;
}

// ============================================================================
// 核心算法1: 计算切向量（单位向量）
// ============================================================================
/**
 * @brief 计算两点之间的切向量（归一化）
 * @param p1 起点
 * @param p2 终点
 * @param tangent 输出：单位切向量
 * @return 成功返回true，失败（两点重合）返回false
 */
bool calcTangent(const ara::adsfi::MsgHafTrajectoryPoint& p1, const ara::adsfi::MsgHafTrajectoryPoint& p2,
                 cv::Point2f& tangent) {
    float dx = p2.wayPoint.x - p1.wayPoint.x;
    float dy = p2.wayPoint.y - p1.wayPoint.y;
    float len = std::hypot(dx, dy);

    if (len < 1e-3f) {                     // 两点距离小于1mm，认为重合
        tangent = cv::Point2f(1.0f, 0.0f); // 默认向前
        return false;
    }

    // 归一化
    tangent.x = dx / len;
    tangent.y = dy / len;
    return true;
}

// ============================================================================
// 核心算法2: 计算法向量（逆时针旋转90度）
// ============================================================================
/**
 * @brief 计算法向量（垂直于切向量，指向车辆左侧）
 * @param tangent 切向量
 * @return 法向量
 */
cv::Point2f calcNormal(const cv::Point2f& tangent) {
    // 车辆坐标系：X前，Y左
    // 逆时针旋转90度: (x, y) -> (-y, x)
    return cv::Point2f(-tangent.y, tangent.x);
}

// ============================================================================
// 核心算法3: 计算平均切向量（用于平滑）
// ============================================================================
/**
 * @brief 计算三个点的平均切向量（中心差分）
 * @param prev 前一个点
 * @param curr 当前点
 * @param next 后一个点
 * @param tangent_avg 输出：平均切向量
 * @return 成功返回true
 */
bool calcAverageTangent(const ara::adsfi::MsgHafTrajectoryPoint& prev, const ara::adsfi::MsgHafTrajectoryPoint& curr,
                        const ara::adsfi::MsgHafTrajectoryPoint& next, cv::Point2f& tangent_avg) {
    cv::Point2f T1, T2;

    // 前段切向量: prev -> curr
    if (!calcTangent(prev, curr, T1)) {
        return false;
    }

    // 后段切向量: curr -> next
    if (!calcTangent(curr, next, T2)) {
        return false;
    }

    // 平均切向量
    tangent_avg = (T1 + T2) / 2.0f;

    // 处理S弯或掉头情况（切向量相反）
    float len = cv::norm(tangent_avg);
    if (len < 1e-3f) {
        // 前后方向相反，使用后段切向量
        tangent_avg = T2;
        len = 1.0f;
    }

    // 归一化
    if (len > 1e-3f) {
        tangent_avg /= len;
    }

    return true;
}

// ============================================================================
// 核心算法4: 计算曲率（用于自适应平滑）
// ============================================================================
/**
 * @brief 计算曲率（两个切向量的夹角）
 * @param T1 前段切向量
 * @param T2 后段切向量
 * @return 曲率（弧度），范围 [0, π]
 */
float calcCurvature(const cv::Point2f& T1, const cv::Point2f& T2) {
    // 计算点积: cos(θ) = T1·T2
    float dot_product = T1.dot(T2);

    // 限制到 [-1, 1] 范围（避免浮点误差）
    dot_product = std::clamp(dot_product, -1.0f, 1.0f);

    // 反余弦得到夹角（弧度）
    float curvature = std::acos(dot_product);

    return curvature; // 范围: [0, π]
}

// ============================================================================
// 核心算法5: 自适应平滑系数
// ============================================================================
/**
 * @brief 根据曲率计算自适应平滑系数
 * @param curvature 曲率（弧度）
 * @return 平滑系数 alpha，范围 [0.5, 0.95]
 *         直行时 alpha ≈ 0.5（弱平滑，快速响应）
 *         急转时 alpha ≈ 0.95（强平滑，消除锯齿）
 */
float calcAdaptiveAlpha(float curvature) {
    const float min_alpha = 0.5f;        // 最小平滑系数（直行）
    const float max_alpha = 0.95f;       // 最大平滑系数（急转）
    const float threshold = M_PI / 4.0f; // 阈值：45度

    if (curvature < threshold) {
        // 直行或缓转：线性插值
        float ratio = curvature / threshold;
        return min_alpha + (max_alpha - min_alpha) * ratio;
    } else {
        // 急转：使用最大平滑
        return max_alpha;
    }
}

// ============================================================================
// 核心算法6: 平滑法向量（指数移动平均 + 方向一致性检查）
// ============================================================================
/**
 * @brief 平滑法向量，消除锯齿
 * @param current_normal 当前计算的法向量
 * @param last_normal 上一个法向量
 * @param curvature 当前曲率
 * @param smoothed_normal 输出：平滑后的法向量
 * @return 成功返回true
 */
bool smoothNormal(const cv::Point2f& current_normal, const cv::Point2f& last_normal, float curvature,
                  cv::Point2f& smoothed_normal) {
    // 首次调用，无需平滑
    if (cv::norm(last_normal) < 1e-3f) {
        smoothed_normal = current_normal;
        return true;
    }

    cv::Point2f normal = current_normal;

    // 方向一致性检查：若当前法向量与上一个反向，则翻转
    if (normal.dot(last_normal) < 0.0f) {
        normal = -normal;
    }

    // 自适应平滑系数
    float alpha = calcAdaptiveAlpha(curvature);

    // 指数移动平均 (EMA)
    smoothed_normal = alpha * last_normal + (1.0f - alpha) * normal;

    // 重新归一化
    float len = cv::norm(smoothed_normal);
    if (len > 1e-3f) {
        smoothed_normal /= len;
    } else {
        smoothed_normal = last_normal; // 退化情况，使用上一个
    }

    return true;
}

// ============================================================================
// 核心算法7: 速度自适应边界宽度
// ============================================================================
/**
 * @brief 根据速度计算自适应边界宽度
 * @param velocity 速度 (m/s)
 * @param base_width 基础宽度 (m)
 * @return 调整后的宽度 (m)
 *         低速: 0.8x 基础宽度
 *         高速: 1.5x 基础宽度
 */
float calcAdaptiveWidth(float velocity, float base_width) {
    // 速度归一化到 [0, 1]
    float normalized_v = std::clamp(std::fabs(velocity) / g_camera_config.max_velocity, 0.0f, 1.0f);

    // 计算宽度因子（线性插值）
    float factor = g_camera_config.min_speed_factor +
                   (g_camera_config.max_speed_factor - g_camera_config.min_speed_factor) * normalized_v;

    return base_width * factor;
}

// ============================================================================
// 核心算法8: 计算单个点的边界点（统一算法）
// ============================================================================
/**
 * @brief 计算单个轨迹点的左右边界点
 * @param curr 当前轨迹点
 * @param normal 法向量（单位向量）
 * @param is_front 是否为前视相机
 * @param left_pt 输出：左边界3D点
 * @param right_pt 输出：右边界3D点
 */
void calcBoundaryPoints(const ara::adsfi::MsgHafTrajectoryPoint& curr, const cv::Point2f& normal, bool is_front,
                        cv::Point3f& left_pt, cv::Point3f& right_pt) {
    // 选择相机参数（使用缓存配置）
    double cam_offset_x = is_front ? g_camera_config.front_x : g_camera_config.back_x;
    double cam_height = is_front ? g_camera_config.front_height : g_camera_config.back_height;
    double base_left = is_front ? g_camera_config.front_left : g_camera_config.back_left;
    double base_right = is_front ? g_camera_config.front_right : g_camera_config.back_right;

    // 速度自适应宽度
    float left_width = calcAdaptiveWidth(curr.speed, base_left);
    float right_width = calcAdaptiveWidth(curr.speed, base_right);

    // 左边界（法向量方向，车辆左侧）
    left_pt.x = curr.wayPoint.x + normal.x * left_width + cam_offset_x;
    left_pt.y = curr.wayPoint.y + normal.y * left_width;
    left_pt.z = -cam_height;

    // 右边界（法向量反方向，车辆右侧）
    right_pt.x = curr.wayPoint.x - normal.x * right_width + cam_offset_x;
    right_pt.y = curr.wayPoint.y - normal.y * right_width;
    right_pt.z = -cam_height;
}

// ============================================================================
// 优化后的统一边界计算函数（用于遥控模式）
// ============================================================================
/**
 * @brief 计算两个点之间的边界点（统一法向量算法）
 * @param point 当前点
 * @param next 下一个点
 * @param x_lp 输出：左边界点
 * @param x_rp 输出：右边界点
 * @return 成功返回true
 *
 * 优化说明：
 * 1. 使用配置缓存，避免重复读取（性能优化）
 * 2. 使用法向量算法替代原来的转弯判断（准确性优化）
 * 3. 不再抛出异常，改为返回false（安全性优化）
 */
bool addPathPlanPoint(const ara::adsfi::MsgHafTrajectoryPoint& point, const ara::adsfi::MsgHafTrajectoryPoint& next,
                      cv::Point3f& x_lp, cv::Point3f& x_rp) {
    // 输入验证
    if (std::isnan(point.wayPoint.x) || std::isnan(point.wayPoint.y) || std::isnan(next.wayPoint.x) ||
        std::isnan(next.wayPoint.y)) {
        ApError("video_editor") << "Invalid trajectory point: NaN detected";
        EC400::Instance()->ec_add(EC400::PLAN_INVALID, "addPathPlanPoint");
        return false;
    }
    EC400::Instance()->ec_remove(EC400::PLAN_INVALID);

    // 检查配置是否已初始化
    if (!g_camera_config.initialized) {
        ApError("video_editor") << "Camera config not initialized";
        return false;
    }

    // 计算切向量
    cv::Point2f tangent;
    if (!calcTangent(point, next, tangent)) {
        ApInfo("video_editor") << "Points too close, using default tangent";
    }

    // 计算法向量
    cv::Point2f normal = calcNormal(tangent);

    // 判断是前视还是后视相机
    bool is_front = (point.wayPoint.x >= 0.0f);

    // 计算边界点
    calcBoundaryPoints(point, normal, is_front, x_lp, x_rp);

    return true;
}

/**
 * @brief 在轨迹开头插入起始点，确保辅助线从车身底部开始
 * @param trajectory 原始轨迹点序列
 * @param need_start_point 是否需要插入起始点（输出参数）
 * @return 插入的起始点
 *
 * 说明：
 * - 如果第一个点不在车身附近，需要插入起始点
 * - 前相机（x >= 0）：插入 (0.001, 0.001)
 * - 后相机（x < 0）：插入 (-0.001, 0.001)
 * - 起始点继承第一个轨迹点的速度
 */
ara::adsfi::MsgHafTrajectoryPoint createStartPoint(const std::vector<ara::adsfi::MsgHafTrajectoryPoint>& trajectory,
                                                   bool& need_start_point) {
    need_start_point = false;

    if (trajectory.empty()) {
        return ara::adsfi::MsgHafTrajectoryPoint();
    }

    const auto& first_point = trajectory[0];

    // 从配置缓存获取距离阈值
    const float min_distance_threshold = 0.005;

    bool  is_front_camera = (first_point.wayPoint.x >= 0.0f);
    float distance_from_vehicle = std::fabs(first_point.wayPoint.x);

    if (distance_from_vehicle > min_distance_threshold) {
        need_start_point = true;

        ara::adsfi::MsgHafTrajectoryPoint start_point;

        // 设置起始点位置
        if (is_front_camera) {
            // 前相机：车头位置（稍微偏前一点点，避免除零）
            start_point.wayPoint.x = 0.15f;
            start_point.wayPoint.y = 0.001f;
        } else {
            // 后相机：车尾位置（稍微偏后一点点）
            start_point.wayPoint.x = -0.15f;
            start_point.wayPoint.y = 0.001f;
        }

        // 继承第一个点的其他属性
        start_point.speed = first_point.speed;
        return start_point;
    }

    return ara::adsfi::MsgHafTrajectoryPoint();
}

// ============================================================================
// 优化后的自主模式边界计算函数
// ============================================================================
/**
 * @brief 计算自主模式的边界点（带平滑）
 * @param prev 前一个点
 * @param curr 当前点
 * @param next 后一个点
 * @param cam 相机参数（已废弃，使用全局配置）
 * @param left_pt 输出：左边界点
 * @param right_pt 输出：右边界点
 * @param last_normal 上一个法向量（用于平滑）
 * @return 成功返回true
 *
 * 优化说明：
 * 1. 修复 cam_x=0 bug（使用配置缓存中的正确值）
 * 2. 自适应平滑系数（基于曲率）
 * 3. 速度自适应边界宽度
 */
bool addPathPlanPointAuto(const ara::adsfi::MsgHafTrajectoryPoint& prev, const ara::adsfi::MsgHafTrajectoryPoint& curr,
                          const ara::adsfi::MsgHafTrajectoryPoint& next,
                          const CameraParam&                       cam, // 保留参数兼容性
                          cv::Point3f& left_pt, cv::Point3f& right_pt, cv::Point2f& last_normal) {
    // 输入验证
    if (std::isnan(curr.wayPoint.x) || std::isnan(curr.wayPoint.y) || std::isnan(curr.speed)) {
        ApError("video_editor") << "Invalid trajectory point: NaN detected";
        EC400::Instance()->ec_add(EC400::PLAN_INVALID, "addPathPlanPointAuto");
        return false;
    }
    EC400::Instance()->ec_remove(EC400::PLAN_INVALID);

    // 检查配置是否已初始化
    if (!g_camera_config.initialized) {
        ApError("video_editor") << "Camera config not initialized";
        return false;
    }

    // 计算平均切向量
    cv::Point2f tangent_avg;
    if (!calcAverageTangent(prev, curr, next, tangent_avg)) {
        ApError("video_editor") << "Failed to calculate average tangent";
        return false;
    }

    // 计算法向量
    cv::Point2f normal = calcNormal(tangent_avg);

    // 计算曲率（用于自适应平滑）
    cv::Point2f T1, T2;
    calcTangent(prev, curr, T1);
    calcTangent(curr, next, T2);
    float curvature = calcCurvature(T1, T2);

    // 平滑法向量
    cv::Point2f smoothed_normal;
    smoothNormal(normal, last_normal, curvature, smoothed_normal);

    // 更新 last_normal
    last_normal = smoothed_normal;

    // 判断是前视还是后视相机
    bool is_front = (curr.wayPoint.x >= 0.0f);

    // 计算边界点
    calcBoundaryPoints(curr, smoothed_normal, is_front, left_pt, right_pt);

    return true;
}

// ============================================================================
// 辅助函数：处理首尾点
// ============================================================================
/**
 * @brief 处理首点的边界计算（前向差分）
 */
bool calcFirstPointBoundary(const ara::adsfi::MsgHafTrajectoryPoint& first,
                            const ara::adsfi::MsgHafTrajectoryPoint& second, cv::Point3f& left_pt,
                            cv::Point3f& right_pt, cv::Point2f& normal_out) {
    cv::Point2f tangent;
    if (!calcTangent(first, second, tangent)) {
        return false;
    }

    cv::Point2f normal = calcNormal(tangent);
    normal_out = normal;

    bool is_front = (first.wayPoint.x >= 0.0f);
    calcBoundaryPoints(first, normal, is_front, left_pt, right_pt);

    return true;
}

/**
 * @brief 处理尾点的边界计算（后向差分）
 */
bool calcLastPointBoundary(const ara::adsfi::MsgHafTrajectoryPoint& second_last,
                           const ara::adsfi::MsgHafTrajectoryPoint& last, const cv::Point2f& last_normal,
                           cv::Point3f& left_pt, cv::Point3f& right_pt) {
    cv::Point2f tangent;
    if (!calcTangent(second_last, last, tangent)) {
        return false;
    }

    cv::Point2f normal = calcNormal(tangent);

    // 继承上一点的方向一致性
    if (cv::norm(last_normal) > 1e-3f && normal.dot(last_normal) < 0.0f) {
        normal = -normal;
    }

    bool is_front = (last.wayPoint.x >= 0.0f);
    calcBoundaryPoints(last, normal, is_front, left_pt, right_pt);

    return true;
}

// ============================================================================
// 主函数：保存路径规划点（优化版）
// ============================================================================
void savePathplanPoints(const ara::adsfi::MsgHafEgoTrajectory& pathplan,
                        const ara::adsfi::MsgHafEgoTrajectory& lpathplan, const CameraParam& cam) {
    ApInfo("video_editor") << "lpathplan.trajectory size:" << lpathplan.trajectoryPoints.size()
                           << ", pathplan.trajectory size: " << pathplan.trajectoryPoints.size();

    XImagePathPlan* planEdit = XImagePathPlan::Instance();
    if (planEdit) {
        planEdit->clearPathPoint();
    }

    // 初始化配置缓存
    if (!initCameraConfigOnce(cam)) {
        ApError("video_editor") << "Failed to initialize camera config";
        return;
    }

    // Lambda: 处理遥控轨迹（使用统一法向量算法）
    auto setPoints = [&](const ara::adsfi::MsgHafEgoTrajectory& traj, auto setL, auto setR, int risk_level = -1) {
        if (traj.trajectoryPoints.size() < 2) {
            EC400::Instance()->ec_add(EC400::PLAN_INVALID);
            ApError("video_editor") << "trajectory too short (size=" << traj.trajectoryPoints.size() << ")";
            return;
        } else {
            EC400::Instance()->ec_remove(EC400::PLAN_INVALID);
        }

        for (size_t i = 0; i + 1 < traj.trajectoryPoints.size(); ++i) {
            if (risk_level != -1 && traj.trajectoryPoints[i].risk_level != risk_level)
                continue;

            cv::Point3f x_lp, x_rp;
            if (addPathPlanPoint(traj.trajectoryPoints[i], traj.trajectoryPoints[i + 1], x_lp, x_rp)) {
                (planEdit->*setL)(x_lp);
                (planEdit->*setR)(x_rp);
            } else {
                ApInfo("video_editor") << "Failed to calculate boundary at index " << i;
            }
        }
    };

    // 处理遥控预警区域
    if (!lpathplan.trajectoryPoints.empty()) {
        setPoints(lpathplan, &XImagePathPlan::setLeftPathPoint, &XImagePathPlan::setRightPathPoint, 0);
        setPoints(lpathplan, &XImagePathPlan::setWorningPathPointLeft, &XImagePathPlan::setWornintPathPointRight, 2);
        setPoints(lpathplan, &XImagePathPlan::setRunPathPointLeft, &XImagePathPlan::setRunPathPointRight, 1);
    }

    // 处理循迹轨迹（自主模式）- 最终修复版本：支持速度段独立绘制
    if (!pathplan.trajectoryPoints.empty()) {
        if (cam.path_type == 2) { // 循迹轨迹模式
            if (pathplan.trajectoryPoints.size() < 2) {
                ApError("video_editor") << "Auto trajectory too short (size=" << pathplan.trajectoryPoints.size()
                                        << ")";
                return;
            }

            std::vector<ara::adsfi::MsgHafTrajectoryPoint> trajectory_with_start;
            trajectory_with_start.reserve(pathplan.trajectoryPoints.size() + 1);

            // 检查是否需要插入起始点
            bool need_start_point = false;
            auto start_point = createStartPoint(pathplan.trajectoryPoints, need_start_point);
            if (need_start_point) {
                // 插入起始点
                trajectory_with_start.push_back(start_point);
                // ApInfo("video_editor") << "Start point inserted: (" << start_point.x << ", " << start_point.y
                //       << "), first_trajectory_point: (" << pathplan.trajectoryPoints[0].x << ", " <<
                //       pathplan.trajectoryPoints[0].y
                //       << ")";
            }

            // 添加所有原始轨迹点
            trajectory_with_start.insert(trajectory_with_start.end(), pathplan.trajectoryPoints.begin(),
                                         pathplan.trajectoryPoints.end());

            // 使用处理后的轨迹（包含起始点）
            const auto& trajectory = trajectory_with_start;

            // 临时存储结构：保存所有计算好的边界点（按原始顺序）
            struct BoundaryPointWithGroup {
                cv::Point3f   left_pt;
                cv::Point3f   right_pt;
                VelocityGroup velocity_group;
            };
            std::vector<BoundaryPointWithGroup> ordered_points;
            ordered_points.reserve(trajectory.size());

            cv::Point2f last_normal(0.f, 0.f);

            // 阶段1: 计算所有点的边界（保持顺序）
            // 处理首点（前向差分）
            if (trajectory.size() >= 2) {
                cv::Point3f lp, rp;
                if (calcFirstPointBoundary(trajectory[0], trajectory[1], lp, rp, last_normal)) {
                    bool  is_front = (trajectory[0].wayPoint.x >= 0.0f);
                    float left_width = is_front ? cam.front_left : cam.back_left;
                    float right_width = is_front ? cam.front_right : cam.back_right;
                    float front_length = is_front ? 0.35 : -0.35;
                    lp = {front_length, left_width, 0};
                    rp = {front_length, -right_width, 0};
                    VelocityGroup group = classifyVelocity(trajectory[0].speed);
                    ordered_points.push_back({lp, rp, group});
                } else {
                    ApInfo("video_editor") << "first point is not used";
                    bool  is_front = (trajectory[0].wayPoint.x >= 0.0f);
                    float left_width = is_front ? cam.front_left : cam.back_left;
                    float right_width = is_front ? cam.front_right : cam.back_right;
                    float front_length = is_front ? 0.15 : -0.15;
                    lp = {front_length, left_width, 0};
                    rp = {front_length, -right_width, 0};
                    VelocityGroup group = classifyVelocity(trajectory[0].speed);
                    ordered_points.push_back({lp, rp, group});
                }
            }

            // 处理中间点（中心差分 + 平滑）
            for (size_t i = 1; i + 1 < trajectory.size(); ++i) {
                const auto& prev = trajectory[i - 1];
                const auto& curr = trajectory[i];
                const auto& next = trajectory[i + 1];

                VelocityGroup cur_group = classifyVelocity(curr.speed);

                cv::Point3f lp, rp;
                if (addPathPlanPointAuto(prev, curr, next, cam, lp, rp, last_normal)) {
                    ordered_points.push_back({lp, rp, cur_group});
                } else {
                    ApInfo("video_editor") << "addPathPlanPointAuto failed at index " << i;
                }
            }

            // 处理尾点（后向差分）
            if (trajectory.size() >= 2) {
                size_t      last_idx = trajectory.size() - 1;
                cv::Point3f lp, rp;
                if (calcLastPointBoundary(trajectory[last_idx - 1], trajectory[last_idx], last_normal, lp, rp)) {
                    VelocityGroup group = classifyVelocity(trajectory[last_idx].speed);
                    ordered_points.push_back({lp, rp, group});
                }
            }

            // 阶段2: 按速度变化分段，每段独立绘制（关键修复）
            if (!ordered_points.empty()) {
                size_t        segment_start = 0;
                VelocityGroup current_group = ordered_points[0].velocity_group;

                for (size_t i = 1; i <= ordered_points.size(); ++i) {
                    // 检测速度组变化或到达末尾
                    bool group_changed =
                        (i < ordered_points.size() && ordered_points[i].velocity_group != current_group);
                    bool is_end = (i == ordered_points.size());

                    if (group_changed || is_end) {
                        // 提取当前段的所有点
                        std::vector<cv::Point3f> segment_left;
                        std::vector<cv::Point3f> segment_right;

                        for (size_t j = segment_start; j < i; ++j) {
                            segment_left.push_back(ordered_points[j].left_pt);
                            segment_right.push_back(ordered_points[j].right_pt);
                        }

                        // 将当前段作为独立段添加（不与其他段连接）
                        planEdit->addAutoPathSegment(current_group, segment_left, segment_right);

                        // ApInfo("video_editor") << "   Auto segment added: group=" << static_cast<int>(current_group)
                        //       << ", start=" << segment_start << ", end=" << (i - 1)
                        //       << ", points=" << segment_left.size();

                        // 速度组变化时，开启新段
                        if (group_changed) {
                            segment_start = i;
                            current_group = ordered_points[i].velocity_group;
                        }
                    }
                }
            }

        } else if (cam.path_type == 1) { // 点轨迹模式
            for (const auto& pt : pathplan.trajectoryPoints) {
                planEdit->setPathPlanPoints(cv::Point3f(pt.wayPoint.x, pt.wayPoint.y, 0));
            }
        }
    }
}

// ============================================================================
// 保留的辅助函数（未修改）
// ============================================================================

void selectCameraParam(bool forward, const CameraParam& cam, double& cam_x, double& left_dist, double& right_dist,
                       double& cam_height) {
    if (forward) {
        cam_x = cam.front_x;
        left_dist = cam.front_left;
        right_dist = cam.front_right;
        cam_height = cam.front_height;
    } else {
        cam_x = cam.back_x;
        left_dist = cam.back_left;
        right_dist = cam.back_right;
        cam_height = cam.back_height;
    }
}

bool isForward(const ara::adsfi::MsgHafTrajectoryPoint& curr, const ara::adsfi::MsgHafTrajectoryPoint& next) {
    if (std::fabs(curr.wayPoint.x) > 0.1) {
        return curr.wayPoint.x > 0.0;
    }
    return (next.wayPoint.x - curr.wayPoint.x) >= 0.0;
}

// 保留旧的 calcNormal 函数（兼容性）
bool calcNormal(const ara::adsfi::MsgHafTrajectoryPoint& p0, const ara::adsfi::MsgHafTrajectoryPoint& p1,
                cv::Point2f& normal) {
    float dx = p1.wayPoint.x - p0.wayPoint.x;
    float dy = p1.wayPoint.y - p0.wayPoint.y;
    float len = std::hypot(dx, dy);
    if (len < 1e-3f)
        return false;

    // 切向量 T = (dx, dy)
    // 法向量 N = (-dy, dx)
    normal.x = -dy / len;
    normal.y = dx / len;
    return true;
}

// 保留旧的 calcSmoothNormal 函数（兼容性）
bool calcSmoothNormal(const ara::adsfi::MsgHafTrajectoryPoint& prev, const ara::adsfi::MsgHafTrajectoryPoint& curr,
                      const ara::adsfi::MsgHafTrajectoryPoint& next, cv::Point2f& normal) {
    cv::Point2f d1(curr.wayPoint.x - prev.wayPoint.x, curr.wayPoint.y - prev.wayPoint.y);
    cv::Point2f d2(next.wayPoint.x - curr.wayPoint.x, next.wayPoint.y - curr.wayPoint.y);

    float len1 = cv::norm(d1);
    float len2 = cv::norm(d2);
    if (len1 < 1e-3f || len2 < 1e-3f) {
        return false;
    }

    d1 /= len1;
    d2 /= len2;

    cv::Point2f tangent = d1 + d2;

    // 防止 S 弯 / 掉头
    if (cv::norm(tangent) < 1e-3f) {
        tangent = d2;
    }

    tangent /= cv::norm(tangent);
    normal = cv::Point2f(-tangent.y, tangent.x);
    return true;
}

struct ChannelStatus {
    int                                   last_index = -1;
    int                                   unchanged_count = 0;
    std::chrono::steady_clock::time_point last_update;
    bool                                  stuck = false;
};

bool ImageIsNull(const std::string& name, const std::vector<std::string>& list) {
    if (list.empty())
        return false;

    return std::find(list.begin(), list.end(), name) != list.end();
}

// ============================================================================
// 主函数：XDrawAssistLines（优化版）
// ============================================================================
bool XDrawAssistLines(const ara::adsfi::MsgImageData&   image_in, // 绘制图像数据
                      const compatible::VideoParameter& parame,   // 视频绘制参数
                      ara::adsfi::MsgImageData&         image_out) {
    ApInfo("video_editor") << "draw start";
    ApInfo("video_editor") << fmt::format("XDrawAssistLines input image name: {}, time: {:20.8f}", image_in.frameID,
                                          app_common::DatetimeUtil::double_time_from_sec_nsec(image_in.timestamp));
    ApInfo("video_editor") << fmt::format("XDrawAssistLines input image name: {}, time: {:20.8f}", image_out.frameID,
                                          app_common::DatetimeUtil::double_time_from_sec_nsec(image_out.timestamp));

    static bool           init{false};
    static std::string    front_ktdx_{"front_120"};
    static std::string    back_ktdx_{"back_120"};
    static std::string    front_night_ktdx_{"front_night_120"};
    static std::string    back_night_ktdx_{"back_night_120"};
    static CameraParam    camera_param{};
    static unsigned int   seq;
    static ZeroCopyShmMem zero_shm_mem_image;

    // 检测数据有效性
    if (parame.editor_parameter.split_parames_.size() <= 0) {
        ApError("video_editor") << "XVideoEditor_Dvpp error: parame.editor_parameter.split_parames_.size() <= 0";
        EC400::Instance()->ec_add(EC400::SPLIT_INVALID, "XDrawAssistLines");
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::SPLIT_INVALID);
    }

    // 初始化配置（仅首次执行）
    if (!init) {
        std::string prefix;
        auto        ptr = CustomStack::Instance();
        // 加载相机类型配置
        auto front_ktdx_path = prefix + "video/edit/front_ktdx_type";
        if (!ptr->GetProjectConfigValue(front_ktdx_path, front_ktdx_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_ktdx_path);
            ApError("video_editor") << "read project config " << front_ktdx_path << " failed";
            return false;
        }

        auto back_ktdx_path = prefix + "video/edit/back_ktdx_type";
        if (!ptr->GetProjectConfigValue(back_ktdx_path, back_ktdx_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_ktdx_path);
            ApError("video_editor") << "read project config " << back_ktdx_path << " failed";
            return false;
        }

        auto front_night_ktdx_path = prefix + "video/edit/front_night_ktdx_type";
        if (!ptr->GetProjectConfigValue(front_night_ktdx_path, front_night_ktdx_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_night_ktdx_path);
            ApError("video_editor") << "read project config " << front_night_ktdx_path << " failed";
            return false;
        }

        auto back_night_ktdx_path = prefix + "video/edit/back_night_ktdx_type";
        if (!ptr->GetProjectConfigValue(back_night_ktdx_path, back_night_ktdx_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_night_ktdx_path);
            ApError("video_editor") << "read project config " << back_night_ktdx_path << " failed";
            return false;
        }

        // 加载相机参数
        auto front_path_x = prefix + "video/edit/camera_front_x";
        if (!ptr->GetProjectConfigValue(front_path_x, camera_param.front_x)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_path_x);
            ApError("video_editor") << "read project config " << front_path_x << " failed";
            return false;
        }

        auto back_path_x = prefix + "video/edit/camera_back_x";
        if (!ptr->GetProjectConfigValue(back_path_x, camera_param.back_x)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_path_x);
            ApError("video_editor") << "read project config " << back_path_x << " failed";
            return false;
        }

        auto front_l_path = prefix + "video/edit/camera_front_left_length";
        if (!ptr->GetProjectConfigValue(front_l_path, camera_param.front_left)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_l_path);
            ApError("video_editor") << "read project config " << front_l_path << " failed";
            return false;
        }

        auto back_l_path = prefix + "video/edit/camera_back_left_length";
        if (!ptr->GetProjectConfigValue(back_l_path, camera_param.back_left)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_l_path);
            ApError("video_editor") << "read project config " << back_l_path << " failed";
            return false;
        }

        auto front_r_path = prefix + "video/edit/camera_front_right_length";
        if (!ptr->GetProjectConfigValue(front_r_path, camera_param.front_right)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_r_path);
            ApError("video_editor") << "read project config " << front_r_path << " failed";
            return false;
        }

        auto back_r_path = prefix + "video/edit/camera_back_right_length";
        if (!ptr->GetProjectConfigValue(back_r_path, camera_param.back_right)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_r_path);
            ApError("video_editor") << "read project config " << back_r_path << " failed";
            return false;
        }

        auto front_height_path = prefix + "video/edit/camera_front_height";
        if (!ptr->GetProjectConfigValue(front_height_path, camera_param.front_height)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_height_path);
            ApError("video_editor") << "read project config " << front_height_path << " failed";
            return false;
        }

        auto back_height_path = prefix + "video/edit/camera_back_height";
        if (!ptr->GetProjectConfigValue(back_height_path, camera_param.back_height)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_height_path);
            ApError("video_editor") << "read project config " << back_height_path << " failed";
            return false;
        }

        auto path_type_path = prefix + "video/edit/path_type";
        if (!ptr->GetProjectConfigValue(path_type_path, camera_param.path_type)) {
            EC400::Instance()->ec_add(EC400::CONFIG, path_type_path);
            ApError("video_editor") << "read project config " << path_type_path << " failed";
            return false;
        }

        auto path_length_path = prefix + "video/edit/plan_path_length";
        if (!ptr->GetProjectConfigValue(path_length_path, camera_param.plan_path_length)) {
            EC400::Instance()->ec_add(EC400::CONFIG, path_length_path);
            ApError("video_editor") << "read project config " << path_length_path << " failed";
            return false;
        }

        init = true;
        ApInfo("video_editor") << "XDrawAssistLines initialization completed successfully";
    }

    // 添加路径点
    savePathplanPoints(parame.editor_parameter.path, parame.editor_parameter.remote_control, camera_param);

    // 判断数据
    if (image_in.width == 0 || image_in.height == 0) {
        ApError("video_editor") << "image_in.data is empty";
        EC400::Instance()->ec_add(EC400::DATA_INVALID);
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::DATA_INVALID);
    }

    auto        camera_name = image_in.frameID;
    std::string type = "front_120";
    if (camera_name == "put_front" || camera_name == "put_back") {
        if (camera_name == "put_front") {
            type = front_ktdx_;
        } else if (camera_name == "put_back") {
            type = back_ktdx_;
        }
    } else {
        if (camera_name == "put_night") {
            type = front_ktdx_;
        }
    }

    ApInfo("video_editor") << "type=" << type;
    auto shm_image_ptr = zero_shm_mem_image.GetAllocShmMem(
        image_in.frameID, VIDEO_OUPUT_SHM_MAX_WIDTH * VIDEO_OUPUT_SHM_MAX_HEIGHT * 3 / 2, image_in.seq);
    if (shm_image_ptr == nullptr) {
        ApError("video_editor") << "zero_shm_mem_image.GetAllocShmMem error this is shm_ptr is nullther";
        EC400::Instance()->ec_add(EC400::SHM_FAILED, "XDrawAssistLines");
        return false;
    } else {
        EC400::Instance()->ec_remove(EC400::SHM_FAILED);
    }

    auto plan = XImagePathPlan::Instance();
    plan->drawRemotePathPlan((void*)shm_image_ptr, type, camera_name);
    plan->drawAutoPathPlan((void*)shm_image_ptr, type, camera_name);

    image_out = image_in;

    ApInfo("video_editor") << "draw end ";
    return true;
}
