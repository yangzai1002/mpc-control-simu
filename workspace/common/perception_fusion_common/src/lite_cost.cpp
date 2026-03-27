#include "log/log.h"
#include "perception_fusion_common/Xlidar_mot/matching/lite_cost.h"
namespace avos {
namespace perception {
namespace xlidarMOT{
LiteCost::LiteCost()
{
}
LiteCost::~LiteCost()
{
}
int LiteCost::Init(CostConfigType config)
{
  config_ = config;
  return 0;
}
std::string LiteCost::Name() {
  return "Lite Cost Method.";
}

bool IsTopBottomReversed(const bool& trk_is_top_bottom_reversed,
                         const float& trk_heading_vcs,
                         const float& det_heading_vcs) {
  bool is_top_bottom_reversed = false;
  float norm_trk = trk_heading_vcs >= 0 ? trk_heading_vcs : (trk_heading_vcs + 180);
  float norm_det = det_heading_vcs >= 0 ? det_heading_vcs : (det_heading_vcs + 180);
  if (norm_trk > 75 && norm_trk < 90
      && norm_det > 90 && norm_det < 105
      && fabs(norm_det - norm_trk) < 30) {
    is_top_bottom_reversed = true;
  }
  if (norm_det > 75 && norm_det < 90
      && norm_trk > 90 && norm_trk < 105
      && fabs(norm_trk - norm_det) < 30) {
    is_top_bottom_reversed = true;
  }

  if (trk_is_top_bottom_reversed == false && is_top_bottom_reversed == true) {
    return true;
  } else if (trk_is_top_bottom_reversed == true && is_top_bottom_reversed == false) {
    return true;
  } else {
    return false;
  }
}

int LiteCost::Cost(avos::perception::xfusion_new::Matrix<double> &mat,
                   const std::vector<FusionTracker> &tracks,
                   const std::vector<ObjectDetection_> &dets)
{
  if (dets.size() <= 0 || tracks.size() <= 0) {
    AINFO  << "return 1!";
    return 1;
  }
  mat = avos::perception::xfusion_new::Matrix<double>(tracks.size(), dets.size());
  CostParamType* c_p = nullptr;
  for (int i = 0; i < tracks.size(); i++) {
    // AINFO  << "[" << tracks[i].trk.id << "] " << tracks[i].trk.x << "," << tracks[i].trk.y << "," << tracks[i].trk.length;

    for (int j = 0; j < dets.size(); j++) {
      switch (dets[j].source & SOURCE_MASK) {
      case (LIDAR_SOURCE): {
        c_p = &(config_.cost_lidar);
      } break;
      case (RADAR_SOURCE): {
        c_p = &(config_.cost_radar);
      } break;
      case (VISION_SOURCE): {
        c_p = &(config_.cost_vision);
      } break;
      default: {
        c_p = &(config_.cost_vision);
      }
      }

      // get detection infos under matching frame
      double x_det = dets[j].x_dr;
      double y_det = dets[j].y_dr;
      float vx_det = dets[j].vx_dr;
      float vy_det = dets[j].vy_dr;
      float width_det = dets[j].width;
      if ((dets[j].source & SOURCE_MASK) == LIDAR_SOURCE) {
        if (tracks[i].trk.fuse_point == FUSE_POINT_TOP) {
          x_det = dets[j].lidar_bbox_dr.geoPoints[TOP_CENTER].x;
          y_det = dets[j].lidar_bbox_dr.geoPoints[TOP_CENTER].y;
        } else if (tracks[i].trk.fuse_point == FUSE_POINT_BOTTOM) {
          x_det = dets[j].lidar_bbox_dr.geoPoints[BOTTOM_CENTER].x;
          y_det = dets[j].lidar_bbox_dr.geoPoints[BOTTOM_CENTER].y;
        }
        // check heading near 90deg
        if (IsTopBottomReversed(tracks[i].trk.is_top_bottom_reversed,
                                tracks[i].trk.heading_vcs,
                                dets[j].heading_vcs)) {
          if (tracks[i].trk.fuse_point == FUSE_POINT_TOP) {
            x_det = dets[j].lidar_bbox_dr.geoPoints[BOTTOM_CENTER].x;
            y_det = dets[j].lidar_bbox_dr.geoPoints[BOTTOM_CENTER].y;
          } else if (tracks[i].trk.fuse_point == FUSE_POINT_BOTTOM) {
            x_det = dets[j].lidar_bbox_dr.geoPoints[TOP_CENTER].x;
            y_det = dets[j].lidar_bbox_dr.geoPoints[TOP_CENTER].y;
          }
        }
      }

      // get track infos under matching frame
      double x_trk = tracks[i].trk.x;
      double y_trk = tracks[i].trk.y;
      float vx_trk = tracks[i].trk.vx;
      float vy_trk = tracks[i].trk.vy;
      float width_trk = tracks[i].trk.width;

      double delta_x = x_det - x_trk;
      double delta_y = y_det - y_trk;
    // if ((dets[j].source & SOURCE_MASK) == LIDAR_SOURCE) {
        // AINFO << " radar v reset ";
        // vx_det = 0;
        // vy_det = 0;
        // vx_trk = 0;
        // vy_trk = 0;
    // }
#if 0
      float theta = tracks[i].trk.heading / 180.0 * 3.1415926;
      double delta_lon = delta_x * cos(theta) + delta_y * sin(theta);
      double delta_lat = -delta_x * sin(theta) + delta_y * cos(theta);
      // calculate cost matrix
      auto trk_type = tracks[i].trk.type;
      if (trk_type == CAR_TYPE || trk_type == TRUCK_TYPE) {
        mat(i, j) =
          c_p->cost_x * pow((delta_lon), 2) +
          2.0 * c_p->cost_y * pow((delta_lat), 2) +
          c_p->cost_vx * pow((vx_det - vx_trk), 2) +
          c_p->cost_vy * pow((vy_det - vy_trk), 2);
        // + c_p->cost_width * fabs(width_det - width_trk);
      } else {
        mat(i, j) =
          c_p->cost_x * pow((delta_x), 2) +
          c_p->cost_y * pow((delta_y), 2) +
          c_p->cost_vx * pow((vx_det - vx_trk), 2) +
          c_p->cost_vy * pow((vy_det - vy_trk), 2);
        // + c_p->cost_width * fabs(width_det - width_trk);
      }
#else
      mat(i, j) =
        c_p->cost_x * pow((delta_x), 2) +
        c_p->cost_y * pow((delta_y), 2) +
        c_p->cost_vx * pow((vx_det - vx_trk), 2) +
        c_p->cost_vy * pow((vy_det - vy_trk), 2);
      // + c_p->cost_width * fabs(width_det - width_trk);
#endif
      // if ((dets[j].source & SOURCE_MASK) == LIDAR_SOURCE) {
      //   if (tracks[i].trk.type != dets[j].type) {
      //     mat(i, j) += 10;
      //   }
      // }
      if (mat(i, j) > 25) {
        mat(i, j) = 100;
      }
      // AINFO  << "[ " << x_det << ", " << y_det << "] " << mat(i, j) << ", ";
    }
    // AINFO ;
  }
  return 0;
}

bool LiteCost::Gating(const FusionTracker & track,
                      const ObjectDetection_ & det) {
  auto trk_type = track.trk.type;
  float ratio = 1.0;
  GateParamType* gate = nullptr;
  switch (det.source & SOURCE_MASK) {
  case (LIDAR_SOURCE): {
    gate = &(config_.gate_lidar);
    if (trk_type == PEDESTRIAN_TYPE) {
      ratio = 0.6; // 3m * 0.6 = 1.8m
    } else if (trk_type == CYCLIST_TYPE) {
      ratio = 0.8; // 3m * 0.8 = 2.4m
    } else if (trk_type == UNKNOWN_TYPE) {
      ratio = 0.8;
      // more dirty tricks
      if (det.type == UNKNOWN_TYPE && fabs(det.x_vcs) >= 60 && fabs(det.x_vcs) <= 70) {
        ratio = 1.5;
      }
    }
  } break;
  case (RADAR_SOURCE): {
    gate = &(config_.gate_radar);
    if (trk_type == PEDESTRIAN_TYPE || trk_type == CYCLIST_TYPE) {
      ratio = 0.2;
    } else if (trk_type == UNKNOWN_TYPE) {
      ratio = 0.0;
    }
  } break;
  default: { //vision:
    gate = &(config_.gate_vision);
  }
  }

  // get detection infos under matching frame
  double x_det = det.x_dr;
  double y_det = det.y_dr;
  float vx_det = det.vx_dr;
  float vy_det = det.vy_dr;
  if ((det.source & SOURCE_MASK) == LIDAR_SOURCE) {
    if (track.trk.fuse_point == FUSE_POINT_TOP) {
      x_det = det.lidar_bbox_dr.geoPoints[TOP_CENTER].x;
      y_det = det.lidar_bbox_dr.geoPoints[TOP_CENTER].y;
    } else if (track.trk.fuse_point == FUSE_POINT_BOTTOM) {
      x_det = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].x;
      y_det = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].y;
    }
    // check heading near 90deg
    if (IsTopBottomReversed(track.trk.is_top_bottom_reversed,
                            track.trk.heading_vcs,
                            det.heading_vcs)) {
      if (track.trk.fuse_point == FUSE_POINT_TOP) {
        x_det = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].x;
        y_det = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].y;
      } else if (track.trk.fuse_point == FUSE_POINT_BOTTOM) {
        x_det = det.lidar_bbox_dr.geoPoints[TOP_CENTER].x;
        y_det = det.lidar_bbox_dr.geoPoints[TOP_CENTER].y;
      }
    }
  }
  float bbox_area_det = det.width * det.length;

  // get track infos under matching frame
  double x_trk = track.trk.x;
  double y_trk = track.trk.y;
  float vx_trk = track.trk.vx;
  float vy_trk = track.trk.vy;
  float bbox_area_trk = track.trk.width * track.trk.length;

  double delta_x = x_det - x_trk;
  double delta_y = y_det - y_trk;

  float theta = track.trk.heading / 180.0 * 3.1415926;
  double delta_lon = delta_x * cos(theta) + delta_y * sin(theta);
  double delta_lat = -delta_x * sin(theta) + delta_y * cos(theta);

  // AINFO  << "zr-debug-gate: " << x_det << "," << y_det << " | "
  //           << track.trk.id << "," << track.trk.age << " | " << x_trk << "," << y_trk << " | " << vx_trk << " | " << vy_trk << ", "
  //           << ratio << " | " << gate->gate_x << " | " << gate->gate_y  << " | " << gate->gate_vx  << " | " << gate->gate_vy
  //          ;

  if (fabs(delta_x) > gate->gate_x * ratio || fabs(delta_y) > gate->gate_y * ratio) {
    return false;
  }

  if ((trk_type == CAR_TYPE || trk_type == TRUCK_TYPE)
      && (fabs(delta_lon) > 4.0 || fabs(delta_lat) > 1.5)
      && track.trk.length <= 15) {
    // AINFO  << "zr-debug-gate: id = " << track.trk.id
    //           << ", trk_heading_vcs = " << track.trk.heading_vcs
    //           << ", trk_heading_dr = " << track.trk.heading
    //           << ", delta_x = " << delta_x << ", delta_y = " << delta_y
    //           << ", delta_lon = " << delta_lon << ", delta_lat = " << delta_lat
    //          ;
    return false;
  }

  if (track.trk.age >= 3 && (det.source & SOURCE_MASK) == RADAR_SOURCE &&
      (fabs(vx_det - vx_trk) > gate->gate_vx)
      || fabs(vy_det - vy_trk) > gate->gate_vy) {
    return false;
  }

  // float bbox_ratio = bbox_area_det / bbox_area_trk;
  // if ((det.source & SOURCE_MASK) == LIDAR_SOURCE && (bbox_ratio >= 4.0 || bbox_ratio < 0.25)) {
  //   return false;
  // }

  // 2021-06-09, zr-add
  // rule out incorrect type correlation
  if ((trk_type == CAR_TYPE || trk_type == TRUCK_TYPE)
      && (det.type == PEDESTRIAN_TYPE || det.type == UNKNOWN_UNMOVABLE_TYPE ||
	  det.type == SUBULATE_TYPE || det.type == ROADBLOCK_TYPE || det.type == ANTITANK_TYPE)
      || (det.type == CAR_TYPE || det.type == TRUCK_TYPE)
      && (trk_type == PEDESTRIAN_TYPE || trk_type == UNKNOWN_UNMOVABLE_TYPE ||
	  trk_type == SUBULATE_TYPE || trk_type == ROADBLOCK_TYPE || trk_type == ANTITANK_TYPE)) {
    return false;
  }

  return true;
}

}
}//namespace avos
}//namespace perception
