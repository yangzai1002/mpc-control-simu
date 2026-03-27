#pragma once
#include <numeric>
#include <vector>

#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/tracker/base_tracker.h"
#include "perception_fusion_common/Xlidar_mot/geometry/bbox_calculator.h"
#include "perception_fusion_common/Xlidar_mot/math/numerical.h"

namespace avos {
namespace perception {
namespace xlidarMOT{
    
class FusionTracker: public BaseTracker {
public:
  FusionTracker(const ObjectTrack& det);
  ~FusionTracker();

  static int Init(FuseConfigType config);


  void PredictFusionTrack();

  void UpdateFusionTrack(const double timestamp,
                         const PerceptionLocType &locpos);

  void UpdateFakeTrack(const double timestamp,
                       const PerceptionLocType &locpos);

  void ClearMatchedDetections();

  void InitFilterParams();

  void UpdateVelocityStatus();
  void CalcVelocityRMS(VelocityRMS & trk_vel, const float v);
  void CalcVelocityHeadingRMS(VelocityRMS & trk_heading, const float &vx, const float &vy);

  std::vector<ObjectDetection_> matched_lidar_dets_;
  std::vector<ObjectDetection_> matched_radar_dets_;
  std::vector<ObjectDetection_> matched_vision_dets_;

private:
  void ProcessLidarDets();
  void UpdateLidar2CameraInfo(const ObjectDetection_ &det);
  void UpdateObjectType(const ObjectDetection_ &det);
  void UpdateObjectDimension(const ObjectDetection_ &det, const PerceptionLocType &locpos);
  void UpdateObjectDimension_SPC(const ObjectDetection_ &det, const PerceptionLocType &locpos);
  void UpdateFuseSource(int &fuse_source);
  bool IsRadarValid(const double x_vcs, const double y_vcs);
  void UpdataImg2DBox(const ObjectDetection_& mtd_det);

  bool IsCarOrTruck(const int &type) {
    if (type == CAR_TYPE || type == TRUCK_TYPE) {
      return true;
    }
    return false;
  }
  bool IsPedestrainOrCyclist(const int &type) {
    if (type == PEDESTRIAN_TYPE || type == CYCLIST_TYPE) {
      return true;
    }
    return false;
  }
  bool IsPedestrain(const int &type) {
    if (type == PEDESTRIAN_TYPE) {
      return true;
    }
    return false;
  }
  bool IsCyclist(const int &type) {
    if (type == CYCLIST_TYPE) {
      return true;
    }
    return false;
  }
  bool IsVisionAvailable(const int &type, const int &x_vcs, const int &y_vcs) {
    // if (IsCarOrTruck(type) && x_vcs >= 2
    //     && ((x_vcs <= 30 && fabs(y_vcs) <= 20)
    //         || (x_vcs <= 50 && fabs(y_vcs) <= 10)
    //         || (x_vcs <= 80 && fabs(y_vcs) <= 5))) {
    //   return true;
    // }
    // 20210929: 收紧过滤范围
    // if (IsCarOrTruck(type) && x_vcs >= 2
    //     && ((x_vcs <= 30 && fabs(y_vcs) <= 20)
    //         || (x_vcs <= 50 && fabs(y_vcs) <= 10))) {
    //   return true;
    // }
    // if (IsPedestrainOrCyclist(type)
    //     && (x_vcs <= 35 && fabs(y_vcs) <= 5)) {
    //   return true;
    // }
    return false;
  }
  bool IsSameType(const int &type1, const int &type2) {
    if (type1 == type2) {
      return true;
    }
    if (IsCarOrTruck(type1) && IsCarOrTruck(type2)) {
      return true;
    }
    return false;
  }

  float NormalizeHeading(const float &in_deg) {
    auto out_deg = in_deg;
    if (out_deg < -180.0) { out_deg += 360.0; }
    if (out_deg > 180.0) { out_deg -= 360.0; }
    return out_deg;
  }

  bool IsTopBottomReversed(const float& trk_heading_vcs, const float& det_heading_vcs) {
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
    return is_top_bottom_reversed;
  }


  BBoxCalculator bbox_;


  static FuseConfigType config_;
  static bool inited_;
  bool max_length_observed_ = false;
  static bool vision_person_filter_;

  int VELOCITY_BUFFER_SIZE = 5;

};

}
}
}
