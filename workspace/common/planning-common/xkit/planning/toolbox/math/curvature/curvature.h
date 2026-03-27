/******************************************************************************
* Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
*
* NodeName: toolbox
* FileName: math_common.h
*
* Description: generate random number with convenient API
*
* History:
* Feng younan          2018/11/21          v1.0.0        Build this model.
******************************************************************************/
#ifndef COMMON_TOOLBOX_MATH_INCLUDE_CURvATURE_H_
#define COMMON_TOOLBOX_MATH_INCLUDE_CURvATURE_H_

#include "xkit/planning/toolbox/geometry/geoheader.h"
#include "curvature.hpp"

namespace avos {
namespace planning {
namespace math {

using geometry::Site;
using geometry::SiteVec;


class Curvature {
  public:
  Curvature();
  ~Curvature();
  
  /**
   * @brief   caculate curve of points in list
   * @param  list    vector to be caculated
   * @return   0 success
   * @return  -1 failure
  **/
  int Curve(SiteVec& list);
  int CurveUTM(SiteVec& list);

private:
  double curve_end_;
  void CalculateLengthOfPoint(SiteVec &list);

};


} // math
} // planning
} // avos

#endif 
