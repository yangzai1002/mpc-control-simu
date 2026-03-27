/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: circular_curve.cpp
 *
 * Description: declare circular curve methods
 *
 * History:
 * Steckon         2020/09/14    v1.0.0        Build this model.
 ******************************************************************************/
#pragma once

namespace geometry {

/*
calculate several constant values
input none
output int: (0: normal radius cases, 1: radius inf, 2: radius == 0.5 * dis_between_start_end, -1: failed)
*/
inline int
CircularCurve::CalculateConstantValues()
{
	int rtvalue = -1;
	// _k1 = std::atan2(_start.y, _start.x);
	// _k2 = std::atan2(_end.y, _end.x);
	// _y1_minus_y2 = _start.y - _end.y;
	// _x1_minus_x2 = _start.x - _end.x;
	// _theta_range = Normalize_Rad(_end.angle * M_PI / 180.0) - Normalize_Rad(_start.angle * M_PI / 180.0);
	// if(_theta_range < 0) {_theta_range += 2 * M_PI;}// keep the _theta_range positive

	// _radius_square = ((1 + _k1*_k1) / (std::pow((_k1 - _k2), 2))) * std::pow(_y1_minus_y2 - _k2 * _x1_minus_x2, 2);
	// _radius = std::sqrt(1 + _k1 * _k1) * (std::fabs(_y1_minus_y2 - _k2 * _x1_minus_x2) / std::fabs(_k1 - _k2));

	// std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<"\n [params] _k1: "<<_k1
	// 											<<"\n _k2: "<<_k2
	// 											<<"\n _y1_minus_y2: "<<_y1_minus_y2
	// 											<<"\n _x1_minus_x2: "<<_x1_minus_x2
	// 											<<"\n _theta_range: "<<_theta_range
	// 											<<"\n _radius_square: "<<_radius_square
	// 											<<"\n _radius: "<<_radius
	// 											<<std::endl;


	_y1_minus_y2 = _start.y - _end.y;
	_x1_minus_x2 = _start.x - _end.x;
	auto alpha = _start.angle * M_PI / 180.0;//rad
	auto theta = Normalize_Rad(std::atan2(-_y1_minus_y2, -_x1_minus_x2));//rad
	auto dis_between_start_end = std::hypot(_end.x - _start.x, _end.y - _start.y);
	auto theta_minus_alpha = Normalize_Rad(theta - alpha);//rad
	_theta_minus_alpha = theta_minus_alpha;
	if(std::fabs(theta_minus_alpha) < 1e-3)
	{
		rtvalue = 1;
		_radius = std::numeric_limits<float>::max();
		// std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" infinitive radius "<<std::endl;
		return rtvalue;
	}
	// else if(std::fabs(theta_minus_alpha - M_PI / 2.0)<1e-3 || std::fabs(theta_minus_alpha + M_PI / 2.0)<1e-3)
	// {
	// 	_radius = 0.5 * dis_between_start_end;
	// 	rtvalue = 2;
	// 	std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" zero cos(), get radius: "<<_radius<<std::endl;
	// 	return rtvalue;
	// }

	_theta_range = 2*theta_minus_alpha;//rad shoyld in [0, 360], Normalize_Rad() will limit in [0, 180]

	// if(_theta_range < 0) {_theta_range += 2 * M_PI;}// keep the _theta_range positive

	_radius = std::fabs(0.5 * dis_between_start_end / sin(theta_minus_alpha));
	// std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<"\n [params] alpha: "<<alpha
	// 											<<"\n theta: "<<theta
	// 											<<"\n theta_minus_alpha: "<<theta_minus_alpha
	// 											<<"\n dis_between_start_end: "<<dis_between_start_end
	// 											<<"\n _y1_minus_y2: "<<_y1_minus_y2
	// 											<<"\n _x1_minus_x2: "<<_x1_minus_x2
	// 											<<"\n _theta_range: "<<_theta_range
	// 											<<"\n _radius: "<<_radius
	// 											<<std::endl;
	//std::cout<<"_radius = "<<_radius<<",_min_turning_radius = "<<_min_turning_radius<<std::endl;
	if(_radius > (_min_turning_radius - 1e-3)) {rtvalue = 0;}
	return rtvalue;
}


/*
calculate circular curve
input none
output int: (0: sucess, 1: angle failed, -1: failed)
*/
inline int
CircularCurve::CalculateCurve(SiteVec& curve, const int& constant_value_rt)
{
	int rtvalue = -1;
	curve.clear();
	if(std::fabs(_theta_range) > M_PI / 2.0)
	{
		return rtvalue;
	} 

	if(1 == constant_value_rt)
	{//straigh line
		auto dis_between_start_end = std::hypot(_end.x - _start.x, _end.y - _start.y);
		auto point_num = std::ceil(dis_between_start_end / _reselution);
		auto delta_dis = dis_between_start_end / point_num;
		for(int i = 0; i < point_num; ++i)
		{
			Site p;
			p.x = _start.x + i * delta_dis;
			p.y = _start.y;
			p.angle = _start.angle;
			curve.emplace_back(p);
			rtvalue = 0;
		}
		// std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" straight curve's size: "<<curve.size()<<std::endl;
		return rtvalue;
	}
	

	bool negtive_theta = false;
	auto delta_angle = _reselution / _radius; // theta = curve / radius
	if(_theta_minus_alpha < 0) {negtive_theta = true;}
	if(!negtive_theta)
	{
		for(float i = 0.0; i < std::fabs(_theta_range); i += delta_angle)
		{
			Site p;
			auto alpha = _start.angle * M_PI / 180.0;
			p.x = _radius * (sin(alpha + i) - sin(alpha)) + _start.x;
			p.y = _radius * (cos(alpha) - cos(alpha + i)) + _start.y;
			p.angle = i * 180.0 / M_PI + _start.angle;				

			p.angle = Normalize_Rad(p.angle * M_PI / 180.0) * 180.0 / M_PI;
			//assaign gccs
			curve.emplace_back(p);
			rtvalue = 0;
		}		
	}
	else
	{
		for(float i = 0.0; i < std::fabs(_theta_range); i += delta_angle)
		{
			Site p;
			auto alpha = _start.angle * M_PI / 180.0;

			p.x = _radius * (sin(alpha) - sin(alpha - i)) + _start.x;
			p.y = _radius * (cos(alpha - i) - cos(alpha)) + _start.y;
			p.angle = _start.angle - i * 180.0 / M_PI;				

			p.angle = Normalize_Rad(p.angle * M_PI / 180.0) * 180.0 / M_PI;
			//assaign gccs
			curve.emplace_back(p);
			rtvalue = 0;
		}
	}

	// std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" curve's size: "<<curve.size()<<std::endl;

	return rtvalue;
}

/*
calculate CircularCurve Curve API
input Site(using x, y, angle(DEG)): start,end, pre: precision
output int: (0: sucess, -1: failed) curve: result
*/
inline int
CircularCurve::GenerateCircularCurve(const Site &start,const Site &end, SiteVec& curve)
{
    int rtvalue = -1;
    _start = start;
    _end = end;
	// /*debug start*/
	// Site temp_site(11.1, 2.0), temp_site_last(8.6, -1.5);
	// if(std::hypot(_end.x - temp_site.x, _end.y - temp_site.y) < 1e-3
	// && std::hypot(_start.x - temp_site_last.x, _start.y - temp_site_last.y) < 1e-3) 
	// {
	// 	_DEBUG_FLAG = true;
	// }
	// else
	// {
	// 	_DEBUG_FLAG = false;
	// }
	// /*debug end*/
    auto constant_value_rt = CalculateConstantValues();
    if(-1 == constant_value_rt)
    {//calc ctrl points failed
		//std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" CalculateConstantValues fail"<<std::endl;
        return rtvalue;
    }

    if(0 != CalculateCurve(curve, constant_value_rt))
    {//calc curve failed
		std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" CalculateCurve fail"<<std::endl;
        return rtvalue;
    }

    rtvalue = 0;
    return rtvalue;
}


inline double
CircularCurve::Normalize_Rad(double angle)
{
    double a = std::fmod(angle + M_PI, 2.0*M_PI);
    if(a<0.0){
        a += (2.0 * M_PI);
    }
    return (a - M_PI);
}

}//namespace