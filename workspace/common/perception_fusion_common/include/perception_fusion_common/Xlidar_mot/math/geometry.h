/*
 * Copyright (c) 2015-2020 idriverplus(Beijing ZhiXingZhe Inc.)
 * website: www.idriverplus.com
 * author: wangxiao
 * update log:
 * 20170501:
 * 1) make up the basic static algorithm framework
*/

#pragma once
//c++ lib
#include <vector>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/PointIndices.h>

namespace avos {
namespace perception {
namespace xlidarMOT {   
namespace math {

struct Point2D {
  Point2D()
    : x(0)
    , y(0) {}

  float x;
  float y;
};
struct Line2DEquation {
  Line2DEquation()
    : k(0)
    , b(0)
    , isValid(false) {}
  //y=kx+b;
  float k;
  float b;
  Point2D start;
  Point2D end;
  pcl::PointIndices::Ptr inliers;
  bool isValid;
  //Ax+By+C=0
  //A=k
  //B=-1
  //C=b

};
struct PlaneEquation {
  PlaneEquation()
    : a(0)
    , b(0)
    , c(0)
    , isValid(false) {}

  float a;
  float b;
  float c;
  bool isValid;
};
struct Curve2DEquation {
  Curve2DEquation()
    : a(0)
    , b(0)
    , c(0)
    , isValid(false) {}

  //y = ax2+bx+c
  float a;
  float b;
  float c;
  Point2D start;
  Point2D end;
  std::vector<int> indices;
  bool isValid;
};

class Geometry {
public:
  template <typename PointT> static Line2DEquation
  LineFitting(const typename pcl::PointCloud<PointT>::Ptr& cloud_ptr,
              const float inlier_threshold);

  template <typename PointT> static PlaneEquation
  PlaneFitting(const typename pcl::PointCloud<PointT>::Ptr& cloud_ptr,
               const float inlier_threshold);

  template <typename Point2DT> static float
  DistancePoint2D2Line(const Point2DT& p, const Line2DEquation& line);

  template <typename PointT> static float
  DistancePoint2Plane(const PointT& p, const PlaneEquation& pl,
                      const int mode = 1);
};

}
}  // namespace math
}  // namespace perception
}  // namespace avos

#include "geometry.h"

