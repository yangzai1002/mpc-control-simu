#include "HeadingAdjustment.h"
#include <cmath>
#include "ap_log/ap_log_interface.h"
#include "log/log.h"
#include "base_control/adsfi_struct_adapter.h"

/**
 * @brief 处理航向调整请求的核心函数实现（单点版本）
 * * @param command 业务命令输入，包含期望的目标航向角 (command.param.pose.angle)
 * @param loc 定位信息，提供车辆当前的位姿
 * @param trajectory [输出] 生成的规划轨迹
 */
void HeadingAdjustment::Process(const ara::adsfi::BusinessCommand& command, 
      const avos::planning::LocalizationData& localization_data, 
      const avos::planning::Perception& perception, 
      PlanningTrajectory& trajectory) {    
    
    // 1. 清空旧的轨迹数据，确保输出的是全新的指令
    trajectory.trajectory.clear();

    // ====================== 新增逻辑预留位置 ======================
    // TODO: 未来在此处添加处理障碍物和地图信息的逻辑
    // 例如，可以检查车辆旋转范围内是否存在障碍物。
    // 如果存在障碍物，可以选择不生成轨迹或生成表示“失败”的轨迹。
    // for (const auto& obj : perception_obj.objects) {
    //     // 检查障碍物 obj 是否会影响原地转向...
    // }
    // ==========================================================

    // 2. 从业务命令中获取期望的目标绝对航向角（单位：度）
    const double target_global_angle = command.param.pose.orientation.z;
    double current_angle = localization_data.yaw;
    double diff = target_global_angle - current_angle;

    // 归一化误差到 [-180, 180] 区间
    while (diff > 180.0) diff -= 360.0;
    while (diff < -180.0) diff += 360.0;

    // 打印关键调试信息
    AINFO_IF(FLAGS_enable_log) << "[HeadingAdjustment] Process: Target=" << target_global_angle 
          << " deg, Current=" << current_angle 
          << " deg, Diff=" << diff << " deg.";

    // 3. 创建唯一的轨迹点
    PlanningTrajectoryPoint point;
    
    // 4. 填充路点核心信息
    // 设置轨迹点类型为 1，表示“原地转向”。
    // 控制模块将依据此标志位执行原地旋转，而不是轨迹跟随。
    point.type = 1; 

    // 设置期望的绝对航向角。
    // 控制模块旋转的目标。
    point.angleglobal = static_cast<float>(target_global_angle);

    // 使用车辆当前的绝对坐标作为转向中心。
    // 车辆将在此位置进行旋转。
    point.xg = localization_data.xg;
    point.yg = localization_data.yg;
    
    // 将其他与循迹相关的参数设置为安全的默认值（0）
    point.x = 0.0f;          // 相对坐标系下的位置为原点
    point.y = 0.0f;
    point.angle = 0.0f;      // 相对角度为0
    point.velocity = 0.0f;   // 原地转向，目标线速度为0
    point.a = 0.0f;          // 目标加速度为0
    point.curvature = 0.0f;  // 目标轨迹曲率为0
    point.t = 0.0f;          // 相对时间戳为0，代表立即执行的指令

    // 5. 将这一个路点添加到轨迹中
    trajectory.trajectory.push_back(point);

    // 6. 填充轨迹的头部信息，如时间戳和坐标系
    trajectory.header.time = localization_data.time; // 使用最新的定位时间戳
}

bool HeadingAdjustment::IsHeadingReached(const double target_deg, const double current_deg, const double threshold_deg) const {
    // 1. 直接计算差值
    double diff = target_deg - current_deg;

    // 2. 角度归一化到 [-180, 180]
    while (diff > 180.0) diff -= 360.0;
    while (diff < -180.0) diff += 360.0;

    // 3. 绝对值判断
    return std::abs(diff) < threshold_deg;
}