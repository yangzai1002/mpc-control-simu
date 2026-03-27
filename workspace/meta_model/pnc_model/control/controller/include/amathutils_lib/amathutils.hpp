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

#ifndef AMATHUTILS_LIB_AMATHUTILS_HPP
#define AMATHUTILS_LIB_AMATHUTILS_HPP

#include <cmath>
#include <iostream>
#include "core/types.h"
namespace amathutils
{

#define G_MPSS 9.80665  // m/s^2

inline double rad2deg(double _angle)
{
  return _angle * 180.0 / M_PI;
}
inline double deg2rad(double _angle)
{
  return _angle / 180.0 * M_PI;
}

inline double mps2kmph(double _mpsval)
{
  return (_mpsval * 3.6);  // mps * 60sec * 60minutes / 1000m
}

inline double kmph2mps(double _kmphval)
{
  return (_kmphval * 1000.0 / 60.0 / 60.0);  // kmph * 1000m / 60sec / 60sec
}

inline double getGravityAcceleration(double _acceleration_mpss)
{
  return _acceleration_mpss / G_MPSS;
}

inline double getAcceleration(double _v0, double _v, double _x)
{
  return (_v * _v - _v0 * _v0) / 2.0 / _x;
}

inline double getTimefromAcceleration(double _v0, double _v, double _a)
{
  return (_v - _v0) / _a;
}

bool getIntersect(double x1, double y1, double x2, double y2, double x3,
  double y3, double x4, double y4, double* intersect_x, double* intersect_y);
bool getIntersect(Adsfi::Point3d p1, Adsfi::Point3d p2, Adsfi::Point3d p3,
  Adsfi::Point3d p4, Adsfi::Point3d* intersect);

Adsfi::Point3d getNearPtOnLine(const Adsfi::Point3d &_p, const Adsfi::Point3d &_a,
                                     const Adsfi::Point3d &_b);
double find_distance(const Adsfi::Point3d &_from, const Adsfi::Point3d &_to);
double find_distance(const Adsfi::HafPose&_from, const Adsfi::HafPose&_to);
double find_angle(const Adsfi::Point3d &_from, const Adsfi::Point3d &_to);
bool isIntersectLine(const Adsfi::Point3d &_l1_p1, const Adsfi::Point3d &_l1_p2,
  const Adsfi::Point3d &_l2_p1, const Adsfi::Point3d &_l2_p2);
int isPointLeftFromLine(const Adsfi::Point3d &_target, const Adsfi::Point3d &_line_p1,
  const Adsfi::Point3d &_line_p2);

/**
 * @fn distanceFromSegment
 * @brief calculates the distance between from a point to closest point on a line segment
 * @param  _l1  first point of line segment        
 * @param  _l2  second point of line segment        
 * @param  _p   the point to find distance
 * @return distance between point _p and line segment(_l1,_l2) 
 */
double distanceFromSegment(const Adsfi::Point3d &_l1, const Adsfi::Point3d &_l2,
  const Adsfi::Point3d &_p);
double getPoseYawAngle(const Adsfi::HafPose &_pose);

/**
 * @brief convert from yaw to ros-Quaternion
 * @param [in] yaw input yaw angle
 * @return quaternion
 */
Adsfi::HafQuaternion getQuaternionFromYaw(const double &_yaw);

/**
 * @brief normalize angle into [-pi to pi]
 * @param [in] _angle input angle
 * @return normalized angle
 */
double normalizeRadian(const double _angle);

double calcPosesAngleDiffRaw(const Adsfi::HafPose&p_from, const Adsfi::HafPose&_p_to);
double calcPosesAngleDiffDeg(const Adsfi::HafPose&_p_from, const Adsfi::HafPose&_p_to);
double calcPosesAngleDiffRad(const Adsfi::HafPose&_p_from, const Adsfi::HafPose&_p_to);
double longituteTrans(double longitute,double origin);
double latitudeTrans(double latitude,double origin);
double Translongitute(double x,double origin);
double Translatitude(double y,double origin);

}  // namespace amathutils

#endif  // AMATHUTILS_LIB_AMATHUTILS_HPP
