/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: OptimizeCurve3.cpp
 *
 * Description: declare 3-order-OptimizeCurve3 curve methods
 *
 * History:
 * Steckon         2019/10/25    v1.0.0        Build this model.
 * herolin         2019/12/04    v2.0.0        Modify this model.
 ******************************************************************************/
#pragma once

namespace geometry {

/*
calculate OptimizeCurve3 Curve's position and angle
input SiteVec& curve
output int: (0: sucess, 1: angle failed, -1: failed)
*/
inline int OptimizeCurve3::CalculateCurve(SiteVec& curve)
{
	int rtvalue = -1;
	curve.clear();
	_max_curvature = 0.0;
	_max_curvature_x = 0.0;
	_mean_curvature = 0.0;
	double y_order_1 = 0.0, y_order_2 = 0.0;
	double t = 0.0;
	for(t = 0.0; t <= 1.0; t += _resolution)
	{
		if(t >= 1.0 - _resolution && t < 1.0 - 0.5*_resolution) t = 1.0-1.01*_resolution;
		if(t >= 1.0 - 0.5*_resolution) t = 1.0;
		Site pt;
		pt.x = _start_pt.x+(_end_pt.x - _start_pt.x)*t;

		pt.y = GetDerivation(_param_a, _param_b, _param_c, _param_d, pt.x, 0);

		y_order_1 = GetDerivation(_param_a, _param_b, _param_c, _param_d, pt.x, 1);
		y_order_2 = GetDerivation(_param_a, _param_b, _param_c, _param_d, pt.x, 2);

		pt.curvature = GetCurvature(y_order_1,y_order_2);
		if(std::fabs(_max_curvature) < std::fabs(pt.curvature))
		{
			_max_curvature = pt.curvature;
			_max_curvature_x = pt.x;
		}
		_mean_curvature = std::fabs(pt.curvature)*_resolution;
		curve.emplace_back(pt);		
	}
	if(curve.size() < 3) 
	{
		curve.clear();
		return rtvalue;
	}
	curve.front().angle = _start_pt.angle;
	
    auto iter = std::next(curve.begin());
	for(; iter!=curve.end() && std::next(iter)!=curve.end(); ++iter)
	{
		iter->angle = std::atan2(std::next(iter)->y - iter->y, std::next(iter)->x - iter->x)*180/PI;
	}
	curve.back().angle = std::prev(std::prev(curve.end()))->angle;
	rtvalue = 0;
	return rtvalue;
}

inline double OptimizeCurve3::GetDerivation(
  const double a, const double b, const double c, 
  const double d, const double param_x, const int order)
{
  	double result = 0.0;
  	switch(order)
  	{
  	case 0:
  	  result = a*std::pow(param_x,3)+b*std::pow(param_x,2)+c*param_x+d;
  	  break;
  	case 1:
  	  result = 3*a*std::pow(param_x,2)+2*b*param_x+c;
  	  break;
  	case 2:
  	  result = 6*a*param_x+2*b;
  	  break;
  	}
  	return result;
}
inline double OptimizeCurve3::GetCurvature(double y_order_1, double y_order_2)
{
  	double curvature = 0.0;
  	double deno = 1 + std::pow(y_order_1,2);
  	curvature = y_order_2/std::pow(deno,1.5);
  	return curvature;
}
inline double OptimizeCurve3::GetGradient(double end_angle)
{
	double Y1 = 3*_param_a*std::pow(_max_curvature_x,2)+2*_param_b*_max_curvature_x+_param_c;
	double Y2 = 6*_param_a*_max_curvature_x+2*_param_b;
	double A = 1.0 + std::pow(Y1,2);	

	double partial_F_Y1 = -3*Y1*Y2/std::pow(A,2.5);
	double partial_F_Y2 = 1.0/std::pow(A,1.5);

	double x0 = _start_pt.x;
	double x1 = _end_pt.x;
	double B = (1+std::pow(std::tan(end_angle),2))/std::pow(_start_pt.x - _end_pt.x,2.0);
	double partial_Y1_angle = 3*std::pow(_max_curvature_x,2)*B-2*_max_curvature_x*B*(2*x0+x1)+B*(x0*x0+2*x0*x1);
	double partial_Y2_angle = 6*B*_max_curvature_x-2*B*(2*x0+x1);

	double result = partial_F_Y1*partial_Y1_angle+partial_F_Y2*partial_Y2_angle;
	return result;
}
/**
*@brief 生成优化曲线(替代贝塞尔曲线)
*@param start 起点坐标
*@param start 终点坐标
*@param end_refpath_angle 终点对应的参考路径点的方向角,单位度
*@param sample_count 采样点数
*@param curve 结果曲线
*@return -1: , 0: 
*/
inline int OptimizeCurve3::GenerateOptimizeCurve3(const Site &start,const Site &end,
          SiteVec& curve, double end_refpath_angle)
{
  //如果起点和终点直线距离小于1m，则生成曲线失败 
  if(std::hypot(end.x - start.x, end.y - start.y) < 1.0) return -1;
  if(std::hypot(end.x - start.x, end.y - start.y) >= 35.0) return -1;
  //如果起点和终点夹角大于120度，则生成曲线失败
  if(std::fabs(Normalize_Deg(end.angle - start.angle)) >= 120) return -1;
  //如果两点连线与起点夹角大于等于90度，则生成曲线失败
  if(std::cos((start.angle)*PI/180.0)*(end.x - start.x)
  	+std::sin((start.angle)*PI/180.0)*(end.y - start.y) <= 0) return -1;	
  //如果两点连线与终点夹角大于等于90度，则生成曲线失败
  if(std::cos((end.angle)*PI/180.0)*(end.x - start.x)
  	+std::sin((end.angle)*PI/180.0)*(end.y - start.y) <= 0) return -1;
  if(end_refpath_angle > 361.0) end_refpath_angle = end.angle;
  //期望角度归一化
  end_refpath_angle = Normalize_Deg(end_refpath_angle);
  //如果两点连线与期望角度夹角大于等于90度，则生成曲线失败
  if(std::cos(end_refpath_angle*PI/180.0)*(end.x - start.x)
  	+std::sin(end_refpath_angle*PI/180.0)*(end.y - start.y) <= 0) return -1; 

  double rotate_angle = 0.0;
  if(std::fabs(Normalize_Deg(start.angle)+90) < 30)
  {
  	rotate_angle = -PI/2;  	
  }
  if(std::fabs(Normalize_Deg(start.angle)-90) < 30)
  {
  	rotate_angle = PI/2; 
  }
  if(std::fabs(Normalize_Deg(Normalize_Deg(start.angle)-180)) < 30)
  {
  	rotate_angle = PI; 
  }  


  _start_pt.x = 0;
  _start_pt.y = 0;
  _start_pt.angle = Normalize_Rad(Normalize_Rad((start.angle)*PI/180)+rotate_angle)*180/PI;  	
  _end_pt.x = std::cos(rotate_angle)*(end.x - start.x)-std::sin(rotate_angle)*(end.y - start.y);
  _end_pt.y = std::sin(rotate_angle)*(end.x - start.x)+std::cos(rotate_angle)*(end.y - start.y);
  _end_pt.angle = Normalize_Rad(Normalize_Rad((end.angle)*PI/180)+rotate_angle)*180/PI; 
  end_refpath_angle = Normalize_Rad(Normalize_Rad(end_refpath_angle*PI/180)+rotate_angle)*180/PI;
  double angle_limit = std::fabs(_limit_angle*PI/180.0);
  double start_init_angle = Normalize_Rad(_start_pt.angle*PI/180);
  double end_init_angle = std::atan2(_end_pt.y-_start_pt.y,_end_pt.x-_start_pt.x);
  
  if(Normalize_Rad(end_init_angle - end_refpath_angle*PI/180.0) > angle_limit)
  {
    end_init_angle = Normalize_Rad(end_refpath_angle*PI/180.0+angle_limit);
  }
  if(Normalize_Rad(end_init_angle - end_refpath_angle*PI/180.0) < -angle_limit)
  {
    end_init_angle = Normalize_Rad(end_refpath_angle*PI/180.0-angle_limit);
  }
  double max_angle = Normalize_Rad(end_refpath_angle*PI/180.0+angle_limit);
  double min_angle = Normalize_Rad(end_refpath_angle*PI/180.0-angle_limit);

  curve.clear();
  _resolution = 0.25/std::hypot(_end_pt.x - _start_pt.x, _end_pt.y - _start_pt.y);
  if(_resolution > 0.1) _resolution = 0.1;
  double curve_max_curvature = std::numeric_limits<double>::max();
  double curve_mean_curvature = _mean_curvature;
  double curve_end_angle = end_init_angle;
  double end_angle = curve_end_angle;
  double grad = 0.0;
  SiteVec current_curve;
  int k = 0;
  double _param_a_1 = (2*_end_pt.y-2*_start_pt.y)/std::pow(_start_pt.x-_end_pt.x,3)
  					+ std::tan(start_init_angle)/std::pow(_start_pt.x-_end_pt.x,2);
  double _param_a_2 = 1.0/std::pow(_start_pt.x-_end_pt.x,2);

  double _param_b_1 = 3*(_start_pt.x+_end_pt.x)*(_start_pt.y-_end_pt.y)/std::pow(_start_pt.x-_end_pt.x,3)
  					- (_start_pt.x+2*_end_pt.x)*std::tan(start_init_angle)/std::pow(_start_pt.x-_end_pt.x,2);
  double _param_b_2 = -(2*_start_pt.x+_end_pt.x)/std::pow(_start_pt.x-_end_pt.x,2);

  double _param_c_1 = 6*(_start_pt.x*_end_pt.x)*(_end_pt.y-_start_pt.y)/std::pow(_start_pt.x-_end_pt.x,3)
  					+ (_end_pt.x*_end_pt.x+2*_start_pt.x*_end_pt.x)*std::tan(start_init_angle)/std::pow(_start_pt.x-_end_pt.x,2);
  double _param_c_2 = (_start_pt.x*_start_pt.x+2*_start_pt.x*_end_pt.x)/std::pow(_start_pt.x-_end_pt.x,2);

  double _param_d_1 = (3*_end_pt.x*_end_pt.x*_start_pt.x-std::pow(_end_pt.x, 3))*_start_pt.y/std::pow(_start_pt.x-_end_pt.x,3)
  					+ (std::pow(_start_pt.x, 3)-3*_end_pt.x*_start_pt.x*_start_pt.x)*_end_pt.y/std::pow(_start_pt.x-_end_pt.x,3)
  					- _end_pt.x*_end_pt.x*_start_pt.x*std::tan(start_init_angle)/std::pow(_start_pt.x-_end_pt.x,2);
  double _param_d_2 = -_end_pt.x*_start_pt.x*_start_pt.x/std::pow(_start_pt.x-_end_pt.x,2);     
  while(k < _opitmize_times)
  {
  	if(std::fabs(Normalize_Rad(end_angle)-PI/2) < 1e-3
  		|| std::fabs(Normalize_Rad(end_angle)+PI/2) < 1e-3)
  	{
  		break;
  	}	
	_param_a = _param_a_1+_param_a_2*std::tan(end_angle);
	_param_b = _param_b_1+_param_b_2*std::tan(end_angle);;
	_param_c = _param_c_1+_param_c_2*std::tan(end_angle);;
	_param_d = _param_d_1+_param_d_2*std::tan(end_angle);;

	// std::cout<<"end_angle = "<<end_angle*57.3<<std::endl;
	// std::cout<<"_param_a = "<<_param_a<<std::endl;
	// std::cout<<"_param_b = "<<_param_b<<std::endl;
	// std::cout<<"_param_c = "<<_param_c<<std::endl;
	// std::cout<<"_param_d = "<<_param_d<<std::endl;
	CalculateCurve(current_curve);
	grad = GetGradient(end_angle);
	// std::cout<<"_max_curvature_x = "<<_max_curvature_x<<std::endl;
	// std::cout<<"_max_curvature = "<<_max_curvature<<std::endl;
	// std::cout<<"_mean_curvature = "<<_mean_curvature<<std::endl;	
	// std::cout<<"grad = "<<grad<<std::endl;
	int situation = 0;
	if(std::fabs(curve_max_curvature) <= _curvature_threshold) situation += 1<<1;
	if(std::fabs(_max_curvature) <= _curvature_threshold) situation += 1;
	//std::cout<<"case = "<<situation<<std::endl;
	switch(situation)
	{
	case 0://两条曲线都不符合曲率要求
		curve = current_curve;
		curve_max_curvature = _max_curvature;
		curve_mean_curvature = _mean_curvature;
		curve_end_angle = end_angle;
		if(std::fabs(grad) < 1e-3) return -1;	
		if(grad*_max_curvature > 0)
		{
			max_angle = end_angle;
			end_angle = Normalize_Rad(max_angle - 0.618*Normalize_Rad(max_angle - min_angle));
		}
		else
		{
			min_angle = end_angle;
			end_angle = Normalize_Rad(min_angle + 0.618*Normalize_Rad(max_angle - min_angle));
		}
		//std::cout<<"case = "<<situation<<"curve.size = "<<curve.size()<<std::endl;
		break;
	case 1://旧曲线不符合要求而新曲线符合要求
		curve = current_curve;
		curve_max_curvature = _max_curvature;
		curve_mean_curvature = _mean_curvature;
		curve_end_angle = end_angle;
		if(Normalize_Rad(end_angle - end_refpath_angle*PI/180.0) > 0)
		{
			max_angle = curve_end_angle;
			end_angle = Normalize_Rad(max_angle - 0.618*Normalize_Rad(max_angle - min_angle));				
		}
		else
		{
			min_angle = curve_end_angle;
			end_angle = Normalize_Rad(min_angle + 0.618*Normalize_Rad(max_angle - min_angle));								
		}
		//std::cout<<"case = "<<situation<<"curve.size = "<<curve.size()<<std::endl;
		break;		
	case 2://旧曲线符合要求但新曲线不符合要求	
		if(Normalize_Rad(curve_end_angle-end_angle) > 0)
		{
			min_angle = end_angle;
			end_angle = Normalize_Rad(max_angle - 0.618*Normalize_Rad(max_angle - min_angle));		
		}
		else
		{
			max_angle = end_angle;
			end_angle = Normalize_Rad(min_angle + 0.618*Normalize_Rad(max_angle - min_angle));	
		}
		//std::cout<<"case = "<<situation<<"curve.size = "<<curve.size()<<std::endl;
		break;
	case 3://两条曲线都符合要求
		if(std::fabs(Normalize_Rad(curve_end_angle - end_refpath_angle*PI/180.0)) 
			< std::fabs(Normalize_Rad(end_angle - end_refpath_angle*PI/180.0)))
		{
			if(Normalize_Rad(curve_end_angle - end_refpath_angle*PI/180.0) > 0)
			{
				max_angle = curve_end_angle;
				end_angle = Normalize_Rad(max_angle - 0.618*Normalize_Rad(max_angle - min_angle));	
			}
			else
			{
				min_angle = curve_end_angle;
				end_angle = Normalize_Rad(min_angle + 0.618*Normalize_Rad(max_angle - min_angle));					
			} 
		}
		else
		{
			curve = current_curve;
			curve_max_curvature = _max_curvature;
			curve_mean_curvature = _mean_curvature;
			curve_end_angle = end_angle;
			if(std::fabs(grad) < 1e-3) return 0;
			if(Normalize_Rad(end_angle - end_refpath_angle*PI/180.0) > 0)
			{
				max_angle = curve_end_angle;
				end_angle = Normalize_Rad(max_angle - 0.618*Normalize_Rad(max_angle - min_angle));				
			}
			else
			{
				min_angle = curve_end_angle;
				end_angle = Normalize_Rad(min_angle + 0.618*Normalize_Rad(max_angle - min_angle));								
			} 						
		}
		//std::cout<<"case = "<<situation<<"curve.size = "<<curve.size()<<std::endl;	
		break;							
	}
	//std::cout<<"_curvature_threshold = "<<_curvature_threshold<<std::endl;
	//std::cout<<"=========================================================== "<<std::endl;
	if(std::fabs(Normalize_Rad(max_angle - min_angle)) < 5*PI/180) break;
	k++; 	
  }
  //std::cout<<"gap length = "<<std::fabs(Normalize_Rad(max_angle - min_angle))<<std::endl;
  double tmp_x = 0, tmp_y = 0;
  for(int i = 0; i < curve.size(); i++)
  {
  	tmp_x = curve.at(i).x;
  	tmp_y = curve.at(i).y;
  	curve.at(i).x = start.x+tmp_x*std::cos(-rotate_angle)-tmp_y*std::sin(-rotate_angle);
  	curve.at(i).y = start.y+tmp_x*std::sin(-rotate_angle)+tmp_y*std::cos(-rotate_angle);
  	curve.at(i).angle = Normalize_Rad(curve.at(i).angle*PI/180-rotate_angle)*180/PI;		
  }
  _max_curvature = curve_max_curvature;
  _mean_curvature = curve_mean_curvature;
  // std::cout<<"curve.size = "<<curve.size()<<",std::fabs(_max_curvature) = "
  // <<std::fabs(_max_curvature)<<",_curvature_threshold = "<<_curvature_threshold<<std::endl;
  if(std::fabs(_max_curvature) > _curvature_threshold) return -1; 
  return 0;
}

}//namespace
