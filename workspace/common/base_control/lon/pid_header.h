#pragma once

typedef struct PIDInfo
{
	double kp;
	double ki;
	double kd;
	double error;
	double last_error;
	double integ;
	double integ_max;
	double integ_min;
	double deriv;
	double d_lpf_fcut;
	double alpha;
	double output;

} PIDInfo;
typedef struct
{
	double inte;
	double dt;
} InteParam;



// #endif