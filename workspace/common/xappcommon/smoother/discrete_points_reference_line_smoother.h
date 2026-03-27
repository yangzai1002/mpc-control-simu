#pragma once

#include <Eigen/Eigen>
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
//#include "cyber/common/file.h"
//#include "log.h"
//#include "modules/common/util/util.h"
//#include "modules/planning/common/planning_gflags.h"
#include "discrete_points_math.h"
#include "fem_pos_deviation_smoother.h"

namespace avos::planning {

    class DiscretePointsReferenceLineSmoother {
    public:
        explicit DiscretePointsReferenceLineSmoother() = default;

        ~DiscretePointsReferenceLineSmoother() = default;
        void test(){

        };
        bool Smooth(const std::vector<double> &raw_x, const std::vector<double> &raw_y, std::vector<double> &solve_x,
                    std::vector<double> &solve_y) {
            std::vector<std::pair<double, double>> raw_point2d;
            std::vector<double>                    anchorpoints_lateralbound;

            for (int i = 0; i < raw_x.size(); ++i) {
                raw_point2d.emplace_back(raw_x.at(i), raw_y.at(i));
                anchorpoints_lateralbound.emplace_back(1.0);
            }
            // fix front and back points to avoid end states deviate from the center of
            // road
            anchorpoints_lateralbound.front() = 0.0;
            anchorpoints_lateralbound.back() = 0.0;

            NormalizePoints(&raw_point2d);

            bool                                   status = false;
            std::vector<std::pair<double, double>> smoothed_point2d;
            status = FemPosSmooth(raw_point2d, anchorpoints_lateralbound, &smoothed_point2d);

            if (!status) {
                std::cout << "discrete_points reference line smoother fails" << '\n';
                return false;
            }

            DeNormalizePoints(&smoothed_point2d);
            if (smoothed_point2d.size() < 2) {
                std::cout << "Fail to generate smoothed reference line." << '\n';
                return false;
            }

            for (const auto &item : smoothed_point2d) {
                solve_x.emplace_back(item.first);
                solve_y.emplace_back(item.second);
            }

            return true;
        }

        bool Smooth(const std::vector<double> &raw_x, const std::vector<double> &raw_y, std::vector<double> &solve_x,
                    std::vector<double> &solve_y,
                    const std::vector<double>& anchor) {
            std::vector<std::pair<double, double>> raw_point2d;
            std::vector<double>                    anchorpoints_lateralbound;

            for (int i = 0; i < raw_x.size(); ++i) {
                raw_point2d.emplace_back(raw_x.at(i), raw_y.at(i));
            }
            // fix front and back points to avoid end states deviate from the center of
            // road
            anchorpoints_lateralbound.front() = 0.0;
            anchorpoints_lateralbound.back() = 0.0;

            NormalizePoints(&raw_point2d);

            bool                                   status = false;
            std::vector<std::pair<double, double>> smoothed_point2d;
            status = FemPosSmooth(raw_point2d, anchor, &smoothed_point2d);

            if (!status) {
                std::cout << "discrete_points reference line smoother fails" << '\n';
                return false;
            }

            DeNormalizePoints(&smoothed_point2d);
            if (smoothed_point2d.size() < 2) {
                std::cout << "Fail to generate smoothed reference line." << '\n';
                return false;
            }

            for (const auto &item : smoothed_point2d) {
                solve_x.emplace_back(item.first);
                solve_y.emplace_back(item.second);
            }

            return true;
        }

    private:
        static bool FemPosSmooth(const std::vector<std::pair<double, double>> &raw_point2d,
                                 const std::vector<double> &                   bounds,
                                 std::vector<std::pair<double, double>> *      ptr_smoothed_point2d) {
            // const auto& fem_pos_config =
            //     config_.discrete_points().fem_pos_deviation_smoothing();

            // FemPosDeviationSmoother smoother(fem_pos_config);
            FemPosDeviationSmoother smoother;

            // box contraints on pos are used in fem pos smoother, thus shrink the
            // bounds by 1.0 / sqrt(2.0)
            std::vector<double> box_bounds = bounds;
            //    const double box_ratio =1.0 / sqrt(2.0);
            const double box_ratio = 2.0 / sqrt(2.0);
            // 3.0 / std::sqrt(2.0);
            for (auto &bound : box_bounds) {
                bound *= box_ratio;
            }

            std::vector<double> opt_x;
            std::vector<double> opt_y;
            bool                status = smoother.Solve(raw_point2d, box_bounds, &opt_x, &opt_y);

            if (!status) {
                //    AERROR << "Fem Pos reference line smoothing failed";
                return false;
            }

            if (opt_x.size() < 2 || opt_y.size() < 2) {
                //    AERROR << "Return by fem pos smoother is wrong. Size smaller than 2 ";
                return false;
            }

            //  CHECK_EQ(opt_x.size(), opt_y.size()) << "x and y result size not equal";

            size_t point_size = opt_x.size();
            for (size_t i = 0; i < point_size; ++i) {
                ptr_smoothed_point2d->emplace_back(opt_x[i], opt_y[i]);
            }

            return true;
        }

        void NormalizePoints(std::vector<std::pair<double, double>> *xy_points) {
            zero_x_ = xy_points->front().first;
            zero_y_ = xy_points->front().second;
            std::for_each(xy_points->begin(), xy_points->end(), [this](std::pair<double, double> &point) {
                auto                      curr_x = point.first;
                auto                      curr_y = point.second;
                std::pair<double, double> xy(curr_x - zero_x_, curr_y - zero_y_);
                point = std::move(xy);
            });
        }

        void DeNormalizePoints(std::vector<std::pair<double, double>> *xy_points) {
            std::for_each(xy_points->begin(), xy_points->end(), [this](std::pair<double, double> &point) {
                auto                      curr_x = point.first;
                auto                      curr_y = point.second;
                std::pair<double, double> xy(curr_x + zero_x_, curr_y + zero_y_);
                point = std::move(xy);
            });
        }

        // bool GenerateRefPointProfile(
        //     const ReferenceLine& raw_reference_line,
        //     const std::vector<std::pair<double, double>>& xy_points,
        //     std::vector<ReferencePoint>* reference_points);

        //    std::vector<AnchorPoint> anchor_points_{};

        double zero_x_ = 0.0;

        double zero_y_ = 0.0;
    };

} // namespace avos::planning
