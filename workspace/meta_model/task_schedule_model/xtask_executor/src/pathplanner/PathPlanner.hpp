#ifndef _PATH_PLANNER_HPP_
#define _PATH_PLANNER_HPP_

#include <map>
#include <stdexcept>
#include <vector>

#include "GeoUtils.hpp"
#include "ReferenceLine.hpp"
#include "Waypoint.h"
#include "ara/adsfi/impl_type_businesspoint.h"
#include "faulthandle_api.hpp"
#include "smoother/discrete_points_reference_line_smoother.h"

namespace app_common {
    class ExceptionLoadPlannerConfig : public std::exception {
    public:
        const std::string config_name;
        explicit ExceptionLoadPlannerConfig(const std::string& config_name_)
            : std::exception(), config_name(config_name_) {
        }
    };

    class ExceptionLoadTrackFile : public std::exception {
    public:
        const std::string track_file;
        explicit ExceptionLoadTrackFile(const std::string& track_file_) : std::exception(), track_file(track_file_) {
        }
    };

    class ExceptionInvalidDestination : public std::exception {
    public:
        const std::string track_file;
        const std::string dest;
        ExceptionInvalidDestination(const std::string& track_file_, const std::string& dest_)
            : std::exception(), track_file(track_file_), dest(dest_) {
        }
    };

    class ExceptionTrackPointsTooFew : public std::exception {
    public:
        const std::string track_file;
        const size_t      trackpoints;

        ExceptionTrackPointsTooFew(const std::string& track_file_, size_t trackpoints_)
            : std::exception(), track_file(track_file_), trackpoints(trackpoints_) {
        }
    };

    template <typename P> class AdaptiveCurvatureSmoother {
    public:
        explicit AdaptiveCurvatureSmoother(double high_curvature_threshold = 0.8, double low_alpha = 0.1,
                                           double high_alpha = 0.8, int window_size = 5, int max_iterations = 20,
                                           double convergence_threshold = 1e-6)
            : high_curvature_threshold_(high_curvature_threshold)
            , low_alpha_(low_alpha)
            , high_alpha_(high_alpha)
            , window_size_(window_size)
            , max_iterations_(max_iterations)
            , convergence_threshold_(convergence_threshold) {
        }

        // 执行平滑
        std::vector<P> smooth(const std::vector<P>& input_path) {
            if (input_path.size() < 3) {
                ApError("task_executor") << "输入路径点数量不足，至少需要3个点";
                return input_path;
            }

            std::vector<P> smoothed_path = input_path;

            // 迭代平滑
            for (int iter = 0; iter < max_iterations_; ++iter) {
                auto prev_path = smoothed_path;

                // 估计曲率
                std::vector<double> curvatures = estimateCurvature(smoothed_path);

                // 迭代平滑
                smoothed_path = iterativeSmoothing(smoothed_path, curvatures);

                // 检查收敛
                double max_change = 0.0;
                for (size_t i = 0; i < smoothed_path.size(); ++i) {
                    max_change = std::max(max_change, distance(smoothed_path[i], prev_path[i]));
                }

                if (max_change < convergence_threshold_) {
                    ApInfo("task_executor") << "迭代" << iter + 1 << "次后收敛，最大变化: " << max_change;
                    break;
                }
            }

            return smoothed_path;
        }

    private:
        double       high_curvature_threshold_;
        double       low_alpha_;
        double       high_alpha_;
        int          window_size_;
        int          max_iterations_;
        double       convergence_threshold_;
        const double eps_ = 1e-8; // 数值稳定性参数

        // 计算两点之间的距离
        double distance(const P& a, const P& b) {
            double dx = a.x - b.x;
            double dy = a.y - b.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        // 估计每个点的曲率
        std::vector<double> estimateCurvature(const std::vector<P>& path) {
            int                 n = path.size();
            std::vector<double> curvatures(n, 0.0);

            for (int i = 1; i < n - 1; ++i) {
                const auto& p_prev = path[i - 1];
                const auto& p_curr = path[i];
                const auto& p_next = path[i + 1];

                // 计算步长（防止为0）
                double ds_prev = distance(p_curr, p_prev) + eps_;
                double ds_next = distance(p_curr, p_next) + eps_;

                // 一阶导数（dx/ds, dy/ds）
                double dx = (p_next.x - p_prev.x) / (ds_prev + ds_next);
                double dy = (p_next.y - p_prev.y) / (ds_prev + ds_next);

                // 二阶导数（d²x/ds², d²y/ds²）
                double ddx =
                    ((p_next.x - p_curr.x) / ds_next - (p_curr.x - p_prev.x) / ds_prev) / ((ds_prev + ds_next) / 2.0);
                double ddy =
                    ((p_next.y - p_curr.y) / ds_next - (p_curr.y - p_prev.y) / ds_prev) / ((ds_prev + ds_next) / 2.0);

                // 曲率公式
                double numerator = dx * ddy - dy * ddx;
                double denominator = std::pow(dx * dx + dy * dy, 1.5) + eps_;
                curvatures[i] = numerator / denominator;
            }
            return curvatures;
        }

        // 迭代平滑
        std::vector<P> iterativeSmoothing(const std::vector<P>& path, const std::vector<double>& curvatures) {
            int            n = path.size();
            std::vector<P> new_path = path;

            for (int i = 1; i < n - 1; ++i) {
                // 动态计算平滑强度alpha
                double curvature_abs = std::fabs(curvatures[i]);
                double alpha =
                    (curvature_abs > high_curvature_threshold_) ? low_alpha_ : // 高曲率区域使用低强度平滑
                        (high_alpha_ - (high_alpha_ - low_alpha_) * (curvature_abs / high_curvature_threshold_));

                // 局部窗口（防止越界）
                int half_win = window_size_ / 2;
                int start = std::max(0, i - half_win);
                int end = std::min(n - 1, i + half_win);
                if (end - start + 1 < 3)
                    continue;

                // 高斯加权平均计算平滑位置
                double sum_x = 0.0, sum_y = 0.0, sum_weights = 0.0;
                for (int j = start; j <= end; ++j) {
                    double weight = std::exp(-0.5 * std::pow((j - i) / (double)half_win, 2)); // 高斯权重
                    sum_x += path[j].x * weight;
                    sum_y += path[j].y * weight;
                    sum_weights += weight;
                }

                double smoothed_x = sum_x / sum_weights;
                double smoothed_y = sum_y / sum_weights;

                // 应用平滑：结合原始位置和平滑位置
                new_path[i].x = (1 - alpha) * new_path[i].x + alpha * smoothed_x;
                new_path[i].y = (1 - alpha) * new_path[i].y + alpha * smoothed_y;
            }

            return new_path;
        }
    };
    class PathPlanner {
    public:
        PathPlanner() {
            _min_way_points = 10;
            std::string full_path = "planner/track/min_way_points";
            try {
                _min_way_points = CustomStack::Instance()->GetProjectConfigValue<std::size_t>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadPlannerConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _min_way_points;
        }

        virtual ~PathPlanner() = default;

        virtual std::vector<std::vector<ara::adsfi::BusinessPoint>> paths() = 0;

        void smooth_waypoints(std::vector<Waypoint>& waypoints) {
            if (waypoints.empty()) {
                ApInfo("task_executor") << "路点为空，不进行平滑处理";
                return;
            }

            auto lng = waypoints.front().lon;
            auto longZone = 0.0;
            if (lng < 0.0) {
                longZone = ((180.0 + lng) / 6.0) + 1;
            } else {
                longZone = (lng / 6.0) + 31;
            }

            auto                                   pgccs_l = app_common::GeoUtils::Waypoints2PointGCCS(waypoints);
            std::vector<ara::adsfi::BusinessPoint> local_path;
            std::for_each(pgccs_l.begin(), pgccs_l.end(), [&local_path](const GeoPointGCCS& p) {
                ara::adsfi::BusinessPoint bp{};
                bp.x = p.xg;
                bp.y = p.yg;
                local_path.push_back(bp);
            });

            ApInfo("task_executor") << "smooth " << local_path.size() << " points begin ...";
            AdaptiveCurvatureSmoother<ara::adsfi::BusinessPoint> smoother(0.4, // 高曲率阈值
                                                                          0.5, // 高曲率区域平滑强度
                                                                          0.6, // 低曲率区域平滑强度
                                                                          10,  // 窗口大小
                                                                          20,  // 最大迭代次数
                                                                          1e-6 // 收敛阈值
            );
            auto                                                 smoothed_path = smoother.smooth(local_path);
            ApInfo("task_executor") << "smooth " << smoothed_path.size() << " points end ...";

            pgccs_l.clear();
            for (std::int32_t i = 0; i < smoothed_path.size(); ++i) {
                const auto&  p = smoothed_path[i];
                GeoPointGCCS pgccs;
                pgccs.xg = p.x;
                pgccs.yg = p.y;
                pgccs_l.push_back(pgccs);
            }

            auto new_waypoints = app_common::GeoUtils::PointGCCS2Waypoints(pgccs_l, longZone);
            ApInfo("task_executor") << "smooth_waypoints--new waypoints count: " << new_waypoints.size();
            waypoints = std::move(new_waypoints);
        }

    public:
        std::size_t _min_way_points = 10; // 最少能够执行循迹的路点数（加密后的点数，约 0.1 米分辨率）
        std::vector<Waypoint> _readout_waypoints; // 读取出来的或规划出来的路径点集
    };
} // namespace app_common

#endif
