
#pragma once

#include <math.h>

#include "base_control/lon/pid_header.h"

inline float pid_get_p(PIDInfo* pid, float error) {
	return (pid->kp * error);
}

inline float pid_get_i(PIDInfo* pid, float error, float dt) {
	if (pid->ki > 0 && dt > 0)
		pid->integ += (pid->ki * error * dt);

	if (pid->integ > pid->integ_max) pid->integ = pid->integ_max;
	if (pid->integ < pid->integ_min) pid->integ = pid->integ_min;

	return (pid->integ);
}

inline float get_integrator(PIDInfo* pid) {
	return pid->integ;
}

inline void set_integrator(PIDInfo* pid, float value) {
	pid->integ = value;
}

inline float pid_get_d(PIDInfo* pid, float error, float dt) {
	float new_deriv = 0;
	pid->error = error;
	if (pid->kd > 0 && dt > 0 + 1e-6)
	{
		if (std::isnan(pid->deriv))
		{
			pid->deriv = 0;
			new_deriv = 0;
		}
		else
		{
			new_deriv = (pid->error - pid->last_error) / dt;
		}
		pid->deriv = pid->deriv + pid->alpha * (new_deriv - pid->deriv);

		pid->last_error = pid->error;

		return (pid->kd * pid->deriv);
	}
	return 0;
}

inline void pid_reset_i(PIDInfo* pid) {
	pid->integ = 0;
}

inline void set_d_lpf_alpha(PIDInfo* pid, float fcut, float dt) {
	float rc_d = 1 / (2 * M_PI * fcut);

	pid->alpha = dt / (rc_d + dt);

	pid->d_lpf_fcut = fcut;
}

inline void inte_param_init(InteParam* integrator_param, double dt) {
	integrator_param->inte = 0;
	integrator_param->dt = dt;
}

inline double Integrator(InteParam* integrator_param, double new_data) {
	integrator_param->inte = integrator_param->inte + new_data * integrator_param->dt;
	return integrator_param->inte;
}


