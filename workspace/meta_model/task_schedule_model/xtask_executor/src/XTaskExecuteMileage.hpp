#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "PatrolType.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_taskexecutestatus.h"
#include "ap_log/ap_log_interface.h"
#include "pathexecutor/PathExecutor.hpp"

// XTaskExecuteMileage：progress 计算 + 里程输出/缓存/日志的封装（单位：m，米）。
//
// 背景：
// - XTaskExecute 对外发布 task_execute_status.progress/total_mileage/current_mileage。
// - progress 与里程共享同一套“remaining 基线”，保证二者分母一致、行为一致。
//
// progress 口径：
// - 基于 remaining_length_m() 计算百分比进度：ratio=(baseline-remaining)/baseline，输出 0~100
// - progress 单调不减
// - start/用户结束/退自主：清零输出并清空内部状态
// - NORMAL_END：progress 输出 100，并保持该最终状态，直到出现清零触发
// - 闭环/循环/跟随场景：progress 全程输出 0
//
// 里程口径：
// - total_mileage = progress 基线 remaining（首次进入 progress 可计算分支时的 remaining_length_m）
// - current_mileage = max(0, total_mileage - 当前 remaining_length_m)
//
// 关键行为（与 progress 的“完成保持/清零触发”一致）：
// - 循迹运行中：基线存在时按 remaining_length_m() 实时刷新里程缓存并发布
// - NORMAL_END 后：保持发布最后一次缓存值（即使 progress 逻辑会清空基线）
// - 非循迹/无基线/任务结束：清零输出与缓存
//
// 调用顺序（每周期）：
// 1) 调用 UpdateTaskProgressByRemainingLengthM() 之前，先调用 PreRefreshForNormalEnd()
//    - 原因：progress 的 NORMAL_END 分支会清空基线；清空后无法再从 remaining 推导最终里程
// 2) 调用 UpdateTaskProgressByRemainingLengthM()
// 3) 调用 PostProgressUpdateAndPublish()：负责基线/保持状态变更日志 + 里程发布/清零/节流日志
//
// 数值健壮性：
// - baseline/remaining 可能出现 NaN/inf/负值/0（执行器未就绪、抖动等），统一按 0 处理并节流告警，避免对外透出异常。
//
// 线程与生命周期：
// - 非线程安全；预期在 XTaskExecute 单线程周期调用中使用，通常以 STATIC_DATA 跨周期持有。
class XTaskExecuteMileage {
public:
    // 清空里程缓存与发布节流计数。
    // - 用于“清零触发”场景（例如业务模式切回 0 且非完成保持窗口）。
    // - 不重置异常值节流计数（mileage_invalid_log_counter_），避免短周期内重复刷告警。
    void ClearCache() {
        last_total_mileage_m_ = 0.0;
        last_current_mileage_m_ = 0.0;
        mileage_log_counter_ = 0;
    }

    // 对外推荐入口：单次调用完成 progress 计算 + 里程发布（基于 remaining_length_m）。
    //
    // 内部调用顺序（每周期）：
    // 1) PreRefreshForNormalEnd()
    // 2) UpdateTaskProgressByRemainingLengthM()
    // 3) PostProgressUpdateAndPublish()
    void UpdateByRemainingLengthM(const ara::adsfi::BusinessCommand& business_command, std::int32_t input_command,
                                  std::int32_t last_business_mode, const app_common::PathExecutor* path_executor,
                                  ara::adsfi::TaskExecuteStatus& task_execute_status, bool& has_progress_baseline,
                                  double& progress_baseline_remaining_m, int& last_progress_percent,
                                  bool& hold_completed_progress, std::int32_t& last_input_command) {
        PreRefreshForNormalEnd(business_command, path_executor, task_execute_status, has_progress_baseline,
                               progress_baseline_remaining_m, hold_completed_progress);

        const bool   prev_has_progress_baseline = has_progress_baseline;
        const double prev_progress_baseline_remaining_m = progress_baseline_remaining_m;
        const bool   prev_hold_completed_progress = hold_completed_progress;
        const int    prev_progress_percent = last_progress_percent;

        UpdateTaskProgressByRemainingLengthM(business_command, input_command, last_business_mode, path_executor,
                                             task_execute_status, has_progress_baseline, progress_baseline_remaining_m,
                                             last_progress_percent, hold_completed_progress, last_input_command);

        PostProgressUpdateAndPublish(business_command, path_executor, task_execute_status, has_progress_baseline,
                                     progress_baseline_remaining_m, hold_completed_progress, last_progress_percent,
                                     prev_has_progress_baseline, prev_progress_baseline_remaining_m,
                                     prev_hold_completed_progress, prev_progress_percent);
    }

    // NORMAL_END 预刷新：在 progress 更新之前调用。
    // - 目的：当本周期已检测到 NORMAL_END 且基线有效时，先把最终里程写入缓存；
    //        防止 progress 的 NORMAL_END 分支清空基线后，完成保持期无法再推导里程。
    void PreRefreshForNormalEnd(const ara::adsfi::BusinessCommand&   business_command,
                                const app_common::PathExecutor*      path_executor,
                                const ara::adsfi::TaskExecuteStatus& task_execute_status, bool has_progress_baseline,
                                double progress_baseline_remaining_m, bool hold_completed_progress) {
        if (!path_executor || !has_progress_baseline) {
            return;
        }
        if (task_execute_status.status != (std::int32_t)app_common::PathExecutor::ExecuteStatus::NORMAL_END) {
            return;
        }
        UpdateCacheByRemainingLengthM("NORMAL_END预刷新", business_command, path_executor, task_execute_status,
                                      has_progress_baseline, progress_baseline_remaining_m, hold_completed_progress);
    }

    // progress 计算入口（基于 remaining_length_m；无内部状态，跨周期状态由调用方维护）。
    //
    // 说明：
    // - 本函数不持有状态，所有跨周期状态均通过引用参数由调用方维护：
    //   - has_progress_baseline/progress_baseline_remaining_m：remaining 基线
    //   - last_progress_percent：上次输出的 progress（保证单调不减）
    //   - hold_completed_progress：NORMAL_END 后锁定 100% 的保持标记
    //   - last_input_command：用于检测 start 命令上升沿（0->1）
    // - 同一套“基线 remaining”（progress_baseline_remaining_m）也会用于里程 total/current 口径，保证一致性。
    //
    // 建议调用位置：
    // - 每周期仅调用一次，通常位于 PreRefreshForNormalEnd() 之后、里程发布之前。
    void UpdateTaskProgressByRemainingLengthM(const ara::adsfi::BusinessCommand& business_command,
                                              std::int32_t input_command, std::int32_t last_business_mode,
                                              const app_common::PathExecutor* path_executor,
                                              ara::adsfi::TaskExecuteStatus&  task_execute_status,
                                              bool& has_progress_baseline, double& progress_baseline_remaining_m,
                                              int& last_progress_percent, bool& hold_completed_progress,
                                              std::int32_t& last_input_command) const {
        // progress 计算：基于提路剩余路径长度 remaining_length_m
        // 注意：本函数本身不保存状态，依赖调用方跨周期维护以下内部状态：
        // - has_progress_baseline/progress_baseline_remaining_m：首次进入可计算分支时建立的“剩余距离基线”
        // - last_progress_percent：上一次输出的进度百分比（用于保证单调不减）
        // - hold_completed_progress：任务 NORMAL_END 后将进度“锁定为 100”，直到被清零触发打断
        // - last_input_command：用于识别 start 命令上升沿（0 -> 1）
        // 口径：
        // - A：以“第一次成功提路对齐后的起点”为 0（首次裁剪不计入）
        // - progress 单调不减
        // - 闭环/循环/跟随场景 progress 全程输出 0
        // - start/用户结束/退自主：立即输出 0，并清空内部进度状态
        // - 任务完成（NORMAL_END）：progress 输出 100，并保持该最终状态，直到收到启动/用户结束/退自主等清零触发
        // - enable_progress 关闭/任务 ended 时：若已锁定完成则保持输出，否则清零并清空内部状态
        // - 基线过小（<=1e-3）时：避免除零/数值不稳定，直接沿用上次输出
        // 说明：
        // - 同一套“基线 remaining”（progress_baseline_remaining_m）也会用于对外发布 total_mileage/current_mileage，
        //   以保证“进度”和“里程”口径一致（里程的缓存/保持逻辑在本类实现）。

        // 是否允许输出进度：仅在特定业务模式（1/11）下输出 progress（其他模式保持 0 或保持锁定值）。
        const bool enable_progress = app_common::PathExecutor::is_track_or_arrive(business_command.business_mode);
        // 全程强制输出 0 的场景：闭环/循环/跟随等（该优先级最高，强制清零且不会输出 100）。
        const bool force_zero_progress = IsForceZeroProgressScene(
            business_command.business_mode, business_command.param.patrol_type, business_command.param.patrol_loop);
        // start 命令上升沿：0 -> 1，视为一次新任务开始，需清零并重新建立基线。
        const bool start_command_edge = (1 == input_command) && (0 == last_input_command);
        // 业务模式进入上升沿：last_business_mode == 0 且当前进入 enable_progress 模式，同样按“新任务”处理。
        const bool start_business_mode_edge = (0 == last_business_mode) && enable_progress;
        // 退自主：立即清零并清空内部状态。
        const bool terminate_autonomous =
            task_execute_status.status == (std::int32_t)app_common::PathExecutor::ExecuteStatus::TERMINATE_AUTONOMOUS;
        // 任务 ended：执行器不存在，或执行器进入 IDLE（提路/执行已结束）。
        const bool task_ended = (!path_executor || task_execute_status.status ==
                                                       (std::int32_t)app_common::PathExecutor::ExecuteStatus::IDLE);
        // 正常结束：输出 100，并锁定该最终状态（直到被清零触发打断）。
        const bool task_normal_end =
            task_execute_status.status == (std::int32_t)app_common::PathExecutor::ExecuteStatus::NORMAL_END;
        // 用户结束：立即清零并清空内部状态。
        const bool task_user_terminate =
            task_execute_status.status == (std::int32_t)app_common::PathExecutor::ExecuteStatus::USER_TERMINATE;

        auto clear_internal_progress_state = [&]() {
            // 仅清空“用于计算”的内部基线状态（不直接改输出 progress）。
            has_progress_baseline = false;
            progress_baseline_remaining_m = 0.0;
        };

        auto clear_progress_output_and_state = [&]() {
            // 清零输出 progress，并重置所有内部状态（含锁定完成状态）。
            hold_completed_progress = false;
            clear_internal_progress_state();
            last_progress_percent = 0;
            task_execute_status.progress = 0;
        };

        auto hold_progress_as_completed = [&]() {
            // 将进度锁定为完成（100%），并清空基线（后续不再按 remaining 计算，直到被清零触发）。
            hold_completed_progress = true;
            clear_internal_progress_state();
            last_progress_percent = 100;
            task_execute_status.progress = 100;
        };

        // 每次函数结束前都更新 last_input_command，用于下一周期检测 start 命令上升沿。
        auto update_last_input_command_before_return = [&]() { last_input_command = input_command; };

        // 1) 全程强制 0 的场景：最高优先级，直接清零并返回。
        if (force_zero_progress) {
            static std::uint32_t force_zero_log_counter = 0;
            if ((force_zero_log_counter++ % 50) == 0) {
                ApInfo("task_executor") << "[XTaskExecute][progress] 强制输出0: business_mode=" << business_command.business_mode
                      << ", patrol_type=" << business_command.param.patrol_type
                      << ", patrol_loop=" << business_command.param.patrol_loop
                      << ", status=" << task_execute_status.status << ", input_command=" << input_command
                      << ", last_input_command=" << last_input_command << ", last_business_mode=" << last_business_mode
                      << ", has_baseline=" << has_progress_baseline << ", baseline_m=" << progress_baseline_remaining_m
                      << ", hold_completed=" << hold_completed_progress << ", last_percent=" << last_progress_percent;
            }
            clear_progress_output_and_state();
            update_last_input_command_before_return();
            return;
        }

        // 2) 退自主：清零并返回。
        if (terminate_autonomous) {
            static std::uint32_t terminate_autonomous_log_counter = 0;
            if ((terminate_autonomous_log_counter++ % 10) == 0) {
                ApInfo("task_executor") << "[XTaskExecute][progress] 退自主清零: business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status << ", input_command=" << input_command
                      << ", last_input_command=" << last_input_command << ", has_baseline=" << has_progress_baseline
                      << ", baseline_m=" << progress_baseline_remaining_m
                      << ", hold_completed=" << hold_completed_progress << ", last_percent=" << last_progress_percent;
            }
            clear_progress_output_and_state();
            update_last_input_command_before_return();
            return;
        }

        // 3) 用户结束：清零并返回。
        if (task_user_terminate) {
            static std::uint32_t user_terminate_log_counter = 0;
            if ((user_terminate_log_counter++ % 10) == 0) {
                ApInfo("task_executor") << "[XTaskExecute][progress] 用户结束清零: business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status << ", input_command=" << input_command
                      << ", last_input_command=" << last_input_command << ", has_baseline=" << has_progress_baseline
                      << ", baseline_m=" << progress_baseline_remaining_m
                      << ", hold_completed=" << hold_completed_progress << ", last_percent=" << last_progress_percent;
            }
            clear_progress_output_and_state();
            update_last_input_command_before_return();
            return;
        }

        // 4) 新任务触发（start 命令上升沿 / 业务模式进入上升沿）：先清零并清空内部状态；
        //    不立即 return，让后续逻辑在同一周期重新建立基线并输出 0。
        if (start_command_edge || start_business_mode_edge) {
            const char* reason = (start_command_edge && start_business_mode_edge)
                                     ? "start_edge+mode_edge"
                                     : (start_command_edge ? "start_edge" : "mode_edge");
            ApInfo("task_executor") << "[XTaskExecute][progress] 新任务触发清零(" << reason
                  << "): business_mode=" << business_command.business_mode << ", status=" << task_execute_status.status
                  << ", input_command=" << input_command << ", last_input_command=" << last_input_command
                  << ", last_business_mode=" << last_business_mode << ", has_baseline=" << has_progress_baseline
                  << ", baseline_m=" << progress_baseline_remaining_m << ", hold_completed=" << hold_completed_progress
                  << ", last_percent=" << last_progress_percent;
            clear_progress_output_and_state();
        }

        // 5) 任务正常结束：输出 100 并锁定，直到收到清零触发。
        if (task_normal_end) {
            static std::uint32_t normal_end_log_counter = 0;
            if ((normal_end_log_counter++ % 10) == 0) {
                ApInfo("task_executor") << "[XTaskExecute][progress] NORMAL_END锁定100: business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status << ", has_baseline=" << has_progress_baseline
                      << ", baseline_m=" << progress_baseline_remaining_m
                      << ", hold_completed=" << hold_completed_progress << ", last_percent=" << last_progress_percent;
            }
            hold_progress_as_completed();
            update_last_input_command_before_return();
            return;
        }

        // 6) 非进度输出模式：若已锁定完成则继续输出锁定值，否则清零并清空状态。
        if (!enable_progress) {
            if (hold_completed_progress) {
                task_execute_status.progress = static_cast<unsigned char>(last_progress_percent);
            } else {
                clear_progress_output_and_state();
            }
            update_last_input_command_before_return();
            return;
        }

        // 7) 任务 ended（执行器不存在/IDLE）：若已锁定完成则保持输出，否则清零并清空状态。
        if (task_ended) {
            if (hold_completed_progress) {
                task_execute_status.progress = static_cast<unsigned char>(last_progress_percent);
            } else {
                clear_progress_output_and_state();
            }
            update_last_input_command_before_return();
            return;
        }

        // 8) 正常计算分支：根据“剩余距离相对基线的下降比例”计算百分比进度。
        //    remaining_length_m() 可能出现 NaN/inf/负值：
        //    - NaN/inf：沿用上一次输出，避免触发 lround(NaN) 引发异常
        //    - 负值：按 0 处理
        const double raw_remaining_m = path_executor->remaining_length_m();
        if (!std::isfinite(raw_remaining_m)) {
            static std::uint32_t invalid_remaining_log_counter = 0;
            if ((invalid_remaining_log_counter++ % 50) == 0) {
                ApWarn("task_executor") << "[XTaskExecute][progress] remaining_length_m 异常，沿用上次输出: raw_remaining_m="
                      << raw_remaining_m << ", has_baseline=" << has_progress_baseline
                      << ", baseline_m=" << progress_baseline_remaining_m << ", last_percent=" << last_progress_percent
                      << ", business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status;
            }
            task_execute_status.progress = static_cast<unsigned char>(last_progress_percent);
            update_last_input_command_before_return();
            return;
        }
        const double remaining_m = std::max(0.0, raw_remaining_m);
        if (!has_progress_baseline) {
            // 首次进入可计算分支：建立基线（以当前 remaining 作为 100%->0% 的分母），并输出 0%。
            // 这对应“以第一次成功提路对齐后的起点为 0”：基线建立前的裁剪/对齐不计入进度。
            hold_completed_progress = false;
            progress_baseline_remaining_m = remaining_m;
            has_progress_baseline = true;
            last_progress_percent = 0;
            task_execute_status.progress = 0;
            ApInfo("task_executor") << "[XTaskExecute][progress] 建立基线: baseline_m=" << progress_baseline_remaining_m
                  << ", business_mode=" << business_command.business_mode << ", status=" << task_execute_status.status
                  << ", remaining_m=" << remaining_m;
            update_last_input_command_before_return();
            return;
        }

        if (!std::isfinite(progress_baseline_remaining_m)) {
            static std::uint32_t invalid_baseline_log_counter = 0;
            if ((invalid_baseline_log_counter++ % 50) == 0) {
                ApWarn("task_executor") << "[XTaskExecute][progress] baseline_m 异常，沿用上次输出: baseline_m="
                      << progress_baseline_remaining_m << ", remaining_m=" << remaining_m
                      << ", last_percent=" << last_progress_percent
                      << ", business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status;
            }
            task_execute_status.progress = static_cast<unsigned char>(last_progress_percent);
            update_last_input_command_before_return();
            return;
        }

        if (progress_baseline_remaining_m <= 1e-3) {
            // 基线过小：避免除零/数值抖动，直接沿用上一次输出。
            static std::uint32_t small_baseline_log_counter = 0;
            if ((small_baseline_log_counter++ % 50) == 0) {
                ApInfo("task_executor") << "[XTaskExecute][progress] 基线过小，沿用上次输出: baseline_m=" << progress_baseline_remaining_m
                      << ", remaining_m=" << remaining_m << ", last_percent=" << last_progress_percent
                      << ", business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status;
            }
            task_execute_status.progress = static_cast<unsigned char>(last_progress_percent);
            update_last_input_command_before_return();
            return;
        }

        // ratio 表示“已走过的比例”：(baseline - remaining) / baseline，理论范围 [0, 1]。
        double ratio = (progress_baseline_remaining_m - remaining_m) / progress_baseline_remaining_m;
        if (ratio < 0.0) {
            ratio = 0.0;
        } else if (ratio > 1.0) {
            ratio = 1.0;
        }

        // 百分比进度：四舍五入到整数，并 clamp 到 [0, 100]。
        int percent = static_cast<int>(std::lround(ratio * 100.0));
        if (percent < 0) {
            percent = 0;
        } else if (percent > 100) {
            percent = 100;
        }
        // 保证 progress 单调不减：若当前计算值回退，则维持上一次输出。
        if (percent < last_progress_percent) {
            static std::uint32_t monotonic_clamp_log_counter = 0;
            if ((monotonic_clamp_log_counter++ % 50) == 0) {
                ApInfo("task_executor") << "[XTaskExecute][progress] 单调约束触发: computed=" << percent
                      << ", last=" << last_progress_percent << ", ratio=" << ratio
                      << ", baseline_m=" << progress_baseline_remaining_m << ", remaining_m=" << remaining_m
                      << ", business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status;
            }
            percent = last_progress_percent;
        }

        last_progress_percent = percent;
        // progress 对外输出为 unsigned char（0~100）。
        task_execute_status.progress = static_cast<unsigned char>(percent);
        update_last_input_command_before_return();
    }

    // progress 更新之后的里程发布入口（每周期调用一次）。
    //
    // 入参约定：
    // - has_progress_baseline/progress_baseline_remaining_m/hold_completed_progress/last_progress_percent：
    //   为 progress 逻辑更新后的“当前状态”
    // - prev_*：
    //   为调用 UpdateTaskProgressByRemainingLengthM() 之前快照的“上一周期状态”，用于打印关键状态变更日志
    //
    // 出参行为：
    // - 会写 task_execute_status.total_mileage/current_mileage（单位 m）
    // - 会根据状态发布 ApInfo("task_executor")/ApWarn("task_executor")（含节流）
    void PostProgressUpdateAndPublish(const ara::adsfi::BusinessCommand& business_command,
                                      const app_common::PathExecutor*    path_executor,
                                      ara::adsfi::TaskExecuteStatus& task_execute_status, bool has_progress_baseline,
                                      double progress_baseline_remaining_m, bool hold_completed_progress,
                                      int last_progress_percent, bool prev_has_progress_baseline,
                                      double prev_progress_baseline_remaining_m, bool prev_hold_completed_progress,
                                      int prev_progress_percent) {
        if (!prev_has_progress_baseline && has_progress_baseline) {
            ApInfo("task_executor") << "[XTaskExecute][里程] 建立 progress/里程基线: baseline_m=" << progress_baseline_remaining_m
                  << ", business_mode=" << business_command.business_mode << ", status=" << task_execute_status.status;
        } else if (prev_has_progress_baseline && !has_progress_baseline) {
            ApInfo("task_executor") << "[XTaskExecute][里程] progress/里程基线已清空: prev_baseline_m="
                  << prev_progress_baseline_remaining_m << ", hold_completed=" << hold_completed_progress
                  << ", business_mode=" << business_command.business_mode << ", status=" << task_execute_status.status;
        }

        if (!prev_hold_completed_progress && hold_completed_progress) {
            ApInfo("task_executor") << "[XTaskExecute][里程] 进入完成保持: progress=" << last_progress_percent
                  << "，后续将保持里程缓存输出直至清零触发。";
        } else if (prev_hold_completed_progress && !hold_completed_progress) {
            ApInfo("task_executor") << "[XTaskExecute][里程] 退出完成保持: prev_progress=" << prev_progress_percent
                  << ", business_mode=" << business_command.business_mode << ", status=" << task_execute_status.status;
        }

        const char* mileage_publish_scene = "清零";
        if (hold_completed_progress) {
            task_execute_status.total_mileage = last_total_mileage_m_;
            task_execute_status.current_mileage = last_current_mileage_m_;
            mileage_publish_scene = "完成保持";
        } else if (IsTrackBusinessMode(business_command) && path_executor && has_progress_baseline) {
            UpdateCacheByRemainingLengthM(nullptr, business_command, path_executor, task_execute_status,
                                          has_progress_baseline, progress_baseline_remaining_m,
                                          hold_completed_progress);
            task_execute_status.total_mileage = last_total_mileage_m_;
            task_execute_status.current_mileage = last_current_mileage_m_;
            mileage_publish_scene = "运行中刷新";
        } else {
            const double old_total_mileage = last_total_mileage_m_;
            const double old_current_mileage = last_current_mileage_m_;
            last_total_mileage_m_ = 0.0;
            last_current_mileage_m_ = 0.0;
            task_execute_status.total_mileage = 0.0;
            task_execute_status.current_mileage = 0.0;
            if (old_total_mileage > 0.0 || old_current_mileage > 0.0) {
                ApInfo("task_executor") << "[XTaskExecute][里程] 触发里程清零: old_total_m=" << old_total_mileage
                      << ", old_current_m=" << old_current_mileage
                      << ", business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status << ", has_baseline=" << has_progress_baseline
                      << ", hold_completed=" << hold_completed_progress;
            }
        }

        const bool should_log_mileage =
            hold_completed_progress || has_progress_baseline || IsTrackBusinessMode(business_command);
        if (should_log_mileage) {
            // 发布快照日志节流：默认每 20 次周期打印一次。
            if ((mileage_log_counter_++ % 20) == 0) {
                const double raw_remaining_m = path_executor ? path_executor->remaining_length_m() : -1.0;
                ApInfo("task_executor") << "[XTaskExecute][里程] 发布(" << mileage_publish_scene
                      << "): business_mode=" << business_command.business_mode
                      << ", status=" << task_execute_status.status
                      << ", progress=" << static_cast<int>(task_execute_status.progress)
                      << ", patrol_type=" << business_command.param.patrol_type
                      << ", patrol_loop=" << business_command.param.patrol_loop
                      << ", hold_completed=" << hold_completed_progress << ", has_baseline=" << has_progress_baseline
                      << ", baseline_m=" << progress_baseline_remaining_m << ", remaining_m=" << raw_remaining_m
                      << ", total_mileage=" << task_execute_status.total_mileage
                      << ", current_mileage=" << task_execute_status.current_mileage;
            }
        } else {
            mileage_log_counter_ = 0;
        }
    }

private:
    // 仅循迹模式（1/11）允许按 remaining 刷新里程；其他业务模式按规则保持/清零。
    static bool IsTrackBusinessMode(const ara::adsfi::BusinessCommand& business_command) {
        return app_common::PathExecutor::is_track_or_arrive(business_command.business_mode);
    }

    bool IsForceZeroProgressScene(std::int32_t business_mode, std::int32_t patrol_type,
                                  std::int32_t patrol_loop) const {
        // 需求：闭环/循环/跟随场景 progress 全程输出 0。
        // - 跟随：business_mode == 3/31
        // - 闭环：PatrolType::ClosedLoop
        // - 循环：patrol_loop == 1
        if (app_common::PathExecutor::is_follow(business_mode)) {
            return true;
        }
        if (app_common::PathExecutor::is_track_or_arrive(business_mode)) {
            if (static_cast<std::int32_t>(PatrolType::ClosedLoop) == patrol_type) {
                return true;
            }
            if (1 == patrol_loop) {
                return true;
            }
        }
        return false;
    }

    // 里程数值兜底：
    // - meters 非有限值或 <= 0：按 0 处理，并按计数节流打印 ApWarn("task_executor")，避免刷屏。
    double SanitizeMeters(const char* name, double meters, const ara::adsfi::BusinessCommand& business_command,
                          const ara::adsfi::TaskExecuteStatus& task_execute_status, bool has_progress_baseline,
                          double progress_baseline_remaining_m, bool hold_completed_progress) {
        if (std::isfinite(meters) && meters > 0.0) {
            return meters;
        }
        // 异常值告警节流：默认每 50 次命中打印一次。
        if ((mileage_invalid_log_counter_++ % 50) == 0) {
            ApWarn("task_executor") << "[XTaskExecute][里程] " << name << " 异常: " << meters << "，将按 0 处理。"
                  << " business_mode=" << business_command.business_mode << ", status=" << task_execute_status.status
                  << ", has_baseline=" << has_progress_baseline << ", baseline_m=" << progress_baseline_remaining_m
                  << ", hold_completed=" << hold_completed_progress;
        }
        return 0.0;
    }

    // 将 remaining_length_m() 口径转换为 total/current 并写入缓存。
    // - scene != nullptr：认为是低频关键点刷新，会打印一条 ApInfo("task_executor")（用于 NORMAL_END 预刷新等）
    // - scene == nullptr：认为是高频刷新，不打印日志（避免刷屏）
    void UpdateCacheByRemainingLengthM(const char* scene, const ara::adsfi::BusinessCommand& business_command,
                                       const app_common::PathExecutor*      path_executor,
                                       const ara::adsfi::TaskExecuteStatus& task_execute_status,
                                       bool has_progress_baseline, double progress_baseline_remaining_m,
                                       bool hold_completed_progress) {
        if (!path_executor || !has_progress_baseline) {
            return;
        }
        const double raw_total_m = progress_baseline_remaining_m;
        const double raw_remaining_m = path_executor->remaining_length_m();
        const double total_m =
            SanitizeMeters("baseline_remaining_m", raw_total_m, business_command, task_execute_status,
                           has_progress_baseline, progress_baseline_remaining_m, hold_completed_progress);
        const double remaining_m =
            SanitizeMeters("remaining_length_m", raw_remaining_m, business_command, task_execute_status,
                           has_progress_baseline, progress_baseline_remaining_m, hold_completed_progress);
        const double current_m = std::max(0.0, total_m - remaining_m);
        last_total_mileage_m_ = total_m;
        last_current_mileage_m_ = current_m;
        if (nullptr != scene) {
            ApInfo("task_executor") << "[XTaskExecute][里程] 刷新里程缓存(" << scene << "): raw_baseline_m=" << raw_total_m
                  << ", raw_remaining_m=" << raw_remaining_m << ", total_m=" << total_m << ", current_m=" << current_m;
        }
    }

private:
    // 最近一次发布的里程缓存（单位：m，米）。
    double last_total_mileage_m_ = 0.0;
    double last_current_mileage_m_ = 0.0;
    // 日志节流计数器（避免高频刷屏）。
    std::uint32_t mileage_log_counter_ = 0;
    std::uint32_t mileage_invalid_log_counter_ = 0;
};
