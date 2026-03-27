#include "log/log.h"
// --------------------------- Example of Extended Kalman filter ------------------------//
/*
% A plane flights in a 2D space where the x axis is the distance traveled
% by the plane and y axis is its altitude.  This system can be represented
% by the fallowing equations:
% (This is just an example)
%
% xpp = F/m - bx/m * xp^2
% ypp = p/m * xp^2 - g
%
% where m is the plane's weight (1000 kg)
%       bx is the drag coefficient (0.35 N/m?/s?)
%       p is the lift force (3.92 N/m?/s?)
%       g is the gravitational acceleration (9.8 m/s?)
%       F is the motor's thrust
%
% A station on the ground (at the origin) mesures the angle between the
% plane and the ground (x axis) and the distance between the plane and the station.
% These measures are based and the fallowing equations:
%
% theta = atan2(y,x)
% r = sqrt(x^2+y^2)
%
% The variance error matrix of the mesures is:
%
% R = [0.01^2  0
%      0       50^2]
%
% V = [1 0;
%      0 1];
%
% The variance error matrix of the plane's model is: WQW'
%
% Q = [0.01^2    0;
%      0         0.01^2];
%
% W = [0 0;
%      1 0;
%      0 0;
%      0 1];
%
*/

#include "perception_fusion_common/Xlidar_mot/filter/objfusekf.h"
#include <cmath>
#include <iostream>
namespace avos {
namespace perception {
namespace xlidarMOT{
using namespace std;

// Build CTRA model
// x = [x, y, v, theta, yawrate,accel]
// z = [x0, y0, x1, y1, x2, y2, vx, vy]
#define USE_VELOCITY

#define PI 3.1415926
cObjfusEKF::cObjfusEKF()
{
	// The function setDim() sets:
	// the number of states,
	// the number of inputs,
	// the number of process noise random variables,
	// the number of measures,
	// the number of measurement noise random variables.
	setDim(Num_State, 0, Num_State, Num_Measure, Num_Measure);
	resetPeriod(0.1);
	tmp_R.resize(Num_Measure);
}


void cObjfusEKF::makeA()
{
	// cout << "*********************************use CTRA model****************************" << endl;

	double v = x(2);
	double w = x(4);
	double dt = Period;

	double sin_theta = sin(x(3));
	double cos_theta = cos(x(3));

	double sin_theta_wt = sin(x(3) + w * dt);
	double cos_theta_wt = cos(x(3) + w * dt);

	// Calculated Jacobian matrix
	if (fabs(w) < 0.0001)
	{
		A(0, 0) = 1;
		A(0, 1) = 0;
		A(0, 2) = dt * cos_theta;
		A(0, 3) = -dt * v * sin_theta;
		A(0, 4) = 0;
		A(0, 5) = 0;

		A(1, 0) = 0;
		A(1, 1) = 1;
		A(1, 2) = dt * sin_theta;
		A(1, 3) = dt * v * cos_theta;
		A(1, 4) = 0;
		A(1, 5) = 0;
	}
	else
	{
		A(0, 0) = 1;
		A(0, 1) = 0;
		A(0, 2) = 1 / w * (-sin_theta + sin_theta_wt);
		A(0, 3) = v / w * (-cos_theta + cos_theta_wt);
		A(0, 4) = dt * v / w * cos_theta_wt
		          - v / (w * w) * (-sin_theta + sin_theta_wt);
		A(0, 5) = 0;

		A(1, 0) = 0;
		A(1, 1) = 1;
		A(1, 2) = 1 / w * (cos_theta - cos_theta_wt);
		A(1, 3) = v / w * (-sin_theta + sin_theta_wt);
		A(1, 4) = dt * v / w * sin_theta_wt
		          - v / (w * w) * (cos_theta - cos_theta_wt);
		A(1, 5) = 0;


	}

	A(2, 0) = 0;
	A(2, 1) = 0;
	A(2, 2) = 1;
	A(2, 3) = 0;
	A(2, 4) = 0;
	A(2, 5) = dt;

	A(3, 0) = 0;
	A(3, 1) = 0;
	A(3, 2) = 0;
	A(3, 3) = 1;
	A(3, 4) = dt;
	A(3, 5) = 0;

	A(4, 0) = 0;
	A(4, 1) = 0;
	A(4, 2) = 0;
	A(4, 3) = 0;
	A(4, 4) = 1;
	A(4, 5) = 0;

	A(5, 0) = 0;
	A(5, 1) = 0;
	A(5, 2) = 0;
	A(5, 3) = 0;
	A(5, 4) = 0;
	A(5, 5) = 1;

}


void cObjfusEKF::makeBaseW()
{
	W(0, 0) = 1.0;
	W(1, 1) = 1.0;
	W(2, 2) = 1.0;
	W(3, 3) = 1.0;
	W(4, 4) = 1.0;
	W(5, 5) = 1.0;
}


void cObjfusEKF::makeBaseQ()
{
	// Q.size 6*6
	// process noise standard deviation for a
	double std_noise_a = 0.25;
	// process noise standard deviation for yaw acceleration
	double std_noise_yaw_dd = 2;
	// double std_noise_yaw_dd = 0.1; // zr-20200317
	// double std_noise_yaw_dd = 0.5; // zr-20200603, around 30deg/s

	Q(0, 0) = pow(std_noise_a * half_p_2, 2);
	Q(1, 1) = pow(std_noise_a * half_p_2, 2);
	Q(2, 2) = pow(std_noise_a * Period, 2);
	Q(3, 3) = pow(std_noise_yaw_dd * Period, 2);
	Q(4, 4) = pow(std_noise_yaw_dd, 2);
	Q(5, 5) = pow(std_noise_a, 2);
}

void cObjfusEKF::makeH()
{
	H(0, 0) = 1.0;
	H(0, 1) = 0.0;
	H(0, 2) = 0.0;
	H(0, 3) = 0.0;
	H(0, 4) = 0.0;
	H(0, 5) = 0.0;

	H(1, 0) = 0.0;
	H(1, 1) = 1.0;
	H(1, 2) = 0.0;
	H(1, 3) = 0.0;
	H(1, 4) = 0.0;
	H(1, 5) = 0.0;

	H(2, 0) = 1.0;
	H(2, 1) = 0.0;
	H(2, 2) = 0.0;
	H(2, 3) = 0.0;
	H(2, 4) = 0.0;
	H(2, 5) = 0.0;

	H(3, 0) = 0.0;
	H(3, 1) = 1.0;
	H(3, 2) = 0.0;
	H(3, 3) = 0.0;
	H(3, 4) = 0.0;
	H(3, 5) = 0.0;

	H(4, 0) = 1.0;
	H(4, 1) = 0.0;
	H(4, 2) = 0.0;
	H(4, 3) = 0.0;
	H(4, 4) = 0.0;
	H(4, 5) = 0.0;

	H(5, 0) = 0.0;
	H(5, 1) = 1.0;
	H(5, 2) = 0.0;
	H(5, 3) = 0.0;
	H(5, 4) = 0.0;
	H(5, 5) = 0.0;


#ifdef USE_VELOCITY
	//velocity: Vx = V*cos(theta)
	H(6, 0) = 0.0;
	H(6, 1) = 0.0;
	H(6, 2) = cos(x(3));
	H(6, 3) = -x(2) * sin(x(3));
	H(6, 4) = 0.0;
	H(6, 5) = 0.0;

	//veloctiy: Vy = V*sin(theta)
	H(7, 0) = 0.0;
	H(7, 1) = 0.0;
	H(7, 2) = sin(x(3));
	H(7, 3) = x(2) * cos(x(3));
	H(7, 4) = 0.0;
	H(7, 5) = 0.0;

#else
	H(6, 0) = 0.0;
	H(6, 1) = 0.0;
	H(6, 2) = 0.0;
	H(6, 3) = 0.0;
	H(6, 4) = 0.0;
	H(6, 5) = 0.0;

	H(7, 0) = 0.0;
	H(7, 1) = 0.0;
	H(7, 2) = 0.0;
	H(7, 3) = 0.0;
	H(7, 4) = 0.0;
	H(7, 5) = 0.0;
#endif
}


void cObjfusEKF::makeBaseV()
{
	V(0, 0) = 1.0;
	V(1, 1) = 1.0;
	V(2, 2) = 1.0;
	V(3, 3) = 1.0;
	V(4, 4) = 1.0;
	V(5, 5) = 1.0;
	V(6, 6) = 1.0;
	V(7, 7) = 1.0;
}


void cObjfusEKF::resetR(KVector<double, 0, 1> _tmp_R)
{
	tmp_R = _tmp_R;
}
void cObjfusEKF::resetQ(KVector<double, 0, 1> _tmp_Q)
{
}
void cObjfusEKF::resetPeriod(double t)
{
	Period = t;
	half_p_2 = 0.5 * Period * Period;
}

void cObjfusEKF::makeR()
{
	R(0, 0) = tmp_R(0);
	R(1, 1) = tmp_R(1);
	R(2, 2) = tmp_R(2);
	R(3, 3) = tmp_R(3);
	R(4, 4) = tmp_R(4);
	R(5, 5) = tmp_R(5);
	R(6, 6) = tmp_R(6);
	R(7, 7) = tmp_R(7);
}

void cObjfusEKF::makeProcess()
{
	Vector x_(x.size());

	double v = x(2);
	double w = x(4);
	double dt = Period;

	double sin_theta = sin(x(3));
	double cos_theta = cos(x(3));

	double sin_theta_wt = sin(x(3) + w * dt);
	double cos_theta_wt = cos(x(3) + w * dt);

	if (fabs(w) < 0.0001)
	{
		x_(0) = x(0) + v * cos_theta * Period;
		x_(1) = x(1) + v * sin_theta * Period;
		x_(2) = x(2) + x(5) * Period;
		x_(3) = x(3) + x(4) * Period;
		x_(4) = x(4);
		x_(5) = x(5);
	}
	else
	{
		x_(0) = x(0) + (v / w) * (sin_theta_wt - sin_theta);
		x_(1) = x(1) + (v / w) * (-cos_theta_wt + cos_theta);
		x_(2) = x(2) + x(5) * Period;
		x_(3) = x(3) + x(4) * Period;
		x_(4) = x(4);
		x_(5) = x(5);
	}

	if (x_(3) > PI)
	{
		x_(3) -= 2 * PI;
	}
	else if (x_(3) < -PI)
	{
		x_(3) += 2 * PI;
	}

	x.swap(x_);
}

void cObjfusEKF::makeMeasure()
{
	z(0) = x(0);
	z(1) = x(1);
	z(2) = x(0);
	z(3) = x(1);
	z(4) = x(0);
	z(5) = x(1);
#ifdef USE_VELOCITY
	z(6) = x(2) * cos(x(3));
	z(7) = x(2) * sin(x(3));
#else
	z(6) = 0;
	z(7) = 0;
#endif

}

}
}
}


