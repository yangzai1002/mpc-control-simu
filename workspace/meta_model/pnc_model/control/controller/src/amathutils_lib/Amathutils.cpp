/*
 * Copyright 2018-2019 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <amathutils_lib/amathutils.hpp>

namespace amathutils
{
Adsfi::Point3d getNearPtOnLine(const Adsfi::Point3d &_p, const Adsfi::Point3d &_a,
                                     const Adsfi::Point3d &_b)
{
  double len = find_distance(_a, _b);

  Adsfi::Point3d vnab;
  Adsfi::Point3d vap;
  Adsfi::Point3d ret;

  vnab.x = (_b.x - _a.x) / len;
  vnab.y = (_b.y - _a.y) / len;
  vnab.z = (_b.z - _a.z) / len;

  vap.x = _p.x - _a.x;
  vap.y = _p.y - _a.y;
  vap.z = _p.z - _a.z;

  double dist_ax = vnab.x * vap.x + vnab.y * vap.y + vnab.z * vap.z;

  ret.x = _a.x + (vnab.x * dist_ax);
  ret.y = _a.y + (vnab.y * dist_ax);
  ret.z = _a.z + (vnab.z * dist_ax);

  return ret;
}

double find_distance(const Adsfi::Point3d &_from, const Adsfi::Point3d&_to)
{
  return std::hypot(std::hypot(_from.x - _to.x, _from.y - _to.y), _from.z - _to.z);
}

double find_distance(const Adsfi::HafPose &_from, const Adsfi::HafPose &_to)
{
  return find_distance(_from.position, _to.position);
}

double find_angle(const Adsfi::Point3d &_from, const Adsfi::Point3d&_to)
{
  double _angle = std::atan2(_to.y - _from.y, _to.x - _from.x);
  if (_angle < 0.0)
    _angle = _angle + 2 * M_PI;

  return _angle * 360 / (2 * M_PI);
}

bool isIntersectLine(const Adsfi::Point3d &_l1_p1, const Adsfi::Point3d &_l1_p2,
                     const Adsfi::Point3d &_l2_p1, const Adsfi::Point3d &_l2_p2)
{
  const double ta = (_l2_p1.x - _l2_p2.x) * (_l1_p1.y - _l2_p1.y) + (_l2_p1.y - _l2_p2.y) * (_l2_p1.x - _l1_p1.x);
  const double tb = (_l2_p1.x - _l2_p2.x) * (_l1_p2.y - _l2_p1.y) + (_l2_p1.y - _l2_p2.y) * (_l2_p1.x - _l1_p2.x);
  const double tc = (_l1_p1.x - _l1_p2.x) * (_l2_p1.y - _l1_p1.y) + (_l1_p1.y - _l1_p2.y) * (_l1_p1.x - _l2_p1.x);
  const double td = (_l1_p1.x - _l1_p2.x) * (_l2_p2.y - _l1_p1.y) + (_l1_p1.y - _l1_p2.y) * (_l1_p1.x - _l2_p2.x);

  if (tc * td < 0 && ta * tb < 0)
    return true;
  else
    return false;
}

/*
 *        |
 *     .　|       =>LEFT = 1
 *     　 |   .   =>RIGHT = -1
 *      　|　
 */

int isPointLeftFromLine(const Adsfi::Point3d &_target, const Adsfi::Point3d &_line_p1,
                        const Adsfi::Point3d &_line_p2)
{
  const int LEFT = 1;
  const int RIGHT = -1;
  const int ONLINE = 0;
  const double n = _target.x * (_line_p1.y - _line_p2.y) + _line_p1.x * (_line_p2.y - _target.y) +
                   _line_p2.x * (_target.y - _line_p1.y);

  return n > 0 ? LEFT : n < 0 ? RIGHT : ONLINE;
}


// Following implementation comes from the website below:
// Author: Dan Sunday
// site: http://geomalgorithms.com/a02-_lines.html
// viewed: 2019/5/20
double distanceFromSegment(
  const Adsfi::Point3d &_l1, const Adsfi::Point3d &_l2, const Adsfi::Point3d &_p)
{
  Adsfi::Point3d v, w;
  v.x = _l2.x - _l1.x;
  v.y = _l2.y - _l1.y;
  v.z = _l2.z - _l1.z;

  w.x = _p.x - _l1.x;
  w.y = _p.y - _l1.y;
  w.z = _p.z - _l1.z;

  double dot_vw = v.x * w.x + v.y * w.y + v.z * w.z;
  double dot_vv = v.x * v.x + v.y * v.y + v.z * v.z;

  if (dot_vw <= 0)
  {
    return find_distance(_p, _l1);
  }

  if (dot_vv <= dot_vw)
  {
    return find_distance(_p, _l2);
  }

  double b = dot_vw / dot_vv;
  Adsfi::Point3d pb;
  pb.x = _l1.x + b * v.x;
  pb.y = _l1.y + b * v.y;
  pb.z = _l1.z + b * v.z;

  return find_distance(_p, pb);
}

double getPoseYawAngle(const Adsfi::HafPose &_pose)
{
 
    double x = _pose.orientation.x;
    double y = _pose.orientation.y;
    double z = _pose.orientation.z;
    double w = _pose.orientation.w;

    // 核心公式计算偏航角
    double sin_yaw = 2.0 * (w * y - x * z);
    double cos_yaw = 1.0 - 2.0 * (y * y + z * z);
    
    // atan2 返回范围[-π, π]，符合航向角需求
    double yaw = std::atan2(sin_yaw, cos_yaw);

    return yaw;
}

Adsfi::HafQuaternion getQuaternionFromYaw(const double &_yaw)
{
  Adsfi::HafQuaternion q;
  double roll = 0,pitch = 0;
  double yaw = _yaw;

  // 计算半角的正弦和余弦
  double cr = cos(roll / 2.0);
  double sr = sin(roll / 2.0);
  double cp = cos(pitch / 2.0);
  double sp = sin(pitch / 2.0);
  double cy = cos(yaw / 2.0);
  double sy = sin(yaw / 2.0);

  q.x = sr * cp * cy - cr * sp * sy;
  q.y = cr * sp * cy + sr * cp * sy;
  q.z = cr * cp * sy - sr * sp * cy;
  q.w = cr * cp * cy + sr * sp * sy;

  return q;
  
}

double calcPosesAngleDiffRaw(const Adsfi::HafPose  &p_from, const Adsfi::HafPose &_p_to)
{
  return getPoseYawAngle(p_from) - getPoseYawAngle(_p_to);
}

double normalizeRadian(const double _angle)
{
  double n_angle = std::fmod(_angle, 2 * M_PI);
  n_angle = n_angle > M_PI ? n_angle - 2 * M_PI : n_angle < -M_PI ? 2 * M_PI + n_angle : n_angle;

  // another way
  // Math.atan2(Math.sin(_angle), Math.cos(_angle));
  return n_angle;
}

double calcPosesAngleDiffDeg(const Adsfi::HafPose  &_p_from, const Adsfi::HafPose  &_p_to)
{
  // convert to [-pi : pi]
  return rad2deg(normalizeRadian(calcPosesAngleDiffRaw(_p_from, _p_to)));
}

double calcPosesAngleDiffRad(const Adsfi::HafPose  &_p_from, const Adsfi::HafPose  &_p_to)
{
  // convert to [-pi : pi]
  return normalizeRadian(calcPosesAngleDiffRaw(_p_from, _p_to));
}

bool getIntersect(Adsfi::Point3d p1, Adsfi::Point3d p2, Adsfi::Point3d p3,
  Adsfi::Point3d p4, Adsfi::Point3d* intersect)
{
  return getIntersect(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, &intersect->x, &intersect->y);
}

bool getIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4,
  double y4, double* intersect_x, double* intersect_y )
{
    // let p1(x1, y1), p2(x2, y2), p3(x3, y3), p4(x4,y4)
    // intersect of line segment p1 to p2 and p3 to p4 satisfies
    // p1 + r(p2 - p1) = p3 + s(p4 - p3)
    // 0 <= r <= 1
    // 0 <= s <= 1
    double denominator = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);

    if (denominator == 0)
    {
      // line is parallel
      return false;
    }

    double r = ( (y4 - y3) * (x3 - x1) - (x4 - x3) * (y3 - y1) ) / denominator;
    double s = ( (y2 - y1) * (x3 - x1) - (x2 - x1) * (y3 - y1) ) / denominator;

    if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
    {
      *intersect_x = x1 + r * (x2 - x1);
      *intersect_y = y1 + r * (y2 - y1);
      return true;
    }
    else
    {
      return false;
    }
}

double longituteTrans(double longitute,double originlo)
{
  double origin = originlo;
  double transfer = (longitute - origin)*(M_PI/180.0*6371000.0*0.86602);
  return transfer;
};

double latitudeTrans(double latitude,double originla)
{
  double origin = originla;
  double transfer = (latitude - origin)*(M_PI/180.0*6371000.0);
  return transfer;
};

double Translongitute(double x,double originlo)
{
  double origin = originlo;
  double transfer = x/(M_PI/180.0*6371000.0) + origin;
  return transfer;
};

double Translatitude(double y,double originla)
{
  double origin = originla;
  double transfer = y/(M_PI*63710000.0) + origin ;
  return transfer;
};

}  // namespace amathutils
