#pragma once
#include <math.h>
#include "log/log.h"
#include <istream>
#include <assert.h>
namespace avos {
namespace perception {
namespace xlidarMOT { 
// struct PointGCCS {
//   double xg;
//   double yg;
//   double angle;
// };
// struct VCP {
//   double x;
//   double y;
//   double angle;
// };
template <typename GCP, typename VCP> 
class perception_geotool {
public:
  perception_geotool(){}
  ~perception_geotool(){}
  bool VCS2GCCS(const GCP &s, double t_x, double t_y, GCP &res)
  {
    double cosa = cos(s.angle * deg2rad);
    double sina = sin(s.angle * deg2rad);
    res.xg = s.xg + t_x * cosa - t_y * sina;
    res.yg = s.yg + t_x * sina + t_y * cosa;
    if(fabs(res.xg)<0.1){
      // AINFO<<"selfcar.xg:"<<s.xg<<" yg:"<<s.yg;
      // AINFO<<"tar.x:"<<tar.x<<" y:"<<tar.y;
      // AINFO<<"res.xg:"<<res.xg<<" yg:"<<res.yg;
    }
    res.angle = 0.0;
    return true;
  }
  bool VCS2GCCS(const GCP &s, const VCP &tar, GCP &res)
  {
    double cosa = cos(s.angle * deg2rad);
    double sina = sin(s.angle * deg2rad);
    res.xg = s.xg + tar.x * cosa - tar.y * sina;
    res.yg = s.yg + tar.x * sina + tar.y * cosa;
    if(fabs(res.xg)<0.1){
      // AINFO<<"selfcar.xg:"<<s.xg<<" yg:"<<s.yg;
      // AINFO<<"tar.x:"<<tar.x<<" y:"<<tar.y;
      // AINFO<<"res.xg:"<<res.xg<<" yg:"<<res.yg;
    }
    res.angle = 0.0;
    return true;
  }
  // bool VCS2GCCSRad(const GCP &s, const VCP &tar, GCP &res)
  // {
  //   double cosa = cos(s.angle );
  //   double sina = sin(s.angle );
  //   res.xg = s.xg + tar.x * cosa - tar.y * sina;
  //   res.yg = s.yg + tar.x * sina + tar.y * cosa;
  //   if(fabs(res.xg)<0.1){
  //     // AINFO<<"selfcar.xg:"<<s.xg<<" yg:"<<s.yg;
  //     // AINFO<<"tar.x:"<<tar.x<<" y:"<<tar.y;
  //     // AINFO<<"res.xg:"<<res.xg<<" yg:"<<res.yg;
  //   }
  //   res.angle = 0.0;
  //   return true;
  // }
  template<typename T, typename Q, typename P>
  bool VCS2GCCSRad(const T &s, const Q &tar, P &res)
  {
    double cosa = cos(s.angle );
    double sina = sin(s.angle );
    res.xg = s.xg + tar.x * cosa - tar.y * sina;
    res.yg = s.yg + tar.x * sina + tar.y * cosa;
    return true;
  }
  bool GCCS2VCS(const GCP &s, const GCP &tar, VCP &res)
  {
    double cosa = cos(s.angle * deg2rad);
    double sina = sin(s.angle * deg2rad);
    double dx = tar.xg - s.xg;
    double dy = tar.yg - s.yg;
    res.x = dx * cosa + dy * sina;
    res.y = -dx * sina + dy * cosa;
    if(fabs(res.x)<0.1){
      // AINFO<<"selfcar.xg:"<<s.xg<<" yg:"<<s.yg;
      // AINFO<<"tar.xg:"<<tar.xg<<" y:"<<tar.yg;
      // AINFO<<"res.x:"<<res.x<<" y:"<<res.y;
    }
    res.angle = 0.0;
    return true;
  }
  template<typename T, typename Q, typename P>
  bool GCCS2VCS(const T &s, const Q &tar, P &res)
  {
    double cosa = cos(s.angle * deg2rad);
    double sina = sin(s.angle * deg2rad);
    double dx = tar.xg - s.xg;
    double dy = tar.yg - s.yg;
    res.x = dx * cosa + dy * sina;
    res.y = -dx * sina + dy * cosa;
    return true;
  }
  bool GCCS2VCSRad(const GCP &s, const GCP &tar, VCP &res)
  {
    double cosa = cos(s.angle);
    double sina = sin(s.angle);
    double dx = tar.xg - s.xg;
    double dy = tar.yg - s.yg;
    res.x = dx * cosa + dy * sina;
    res.y = -dx * sina + dy * cosa;
    if(fabs(res.x)<0.1){
      // AINFO<<"selfcar.xg:"<<s.xg<<" yg:"<<s.yg;
      // AINFO<<"tar.xg:"<<tar.xg<<" y:"<<tar.yg;
      // AINFO<<"res.x:"<<res.x<<" y:"<<res.y;
    }
    res.angle = 0.0;
    return true;
  }

public:
  const double pi = 3.1415926536;
  const double deg2rad = pi / 180.0;
  const double rad2deg = 180.0 / pi;
};

/// The get type macro function.
#define gettype(x) __typeof__(x)

class PerceptionMath {
public:
  PerceptionMath() {}
  ~PerceptionMath() {}

  const double pi = 3.1415926539;
  const double arc2deg=180.0/pi;
  const double deg2arc=pi/180.0;

  /// The max function.
  template <typename T>
  T mMax(const T &a, const T &b)
  {
    return a >= b ? a : b;
  }
  
  /// The min function.
  template <typename T>
  T mMin(const T &a, const T &b)
  {
    return a <= b ? a : b;
  }
  
  /// Clamp the input value in the range of a min and max limit.
  /**
   * \param[in] input The input value.
   * \param[in] min_limit The minmun output value.
   * \param[in] max_limit The maxmun output value.
   * \return The output value between the min and max value.
   */
  template <typename T>
  T dataClamp(const T &input, const T &min_limit, const T &max_limit)
  {
    assert(min_limit <= max_limit);
    
    if (input < min_limit) {
      return min_limit;
    }
    
    else if (input > max_limit) {
      return max_limit;
    }
    
    else {
      return input;
    }
  }
  /// Judge the input value is near to the target.
  /**
   * \param[in] input The input value.
   * \param[in] min_limit The minmun output value.
   * \param[in] max_limit The maxmun output value.
   * \return The output value between the min and max value.
   */
  template <typename T>
  bool near(const T &input, const T &target, const T &limit)
  {
    assert(limit > 0.0);
    
    if (target + limit >= input && target - limit <= input) {
      return true;
    }
    
    else {
      return false;
    }
  }
  /// Caculate the overlap.
  /**
   * A,B -> area(A ^ B)/area(A U B).
   * \param[in] left1 The input left value of object 1.
   * \param[in] right1 The input right value of object 1.
   * \param[in] up1 ...
   * \param[in] down1 ...
   * \param[in] left2 The input left value of object 2.
   * \param[in] right2 ...
   * \return The overlap value.
   */
  template <typename T>
  float overlapNormal(const T &left1, const T &right1, const T &up1,
                      const T &down1, const T &left2, const T &right2,
                      const T &up2, const T &down2)
  {
    T max_l = mMax<gettype(T)>(left1, left2);
    T min_r = mMin<gettype(T)>(right1, right2);
    T min_d = mMin<gettype(T)>(down1, down2);
    T max_u = mMax<gettype(T)>(up1, up2);
    
    if (max_l >= min_r || min_d <= max_u) {
      return 0.0;
    }
    
    float area_mid = (min_r - max_l) * (min_d - max_u);
    float area_sum =
      (right1 - left1) * (down1 - up1) + (right2 - left2) * (down2 - up2);
    return area_mid / (area_sum - area_mid);
  }
};
}
}
}
