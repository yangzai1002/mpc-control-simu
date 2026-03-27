#ifndef COMMON_TOOLBOX_GEOMETRY_INCLUDE_POSE_H__
#define COMMON_TOOLBOX_GEOMETRY_INCLUDE_POSE_H__

#include <cmath>
#include <functional>
#include <limits>
#include "xkit/planning/toolbox/geometry/point2D.h"

namespace geometry {

struct Pose {
  Pose() {
  	position.Reset();
  	angle = 0.0;
  }

  ~Pose() = default;

  Point2D position;
  double angle;


  void operator=(const Pose &s) {
    position = s.position;
    angle = s.angle;
  }

  Pose &operator+=(const Pose &s) {
    position += s.position;
    return *this;
  }
  Pose &operator-=(const Pose &s) {
    position -= s.position;
    return *this;
  }

  void Reset() {
  	position.Reset();
  	angle = 0.0;
  }

};


}  // namespace geometry

#endif  
