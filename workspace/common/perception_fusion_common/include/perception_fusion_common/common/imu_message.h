#ifndef _IMU_MSG_H_
#define _IMU_MSG_H_

#include <string>
#include <vector>
#include <math.h>
#include "Eigen/Core"
#include "Eigen/Dense"


namespace avos {
namespace perception {





typedef struct LocationType {
  double trigger_stamp = 0.0;
  double time = 0.0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double roll = 0.0;
  double pitch = 0.0;
  double yaw = 0.0;
  double yawrate = 0.0;
  double speed = 0.0;
  // float vx = 0.0;  // lifan 24_03_04:定位转发用（历史遗留）
  // float vy = 0.0;  // lifan 24_03_04:定位转发用（历史遗留）
  Eigen::Vector3d ego_t = Eigen::Vector3d(0,0,0);
  Eigen::Matrix3d ego_R = Eigen::Matrix3d::Identity();
  Eigen::Matrix3d ego_R_Inverse = Eigen::Matrix3d::Identity();
} LocationType;


struct PerceptionLocType {
  double trigger_stamp = 0.0;
  double xg = 0.0;
  double yg = 0.0;
  double zg = 0.0;
  double yawrate = 0.0;
  float angle = 0.0;
  float pitch = 0.0;
  float roll = 0.0;
  float vx = 0.0;
  float vy = 0.0;
  float accx = 0.0;
  float accy = 0.0;
  float speed = 0.0;
  Eigen::Vector3d ego_t_g = Eigen::Vector3d(0,0,0);
  Eigen::Matrix3d ego_R_g = Eigen::Matrix3d::Identity();
  Eigen::Matrix3d ego_R_g_Inverse = Eigen::Matrix3d::Identity();

  double dr_time;
  double dr_x = 0.0;
  double dr_y = 0.0;
  double dr_z = 0.0;
  float dr_roll = 0.0;
  float dr_pitch = 0.0;
  float dr_yaw = 0.0;
  Eigen::Vector3d ego_t = Eigen::Vector3d(0,0,0);
  Eigen::Matrix3d ego_R = Eigen::Matrix3d::Identity();
  Eigen::Matrix3d ego_R_Inverse = Eigen::Matrix3d::Identity();
};

typedef struct LocTrans {
  PerceptionLocType locpos_, prevlocpos_;
  Eigen::Vector3d own_dr_pos_;
  Eigen::Matrix3d own_dr_rotation_;
  Eigen::Matrix3d own_dr_rotation_inverse_;
  Eigen::Vector3d own_dr_vel_;
} LocTrans;


}  // namespace perception
}  // namespace avos
#endif
