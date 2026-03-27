#include "perception_fusion_common/common/coordinate_util.h"

namespace avos {
namespace perception {
namespace algorithm {

// By default, the ZYX axis order will be used
// 0 1 2 对应 z y x 对应 yaw pitch roll
// utm-coords
Eigen::Vector3d ConvertPointVCS2UTM(const LocationType &locpos,
                                    const Eigen::Vector3d &pos_vcs) {
  Eigen::Vector3d own_utm_pos;
  Eigen::Matrix3d own_utm_rotation;
  Eigen::Vector3d own_utm_rot(locpos.yaw * M_PI / 180.0,
                              locpos.pitch * M_PI / 180.0,
                              locpos.roll * M_PI / 180.0);
  own_utm_rotation = Eigen::AngleAxisd(own_utm_rot[0], Eigen::Vector3d::UnitZ()) *
                     Eigen::AngleAxisd(own_utm_rot[1], Eigen::Vector3d::UnitY()) *
                     Eigen::AngleAxisd(own_utm_rot[2], Eigen::Vector3d::UnitX());
  own_utm_pos << locpos.x, locpos.y, locpos.z;
  auto pos_utm = own_utm_rotation * pos_vcs + own_utm_pos;
  return pos_utm;
}

// utm-coords
Eigen::Vector3d ConvertPointUTM2VCS(const LocationType &locpos,
                                    const Eigen::Vector3d &pos_utm) {
  Eigen::Vector3d own_utm_pos;
  Eigen::Matrix3d own_utm_rotation, own_utm_rotation_inverse;
  Eigen::Vector3d own_utm_rot(locpos.yaw * M_PI / 180.0,
                              locpos.pitch * M_PI / 180.0,
                              locpos.roll * M_PI / 180.0);
  own_utm_rotation = Eigen::AngleAxisd(own_utm_rot[0], Eigen::Vector3d::UnitZ()) *
                     Eigen::AngleAxisd(own_utm_rot[1], Eigen::Vector3d::UnitY()) *
                     Eigen::AngleAxisd(own_utm_rot[2], Eigen::Vector3d::UnitX());
  own_utm_rotation_inverse = own_utm_rotation.inverse();
  own_utm_pos << locpos.x, locpos.y, locpos.z;
  auto pos_vcs = own_utm_rotation_inverse * (pos_utm - own_utm_pos);
  return pos_vcs;
}

// dr-coords
Eigen::Vector3d ConvertPointVCS2DR(const LocationType &locpos_dr,
                                   const Eigen::Vector3d &pos_vcs) {
  Eigen::Vector3d own_dr_pos;
  Eigen::Matrix3d own_dr_rotation;
  Eigen::Vector3d own_dr_rot(locpos_dr.yaw * M_PI / 180.0,
                             locpos_dr.pitch * M_PI / 180.0,
                             locpos_dr.roll * M_PI / 180.0);
  own_dr_rotation = Eigen::AngleAxisd(own_dr_rot[0], Eigen::Vector3d::UnitZ()) *
                    Eigen::AngleAxisd(own_dr_rot[1], Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(own_dr_rot[2], Eigen::Vector3d::UnitX());
  own_dr_pos << locpos_dr.x, locpos_dr.y, locpos_dr.z;
  auto pos_dr = own_dr_rotation * pos_vcs + own_dr_pos;
  return pos_dr;
}

// dr-coords
Eigen::Vector3d ConvertPointDR2VCS(const LocationType &locpos_dr,
                                   const Eigen::Vector3d &pos_dr) {
  Eigen::Vector3d own_dr_pos;
  Eigen::Matrix3d own_dr_rotation, own_dr_rotation_inverse;
  Eigen::Vector3d own_dr_rot(locpos_dr.yaw * M_PI / 180.0,
                             locpos_dr.pitch * M_PI / 180.0,
                             locpos_dr.roll * M_PI / 180.0);
  own_dr_rotation = Eigen::AngleAxisd(own_dr_rot[0], Eigen::Vector3d::UnitZ()) *
                    Eigen::AngleAxisd(own_dr_rot[1], Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(own_dr_rot[2], Eigen::Vector3d::UnitX());
  own_dr_rotation_inverse = own_dr_rotation.inverse();
  own_dr_pos << locpos_dr.x, locpos_dr.y, locpos_dr.z;
  auto pos_vcs = own_dr_rotation_inverse * (pos_dr - own_dr_pos);
  return pos_vcs;
}

Eigen::Vector3d ConvertPointVCS2UTM(const PerceptionLocType &locpos,
                                    const Eigen::Vector3d &pos_vcs)
                                    {
                                      LocationType _loc_gl = ConvertLocationStruct2Global(locpos);
                                      return ConvertPointVCS2UTM(_loc_gl,pos_vcs);
                                    }

Eigen::Vector3d ConvertPointUTM2VCS(const PerceptionLocType &locpos,
                                    const Eigen::Vector3d &pos_utm)
                                    {
                                      LocationType _loc_gl = ConvertLocationStruct2Global(locpos);
                                      return ConvertPointUTM2VCS(_loc_gl,pos_utm);
                                    }

Eigen::Vector3d ConvertPointVCS2DR(const PerceptionLocType &locpos,
                                   const Eigen::Vector3d &pos_vcs)
                                   {
                                     LocationType _loc_dr = ConvertLocationStruct2Dr(locpos);
                                     return ConvertPointVCS2DR(_loc_dr,pos_vcs);
                                   }

Eigen::Vector3d ConvertPointDR2VCS(const PerceptionLocType &locpos,
                                   const Eigen::Vector3d &pos_dr)
                                   {
                                    LocationType _loc_dr = ConvertLocationStruct2Dr(locpos);
                                     return ConvertPointDR2VCS(_loc_dr,pos_dr);
                                   }
                                   
LocationType  ConvertLocationStruct2Dr(PerceptionLocType loc)
{
  LocationType loc_dr;
  loc_dr.x = loc.dr_x;
  loc_dr.y = loc.dr_y;
  loc_dr.z = loc.dr_z;
  loc_dr.yaw = loc.dr_yaw;
  loc_dr.pitch = loc.dr_pitch;
  loc_dr.roll = loc.dr_roll;
  loc_dr.trigger_stamp = loc.dr_time;

  return loc_dr;
}
LocationType  ConvertLocationStruct2Global(PerceptionLocType loc)
{
  LocationType loc_global;
  loc_global.x = loc.xg;
  loc_global.y = loc.yg;
  loc_global.z = loc.zg;
  loc_global.yaw = loc.angle;
  loc_global.pitch = loc.pitch;
  loc_global.roll = loc.roll;
  loc_global.speed = loc.speed;
  loc_global.yawrate = loc.yawrate;
  loc_global.trigger_stamp = loc.trigger_stamp;
  return loc_global;
}
// 将VCS坐标系下的速度转换到utm坐标系下
Eigen::Vector3d ConvertVelocityVCS2UTM(const PerceptionLocType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_vcs_vel) {
  Eigen::Matrix3d own_utm_rotation;
  Eigen::Vector3d own_utm_rot(locpos.angle * M_PI / 180.0,
                             locpos.pitch * M_PI / 180.0,
                             locpos.roll * M_PI / 180.0);
  own_utm_rotation = Eigen::AngleAxisd(own_utm_rot[0], Eigen::Vector3d::UnitZ()) *
                    Eigen::AngleAxisd(own_utm_rot[1], Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(own_utm_rot[2], Eigen::Vector3d::UnitX());
  Eigen::Vector3d own_utm_vel(locpos.speed - locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[1],
                             locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[0],
                             0);
  auto obj_utm_vel = own_utm_rotation * (obj_vcs_vel + own_utm_vel);
  return obj_utm_vel;
}
// 将VCS坐标系下的速度转换到DR坐标系下
Eigen::Vector3d ConvertVelocityVCS2DR(const PerceptionLocType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_vcs_vel) {
  Eigen::Matrix3d own_dr_rotation;
  Eigen::Vector3d own_dr_rot(locpos.dr_yaw * M_PI / 180.0,
                             locpos.dr_pitch * M_PI / 180.0,
                             locpos.dr_roll * M_PI / 180.0);
  own_dr_rotation = Eigen::AngleAxisd(own_dr_rot[0], Eigen::Vector3d::UnitZ()) *
                    Eigen::AngleAxisd(own_dr_rot[1], Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(own_dr_rot[2], Eigen::Vector3d::UnitX());
  Eigen::Vector3d own_dr_vel(locpos.speed - locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[1],
                             locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[0],
                             0);
  auto obj_dr_vel = own_dr_rotation * (obj_vcs_vel + own_dr_vel);
  return obj_dr_vel;
}

// 将VCS坐标系下的速度转换到DR坐标系下
Eigen::Vector3d ConvertVelocityVCS2DR(const LocationType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_vcs_vel) {
  Eigen::Matrix3d own_dr_rotation;
  Eigen::Vector3d own_dr_rot(locpos.yaw * M_PI / 180.0,
                             locpos.pitch * M_PI / 180.0,
                             locpos.roll * M_PI / 180.0);
  own_dr_rotation = Eigen::AngleAxisd(own_dr_rot[0], Eigen::Vector3d::UnitZ()) *
                    Eigen::AngleAxisd(own_dr_rot[1], Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(own_dr_rot[2], Eigen::Vector3d::UnitX());
  Eigen::Vector3d own_dr_vel(locpos.speed - locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[1],
                             locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[0],
                             0);
  auto obj_dr_vel = own_dr_rotation * (obj_vcs_vel + own_dr_vel);
  return obj_dr_vel;
}


// 将DR坐标系下的速度转换到VCS坐标系下
Eigen::Vector3d ConvertVelocityDR2VCS(const PerceptionLocType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_dr_vel) {
  Eigen::Matrix3d own_dr_rotation, own_dr_rotation_inverse;
  Eigen::Vector3d own_dr_rot(locpos.dr_yaw * M_PI / 180.0,
                             locpos.dr_pitch * M_PI / 180.0,
                             locpos.dr_roll * M_PI / 180.0);
  own_dr_rotation = Eigen::AngleAxisd(own_dr_rot[0], Eigen::Vector3d::UnitZ()) *
                    Eigen::AngleAxisd(own_dr_rot[1], Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(own_dr_rot[2], Eigen::Vector3d::UnitX());
  own_dr_rotation_inverse = own_dr_rotation.inverse();
  Eigen::Vector3d own_dr_vel(locpos.speed - locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[1],
                             locpos.yawrate * M_PI / 180.0 * obj_vcs_pos[0],
                             0);
  auto obj_vcs_vel = own_dr_rotation_inverse * obj_dr_vel - own_dr_vel;
  return obj_vcs_vel;
}

}  // namespace algorithm
}  // namespace perception
}  // namespace avos
