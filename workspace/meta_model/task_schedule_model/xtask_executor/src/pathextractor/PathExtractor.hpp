#ifndef _PATH_EXTRACTOR_HPP_
#define _PATH_EXTRACTOR_HPP_

#include <cmath>
#include <vector>

#include "Eigen/Dense"
#include "GeoUtils.hpp"
#include "ara/adsfi/impl_type_businesspath.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
#include "yaml-cpp/yaml.h"

namespace app_common {
    struct LocalPathPolicyItem {
        double expected_speed_kph;  // 期望速度，单位：千米每小时
        double local_path_length_m; // 对应的局部路径长度，单位：米
    };
} // namespace app_common

namespace YAML {
    template <> struct convert<app_common::LocalPathPolicyItem> {
        static Node encode(const app_common::LocalPathPolicyItem& p) {
            Node node;
            node["expected_speed_kph"] = p.expected_speed_kph;
            node["local_path_length_m"] = p.local_path_length_m;
            return node;
        }

        static bool decode(const Node& node, app_common::LocalPathPolicyItem& p) {
            if (!node.IsMap() || !node["expected_speed_kph"] || !node["local_path_length_m"]) {
                return false;
            }

            p.expected_speed_kph = node["expected_speed_kph"].as<double>();
            p.local_path_length_m = node["local_path_length_m"].as<double>();
            return true;
        }
    };
} // namespace YAML

namespace app_common {
    class ExceptionLoadExtractorConfig : public std::exception {
    public:
        const std::string config_name;
        explicit ExceptionLoadExtractorConfig(const std::string& config_name_)
            : std::exception(), config_name(config_name_) {
        }
    };

    class ExceptionStartTooFewPoints : public std::exception {
    public:
        std::size_t point_count;
        explicit ExceptionStartTooFewPoints(std::size_t point_count_) : std::exception(), point_count(point_count_) {
        }
    };

    class PathExtractor {
    public:
        explicit PathExtractor(std::vector<std::vector<ara::adsfi::BusinessPoint>>&& path)
            : _planned_path_segments(std::move(path)), _current_segment_index(0) {
            _min_way_points = 10;
            std::string full_path = "planner/track/min_way_points";
            try {
                _min_way_points = CustomStack::Instance()->GetProjectConfigValue<std::size_t>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _min_way_points;

            full_path = "planner/track/max_distance_to_nearest_point";
            try {
                _max_distance_to_nearest_point = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _max_distance_to_nearest_point;

            full_path = "planner/track/look_ahead_time";
            try {
                _look_ahead_time = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _look_ahead_time;

            full_path = "planner/track/min_look_ahead_distance";
            try {
                _min_look_ahead_distance = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _min_look_ahead_distance;

            full_path = "planner/track/max_heading_diff";
            try {
                _max_heading_diff = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _max_heading_diff;

            full_path = "planner/track/heading_fit_points";
            try {
                _heading_fit_points = CustomStack::Instance()->GetProjectConfigValue<int>(full_path);
            } catch (...) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            ApInfo("task_executor") << full_path << " = " << _heading_fit_points;

            full_path = "planner/track/local_path_policy";
            if (!CustomStack::Instance()->GetProjectConfigArray(full_path, _local_path_policy) ||
                _local_path_policy.empty()) {
                ApError("task_executor") << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            for (const auto& item : _local_path_policy) {
                ApInfo("task_executor") << "  expected_speed = " << item.expected_speed_kph
                      << ", local_path_length = " << item.local_path_length_m;
            }
            // check if local_path_policy is in ascending order of expected_speed
            for (std::size_t i = 1; i < _local_path_policy.size(); ++i) {
                if (_local_path_policy[i].expected_speed_kph <= _local_path_policy[i - 1].expected_speed_kph) {
                    ApError("task_executor") << "local_path_policy is not in ascending order of expected_speed";
                    throw ExceptionLoadExtractorConfig(full_path);
                }
            }

            // remaining_length_m：用于循迹进度（progress）计算。
            // 口径：当前任务剩余轨迹总长度（米），随着提路时路径点被裁剪/段完成而递减。
            // 注意：具体递减逻辑由各派生类（例如 PathExtractorSimple）在 next()/done()/reset() 中维护。
            _remaining_length_m = 0.0;
            for (const auto& seg : _planned_path_segments) {
                _remaining_length_m += app_common::GeoUtils::gccs_distance(seg);
            }
        }

        virtual ~PathExtractor() = default;

        virtual void reset() {
            _planned_path_segments.clear();
            _current_segment_index = 0;
            _remaining_length_m = 0.0;
        }

        virtual void done() = 0;

        virtual bool is_all_done() {
            return _planned_path_segments.empty();
        }

        virtual double remaining_length_m() const {
            return _remaining_length_m;
        }

        // base on local path policy, return expected local path length according to expected speed in km/h
        virtual double expected_local_path_length(double expected_speed_kph) {
            for (const auto& policy : _local_path_policy) {
                if (expected_speed_kph <= policy.expected_speed_kph) {
                    return policy.local_path_length_m;
                }
            }
            return _local_path_policy.back().local_path_length_m;
        }

        virtual ara::adsfi::BusinessPath next(double cur_lat, double cur_lon, double xg, double yg, double yaw,
                                              double cur_speed_mps, double expected_speed_mps) = 0;

        static std::vector<ara::adsfi::BusinessPoint> points_extraction(
            bool forward, const std::vector<ara::adsfi::BusinessPoint>& full_points, size_t center_idx,
            std::size_t num_points) {
            const auto full_points_size = full_points.size();
            if (center_idx >= full_points_size) {
                throw std::runtime_error("center index out of range");
            }
            if (num_points > full_points_size) {
                throw std::runtime_error("too many points to extract");
            }

            std::vector<ara::adsfi::BusinessPoint> pts;
            for (size_t i = 0; i < num_points; ++i) {
                pts.push_back(full_points[center_idx]);
                center_idx = forward ? (center_idx + 1) % full_points_size
                                     : (center_idx + full_points_size - 1) % full_points_size;
            }

            return pts;
        }

        static std::optional<double> estimate_north_heading(const std::vector<ara::adsfi::BusinessPoint>& points) {
            if (points.size() < 2) {
                return 0.0;
            }
            // ApInfo("task_executor") << "heading points: ";
            // for (const auto& p : points) {
            //     ApInfo("task_executor") << "heading point: " << GeoUtils::str_gccs(p);
            // }

            Eigen::Vector2d mean(0.0, 0.0);
            for (const auto& p : points) {
                mean.x() += p.xg;
                mean.y() += p.yg;
            }
            mean /= static_cast<double>(points.size());

            Eigen::Matrix2d cov = Eigen::Matrix2d::Zero();
            for (const auto& p : points) {
                Eigen::Vector2d d(p.xg - mean.x(), p.yg - mean.y());
                cov += d * d.transpose();
            }
            cov /= static_cast<double>(points.size());

            Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> solver(cov);
            if (solver.info() != Eigen::Success) {
                ApError("task_executor") << "Eigen::SelfAdjointEigenSolver failed";
                return std::nullopt; // 特征值分解失败
            }
            Eigen::Vector2d dir = solver.eigenvectors().col(1);
            Eigen::Vector2d ref(points.back().xg - points.front().xg, points.back().yg - points.front().yg);
            if (dir.dot(ref) < 0.0) {
                dir = -dir;
            }
            double heading_rad = std::atan2(dir.x(), dir.y());
            double heading_deg = heading_rad * 180.0 / M_PI;
            if (heading_deg < 0.0) {
                heading_deg += 360.0;
            }

            return heading_deg;
        }

        static double heading_diff_deg(double a, double b) {
            double d = std::fmod(a - b, 360.0);
            if (d < 0) {
                d += 360.0;
            }

            if (d > 180.0) {
                d = 360.0 - d;
            }

            return d;
        }

    public:
        std::size_t _min_way_points = 10; // 最少能够执行循迹的路点数（加密后的点数，约 0.1 米分辨率）
        double _max_distance_to_nearest_point = 20.0; // 车辆距离路径最近点的最大距离（单位：米）
        double _look_ahead_time = 2.0; // 在路径中搜索最佳匹配自车位置的点时，向前搜索的时间约束（单位：秒）
        double _min_look_ahead_distance = 2.0; // 在路径中搜索最佳匹配自车位置的点时，向前搜索的最小距离（单位：米）
        double _max_heading_diff = 0.0; // 绕圈循迹时，自动选择循迹方向时的最大航向差（单位：度）
        std::size_t _heading_fit_points =
            10; // 绕圈循迹时，自动选择循迹方向时用于拟合航向路点数（加密后的点数，约 0.1米分辨率）
        std::vector<LocalPathPolicyItem> _local_path_policy; // 局部参考路径提取策略

        std::vector<std::vector<ara::adsfi::BusinessPoint>> _planned_path_segments;
        std::int32_t                                        _current_segment_index;
        double                                              _remaining_length_m = 0.0;
    };
} // namespace app_common

#endif
