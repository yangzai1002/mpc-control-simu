#ifndef REFERENCELINE_H
#define REFERENCELINE_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

#include "CubicSpline.hpp"
#include "OsqpEigen/OsqpEigen.h"
#include "log/log.h"

namespace app_common {
    class ReferenceLine {
    public:
        struct path_point {
            double x;
            double y;
            double s_;
            double yaw;
            double cur;
        };

    public:
        double lookahead_dist;
        int    match_index;

        ReferenceLine(double lookahead_distance) : lookahead_dist(lookahead_distance), match_index(0) {
        }

        ReferenceLine(double lookahead_distance, std::unordered_map<std::string, double> &referline_params) {
            lookahead_dist = lookahead_distance;
            match_index = 0;
            ref_weight_smooth = referline_params["ref_weight_smooth"];           // 参考线平滑代价
            ref_weight_path_length = referline_params["ref_weight_path_length"]; // 参考线轨迹长度代价
            ref_weight_ref_deviation = referline_params["w_lat_offset"];         // 参考线偏移代价

            //    ROS_INFO("ref_weight_smooth: %f", ref_weight_smooth);
            //    ROS_INFO("ref_weight_path_length: %f", ref_weight_path_length);
            //    ROS_INFO("ref_weight_ref_deviation: %f", ref_weight_ref_deviation);
            // 二次规划几何相似度约束
            x_lower_bound = referline_params["x_lower_bound"];
            x_upper_bound = referline_params["x_upper_bound"];
            y_lower_bound = referline_params["y_lower_bound"];
            y_upper_bound = referline_params["y_upper_bound"];
        }

#if 0
    int search_target_index(const double &cur_x, const double &cur_y, const std::vector<path_point> &waypoints,
                            const double &lookahead_distance) {
        double dist;
        for (int i = match_index; i < path.size(); i++) {
            dist = cal_distance(cur_x, cur_y, path[i].x, path[i].y);
            if (dist > lookahead_distance) {
                return i;
            }
        }
        return path.size() - 1;
    }

    std::vector<path_point> local_path_truncation(const car_state &cur_pose, const std::vector<path_point> &global_path,
                                                  const int &pre_match_index) {
        this->match_index = search_match_index(cur_pose.x, cur_pose.y, pre_match_index, global_path);
        ROS_INFO("Match point_index is %d:", match_index);

        int target_index = search_target_index(cur_pose.x, cur_pose.y, global_path, lookahead_dist);
        // ROS_INFO("Tatget point_index is %d:", target_index);

        // 向匹配点前方5m的点作为参考线起始点
        size_t start_index = 0;
        double front_distance = 0;
        for (size_t i = this->match_index; i > 0; i--) {
            front_distance += std::sqrt(std::pow(global_path[i].x - global_path[i - 1].x, 2) +
                                        std::pow(global_path[i].y - global_path[i - 1].y, 2));
            if (front_distance > 5.0) {
                start_index = i - 1;
                break;
            }
        }

        std::vector<path_point> target_path(global_path.begin() + start_index, global_path.begin() + target_index + 1);
        // ROS_INFO("Size of target_path :%d", target_path.size());
        return target_path;
    }

    // cublic Spiline平滑
    std::vector<path_point> smoothing(double delta_s, double path_length, Spline2D &ref_frenet,
                                      const std::vector<path_point> &local_path) {
        std::vector<path_point> ref_path;
        ref_path.clear();
        for (double i = 0; (i < ref_frenet.s.back() && i < path_length); i += delta_s) {
            std::array<double, 2> point_ = ref_frenet.calc_postion(i);
            path_point            ref_point;
            ref_point.x = point_[0];
            ref_point.y = point_[1];
            ref_point.yaw = ref_frenet.calc_yaw(i);
            //        AINFO << "cubic " << i << " : " << ref_point.yaw;

            ref_point.cur = ref_frenet.calc_curvature(i);
            ref_point.s_ = i;
            ref_path.push_back(ref_point);
        }
        // ROS_INFO("The size of ref_path is:%zu", ref_path.size());
        return ref_path;
    }
#endif
        // 离散点平滑（Apollo）
        std::vector<path_point> discrete_smooth(const std::vector<path_point> &local_path) {
            std::vector<path_point>                smoothed_path;
            std::vector<std::pair<double, double>> path_point2d;
            for (auto point : local_path) {
                path_point2d.push_back(std::make_pair(point.x, point.y));
            }
            // 二次规划求解
            discrete_points_osqp(path_point2d);
            for (auto point2d : path_point2d) {
                path_point p;
                p.x = point2d.first;
                p.y = point2d.second;
                smoothed_path.push_back(p);
            }
            cal_heading(smoothed_path);
            return smoothed_path;
        }

        // 离散点平滑的二次规划求解
        /**
         * @brief 离散点平滑的二次规划求解
         *
         * 二次规划QP
         * 0.5x'Hx + f'x = min
         * lb < Ax < ub
         * 二次规划形式

         * H1 = w_cost_smooth*(A1'*A1) + w_cost_length*(A2'*A2) + w_cost_ref*(A3'*A3)
         * f = -2 * w_cost_ref * referenceline_init
         * x'H1x + f'x = 0.5 * x'(2H1)*x + f'x
         * A1 = [1  0 -2  0  1  0
         *       0  1  0 -2  0  1
         *             1  0 -2  0  1  0
         *             0  1  0 -2  0  1
         *                   ...............

         *A2 = [1  0 -1  0
         *      0  1  0 -1
         *            1  0 -1  0
         *            0  1  0 -1
         *                  ...........
         *A3 为单位矩阵
         * @param path_point2d
         */
        void discrete_points_osqp(std::vector<std::pair<double, double>> &path_point2d) {
            int n = path_point2d.size();

            // 初始化A1,A2,A3，f,lb,ub矩阵
            // 平滑代价系数矩阵，x'A1'A1x, (n-2)
            Eigen::SparseMatrix<double> A1(2 * n, 2 * n);
            // 路径长度代价矩阵 x'A2'A2x,(n-1)
            Eigen::SparseMatrix<double> A2(2 * n, 2 * n);
            // 参考线偏离代价矩阵 x'A3'A3x,单位阵
            Eigen::SparseMatrix<double> A3(2 * n, 2 * n);

            Eigen::SparseMatrix<double> H(2 * n, 2 * n); // 必须是稀疏矩阵
            Eigen::VectorXd             f = Eigen::VectorXd::Zero(2 * n);
            Eigen::SparseMatrix<double> A(2 * n, 2 * n);
            Eigen::VectorXd             lb = Eigen::VectorXd::Zero(2 * n);
            Eigen::VectorXd             ub = Eigen::VectorXd::Zero(2 * n);
            Eigen::VectorXd             qp_solution = Eigen::VectorXd::Zero(2 * n);

            A.setIdentity();

            // 赋值f,lb,ub;
            //  MatrixXd下标从(0,0)开始,(1,2)表示第1行第2列
            for (int i = 0; i < n; i++) {
                f(2 * i) = path_point2d[i].first;
                f(2 * i + 1) = path_point2d[i].second;

                lb(2 * i) = f(2 * i) + x_lower_bound;
                lb(2 * i + 1) = f(2 * i + 1) + y_lower_bound;

                ub(2 * i) = f(2 * i) + x_upper_bound;
                ub(2 * i + 1) = f(2 * i + 1) + y_upper_bound;
            }

            // 赋值A1
            for (int j = 0; j < n - 2; j++) {
                A1.insert(2 * j, 2 * j) = 1;
                A1.insert(2 * j, 2 * j + 2) = -2;
                A1.insert(2 * j, 2 * j + 4) = 1;
                A1.insert(2 * j + 1, 2 * j + 1) = 1;
                A1.insert(2 * j + 1, 2 * j + 3) = -2;
                A1.insert(2 * j + 1, 2 * j + 5) = 1;
            }
            // 赋值A2
            for (int k = 0; k < n - 1; k++) {
                A2.insert(2 * k, 2 * k) = 1;
                A2.insert(2 * k, 2 * k + 2) = -1;
                A2.insert(2 * k + 1, 2 * k + 1) = 1;
                A2.insert(2 * k + 1, 2 * k + 3) = -1; // 好像不对？1
            }

            A3.setIdentity();

            // H = 2 * (config_.weight_smooth * (A1.transpose().dot(A1)) +
            //          config_.weight_path_length * (A2.transpose().dot(A2)) +
            //          config_.weight_ref_deviation * A3);
            H = 2 * (ref_weight_smooth * A1.transpose() * A1 + ref_weight_path_length * A2.transpose() * A2 +
                     ref_weight_ref_deviation * A3);

            f = -2 * ref_weight_ref_deviation * f;

            OsqpEigen::Solver solver;
            solver.settings()->setAbsoluteTolerance(1e-5);
            solver.settings()->setRelativeTolerance(1e-5);
            solver.settings()->setPrimalInfeasibilityTollerance(1e-6);
            solver.settings()->setDualInfeasibilityTollerance(1e-6);
            solver.settings()->setWarmStart(true);
            solver.settings()->setVerbosity(true);
            solver.settings()->setVerbosity(false);
            solver.data()->setNumberOfVariables(2 * n);
            solver.data()->setNumberOfConstraints(2 * n);
            solver.data()->setHessianMatrix(H);
            solver.data()->setGradient(f);
            solver.data()->setLinearConstraintsMatrix(A);
            solver.data()->setLowerBound(lb);
            solver.data()->setUpperBound(ub);

            if (!solver.initSolver()) {
                AERROR << "QSOP init failed";
                return;
            }
            if (!solver.solve()) {
                AERROR << "QSOP solve failed";
                return;
            }
            qp_solution = solver.getSolution();

            for (int i = 0; i < n; i++) {
                path_point2d[i].first = qp_solution(2 * i);
                path_point2d[i].second = qp_solution(2 * i + 1);
            }
        }

        void cal_heading(std::vector<path_point> &waypoints) {
            double x_delta = 0.0;
            double y_delta = 0.0;
            double x_delta_2 = 0.0;
            double y_delta_2 = 0.0;
            double s = 0.0;
            for (int i = 0; i < waypoints.size(); i++) {
                if (i >= 1) {
                    s += cal_distance(waypoints[i].x, waypoints[i].y, waypoints[i - 1].x, waypoints[i - 1].y);
                }
                if (i == 0) {
                    x_delta = (waypoints[i + 1].x - waypoints[i].x);
                    y_delta = (waypoints[i + 1].y - waypoints[i].y);
                    x_delta_2 = (waypoints[i + 2].x - waypoints[i + 1].x) - (waypoints[i + 1].x - waypoints[i].x);
                    y_delta_2 = (waypoints[i + 2].y - waypoints[i + 1].y) - (waypoints[i + 1].y - waypoints[i].y);
                } else if (i == waypoints.size() - 1) {
                    x_delta = (waypoints[i].x - waypoints[i - 1].x);
                    y_delta = (waypoints[i].y - waypoints[i - 1].y);
                    x_delta_2 = (waypoints[i].x - waypoints[i - 1].x) - (waypoints[i - 1].x - waypoints[i - 2].x);
                    y_delta_2 = (waypoints[i].y - waypoints[i - 1].y) - (waypoints[i - 1].y - waypoints[i - 2].y);
                } else {
                    x_delta = 0.5 * (waypoints[i + 1].x - waypoints[i - 1].x);
                    y_delta = 0.5 * (waypoints[i + 1].y - waypoints[i - 1].y);
                    x_delta_2 = (waypoints[i + 1].x - waypoints[i].x) - (waypoints[i].x - waypoints[i - 1].x);
                    y_delta_2 = (waypoints[i + 1].y - waypoints[i].y) - (waypoints[i].y - waypoints[i - 1].y);
                }
                waypoints[i].s_ = s;
                waypoints[i].yaw = std::atan2(y_delta, x_delta);

                //        // 将角度限制在 -π 到 π 之间
                //        while (waypoints[i].yaw < 0) {
                //            waypoints[i].yaw += 2 * M_PI;
                //        }
                //        while (waypoints[i].yaw > 2 * M_PI) {
                //            waypoints[i].yaw -= 2 * M_PI;
                //        }

                //        AINFO << "smooth " << i << " : " << waypoints[i].yaw;

                //  参数方程曲率计算
                waypoints[i].cur = std::abs(y_delta_2 * x_delta - x_delta_2 * y_delta) /
                                   std::pow((x_delta * x_delta + y_delta * y_delta), 3 / 2);
            }
        }

        static inline double cal_distance(double x1, double y1, double x2, double y2) {
            double x = (x1 - x2);
            double y = (y1 - y2);
            return std::sqrt(x * x + y * y);
        }

    private:
        // 离散点平滑平滑相关参数
        double ref_weight_smooth;        // 参考线平滑代价
        double ref_weight_path_length;   // 参考线轨迹长度代价
        double ref_weight_ref_deviation; // 参考线偏移代价
        // 二次规划几何相似度约束
        double x_lower_bound;
        double x_upper_bound;
        double y_lower_bound;
        double y_upper_bound;
    };
} // namespace app_common
#endif // REFERENCELINE_H
