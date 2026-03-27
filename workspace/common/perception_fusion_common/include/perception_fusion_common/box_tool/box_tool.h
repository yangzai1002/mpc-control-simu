#pragma once
#include <opencv2/core.hpp>
#include <Eigen/Eigen>

#include "perception_fusion_common/Xlidar_mot/common/fusion_type.h"
#include "PerceptionObject.h"
namespace avos {
namespace perception {

static void GetCornerPoints(const ara::adsfi::IdpPerceptionObject & obs,std::vector<Eigen::Vector3f> &gout_geo_bbox)
{
  // std::vector<Eigen::Vector3f> gout_geo_bbox;
  gout_geo_bbox.resize(10);
  Eigen::Vector3f min_pt(-obs.obj_size.length / 2.0, -obs.obj_size.width / 2.0, -obs.obj_size.height / 2.0);
  Eigen::Vector3f max_pt(obs.obj_size.length / 2.0, obs.obj_size.width / 2.0, obs.obj_size.height / 2.0);
  gout_geo_bbox[0] = Eigen::Vector3f(min_pt[0], min_pt[1], min_pt[2]);
  gout_geo_bbox[1] = Eigen::Vector3f(min_pt[0], min_pt[1], max_pt[2]);
  gout_geo_bbox[2] = Eigen::Vector3f(max_pt[0], min_pt[1], min_pt[2]);
  gout_geo_bbox[3] = Eigen::Vector3f(max_pt[0], min_pt[1], max_pt[2]);
  gout_geo_bbox[4] = Eigen::Vector3f(max_pt[0], max_pt[1], min_pt[2]);
  gout_geo_bbox[5] = Eigen::Vector3f(max_pt[0], max_pt[1], max_pt[2]);
  gout_geo_bbox[6] = Eigen::Vector3f(min_pt[0], max_pt[1], min_pt[2]);
  gout_geo_bbox[7] = Eigen::Vector3f(min_pt[0], max_pt[1], max_pt[2]);
  gout_geo_bbox[8] = Eigen::Vector3f(0, 0, min_pt[2]);
  gout_geo_bbox[9] = Eigen::Vector3f(obs.obj_size.length / 2.0, 0, min_pt[2]);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project = Eigen::AngleAxis<float>(obs.vcs_info.heading * M_PI / 180.0, Eigen::Matrix<float, 3, 1>::UnitZ()).matrix();
  for (size_t i = 0; i < gout_geo_bbox.size(); ++i)
  {
    gout_geo_bbox[i] = project * gout_geo_bbox[i];
    gout_geo_bbox[i][0] = gout_geo_bbox[i][0] + obs.vcs_info.x;
    gout_geo_bbox[i][1] = gout_geo_bbox[i][1] + obs.vcs_info.y;
    gout_geo_bbox[i][2] = gout_geo_bbox[i][2] + obs.vcs_info.z;//
  }

  // return gout_geo_bbox;
}
static void GetCornerPoints(const xlidarMOT::FusionObject_& obs,std::vector<Eigen::Vector3f> &gout_geo_bbox)
{
  // std::vector<Eigen::Vector3f> gout_geo_bbox;
  gout_geo_bbox.resize(10);
  Eigen::Vector3f min_pt(-obs.length / 2.0, -obs.width / 2.0, -obs.height / 2.0);
  Eigen::Vector3f max_pt(obs.length / 2.0, obs.width / 2.0, obs.height / 2.0);
  gout_geo_bbox[0] = Eigen::Vector3f(min_pt[0], min_pt[1], min_pt[2]);
  gout_geo_bbox[1] = Eigen::Vector3f(min_pt[0], min_pt[1], max_pt[2]);
  gout_geo_bbox[2] = Eigen::Vector3f(max_pt[0], min_pt[1], min_pt[2]);
  gout_geo_bbox[3] = Eigen::Vector3f(max_pt[0], min_pt[1], max_pt[2]);
  gout_geo_bbox[4] = Eigen::Vector3f(max_pt[0], max_pt[1], min_pt[2]);
  gout_geo_bbox[5] = Eigen::Vector3f(max_pt[0], max_pt[1], max_pt[2]);
  gout_geo_bbox[6] = Eigen::Vector3f(min_pt[0], max_pt[1], min_pt[2]);
  gout_geo_bbox[7] = Eigen::Vector3f(min_pt[0], max_pt[1], max_pt[2]);
  gout_geo_bbox[8] = Eigen::Vector3f(0, 0, min_pt[2]);
  gout_geo_bbox[9] = Eigen::Vector3f(obs.length / 2.0, 0, min_pt[2]);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project = Eigen::AngleAxis<float>(obs.heading * M_PI / 180.0, Eigen::Matrix<float, 3, 1>::UnitZ()).matrix();
  for (size_t i = 0; i < gout_geo_bbox.size(); ++i)
  {
    gout_geo_bbox[i] = project * gout_geo_bbox[i];
    gout_geo_bbox[i][0] = gout_geo_bbox[i][0] + obs.x;
    gout_geo_bbox[i][1] = gout_geo_bbox[i][1] + obs.y;
    gout_geo_bbox[i][2] = gout_geo_bbox[i][2] + obs.z;//
  }

  // return gout_geo_bbox;
}
}
}
