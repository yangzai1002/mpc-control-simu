#pragma once

#include <algorithm>
#include <cfloat>
#include <numeric>
#include <vector>

#include <Eigen/Dense>

namespace avos {
namespace perception {
namespace algorithm {

template <class CLOUD_IN_TYPE, class CLOUD_OUT_TYPE>
class ConvexHull2D {
 public:
  ConvexHull2D() : in_cloud_(nullptr) {
    points_.reserve(1000.0);
    polygon_indices_.reserve(1000.0);
  }
  ~ConvexHull2D() { in_cloud_ = nullptr; }
  // main interface to get polygon from input point cloud
  bool GetConvexHull(const CLOUD_IN_TYPE& in_cloud,
                     CLOUD_OUT_TYPE* out_polygon) {
    SetPoints(in_cloud);
    if (!GetConvexHullMonotoneChain(out_polygon)) {
      return MockConvexHull(out_polygon);
    }
    return true;
  }

 private:
  // save points in local memory, and transform to double
  void SetPoints(const CLOUD_IN_TYPE& in_cloud);
  // mock a polygon for some degenerate cases
  bool MockConvexHull(CLOUD_OUT_TYPE* out_polygon);
  // compute convex hull using Andrew's monotone chain algorithm
  bool GetConvexHullMonotoneChain(CLOUD_OUT_TYPE* out_polygon);
  // given 3 ordered points, return true if in counter clock wise.
  bool IsCounterClockWise(const Eigen::Vector2f& p1, const Eigen::Vector2f& p2,
                          const Eigen::Vector2f& p3, const double& eps) {
    Eigen::Vector2f p12 = p2 - p1;
    Eigen::Vector2f p13 = p3 - p1;
    return (p12(0) * p13(1) - p12(1) * p13(0) > eps);
  }

 private:
  std::vector<Eigen::Vector2f> points_;
  std::vector<std::size_t> polygon_indices_;
  const CLOUD_IN_TYPE* in_cloud_;
};

template <class CLOUD_IN_TYPE, class CLOUD_OUT_TYPE>
void ConvexHull2D<CLOUD_IN_TYPE, CLOUD_OUT_TYPE>::SetPoints(
  const CLOUD_IN_TYPE& in_cloud) {
  points_.resize(in_cloud.size());
  for (std::size_t i = 0; i < points_.size(); ++i) {
    points_[i] << in_cloud[i].x(), in_cloud[i].y();
  }
  in_cloud_ = &in_cloud;
}

template <class CLOUD_IN_TYPE, class CLOUD_OUT_TYPE>
bool ConvexHull2D<CLOUD_IN_TYPE, CLOUD_OUT_TYPE>::MockConvexHull(
  CLOUD_OUT_TYPE* out_polygon) {
  if (in_cloud_->size() == 0) {
    return false;
  }
  out_polygon->resize(4);
  Eigen::Matrix<double, 3, 1> maxv;
  Eigen::Matrix<double, 3, 1> minv;
  maxv << in_cloud_->at(0).x(), in_cloud_->at(0).y(), in_cloud_->at(0).z();
  minv = maxv;
  for (std::size_t i = 1; i < in_cloud_->size(); ++i) {
    maxv(0) = std::max<double>(maxv(0), in_cloud_->at(i).x());
    maxv(1) = std::max<double>(maxv(1), in_cloud_->at(i).y());
    maxv(2) = std::max<double>(maxv(2), in_cloud_->at(i).z());

    minv(0) = std::min<double>(minv(0), in_cloud_->at(i).x());
    minv(1) = std::min<double>(minv(1), in_cloud_->at(i).y());
    minv(2) = std::min<double>(minv(2), in_cloud_->at(i).z());
  }

  static const double eps = 1e-3;
  for (std::size_t i = 0; i < 3; ++i) {
    if (maxv(i) - minv(i) < eps) {
      maxv(i) += eps;
      minv(i) -= eps;
    }
  }

  out_polygon->at(0).x() = static_cast<float>(minv(0));
  out_polygon->at(0).y() = static_cast<float>(minv(1));
  out_polygon->at(0).z() = static_cast<float>(minv(2));

  out_polygon->at(1).x() = static_cast<float>(maxv(0));
  out_polygon->at(1).y() = static_cast<float>(minv(1));
  out_polygon->at(1).z() = static_cast<float>(minv(2));

  out_polygon->at(2).x() = static_cast<float>(maxv(0));
  out_polygon->at(2).y() = static_cast<float>(maxv(1));
  out_polygon->at(2).z() = static_cast<float>(minv(2));

  out_polygon->at(3).x() = static_cast<float>(minv(0));
  out_polygon->at(3).y() = static_cast<float>(maxv(1));
  out_polygon->at(3).z() = static_cast<float>(minv(2));
  return true;
}

template <class CLOUD_IN_TYPE, class CLOUD_OUT_TYPE>
bool ConvexHull2D<CLOUD_IN_TYPE, CLOUD_OUT_TYPE>::GetConvexHullMonotoneChain(
  CLOUD_OUT_TYPE* out_polygon) {
  if (points_.size() < 3) {
    return false;
  }

  std::vector<std::size_t> sorted_indices(points_.size());
  std::iota(sorted_indices.begin(), sorted_indices.end(), 0);

  static const double eps = 1e-9;
  std::sort(sorted_indices.begin(), sorted_indices.end(),
  [&](const std::size_t& lhs, const std::size_t& rhs) {
    double dx = points_[lhs](0) - points_[rhs](0);
    if (std::abs(dx) > eps) {
      return dx < 0.0;
    }
    return points_[lhs](1) < points_[rhs](1);
  });
  int count = 0;
  int last_count = 1;
  polygon_indices_.clear();
  polygon_indices_.reserve(points_.size());

  std::size_t size2 = points_.size() * 2;
  for (std::size_t i = 0; i < size2; ++i) {
    if (i == points_.size()) {
      last_count = count;
    }
    const std::size_t& idx =
      sorted_indices[(i < points_.size()) ? i : (size2 - 1 - i)];
    const auto& point = points_[idx];
    while (count > last_count &&
           !IsCounterClockWise(points_[polygon_indices_[count - 2]],
                               points_[polygon_indices_[count - 1]], point,
                               eps)) {
      polygon_indices_.pop_back();
      --count;
    }
    polygon_indices_.push_back(idx);
    ++count;
  }
  --count;
  polygon_indices_.pop_back();
  if (count < 3) {
    return false;
  }
  out_polygon->clear();
  out_polygon->resize(polygon_indices_.size());
  float min_z = static_cast<float>(in_cloud_->at(0).z());
  for (std::size_t id = 0; id < in_cloud_->size(); ++id) {
    min_z = std::min<float>(static_cast<float>(in_cloud_->at(id).z()), min_z);
  }
  for (std::size_t i = 0; i < polygon_indices_.size(); ++i) {
    out_polygon->at(i).x() =
      static_cast<float>(points_[polygon_indices_[i]](0));
    out_polygon->at(i).y() =
      static_cast<float>(points_[polygon_indices_[i]](1));
    out_polygon->at(i).z() = min_z;
  }
  return true;
}

}  // namespace algorithm
}  // namespace perception
}  // namespace avos
