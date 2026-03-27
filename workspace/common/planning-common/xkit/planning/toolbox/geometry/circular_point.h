#ifndef COMMON_TOOLBOX_GEOMETRY_INCLUDE_CIRCULAR_POSITION_H__
#define COMMON_TOOLBOX_GEOMETRY_INCLUDE_CIRCULAR_POSITION_H__

#include <cmath>
#include <functional>
#include <limits>
#include "point2D.h"
#include "circular_curve.h"

namespace geometry {

struct CircularPoint {
  CircularPoint() {
    position.Reset();
    radius = 0.0;

  }

  ~CircularPoint() = default;

  Point2D position;
  double radius;


  void operator=(const CircularPoint &s) {
    position = s.position;
    radius = s.radius;
  }

  CircularPoint &operator+=(const CircularPoint &s) {
    position += s.position;
    return *this;
  }
  CircularPoint &operator-=(const CircularPoint &s) {
    position -= s.position;
    return *this;
  }

  void Reset() {
    position.Reset();
    radius = 0.0;
  }


};


}  // namespace geometry

#endif  
