#include "log/log.h"
#include "perception_fusion_common/Xlidar_mot/geometry/bbox_calculator.h"

namespace avos {
namespace perception {
namespace xlidarMOT{
BBoxCalculator::BBoxCalculator() {
}

BBoxCalculator::~BBoxCalculator() {

}

float normalize(const float angle) {
  if (angle > 0) {return (angle - 180);}
  else {return (angle + 180);}
}
std::vector<GeoPoint> BBoxCalculator::GetLidarBoundingBox(const double x, const double y,
    const float length, const float width, const float heading) {
  std::vector<Eigen::Vector3f> points;
  points.resize(9);
  Eigen::Vector2f min_pt(-length / 2.0, -width / 2.0);
  Eigen::Vector2f max_pt(length / 2.0, width / 2.0);
  points[0] = Eigen::Vector3f(max_pt[0], max_pt[1], 0);
  points[1] = Eigen::Vector3f(max_pt[0], 0,         0);
  points[2] = Eigen::Vector3f(max_pt[0], min_pt[1], 0);
  points[3] = Eigen::Vector3f(0, max_pt[1], 0);
  points[4] = Eigen::Vector3f(0, 0,         0);
  points[5] = Eigen::Vector3f(0, min_pt[1], 0);
  points[6] = Eigen::Vector3f(min_pt[0], max_pt[1], 0);
  points[7] = Eigen::Vector3f(min_pt[0], 0,         0);
  points[8] = Eigen::Vector3f(min_pt[0], min_pt[1], 0);

  Eigen::Vector3f geo_center;
  geo_center = Eigen::Vector3f(x, y, 0);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project =
    Eigen::AngleAxis<float>(normalize(heading) * 3.1415926 / 180.0, Eigen::Matrix<float, 3, 1>::UnitZ())
    .matrix();
  for (size_t i = 0; i < points.size(); ++i) {
    points[i] = project * points[i];
    points[i] = points[i] + geo_center;
  }

  std::vector<GeoPoint> lidar_bbox;
  lidar_bbox.resize(9);
  for (int i = 0; i < 9; i++) {
    lidar_bbox[i].isvalid = true;
    lidar_bbox[i].x = points[i].x();
    lidar_bbox[i].y = points[i].y();
  }

  // deal with heading inconsistancy
  if (points[BOTTOM_CENTER].x() > points[TOP_CENTER].x()) {
    lidar_bbox[TOP_CENTER].x = points[BOTTOM_CENTER].x();
    lidar_bbox[TOP_CENTER].y = points[BOTTOM_CENTER].y();
    lidar_bbox[TOP_LEFT].x = points[BOTTOM_LEFT].x();
    lidar_bbox[TOP_LEFT].y = points[BOTTOM_LEFT].y();
    lidar_bbox[TOP_RIGHT].x = points[BOTTOM_RIGHT].x();
    lidar_bbox[TOP_RIGHT].y = points[BOTTOM_RIGHT].y();
    lidar_bbox[BOTTOM_CENTER].x = points[TOP_CENTER].x();
    lidar_bbox[BOTTOM_CENTER].y = points[TOP_CENTER].y();
    lidar_bbox[BOTTOM_LEFT].x = points[TOP_LEFT].x();
    lidar_bbox[BOTTOM_LEFT].y = points[TOP_LEFT].y();
    lidar_bbox[BOTTOM_RIGHT].x = points[TOP_RIGHT].x();
    lidar_bbox[BOTTOM_RIGHT].y = points[TOP_RIGHT].y();
  }

  return lidar_bbox;
}

bool BBoxCalculator::IsCarTruck(const int &det_type, const int &trk_type) {
  if (det_type == CAR_TYPE || det_type == TRUCK_TYPE 
      || trk_type == CAR_TYPE || trk_type == TRUCK_TYPE ) {
    return true;
  }
  return false;
}

void BBoxCalculator::GetAnchorPoint(const int prev_fuse_point, const int trk_type, const float trk_heading_vcs,
                                    const ObjectDetection_ &det, const bool is_top_bottom_reversed,
                                    int &fuse_point, double &x, double &y) {
  fuse_point = FUSE_POINT_CENTER;
  if (IsCarTruck(det.type, trk_type)) {
    if ((det.heading_vcs >= -60 && det.heading_vcs <= 60)
        || (det.heading_vcs >= -180 && det.heading_vcs <= -120)
        || (det.heading_vcs >= 120 && det.heading_vcs <= 180)) {
      auto x_boundary = det.x_vcs;
      if (prev_fuse_point == FUSE_POINT_CENTER) {
        if (x_boundary > 2) {
          fuse_point = FUSE_POINT_BOTTOM;
        } else if (x_boundary < -2) {
          fuse_point = FUSE_POINT_TOP;
        }
      } else if (prev_fuse_point == FUSE_POINT_TOP) {
        if (x_boundary <= 2) {
          fuse_point = FUSE_POINT_TOP;
        } else {
          fuse_point = FUSE_POINT_BOTTOM;
        }
      } else if (prev_fuse_point == FUSE_POINT_BOTTOM) {
        if (x_boundary >= -2) {
          fuse_point = FUSE_POINT_BOTTOM;
        } else {
          fuse_point = FUSE_POINT_TOP;
        }
      }
    } else {
      float d = fabs(trk_heading_vcs - det.heading_vcs);
      if (d >= 360) {d -= 360;}
      // latch the fuse point
      if (d <= 15 || d >= 165 && d <= 195) {
        fuse_point = prev_fuse_point;
      }

      if (fuse_point == FUSE_POINT_CENTER) {
        // check y
        auto y_top_center = det.lidar_bbox_vcs.geoPoints[TOP_CENTER].y;
        auto y_bottom_center = det.lidar_bbox_vcs.geoPoints[BOTTOM_CENTER].y;
        if (fabs(y_top_center) < fabs(y_bottom_center)) {
          fuse_point = FUSE_POINT_TOP;
        } else {
          fuse_point = FUSE_POINT_BOTTOM;
        }
      }
    } // end if
  }

  if (det.type == UNKNOWN_TYPE) {
    fuse_point = prev_fuse_point;
  }

  // get anchor point position
  x = det.lidar_bbox_dr.geoPoints[CENTER].x;
  y = det.lidar_bbox_dr.geoPoints[CENTER].y;
  if (det.type == CAR_TYPE || det.type == TRUCK_TYPE
      || trk_type == CAR_TYPE || trk_type == TRUCK_TYPE) {
    if (fuse_point == FUSE_POINT_BOTTOM) {
      x = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].x;
      y = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].y;
    } else if (fuse_point == FUSE_POINT_TOP) {
      x = det.lidar_bbox_dr.geoPoints[TOP_CENTER].x;
      y = det.lidar_bbox_dr.geoPoints[TOP_CENTER].y;
    }
    if (is_top_bottom_reversed == true) {
      if (fuse_point == FUSE_POINT_BOTTOM) {
        x = det.lidar_bbox_dr.geoPoints[TOP_CENTER].x;
        y = det.lidar_bbox_dr.geoPoints[TOP_CENTER].y;
      } else if (fuse_point == FUSE_POINT_TOP) {
        x = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].x;
        y = det.lidar_bbox_dr.geoPoints[BOTTOM_CENTER].y;
      }
    }
  }
}


void BBoxCalculator::UpdateBBoxBuffer(ObjectTrack &trk,
                                      const ObjectDetection_ &det) {
  GeoBBox bbox;
  bbox.timestamp = det.system_time;
  bbox.fusion_point.x = trk.x;
  bbox.fusion_point.y = trk.y;
  bbox.fusion_point.isvalid = true;
  bbox.geoPoints = det.lidar_bbox_dr.geoPoints;

  trk.bbox_dr_buffer.push_back(bbox);
  while (trk.bbox_dr_buffer.size() > 5) {
    trk.bbox_dr_buffer.pop_front();
  }
}

// get value in a[], which is the closest to t:
float closestValue(std::vector<float> a, int l, float t) {
  if (l < 0) {
    return 0.0;
  } else if (l == 1) {
    return a[0];
  } else {
    float min_dis = fabs(a[0] - t);
    int min_id = 0;
    for (int j = 1; j < l; j++) {
      if (fabs(a[j] - t) < min_dis) {
        min_dis = fabs(a[j] - t);
        min_id = j;
      }
    }
    return a[min_id];
  }
}
void BBoxCalculator::CalculateBBoxVelocity(ObjectTrack &trk) {
  float dt = 0;
  if (trk.bbox_dr_buffer.size() >= 2) {

    std::vector<float> new_vx, new_vy;
    GeoBBox latest_bbox, oldest_bbox;
    latest_bbox = trk.bbox_dr_buffer.back();
    oldest_bbox = trk.bbox_dr_buffer.front();

    float vx, vy;
    const float MAX_VEL = 25;

    dt = latest_bbox.timestamp - oldest_bbox.timestamp;
    for (int i = 0; i < 9; i++) {
      if ((i == TOP_LEFT || i == TOP_RIGHT || i == BOTTOM_LEFT || i == BOTTOM_RIGHT)
          && latest_bbox.geoPoints[i].isvalid && oldest_bbox.geoPoints[i].isvalid
          && dt > 0) {
        vx = (latest_bbox.geoPoints[i].x - oldest_bbox.geoPoints[i].x) / dt;
        vy = (latest_bbox.geoPoints[i].y - oldest_bbox.geoPoints[i].y) / dt;
        if (fabs(vx) < MAX_VEL && fabs(vy) < MAX_VEL ) {
          trk.bbox_dr_buffer.back().geoPoints[i].vxrel = vx;
          trk.bbox_dr_buffer.back().geoPoints[i].vyrel = vy;
          new_vx.push_back(vx);
          new_vy.push_back(vy);
        }
      }
    }

    // find the oldest valid data
    // handle cases like switching fuse point
    for (const auto &bbox : trk.bbox_dr_buffer) {
      if (bbox.fusion_point.isvalid == true) {
        oldest_bbox = bbox;
        break;
      }
    }

    dt = latest_bbox.timestamp - oldest_bbox.timestamp;
    if (latest_bbox.fusion_point.isvalid && oldest_bbox.fusion_point.isvalid
        && dt > 0) {
      vx = (latest_bbox.fusion_point.x - oldest_bbox.fusion_point.x) / dt;
      vy = (latest_bbox.fusion_point.y - oldest_bbox.fusion_point.y) / dt;
      if (fabs(vx) < MAX_VEL && fabs(vy) < MAX_VEL ) {
        trk.bbox_dr_buffer.back().fusion_point.vxrel = vx;
        trk.bbox_dr_buffer.back().fusion_point.vyrel = vy;
        new_vx.push_back(vx);
        new_vy.push_back(vy);
      }
    }

    if (new_vx.size()) {
      vx = closestValue(new_vx, new_vx.size(), trk.vx_bbox_dr);
      vy = closestValue(new_vy, new_vy.size(), trk.vy_bbox_dr);
      trk.vx_bbox_dr = vx;
      trk.vy_bbox_dr = vy;
    }
  }
}

void BBoxCalculator::GetBBoxCenter(ObjectTrack &trk, const float &x_vcs, const float &y_vcs) {
  std::vector<Eigen::Vector3f> points;
  points.resize(2);
  points[0] = Eigen::Vector3f(-trk.length_predict / 2.0, 0, 0);
  points[1] = Eigen::Vector3f(trk.length_predict / 2.0, 0, 0);
  Eigen::Vector3f geo_fuse_point;
  geo_fuse_point = Eigen::Vector3f(x_vcs, y_vcs, 0);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project =
    Eigen::AngleAxis<float>(normalize(trk.heading_vcs) * 3.1415926 / 180.0, Eigen::Matrix<float, 3, 1>::UnitZ())
    .matrix();
  for (size_t i = 0; i < points.size(); ++i) {
    points[i] = project * points[i];
    points[i] = points[i] + geo_fuse_point;
  }
  trk.x_center_vcs = x_vcs;
  trk.y_center_vcs = y_vcs;
  if (trk.fuse_point == FUSE_POINT_BOTTOM) {
    trk.x_center_vcs = points[1].x();
    trk.y_center_vcs = points[1].y();
    if (points[0].x() >= points[1].x()) {
      trk.x_center_vcs = points[0].x();
      trk.y_center_vcs = points[0].y();
    }
  } else if (trk.fuse_point == FUSE_POINT_TOP) {
    trk.x_center_vcs = points[0].x();
    trk.y_center_vcs = points[0].y();
    if (points[0].x() >= points[1].x()) {
      trk.x_center_vcs = points[1].x();
      trk.y_center_vcs = points[1].y();
    }
  }
}


void BBoxCalculator::GetPredictBBoxCenter(ObjectTrack &trk) {
  // 1. get fuse point
  std::vector<Eigen::Vector3f> points;
  points.resize(2);
  points[0] = Eigen::Vector3f(-trk.length / 2.0, 0, 0);
  points[1] = Eigen::Vector3f(trk.length / 2.0, 0, 0);
  Eigen::Vector3f geo_fuse_point;
  geo_fuse_point = Eigen::Vector3f(trk.x_center_vcs, trk.y_center_vcs, 0);
  // transform geometry bbox from body frame to sensor frame
  Eigen::Matrix3f project =
    Eigen::AngleAxis<float>(normalize(trk.heading_vcs) * 3.1415926 / 180.0, Eigen::Matrix<float, 3, 1>::UnitZ())
    .matrix();
  for (size_t i = 0; i < points.size(); ++i) {
    points[i] = project * points[i];
    points[i] = points[i] + geo_fuse_point;
  }

  float x_fuse = trk.x_center_vcs;
  float y_fuse = trk.y_center_vcs;
  if (trk.fuse_point == FUSE_POINT_BOTTOM) {
    x_fuse = points[0].x();
    y_fuse = points[0].y();
    if (points[0].x() >= points[1].x()) {
      x_fuse = points[1].x();
      y_fuse = points[1].y();
    }
  } else if (trk.fuse_point == FUSE_POINT_TOP) {
    x_fuse = points[1].x();
    y_fuse = points[1].y();
    if (points[0].x() >= points[1].x()) {
      x_fuse = points[0].x();
      y_fuse = points[0].y();
    }
  }
  // 2. use predicted length to re-calculate trk center
  geo_fuse_point = Eigen::Vector3f(x_fuse, y_fuse, 0);
  points[0] = Eigen::Vector3f(-trk.length_predict / 2.0, 0, 0);
  points[1] = Eigen::Vector3f(trk.length_predict / 2.0, 0, 0);
  for (size_t i = 0; i < points.size(); ++i) {
    points[i] = project * points[i];
    points[i] = points[i] + geo_fuse_point;
  }
  trk.x_center_vcs = x_fuse;
  trk.y_center_vcs = y_fuse;
  if (trk.fuse_point == FUSE_POINT_BOTTOM) {
    trk.x_center_vcs = points[1].x();
    trk.y_center_vcs = points[1].y();
    if (points[0].x() >= points[1].x()) {
      trk.x_center_vcs = points[0].x();
      trk.y_center_vcs = points[0].y();
    }
  } else if (trk.fuse_point == FUSE_POINT_TOP) {
    trk.x_center_vcs = points[0].x();
    trk.y_center_vcs = points[0].y();
    if (points[0].x() >= points[1].x()) {
      trk.x_center_vcs = points[1].x();
      trk.y_center_vcs = points[1].y();
    }
  }
}

}
}//namespace avos
}//namespace perception
