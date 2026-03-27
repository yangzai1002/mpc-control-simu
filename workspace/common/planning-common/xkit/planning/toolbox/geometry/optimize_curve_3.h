 /******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: bezier.h
 *
 * Description: declare 3-order-Bezier curve methods
 *
 * History:
 * Steckon         2019/10/25    v1.0.0        Build this model.
 * herolin         2019/12/04    v2.0.0        Modify this model.
 ******************************************************************************/
#ifndef __COMMON_TOOLBOX_GEOMETRY_INCLUDE_OPTIMIZECURVE3_BASE_H__
#define __COMMON_TOOLBOX_GEOMETRY_INCLUDE_OPTIMIZECURVE3_BASE_H__

#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/geometry/geoheader.h"
#include <math.h>
#include <vector>
#include <functional>
#include <limits>
#include <cstdlib>
#include <iostream>
using namespace geometry;

#define PI 3.1416

namespace geometry {
  class OptimizeCurve3 
  {
  public:
      OptimizeCurve3()
      {
        _resolution = 0.1;
        _limit_angle = 45;
        _curvature_threshold = std::numeric_limits<double>::max();
        _max_curvature = std::numeric_limits<double>::max();
        _max_curvature_x = 0.0;
        _mean_curvature = 0.0;
        _opitmize_times = 13;
        _param_a = 0.0;
        _param_b = 0.0;
        _param_c = 0.0;
        _param_d = 0.0;        
      }

      ~OptimizeCurve3() = default;

      void SetCurvatureThreshold(const double& curvature){
        _curvature_threshold = curvature;
      }

      void SetIterTimes(const double& iter_times){
        _opitmize_times = iter_times;
      }        

      void SetLimitAngle(const double& limit_angle){
        _limit_angle = limit_angle;
      } 

      double GetMaxCurvature(){
        return  _max_curvature;
      }

      double GetMeanCurvature(){
        return  _mean_curvature;
      }

      int CalculateCurve(SiteVec & curve);

      double GetDerivation(const double a, const double b, const double c, 
                            const double d, const double param_x, const int order); 

      double GetCurvature(double y_order_1, double y_order_2);

      double GetGradient(double end_angle);

      int GenerateOptimizeCurve3(const Site &start,const Site &end, 
        SiteVec& curve, double end_refpath_angle = 720.0);

      /**
      *@brief 归一化弧度
      *@param angle input 目标角度    
      *@return 转换后的弧度
      */  
      inline double Normalize_Rad(double angle)
      {
          double a = std::fmod(angle + M_PI, 2.0*M_PI);
          if(a<0.0){
              a += (2.0 * M_PI);
          }
          return (a - M_PI);
      }        
      /**
      *@brief 归一化角度
      *@param angle input 目标角度    
      *@return 转换后的角度
      */
      inline double Normalize_Deg(double angle)
      {
          double a = std::fmod(angle + 180.0, 2.0*180.0);
          if(a<0.0){
              a += (2.0 * 180.0);
          }
          return (a - 180.0);
      }

    private:
      Site _start_pt, _end_pt;
      double _resolution;
      double _limit_angle;
      double _curvature_threshold, _max_curvature, _mean_curvature,_max_curvature_x;
      double _param_a,_param_b,_param_c,_param_d;
      int _opitmize_times;
    };
    typedef OptimizeCurve3* OptimizeCurve3Ptr;
    typedef OptimizeCurve3& OptimizeCurve3Ref;
}  // namespace geometry
#include "optimize_curve_3.hpp"

#endif
