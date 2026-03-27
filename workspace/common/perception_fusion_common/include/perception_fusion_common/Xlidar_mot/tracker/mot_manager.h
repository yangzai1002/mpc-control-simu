#pragma once
#include <Eigen/Dense>
// #include <pcl/features/moment_of_inertia_estimation.h>

#include "perception_fusion_common/Xlidar_mot/common/common_type.h"
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/tracker/fusion_tracker.h"
#include "perception_fusion_common/Xlidar_mot/matching/gnn_matching.h"
#include "perception_fusion_common/Xlidar_mot/geometry/bbox_calculator.h"
// #include "algorithm/fusion/logging/logging.h"
// #include "algorithm/fusion/viewer/visualization.h"
// #include "map/map_loader/include/map_loader.h"
// #include "map/basemap/include/basemap.h"

namespace avos {
namespace perception {
namespace xlidarMOT{

class MotManager {
 public:
  MotManager();
  ~MotManager();

  int Init(FuseConfigType config);

  void UpdateLocTF(const PerceptionLocType &locpos);
  int Process_simple_cluster(const ThreadProfile &profile_lidar,
                             const std::vector<ObjectDetection_> &detections,
                             const ThreadProfile &profile_dor,
                             const DorDetection_ &dor_dets,
                             FusionDataType_ &data_out);
  int Process(const ThreadProfile &profile_lidar,
              const std::vector<ObjectDetection_> &detections,
              const ThreadProfile &profile_dor,
              const DorDetection_ &dor_dets,
              FusionDataType_ &data_out);

  double pstart_time_ = 0;
  double lidar_frame_time_ = 0;

 private:

  MotManager& operator=(MotManager const&) = delete;

  void Preprocess(const ThreadProfile &profile_lidar,
                  const std::vector<ObjectDetection_> &detections,
                  const ThreadProfile &profile_dor,const DorDetection_ &dor_dets);

  void VCS2DRTransForm(std::vector<ObjectDetection_> &detections);

  void RecalcObjCenterAndShape(ObjectDetection_ &det);

  template <class T>
  bool CheckPointInPolygon(const T & pt,
                           const  pcl::PointCloud<pcl::PointXYZI>  & bounding_polygon);

  bool PointDistanceToFreeSpace(const double&x_vcs, const double&y_vcs, double & distance);

  void GetObjToBoundaryInfo(std::vector<ObjectDetection_> &detections);

  void RemoveFalseDetections(std::vector<ObjectDetection_> &detections);

  // void MatchLidarToTracks(std::vector<ObjectDetection_> &detections,
  //                              std::vector<ObjectDetection_> &no_match_dets);
  void MatchRadarToTracks(std::vector<ObjectDetection_> &radarDets,
                                    std::vector<ObjectDetection_> &no_match_radarDets);
  void MatchLidarToTracks(std::vector<ObjectDetection_> &detections,
                          std::vector<ObjectDetection_> &no_match_dets);
  void MatchVisionToTracks(std::vector<ObjectDetection_> &detections,
                           std::vector<ObjectDetection_> &no_match_dets);
  void MatchingWithId(std::vector<ObjectDetection_> &detections);

  void PushMatchedDetectionToTrack(const std::vector<int> trks_status,
                                   std::vector<FusionTracker> &tracks,
                                   const std::vector<ObjectDetection_> &detections);

  void GetUnmatchedDetections(const std::vector<int> dets_status,
                              const std::vector<ObjectDetection_> &detections,
                              std::vector<ObjectDetection_> &unmatched_dets);

  void TrackLifeManagement();

  void FakeTrackLifeManagement();

  void TransferFakeTrackToFusionTrack();

  void InitNewFakeTrack(const std::vector<ObjectDetection_> lidarDets,
                        const std::vector<ObjectDetection_> radarDets);

  void PredictTracks();
  void ObstacleSizeFilter(ObjectTrack & trk,FusionObject_ &out);
  void HeadingUtmOpimize(FusionObject_ &out,std::vector<FusionObject_> &out_last_frame_tmp,std::string &trk_heading_source);
  void HeadingUtmOpimizeNew(FusionObject_ &out,std::vector<FusionObject_> &out_last_frame_tmp,std::string &trk_heading_source);
  void StaticCarOptimize(ObjectTrack & trk,FusionObject_ &out,std::vector<FusionObject_> &out_last_frame_tmp);
  void ID_Maintain_CT_RADAR(Radar2CamInfo &current_radarinfo,FusionTracker &track);
  
  void FillInDataOutput(FusionDataType_ &data_out);

  void TransForm(ObjectTrack &trk, FusionObject_ &out);
  void TransForm_simple_cluster(ObjectDetection_ det, FusionObject_ & out);
  void PostProcess();

  float NormalizeHeading(const float &in_deg) {
    auto out_deg = in_deg;
    if (out_deg < -180.0) { out_deg += 360.0; }
    if (out_deg > 180.0) { out_deg -= 360.0; }
    return out_deg;
  }
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
  BaseMatching* fusion_track_matcher_;
  BaseMatching* fake_track_matcher_;

  bool IsTrkUpdated(const int &fuse_source) {
    // if (fuse_source == SOURCE_NONE
    //     || fuse_source == SOURCE_RADAR_ONLY
    //     || fuse_source == SOURCE_VISION_ONLY) {
    //   return false;
    // }
    if (fuse_source == SOURCE_NONE
        || fuse_source == SOURCE_VISION_ONLY) {
      return false;
    }
    return true;
  }


//   avos::map::BaseMap* bmp_;
  std::vector<FusionTracker> fusion_tracks_;
  std::vector<FusionTracker> fake_tracks_;

  BBoxCalculator bbox_;

  FuseConfigType config_;

  PerceptionLocType locpos_;
  Eigen::Vector3d own_dr_pos_;
  Eigen::Matrix3d own_dr_rotation_;
  Eigen::Matrix3d own_dr_rotation_inverse_;
  Eigen::Vector3d own_utm_pos_;
  Eigen::Matrix3d own_utm_rotation_;
  Eigen::Matrix3d own_utm_rotation_inverse_;

  std::vector<ObjectDetection_> detections_lidar_;
  std::vector<ObjectDetection_> detections_lidar_new ;
  std::vector<ObjectDetection_> detections_radar_;
  std::vector<ObjectDetection_> detections_vision_;

  // radar_id - track_id列表
  std::vector<std::pair<Radar2CamInfo,int>> radar_track_id;
  // radar 类别保持
  std::vector<std::pair<int,int>> radar_track_type;
  
  bool inited_;
  size_t p_cnt_ = 0;

  bool vision_person_filter_ = false;
  //add by lxd
  bool steady_size_ = false;
  int perception_mode = -1;
  std::map<uint32_t, std::vector<float>> id_length;
  std::map<uint32_t, std::vector<float>> id_width;
  std::map<uint32_t, std::vector<float>> id_height;
  std::vector<uint32_t> track_ids;
  std::vector<uint32_t> steady_ids;
  float mean_length = 0.0;
  float mean_width = 0.0;
  float mean_height = 0.0;
  float mean_length_before = 0.0;
  float mean_width_before = 0.0;
  float mean_height_before = 0.0;
  bool only_simple_cluster = false;
  std::vector<FusionObject_> out_last_frame;

};

}
}  // namespace perception
}  // namespace avos





