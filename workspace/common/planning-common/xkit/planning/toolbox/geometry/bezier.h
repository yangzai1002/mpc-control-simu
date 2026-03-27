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
#ifndef __COMMON_TOOLBOX_GEOMETRY_INCLUDE_BEZIER_BASE_H__
#define __COMMON_TOOLBOX_GEOMETRY_INCLUDE_BEZIER_BASE_H__

#include <cmath>
#include <vector>
#include <functional>
#include <iostream>
#include "xkit/planning/toolbox/geometry/site.h"

namespace geometry {
using SiteVec = std::vector<Site>;

class Bezier
{

public:
  Bezier()
  {
    _resolution = 0.1;
    _curvature_threshold = std::numeric_limits<float>::max();
    _max_curvature = 0.0;
    _total_curvature = 0.0;
  }

  ~Bezier() = default;

  void GetCtrlPoints(SiteVec &CtrlPoints)
  {
    CtrlPoints.clear();
    CtrlPoints.emplace_back(_p0);
    CtrlPoints.emplace_back(_p1);
    CtrlPoints.emplace_back(_p2);
    CtrlPoints.emplace_back(_p3);
  }

  void SetCurvatureThreshold(const float& curvature) {
    _curvature_threshold = curvature;
  }

  float GetMaxCurvature() {
    return  _max_curvature;
  }

  float GetAccumulatedCurvature() {
    return  _total_curvature;
  }

  int CalculateCtrlPoints();

  int CalculateCurvature(Site& pt, const float& u, const float& t);

  int CalculateCurve(SiteVec & curve);

  int GenerateBezierCurve(const Site &start,const Site &end, SiteVec& curve);

private:
  Site _p0, _p1, _p2, _p3;
  float _resolution;
  float _curvature_threshold, _max_curvature, _total_curvature;
};
typedef Bezier* BezierPtr;
typedef Bezier& BezierRef;
}  // namespace geometry
#include "bezier.hpp"

#endif
