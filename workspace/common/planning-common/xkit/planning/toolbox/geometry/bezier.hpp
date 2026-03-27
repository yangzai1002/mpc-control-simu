/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: bezier.cpp
 *
 * Description: declare 3-order-Bezier curve methods
 *
 * History:
 * Steckon         2019/10/25    v1.0.0        Build this model.
 * herolin         2019/12/04    v2.0.0        Modify this.
 ******************************************************************************/
#pragma once
//#include "bezier.h"

namespace geometry {

/*
calculate Bezier Curve's position and angle
input SiteVec& curve
output int: (0: sucess, 1: angle failed, -1: failed)
*/
inline int Bezier::CalculateCurve(SiteVec& curve)
{
	int rtvalue = -1;
	curve.clear();
	float t, u, t2, u2, u3, t3;
	for(t = 0.0; t <= 1.0; t += _resolution)
	{
		u = 1.0 - t;
		t2 = t * t;
		u2 = u * u;
		u3 = u2 * u;
		t3 = t2 * t;

		Site pt;
		pt.x = u3*_p0.x + 3*u2*t*_p1.x + 3*u*t2*_p2.x + t3*_p3.x;
		pt.y = u3*_p0.y + 3*u2*t*_p1.y + 3*u*t2*_p2.y + t3*_p3.y;

		if(-1 == CalculateCurvature(pt, u, t)){
			//std::cout<<"CalculateCurvature failed"<<std::endl;
			return rtvalue;
		}
		if(pt.curvature > _curvature_threshold){
			//std::cout<<"curvature is out of _curvature_threshold: "<<pt.curvature<<" > "<<_curvature_threshold<<std::endl;
			curve.clear();
			return rtvalue;
		}
		_max_curvature = (_max_curvature < pt.curvature) ? pt.curvature : _max_curvature;
		
		_total_curvature += pt.curvature;

		curve.emplace_back(pt);
		rtvalue = 1;
	}
 	//assign the angle for Site
	if(curve.empty()) {return rtvalue;}

    auto iter = curve.begin();
	for(; iter!=curve.end() && std::next(iter)!=curve.end(); ++iter)
	{
		iter->angle = ((*std::next(iter))- (*iter)).inerangle();
	}
	curve.back().angle = _p3.angle;
	rtvalue = 0;
	return rtvalue;
}

/*
calculate Bezier Curve's control points
input 
output int: (0: sucess, -1: failed)
*/
inline int Bezier::CalculateCtrlPoints()
{
	int rtvalue = -1;
	float dis_p0_p3 = std::hypot(_p0.x - _p3.x, _p0.y - _p3.y);


	_resolution = 0.5/dis_p0_p3;
	if(_resolution > 0.1)_resolution = 0.1;

	if(fabs(dis_p0_p3)<1e-3) {return rtvalue;}

	float move_dis = 1.0/3.0 * dis_p0_p3;

	_p1.x = _p0.x + move_dis * std::cos(_p0.angle * M_PI / 180.0);
	_p1.y = _p0.y + move_dis * std::sin(_p0.angle * M_PI / 180.0);

	_p2.x = _p3.x - move_dis * std::cos(_p3.angle * M_PI / 180.0);
	_p2.y = _p3.y - move_dis * std::sin(_p3.angle * M_PI / 180.0);

	rtvalue = 0;
	return rtvalue;
}

/*
calculate Bezier Curve's curve by points
input u = 1-t, t
output int: (0: sucess, -1: failed)
curvature = |dtheta / ds|
		  = |sec2(theta)dtheta / sec2(theta)ds|
		  = |dtan(theta) / sec2(theta)ds|
		  = |[y''(t)x'(t) - y'(t)x''(t)] / [(x'(t)^2 + y'(t)^2)]^(3/2)|
*/
inline int Bezier::CalculateCurvature(Site& pt, const float& u, const float& t)
{
	int rtvalue = -1;
	float x_dot = -3*(_p0.x - _p1.x)*u*u - 6*(_p1.x - _p2.x)*u*t - 3*(_p2.x - _p3.x)*t*t;
	float y_dot = -3*(_p0.y - _p1.y)*u*u - 6*(_p1.y - _p2.y)*u*t - 3*(_p2.y - _p3.y)*t*t;
	float denominator = std::pow(std::pow(x_dot, 2) + std::pow(y_dot, 2), 1.5);
	// std::cout<<"u: "<<u<<", t: "<<t<<", _p0: "<<_p0.x<<", "<<_p0.y
	// 		 <<", _p1: "<<_p1.x<<", "<<_p1.y
	// 		 <<", _p2: "<<_p2.x<<", "<<_p2.y
	// 		 <<", _p3: "<<_p3.x<<", "<<_p3.y<<std::endl;
	// std::cout<<"-3*(_p0.x + _p1.x)*u*u: "<<-3*(_p0.x + _p1.x)*u*u
	// 		 <<", - 6*(_p1.x - _p2.x)*u*t: "<<- 6*(_p1.x - _p2.x)*u*t
	// 		 <<", - 3*(_p2.x - _p3.x)*t*t: "<<- 3*(_p2.x - _p3.x)*t*t
	// 		 <<", x_dot: "<<x_dot<<std::endl;
 // 	std::cout<<"-3*(_p0.y + _p1.y)*u*u: "<<-3*(_p0.y + _p1.y)*u*u
	// 		 <<", - 6*(_p1.y - _p2.y)*u*t: "<<- 6*(_p1.y - _p2.y)*u*t
	// 		 <<", - 3*(_p2.y - _p3.y)*t*t: "<<- 3*(_p2.y - _p3.y)*t*t
	// 		 <<", y_dot: "<<y_dot<<std::endl;

	if(denominator < 1e-5){
		std::cout<<"ERROR LOGIC: "<<denominator<<std::endl;
		return rtvalue;
	}
	float x_dot_dot = 6*(_p0.x - _p1.x)*u + 6*(_p1.x - _p2.x)*(t - u) -6*(_p2.x - _p3.x)*t;
	float y_dot_dot = 6*(_p0.y - _p1.y)*u + 6*(_p1.y - _p2.y)*(t - u) -6*(_p2.y - _p3.y)*t;
	float numerator = y_dot_dot*x_dot - y_dot*x_dot_dot;
	pt.curvature = std::fabs(numerator / denominator);
	rtvalue = 0;
	return rtvalue;
}



/*
calculate Bezier Curve API
input Site: start,end, pre: precision
output int: (0: sucess, -1: failed) curve: result
*/
inline int Bezier::GenerateBezierCurve(const Site &start,const Site &end,SiteVec& curve)
{
    int rtvalue = -1;
    _p0=start;
    _p3=end;
	_max_curvature = 0.0;
	_total_curvature = 0.0;

    auto calc_ctrl_pt_rt = CalculateCtrlPoints();
    if(calc_ctrl_pt_rt != 0)
    {
        return rtvalue;
    }

    auto cal_curve_rt = CalculateCurve(curve);
    if(cal_curve_rt != 0)
    {
        return rtvalue;
    }

    rtvalue = 0;
    return rtvalue;
}


}//namespace
