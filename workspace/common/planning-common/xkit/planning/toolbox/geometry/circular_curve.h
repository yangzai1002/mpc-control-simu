 /******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: circular_curve.h
 *
 * Description: declare circular curve methods
 *
 * History:
 * Steckon         2020/09/14    v1.0.0        Build this model.
 ******************************************************************************/
#pragma once

#ifndef __COMMON_TOOLBOX_GEOMETRY_INCLUDE_CIRCULAR_CURVE_BASE_H__
#define __COMMON_TOOLBOX_GEOMETRY_INCLUDE_CIRCULAR_CURVE_BASE_H__

#include <cmath>
#include <vector>
#include <functional>
#include <iostream>
#include "xkit/planning/toolbox/geometry/site.h"

namespace geometry {
  using SiteVec = std::vector<Site>;

  class CircularCurve 
  {

  public:
        CircularCurve()
        {
          _reselution = 0.1;
          _radius = 5.0;
          _min_turning_radius = 5.0;
          _DEBUG_FLAG = false;
        }

        ~CircularCurve() = default;

        int 
        GenerateCircularCurve(const Site &start,const Site &end, SiteVec& curve);

        void SetMinTurningRadius(const float& mtr)
        {
          _min_turning_radius = mtr;
        }
        float GetRadius()
        {
          return _radius;
        }

      private:
        int 
        CalculateConstantValues();

        int 
        CalculateCurve(SiteVec& curve, const int& constant_value_rt);
        
        double 
        Normalize_Rad(double angle);
        
        float _reselution;
        Site _start, _end;

        float _k1, _k2, _y1_minus_y2, _x1_minus_x2, _theta_range;//rad
        float _radius, _theta_minus_alpha, _min_turning_radius;

        bool _DEBUG_FLAG;

        void DebugPrint(std::string debug_info){
          if(_DEBUG_FLAG)
          {
            std::cout<<"[DebugPrint] "<<debug_info<<std::endl;
          }
        }
      };

      typedef CircularCurve* CircularCurvePtr;
      typedef CircularCurve& CircularCurveRef;

}  // namespace geometry
#include "circular_curve.hpp"

#endif
