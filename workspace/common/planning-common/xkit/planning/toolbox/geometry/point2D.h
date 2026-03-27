#ifndef COMMON_TOOLBOX_GEOMETRY_INCLUDE_POINT2D_H__
#define COMMON_TOOLBOX_GEOMETRY_INCLUDE_POINT2D_H__

#include <cmath>
#include <functional>
#include <limits>

namespace geometry {

struct Point2D {
  Point2D() {
    x = 0.0;
    y = 0.0;

  }

  Point2D(double _x, double _y) {
    x = _x;
    y = _y;
  }

  ~Point2D() = default;

  double x;
  double y;


  void operator=(const Point2D &s) {
    x = s.x;
    y = s.y;
  }

  Point2D &operator+=(const Point2D &s) {
    x += s.x;
    y += s.y;
    return *this;
  }
  Point2D &operator-=(const Point2D &s) {
    x -= s.x;
    y -= s.y;
    return *this;
  }

  void Reset() {
    x = 0.0;
    y = 0.0;
  }

};


}  // namespace geometry

#endif  

