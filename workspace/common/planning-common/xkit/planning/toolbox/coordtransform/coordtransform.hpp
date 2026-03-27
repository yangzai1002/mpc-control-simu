#pragma once

namespace avos {
namespace planning {

inline CoordTransform::CoordTransform() {}
inline CoordTransform::~CoordTransform() {}

inline bool CoordTransform::GCCS2VCS(const Site &car_pos, const Site &target_pos, Site &vcs) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.xg;
  p1.yg = car_pos.yg;
  p1.angle = car_pos.globalangle;
  p2.xg = target_pos.xg;
  p2.yg = target_pos.yg;
  p2.angle = target_pos.globalangle;
  int ret = GCCS2VCS(p1, p2, p3);
  vcs.x = p3.x;
  vcs.y = p3.y;
  vcs.angle = p3.angle;
  vcs.xg = target_pos.xg;
  vcs.yg = target_pos.yg;
  if (ret == -1) return false;
  return true;
}

inline bool CoordTransform::DRGCCS2VCS(const Site &car_pos, const Site &target_pos, Site &vcs) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.xg;
  p1.yg = car_pos.yg;
  p1.angle = car_pos.globalangle;
  p2.xg = target_pos.xg;
  p2.yg = target_pos.yg;
  p2.angle = target_pos.globalangle;
  int ret = GCCS2VCS(p1, p2, p3);
  vcs.x = p3.x;
  vcs.y = p3.y;
  vcs.angle = p3.angle;
  vcs.xg = target_pos.xg;
  vcs.yg = target_pos.yg;
  if (ret == -1) return false;
  return true;
}

inline bool CoordTransform::GCCS2VCS(const LocalizationData &car_pos, const Site &target_pos, Site &vcs) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.xg;
  p1.yg = car_pos.yg;
  p1.angle = car_pos.yaw;
  p2.xg = target_pos.xg;
  p2.yg = target_pos.yg;
  p2.angle = target_pos.globalangle;
  int ret = GCCS2VCS(p1, p2, p3);
  vcs.x = p3.x;
  vcs.y = p3.y;
  vcs.angle = p3.angle;
  vcs.xg = target_pos.xg;
  vcs.yg = target_pos.yg;
  if (ret == -1) return false;
  return true;
}

inline bool CoordTransform::DRGCCS2VCS(const LocalizationData &car_pos, const Site &target_pos, Site &vcs) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.loc_xg_dr;
  p1.yg = car_pos.loc_yg_dr;
  p1.angle = car_pos.loc_yaw_dr;
  p2.xg = target_pos.xg;
  p2.yg = target_pos.yg;
  p2.angle = target_pos.globalangle;
  int ret = GCCS2VCS(p1, p2, p3);
  vcs.x = p3.x;
  vcs.y = p3.y;
  vcs.angle = p3.angle;
  vcs.xg = target_pos.xg;
  vcs.yg = target_pos.yg;
  if (ret == -1) return false;
  return true;
}

// bool CoordTransform::GCCS2VCS(const PointGCCS &car_pos, const PointGCCS &target_pos, PointVCS &vcs) {
//   int ret = geotool_.GCCS2VCS(car_pos, target_pos, vcs);
//   if (ret == -1) return false;
//   return true;
// }
inline bool CoordTransform::GCCS2VCS(const PointGCCS &car_pos, const PointGCCS &target_pos, PointVCS &vcs) {
  double rad = car_pos.angle * M_PI / 180.0;
  double dx = target_pos.xg - car_pos.xg;
  double dy = target_pos.yg - car_pos.yg;
  vcs.x = +dx * cos(rad) + dy * sin(rad);
  vcs.y = -dx * sin(rad) + dy * cos(rad);
  vcs.angle = target_pos.angle - car_pos.angle;
  if (vcs.angle >= 360) vcs.angle -= 360;
  return true;
}

inline bool CoordTransform::DRGCCS2VCS(const PointGCCS &car_pos, const PointGCCS &target_pos, PointVCS &vcs) {
  double rad = car_pos.angle * M_PI / 180.0;
  double dx = target_pos.xg - car_pos.xg;
  double dy = target_pos.yg - car_pos.yg;
  vcs.x = +dx * cos(rad) + dy * sin(rad);
  vcs.y = -dx * sin(rad) + dy * cos(rad);
  vcs.angle = target_pos.angle - car_pos.angle;
  if (vcs.angle >= 360) vcs.angle -= 360;
  return true;
}

inline bool CoordTransform::VCS2GCCS(const Site &car_pos, const Site &vcs, Site &target_pos) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.xg;
  p1.yg = car_pos.yg;
  p1.angle = car_pos.globalangle;
  p3.x = vcs.x;
  p3.y = vcs.y;
  p3.angle = vcs.angle;
  int ret = VCS2GCCS(p1, p3, p2);
  target_pos.xg = p2.xg;
  target_pos.yg = p2.yg;
  target_pos.globalangle = p2.angle;
  target_pos.x = p3.x;
  target_pos.y = p3.y;
  if (ret == -1) return false;
  return true;
}

inline bool CoordTransform::DRVCS2GCCS(const Site &car_pos, const Site &vcs, Site &target_pos) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.xg;
  p1.yg = car_pos.yg;
  p1.angle = car_pos.globalangle;
  p3.x = vcs.x;
  p3.y = vcs.y;
  p3.angle = vcs.angle;
  int ret = VCS2GCCS(p1, p3, p2);
  target_pos.xg = p2.xg;
  target_pos.yg = p2.yg;
  target_pos.globalangle = p2.angle;
  target_pos.x = p3.x;
  target_pos.y = p3.y;
  if (ret == -1) return false;
  return true;
}

inline bool CoordTransform::VCS2GCCS(const LocalizationData &car_pos, const Site &vcs, Site &target_pos) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.xg;
  p1.yg = car_pos.yg;
  p1.angle = car_pos.yaw;
  p3.x = vcs.x;
  p3.y = vcs.y;
  p3.angle = vcs.angle;
  int ret = VCS2GCCS(p1, p3, p2);
  target_pos.xg = p2.xg;
  target_pos.yg = p2.yg;
  target_pos.globalangle = p2.angle;
  target_pos.x = p3.x;
  target_pos.y = p3.y;
  if (ret == -1) return false;
  return true;
}

inline bool CoordTransform::DRVCS2GCCS(const LocalizationData &car_pos, const Site &vcs, Site &target_pos) {
  PointGCCS p1, p2; 
  PointVCS p3;
  p1.xg = car_pos.loc_xg_dr;
  p1.yg = car_pos.loc_yg_dr;
  p1.angle = car_pos.loc_yaw_dr;
  p3.x = vcs.x;
  p3.y = vcs.y;
  p3.angle = vcs.angle;
  int ret = VCS2GCCS(p1, p3, p2);
  target_pos.xg = p2.xg;
  target_pos.yg = p2.yg;
  target_pos.globalangle = p2.angle;
  target_pos.x = p3.x;
  target_pos.y = p3.y;
  if (ret == -1) return false;
  return true;
}

// bool CoordTransform::VCS2GCCS(const PointGCCS &car_pos, const PointVCS &vcs, PointGCCS &target_pos) {
//   int ret = geotool_.VCS2GCCS(car_pos, vcs, target_pos);
//   if (ret == -1) return false;
//   return true;
// }
inline bool CoordTransform::VCS2GCCS(const PointGCCS &car_pos, const PointVCS &vcs, PointGCCS &target_pos) {
  double rad = car_pos.angle * M_PI / 180.0;
  target_pos.xg = vcs.x * cos(rad) - vcs.y * sin(rad);
  target_pos.yg = vcs.x * sin(rad) + vcs.y * cos(rad);
  target_pos.xg += car_pos.xg;
  target_pos.yg += car_pos.yg;
  target_pos.angle = NormalizeAngle(car_pos.angle + vcs.angle);
  if (target_pos.angle >= 360) target_pos.angle -= 360;
  return true;
}

inline bool CoordTransform::DRVCS2GCCS(const PointGCCS &car_pos, const PointVCS &vcs, PointGCCS &target_pos) {
  double rad = car_pos.angle * M_PI / 180.0;
  target_pos.xg = vcs.x * cos(rad) - vcs.y * sin(rad);
  target_pos.yg = vcs.x * sin(rad) + vcs.y * cos(rad);
  target_pos.xg += car_pos.xg;
  target_pos.yg += car_pos.yg;
  target_pos.angle = NormalizeAngle(car_pos.angle + vcs.angle);
  if (target_pos.angle >= 360) target_pos.angle -= 360;
  return true;
}

inline double CoordTransform::NormalizeAngle(const double heading) {
  double a = std::fmod(heading + 180.0, 2.0 * 180.0);
  if (a < 0.0) {
    a += 2.0 * 180.0;
  }
  return a - 180.0;
}

} // namespace planning
} // namespace avos
