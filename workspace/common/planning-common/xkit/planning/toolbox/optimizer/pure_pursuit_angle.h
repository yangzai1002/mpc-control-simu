/******************************************************************************
* Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
*
* NodeName: toolbox
* FileName: pure_pursuit.h
*
* Description: Path smooth with Pure Pursuit
*
* History:
* Feng younan          2019/01/04          v1.0.0        Build this model.
******************************************************************************/
#ifndef COMMON_TOOLBOX_OPTIMIZER_INCLUDE_PURE_PURSUIT_ANGLE_H_
#define COMMON_TOOLBOX_OPTIMIZER_INCLUDE_PURE_PURSUIT_ANGLE_H_

#include "xkit/planning/toolbox/geometry/geoheader.h"
#include "xkit/planning/toolbox/optimizer/curvedetection.h"

#include <limits>
#include <cmath>



using geometry::Site;
using geometry::SiteVec;
using optimizer::CurveDetection;

namespace toolbox   {
namespace optimizer {
namespace ppangle   {

const double k   = 0.1;  // 
const double dt  = 0.05; // (s)
const double V   = 1.0;  // (m/s)
const double MAX = std::numeric_limits<double>::max();


class PurePursuitAngle {
 public:
  PurePursuitAngle();
  ~PurePursuitAngle() = default;

 public:
  /**
   * @breif : to smooth a path with pure persuit
   * @param : path  to store the smoothed path
   * @return:  0 success
   * @return: -1 failure
   **/
  int DisSmooth(SiteVec& path);
  /**
   * @breif : init this smoother
   * @param : param_L  wheel base
   * @param : param_R  mininum tuning radius
   * @return: none
   **/
  void Init(const Site status, const double param_L = 0.9,
            const double param_R = 1.4, const double param_Lf = 1.0);

 private:
  int AddPreviewDistPoint(SiteVec &path);

  int DisTargetInd4FirstTime();
  /**
   * @breif : find target index by the latest status
   * @return:  0    success
   * @return: -1    failure
   **/
  int DisTargetInd();
  /**
   * @breif : calculate the input of vehicle
   * @param : delta      to store the angle of front wheel
   * @return:  0         success
   * @return: -1         failure
   **/
  int DisControl(double& kai);
  /**
   * @breif : calculate the relative pt from local to target
   * @param : target      output point
   * @return:  0         success
   * @return: -1         failure
   **/
  int PreviewGlobal2Local(Site &target);
  /**
   * @breif : update the status of vehicle
   * @param : delta target front wheel angle
   * @return:  0    success
   * @return: -1    failure
   **/
  int DisUpdate(const double kai);
  /**
   * @breif : the end condition for the pure pursuit
   * @param : 
   * @return: true    success
   * @return: false    failure
   **/
  bool MeetThreshold();
  /**
   * @breif : Normalize the rad
   * @param : delta target front wheel angle
   * @return:  -M_PI to M_PI
   **/
  double NormalizeRad(const double heading);

  double NormalizeAngle(const double heading);


  const double MaxDelta() { return std::atan2(L_, R_); }
  
 private:
  size_t  target_index_;
  size_t  last_near_;

  double  L_;
  double  R_;
  double  Lf_;

  SiteVec raw_path_;
  SiteVec update_path_;
  Site    status_;

  double pursuit_dis_err_;
  double pursuit_angle_err_;
};


} // namespace ppangle
} // namespace optimizer
} // namespace toolbox

#include "pure_pursuit_angle.hpp"

#endif // __TOOLBOX_OPTIMIZER_PURE_PURSUIT_ANGLE_H__

