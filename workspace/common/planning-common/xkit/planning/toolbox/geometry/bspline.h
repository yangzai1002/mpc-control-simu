/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: bspline.h
 *
 * Description: declare 3-order-B-Spline curve methods
 *
 * History:
 * Steckon         2020/08/05    v1.0.0        Build this model.
 ******************************************************************************/
#ifndef __COMMON_TOOLBOX_GEOMETRY_INCLUDE_BSPLINE_BASE_H__
#define __COMMON_TOOLBOX_GEOMETRY_INCLUDE_BSPLINE_BASE_H__

#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
#include "xkit/planning/toolbox/geometry/site.h"

namespace geometry {

using SiteVec = std::vector<Site>;

typedef struct tagPosition {
  double x;
  double y;
  tagPosition(double _x, double _y) {
    x = _x;
    y = _y;
  }
  tagPosition(){};
  bool operator==(const tagPosition &pt) { return (x == pt.x && y == pt.y); }
} CPosition;

class BSpline {
 public:
  BSpline() {}

  ~BSpline() = default;

  void TwoOrderBSplineSmooth(CPosition *pt, int Num);

  void TwoOrderBSplineInterpolatePt(CPosition *&pt, int &Num, int *InsertNum);

  void ThreeOrderBSplineSmooth(CPosition *pt, int Num);

  void ThreeOrderBSplineInterpolatePt(CPosition *&pt, int &Num, int *InsertNum);

  int BSplineFilt(int order, std::list<Site> &result_path,
                  const std::list<Site> &un_filted_path);
  int BSplineFilt(int order, std::vector<Site> &result_path,
                  const std::vector<Site> &un_filted_path);

 private:
  double F02(double t);
  double F12(double t);
  double F22(double t);

  double F03(double t);
  double F13(double t);
  double F23(double t);
  double F33(double t);
};
typedef BSpline *BSplinePtr;
typedef BSpline &BSplineRef;

}  // namespace geometry
#include "bspline.hpp"

#endif