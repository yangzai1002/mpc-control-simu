#pragma once

#include <utility>
#include <vector>

#include "fem_pos_deviation_smoother_config.h"

//#include "../../message/log.h"
#include "fem_pos_deviation_osqp_interface.h"
#include "fem_pos_deviation_sqp_osqp_interface.h"

namespace avos::planning {

    /*
     * @brief:
     * This class solve an optimization problem:
     * Y
     * |
     * |                       P(x1, y1)  P(x2, y2)
     * |            P(x0, y0)                       ... P(x(k-1), y(k-1))
     * |P(start)
     * |
     * |________________________________________________________ X
     *
     *
     * Given an initial set of points from 0 to k-1,  The goal is to find a set of
     * points which makes the line P(start), P0, P(1) ... P(k-1) "smooth".
     */

    class FemPosDeviationSmoother {
    public:
        explicit FemPosDeviationSmoother() = default;

        bool Solve(const std::vector<std::pair<double, double>> &raw_point2d, const std::vector<double> &bounds,
                   std::vector<double> *opt_x, std::vector<double> *opt_y) {
            //    if (config_.apply_curvature_constraint()) {
            //      if (config_.use_sqp()) {
            //        return SqpWithOsqp(raw_point2d, bounds, opt_x, opt_y);
            //      } else {
            //        return NlpWithIpopt(raw_point2d, bounds, opt_x, opt_y);
            //      }
            //    } else {
            return QpWithOsqp(raw_point2d, bounds, opt_x, opt_y);
            //    }
        }

        bool QpWithOsqp(const std::vector<std::pair<double, double>> &raw_point2d, const std::vector<double> &bounds,
                        std::vector<double> *opt_x, std::vector<double> *opt_y) {
            if (opt_x == nullptr || opt_y == nullptr) {
                //    AERROR << "opt_x or opt_y is nullptr";
                return false;
            }

            FemPosDeviationOsqpInterface solver;

            solver.set_weight_fem_pos_deviation(weight_fem_pos_deviation);
            solver.set_weight_path_length(weight_path_length);
            solver.set_weight_ref_deviation(weight_ref_deviation);

            solver.set_max_iter(max_iter);
            solver.set_time_limit(time_limit);
            solver.set_verbose(verbose);
            solver.set_scaled_termination(scaled_termination);
            solver.set_warm_start(warm_start);

            solver.set_ref_points(raw_point2d);
            solver.set_bounds_around_refs(bounds);

            if (!solver.Solve()) {
                return false;
            }

            *opt_x = solver.opt_x();
            *opt_y = solver.opt_y();
            return true;
        }

        bool SqpWithOsqp(const std::vector<std::pair<double, double>> &raw_point2d, const std::vector<double> &bounds,
                         std::vector<double> *opt_x, std::vector<double> *opt_y) {
            if (opt_x == nullptr || opt_y == nullptr) {
                //    AERROR << "opt_x or opt_y is nullptr";
                return false;
            }

            FemPosDeviationSqpOsqpInterface solver;

            solver.set_weight_fem_pos_deviation(weight_fem_pos_deviation);
            solver.set_weight_path_length(weight_path_length);
            solver.set_weight_ref_deviation(weight_ref_deviation);
            solver.set_weight_curvature_constraint_slack_var(weight_curvature_constraint_slack_var);

            solver.set_curvature_constraint(curvature_constraint);

            solver.set_sqp_sub_max_iter(sqp_sub_max_iter);
            solver.set_sqp_ftol(sqp_ftol);
            solver.set_sqp_pen_max_iter(sqp_pen_max_iter);
            solver.set_sqp_ctol(sqp_ctol);

            solver.set_max_iter(max_iter);
            solver.set_time_limit(time_limit);
            solver.set_verbose(verbose);
            solver.set_scaled_termination(scaled_termination);
            solver.set_warm_start(warm_start);

            solver.set_ref_points(raw_point2d);
            solver.set_bounds_around_refs(bounds);

            if (!solver.Solve()) {
                return false;
            }

            std::vector<std::pair<double, double>> opt_xy = solver.opt_xy();

            // TODO(Jinyun): unify output data container
            opt_x->resize(opt_xy.size());
            opt_y->resize(opt_xy.size());
            for (size_t i = 0; i < opt_xy.size(); ++i) {
                (*opt_x)[i] = opt_xy[i].first;
                (*opt_y)[i] = opt_xy[i].second;
            }
            return true;
        }

    private:
        // FemPosDeviationSmootherConfig config_;
    };
} // namespace avos::planning
