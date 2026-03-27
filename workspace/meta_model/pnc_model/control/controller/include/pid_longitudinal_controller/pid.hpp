#ifndef PID_H
#define PID_H

#include <vector>
#include <stdexcept>

namespace pid_longitudinal_controller
{

class PIDController
{

public:
    PIDController();

    double calculate(const double error, const double dt,
                     const bool is_integrated,
                     std::vector<double> &pid_contributions);

    void setGains(const double kp, const double ki, const double kd);

    void setLimits(const double max_ret, const double min_ret,
                   const double max_ret_p, const double min_ret_p,
                   const double max_ret_i, const double min_ret_i,
                   const double max_ret_d, const double min_ret_d);

    void reset();

private:
    // PID parameters
    struct Params
    {
        double kp;
        double ki;
        double kd;
        double max_ret_p;
        double min_ret_p;
        double max_ret_i;
        double min_ret_i;
        double max_ret_d;
        double min_ret_d;
        double max_ret;
        double min_ret;
    };

    Params m_params;

    double m_error_integral;
    double m_prev_error;

    bool m_is_first_time;
    bool m_is_gains_set;
    bool m_is_limits_set;
};

}    // namespace

#endif  // PID_H
