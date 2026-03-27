/*
 * Copyright (c) 2015-2020 idriverplus(Beijing ZhiXingZhe Inc.)
 * website: www.idriverplus.com
 * author: wangxiao
 * update log:
 * 20170501:
 * 1) make up the basic static algorithm framework
*/
#pragma once
#include <cmath>
#include <pcl/ModelCoefficients.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>

namespace avos {
namespace perception {
namespace xlidarMOT { 
namespace math {
//
template <typename PointT> Line2DEquation
Geometry::LineFitting(const typename pcl::PointCloud<PointT>::Ptr& cloud_ptr,
                      const float inlier_threshold) {
  pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
  pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
  pcl::SACSegmentation<PointT> seg;
  seg.setOptimizeCoefficients(true);
  seg.setModelType(pcl::SACMODEL_LINE);
  seg.setMethodType(pcl::SAC_RANSAC);
  seg.setDistanceThreshold(inlier_threshold);
  seg.setInputCloud(cloud_ptr);
  seg.segment(*inliers, *coefficients);
  if (0 == inliers->indices.size()) {
    return Line2DEquation();
  }
  Line2DEquation li;
  li.start.x = coefficients->values[0];
  li.start.y = coefficients->values[1];
  li.end.x   = li.start.x + coefficients->values[3];
  li.end.y   = li.start.y + coefficients->values[4];
  li.k       = (li.end.y - li.start.y) / (li.end.x - li.start.x);
  li.b       = li.end.y - li.k * li.end.x;
  li.inliers = inliers;
  if (std::isfinite(li.k) && std::isfinite(li.b)) {
    li.isValid = true;
  }
  //
  return li;
}
template <typename PointT> PlaneEquation
Geometry::PlaneFitting(const typename pcl::PointCloud<PointT>::Ptr& cloud_ptr,
                       const float inlier_threshold) {
  pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
  pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
  pcl::SACSegmentation<PointT> seg;
  seg.setOptimizeCoefficients(true);
  seg.setModelType(pcl::SACMODEL_PLANE);
  seg.setMethodType(pcl::SAC_RANSAC);
  seg.setDistanceThreshold(inlier_threshold);
  seg.setInputCloud(cloud_ptr);
  seg.segment(*inliers, *coefficients);
  if (0 == inliers->indices.size()) {
    return PlaneEquation();
  }
  PlaneEquation sp;
  //ax+by+cz+1=0
  sp.a = coefficients->values[0] / coefficients->values[3];
  sp.b = coefficients->values[1] / coefficients->values[3];
  sp.c = coefficients->values[2] / coefficients->values[3];
  if (std::isfinite(sp.a) && std::isfinite(sp.b) && std::isfinite(sp.c) &&
      (sp.a != 0 || sp.b != 0 || sp.c != 0)) {
    sp.isValid = true;
  }
  return sp;
}
template <typename Point2DT> float
Geometry::DistancePoint2D2Line(const Point2DT& p, const Line2DEquation& line) {
  //Ax+By+C=0
  //A=k
  //B=-1
  //C=b
  //m=p.x
  //n=p.y
  //|Am+Bn+C|/sqrt(A^2+B^2)
  float dis1 = fabs(line.k * p.x + (-1 * p.y) + line.b);
  float dis2 = hypot(line.k, 1.0);
  return dis1 / dis2;
}

//  mode 0 : ax+by+cz+1=0
//  mode 1 : k = (ax+by+cz+1)/sqrt(a^2+b^2+c^2)
template <typename PointT> float
Geometry::DistancePoint2Plane(const PointT& p, const PlaneEquation& pl,
                              const int mode) {
  if (!pl.isValid ||
      !std::isfinite(p.x) ||
      !std::isfinite(p.y) ||
      !std::isfinite(p.z)) {
    return p.z;
  }
  float dis = 0;
  if (0 == mode) {
    if (pl.c) {
      float z_offset = -1 * (pl.a * p.x + pl.b * p.y + 1) / pl.c;
      dis = p.z - z_offset;
    } else {
      dis = p.z;
    }
  } else if (1 == mode) {
    float dis1 = pl.a * p.x + pl.b * p.y + pl.c * p.z + 1;
    dis1 = pl.c * 10e10 + 1 > 0 ? dis1 : -dis1;
    float dis2 = sqrt(pl.a * pl.a + pl.b * pl.b + pl.c * pl.c);
    if (dis2) {
      dis = dis1 / dis2;
    } else {
      dis = p.z;
    }
  }
  return dis;
}

}
}  // namespace math
}  // namespace perception
}  // namespace avos