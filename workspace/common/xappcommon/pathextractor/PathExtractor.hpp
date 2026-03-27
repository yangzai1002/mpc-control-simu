#ifndef _PATH_EXECUTOR_HPP_
#define _PATH_EXECUTOR_HPP_

#include <cmath>
#include <vector>

#include "Eigen/Dense"
#include "GeoUtils.hpp"
#include "ara/adsfi/impl_type_idpbusinesspath.h"
#include "custom_stack.h"

namespace app_common {
    class ExceptionLoadExtractorConfig : public std::exception {
    public:
        const std::string config_name;
        explicit ExceptionLoadExtractorConfig(const std::string& config_name_)
            : std::exception(), config_name(config_name_) {
        }
    };

    class ExceptionTooFarToNearestPoint : public std::exception {
    public:
        const double distance;
        explicit ExceptionTooFarToNearestPoint(double distance_) : std::exception(), distance(distance_) {
        }
    };

    class ExceptionStartTooFewPoints : public std::exception {
    public:
        std::size_t point_count;
        explicit ExceptionStartTooFewPoints(std::size_t point_count_) : std::exception(), point_count(point_count_) {
        }
    };

    class ExceptionHeadingDecision : public std::exception {
    public:
        const double heading;
        explicit ExceptionHeadingDecision(double heading_) : std::exception(), heading(heading_) {
        }
    };

    class ExceptionPathHeadingInconsistent : public std::exception {
    public:
        ExceptionPathHeadingInconsistent() : std::exception() {
        }
    };

    class PathExtractor {
    public:
        explicit PathExtractor(std::vector<std::vector<ara::adsfi::IdpBusinessPoint>>&& path)
            : _planned_path_segments(std::move(path)), _current_segment_index(0) {
            _min_way_points = 10;
            std::string full_path = "planner/track/min_way_points";
            try {
                _min_way_points = CustomStack::Instance()->GetProjectConfigValue<std::size_t>(full_path);
            } catch (...) {
                AERROR << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            AINFO << full_path << " = " << _min_way_points;

            full_path = "planner/track/max_distance_to_nearest_point";
            try {
                _max_distance_to_nearest_point = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                AERROR << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            AINFO << full_path << " = " << _max_distance_to_nearest_point;

            full_path = "planner/track/look_ahead_time";
            try {
                _look_ahead_time = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                AERROR << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            AINFO << full_path << " = " << _look_ahead_time;

            full_path = "planner/track/min_look_ahead_distance";
            try {
                _min_look_ahead_distance = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                AERROR << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            AINFO << full_path << " = " << _min_look_ahead_distance;

            full_path = "planner/track/max_heading_diff";
            try {
                _max_heading_diff = CustomStack::Instance()->GetProjectConfigValue<double>(full_path);
            } catch (...) {
                AERROR << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            AINFO << full_path << " = " << _max_heading_diff;

            full_path = "planner/track/heading_fit_points";
            try {
                _heading_fit_points = CustomStack::Instance()->GetProjectConfigValue<int>(full_path);
            } catch (...) {
                AERROR << "read project config " << full_path << " failed";
                throw ExceptionLoadExtractorConfig(full_path);
            }
            AINFO << full_path << " = " << _heading_fit_points;
        }

        virtual ~PathExtractor() = default;

        virtual void reset() {
            _planned_path_segments.clear();
            _current_segment_index = 0;
        }

        virtual void done() = 0;

        virtual bool is_all_done() {
            return _planned_path_segments.empty();
        }

        virtual ara::adsfi::IdpBusinessPath next(double cur_lat, double cur_lon, double xg, double yg, double yaw,
                                                 double cur_speed_mps, double expected_speed_mps) = 0;

        static std::vector<ara::adsfi::IdpBusinessPoint> points_extraction(
            bool forward, const std::vector<ara::adsfi::IdpBusinessPoint>& full_points, size_t center_idx,
            std::size_t num_points) {
            const auto full_points_size = full_points.size();
            if (center_idx >= full_points_size) {
                throw std::runtime_error("center index out of range");
            }
            if (num_points > full_points_size) {
                throw std::runtime_error("too many points to extract");
            }

            std::vector<ara::adsfi::IdpBusinessPoint> pts;
            for (size_t i = 0; i < num_points; ++i) {
                pts.push_back(full_points[center_idx]);
                center_idx = forward ? (center_idx + 1) % full_points_size
                                     : (center_idx + full_points_size - 1) % full_points_size;
            }

            return pts;
        }

        static std::optional<double> estimate_north_heading(const std::vector<ara::adsfi::IdpBusinessPoint>& points) {
            if (points.size() < 2) {
                return 0.0;
            }
            // AINFO << "heading points: ";
            // for (const auto& p : points) {
            //     AINFO << "heading point: " << GeoUtils::str_gccs(p);
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
                AERROR << "Eigen::SelfAdjointEigenSolver failed";
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

        std::vector<std::vector<ara::adsfi::IdpBusinessPoint>> _planned_path_segments;
        std::int32_t                                           _current_segment_index;
    };
} // namespace app_common

#endif
