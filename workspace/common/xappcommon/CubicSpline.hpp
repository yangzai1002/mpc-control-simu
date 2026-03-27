#ifndef _CUBIC_SPLINE_HPP
#define _CUBIC_SPLINE_HPP

#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// using Vec_f = std::vector<double>;
// using Poi_f = std::array<double, 2>;
// using Vec_Poi = std::vector<Poi_f>;

namespace app_common {

    inline std::vector<double> vec_diff(std::vector<double> input) {
        std::vector<double> output;
        for (unsigned int i = 1; i < input.size(); i++) {
            output.push_back(input[i] - input[i - 1]);
        }
        return output;
    };

    inline std::vector<double> cum_sum(std::vector<double> input) {
        std::vector<double> output;
        double              temp = 0;
        for (unsigned int i = 0; i < input.size(); i++) {
            temp += input[i];
            output.push_back(temp);
        }
        return output;
    };

    inline void removeConsecutiveDuplicates(std::vector<double>& x, std::vector<double>& y) {
        double epsilon = 1e-2;

        // 检查x和y的长度是否相等
        if (x.size() != y.size()) {
            std::cerr << "Error: Vectors x and y must have the same length." << std::endl;
            return;
        }

        // 用于存储新的点集合
        std::vector<double> newX;
        std::vector<double> newY;

        // 至少添加第一个点
        if (!x.empty()) {
            newX.push_back(x[0]);
            newY.push_back(y[0]);
        }

        //    ROS_INFO("OK1");

        // 遍历点，比较和前一个点是否相同
        for (size_t i = 1; i < x.size(); ++i) {
            //        ROS_INFO("OK2");

            if (!(std::fabs(x[i] - x[i - 1]) < epsilon && std::fabs(y[i] - y[i - 1]) < epsilon)) {
                //            ROS_INFO("OK3");

                // 如果当前点和前一个点不同，则添加到新集合
                newX.push_back(x[i]);
                newY.push_back(y[i]);
            }
        }

        // 更新原始向量
        x = newX;
        y = newY;
    }

    class Spline {
    public:
        std::vector<double> x;
        std::vector<double> y;
        int                 nx;
        std::vector<double> h;
        std::vector<double> a;
        std::vector<double> b;
        std::vector<double> c;
        // Eigen::VectorXf c;
        std::vector<double> d;

        Spline(){};
        // d_i * (x-x_i)^3 + c_i * (x-x_i)^2 + b_i * (x-x_i) + a_i
        Spline(std::vector<double> x_, std::vector<double> y_) {
            //        ROS_INFO("x size:%zu", x_.size());
            // 删除相邻的重复元素，会导致求解失败
            removeConsecutiveDuplicates(x_, y_);
            //        ROS_INFO("new x size:%zu", x_.size());
            x = x_;
            y = y_;
            nx = int(x_.size());
            h = vec_diff(x_);
            a = y_;

            Eigen::MatrixXf A = calc_A();
            Eigen::VectorXf B = calc_B();

            Eigen::VectorXf c_eigen = A.colPivHouseholderQr().solve(B);

            float* c_pointer = c_eigen.data();
            // Eigen::Map<Eigen::VectorXf>(c, c_eigen.rows(), 1) = c_eigen;

            c.assign(c_pointer, c_pointer + c_eigen.rows());

            for (int i = 0; i < nx - 1; i++) {
                d.push_back((c[i + 1] - c[i]) / (3.0 * h[i]));
                b.push_back((a[i + 1] - a[i]) / h[i] - h[i] * (c[i + 1] + 2 * c[i]) / 3.0);
            }
        };

        double calc(double t) {
            if (t < x.front() || t > x.back()) {
                throw std::invalid_argument("received value out of the pre-defined range");
            }
            int    seg_id = bisect(t, 0, nx);
            double dx = t - x[seg_id];
            return a[seg_id] + b[seg_id] * dx + c[seg_id] * dx * dx + d[seg_id] * dx * dx * dx;
        };

        double calc_d(double t) {
            if (t < x.front() || t > x.back()) {
                throw std::invalid_argument("received value out of the pre-defined range");
            }
            int    seg_id = bisect(t, 0, nx - 1);
            double dx = t - x[seg_id];
            return b[seg_id] + 2 * c[seg_id] * dx + 3 * d[seg_id] * dx * dx;
        }

        double calc_dd(double t) {
            if (t < x.front() || t > x.back()) {
                throw std::invalid_argument("received value out of the pre-defined range");
            }
            int    seg_id = bisect(t, 0, nx);
            double dx = t - x[seg_id];
            return 2 * c[seg_id] + 6 * d[seg_id] * dx;
        }

    private:
        Eigen::MatrixXf calc_A() {
            Eigen::MatrixXf A = Eigen::MatrixXf::Zero(nx, nx);
            //        ROS_INFO("nx: %d", nx);
            A(0, 0) = 1;
            for (int i = 0; i < nx - 1; i++) {
                if (i != nx - 2) {
                    A(i + 1, i + 1) = 2 * (h[i] + h[i + 1]);
                }
                A(i + 1, i) = h[i];
                A(i, i + 1) = h[i];
            }
            A(0, 1) = 0.0;
            A(nx - 1, nx - 2) = 0.0;
            A(nx - 1, nx - 1) = 1.0;
            return A;
        };
        Eigen::VectorXf calc_B() {
            Eigen::VectorXf B = Eigen::VectorXf::Zero(nx);
            for (int i = 0; i < nx - 2; i++) {
                B(i + 1) = 3.0 * (a[i + 2] - a[i + 1]) / h[i + 1] - 3.0 * (a[i + 1] - a[i]) / h[i];
            }
            return B;
        };

        int bisect(double t, int start, int end) {
            int mid = (start + end) / 2;
            if (t == x[mid] || end - start <= 1) {
                return mid;
            } else if (t > x[mid]) {
                return bisect(t, mid, end);
            } else {
                return bisect(t, start, mid);
            }
        }
    };

    class Spline2D {
    public:
        Spline              sx;
        Spline              sy;
        std::vector<double> s;

        Spline2D() = default;
        Spline2D(std::vector<double> x, std::vector<double> y) {
            s = calc_s(x, y);
            sx = Spline(s, x);
            sy = Spline(s, y);
        };

        std::array<double, 2> calc_postion(double s_t) {
            double x = sx.calc(s_t);
            double y = sy.calc(s_t);
            return {{x, y}};
        };

        double calc_curvature(double s_t) {
            double dx = sx.calc_d(s_t);
            double ddx = sx.calc_dd(s_t);
            double dy = sy.calc_d(s_t);
            double ddy = sy.calc_dd(s_t);
            return (ddy * dx - ddx * dy) / (dx * dx + dy * dy);
        };

        double calc_yaw(double s_t) {
            double dx = sx.calc_d(s_t);
            double dy = sy.calc_d(s_t);
            return std::atan2(dy, dx);
        };

    private:
        std::vector<double> calc_s(std::vector<double> x, std::vector<double> y) {
            std::vector<double> ds;
            std::vector<double> out_s{0};
            std::vector<double> dx = vec_diff(x);
            std::vector<double> dy = vec_diff(y);

            for (unsigned int i = 0; i < dx.size(); i++) {
                ds.push_back(std::sqrt(dx[i] * dx[i] + dy[i] * dy[i]));
            }

            std::vector<double> cum_ds = cum_sum(ds);
            out_s.insert(out_s.end(), cum_ds.begin(), cum_ds.end());
            return out_s;
        };
    };

} // namespace app_common
#endif
