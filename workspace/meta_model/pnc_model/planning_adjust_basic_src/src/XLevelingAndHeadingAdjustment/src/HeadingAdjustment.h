#pragma once

#include "ara/adsfi/impl_type_businesscommand.h"
#include "PlanningTrajectory.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/perception_input.h"

/**
 * @class HeadingAdjustment
 * @brief 航向调整功能类
 * * 负责处理原地转向的业务请求，生成一个单点轨迹，
 * 引导下游控制模块执行原地调整航向的任务。
 */
class HeadingAdjustment {
public:
    /**
     * @brief 构造函数
     */
    HeadingAdjustment() = default;

    /**
     * @brief 析构函数
     */
    ~HeadingAdjustment() = default;

    /**
     * @brief 处理航向调整请求的核心函数
     * @param command 业务命令输入，包含期望的目标航向角
     * @param loc 定位信息，提供车辆当前的位姿
     * @param perception 感知信息
     * @param trajectory [输出] 生成的规划轨迹
     */
    void Process(const ara::adsfi::BusinessCommand& command, const avos::planning::LocalizationData& localization_data, 
        const avos::planning::Perception& perception, PlanningTrajectory& trajectory);

    /**
     * @brief 静态辅助函数：检查航向是否已达到目标范围内
     * @param target_deg 目标角度 (度)
     * @param current_deg 当前角度 (度)
     * @param threshold_deg 判定阈值 (度)
     * @return true 如果误差在阈值内
     */
    bool IsHeadingReached(const double target_deg, const double current_deg, const double threshold_deg) const;
};