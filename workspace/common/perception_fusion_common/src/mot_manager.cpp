#include "log/log.h"
#include "perception_fusion_common/Xlidar_mot/tracker/mot_manager.h"

// C++17 compatibility fix for FLANN library (used by PCL)
// std::random_shuffle was removed in C++17, provide compatibility
#if __cplusplus >= 201703L
#include <algorithm>
#include <random>
namespace std {
    template<class RandomIt>
    void random_shuffle(RandomIt first, RandomIt last) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(first, last, g);
    }
    template<class RandomIt, class RandomFunc>
    void random_shuffle(RandomIt first, RandomIt last, RandomFunc&&) {
        // Use default random generator since RandomFunc may not be compatible with std::shuffle
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(first, last, g);
    }
}
#endif

#include <pcl/features/moment_of_inertia_estimation.h>

#include <iostream>

namespace avos {
namespace perception {
namespace xlidarMOT{  

MotManager::MotManager() {
  fusion_track_matcher_ = nullptr;
  fake_track_matcher_ = nullptr;
  inited_ = false;
}

MotManager::~MotManager() {
  if (fusion_track_matcher_ != nullptr) {
    delete fusion_track_matcher_;
  }
  if (fake_track_matcher_ != nullptr) {
    delete fake_track_matcher_;
  }
}

int MotManager::Init(FuseConfigType config) {
  AINFO  << "Init MotManager...";
  inited_ = true;
  config_ = config;
//   logs_.Init(config.fusion_track_config);
//   viz_.Init();

  fusion_track_matcher_ = new GNNMatching();
  fusion_track_matcher_->Init(config.matching_config);

  fake_track_matcher_ = new GNNMatching();
  fake_track_matcher_->Init(config.matching_config);

  FusionTracker::Init(config);
  //add by hhl: vision_person_filter
  vision_person_filter_ = config.fusion_track_config.vision_person_filter;
  //add by lxd
  steady_size_ = config.fusion_track_config.steady_size;
  perception_mode = config.fusion_track_config.perception_mode;
  only_simple_cluster = config.fusion_track_config.only_simple_cluster;
  AINFO<<"Init MotManager done.";
  /// init distance_map
  // bmp_ = avos::map::MapLoader::GetBasemapPtr(); //init once
  return 0;
}

void MotManager::UpdateLocTF(const PerceptionLocType &locpos) {
  locpos_ = locpos;
  // By default, the ZYX axis order will be used
  // 0 1 2 对应 z y x 对应 yaw pitch roll

  // dr-coords
  Eigen::Vector3d own_dr_rot(locpos_.dr_yaw * DEG2RAD,
                             locpos_.dr_pitch * DEG2RAD,
                             locpos_.dr_roll * DEG2RAD);
  own_dr_rotation_ = Eigen::AngleAxisd(own_dr_rot[0], Eigen::Vector3d::UnitZ()) *
                     Eigen::AngleAxisd(own_dr_rot[1], Eigen::Vector3d::UnitY()) *
                     Eigen::AngleAxisd(own_dr_rot[2], Eigen::Vector3d::UnitX());
  own_dr_rotation_inverse_ = own_dr_rotation_.inverse();
  own_dr_pos_ << locpos_.dr_x, locpos_.dr_y, locpos_.dr_z;

  // utm-coords
  Eigen::Vector3d own_utm_rot(locpos_.angle * DEG2RAD,
                              locpos_.pitch * DEG2RAD,
                              locpos_.roll * DEG2RAD);
  own_utm_rotation_ = Eigen::AngleAxisd(own_utm_rot[0], Eigen::Vector3d::UnitZ()) *
                      Eigen::AngleAxisd(own_utm_rot[1], Eigen::Vector3d::UnitY()) *
                      Eigen::AngleAxisd(own_utm_rot[2], Eigen::Vector3d::UnitX());
  own_utm_rotation_inverse_ = own_utm_rotation_.inverse();
  own_utm_pos_ << locpos_.xg, locpos_.yg, locpos_.zg;
}

int MotManager::Process_simple_cluster(const ThreadProfile &profile_lidar,
                                       const std::vector<ObjectDetection_> &detections,
                                       const ThreadProfile &profile_dor,
                                       const DorDetection_ &dor_dets,
                                       FusionDataType_ &data_out) {
  // 1. detections preprocess
  Preprocess(profile_lidar, detections, profile_dor,dor_dets);
  /***log internal info***/
  p_cnt_++; if (p_cnt_ > 1e6) {p_cnt_ = 0;}
//   logs_.timestamp_ = pstart_time_;
//   logs_.p_cnt_ = p_cnt_;
//   logs_.locpos_ = locpos_;
//   logs_.LoggingProfile(profile_lidar, profile_dor, lidar_frame_time_);
//   logs_.LoggingRawInputs(detections_lidar_, detections_radar_, detections_vision_);
//   viz_.VisualizeInputs(detections_lidar_, "lidar");

  // 7. fill in data out
  // copy fusion track to output
  data_out.objs.clear();
  data_out.cells.clear();
  data_out.time = lidar_frame_time_;
  for (auto &det : detections) {
    FusionObject_ out;
    out.id = 0;
    out.type = 2;
    out.age = 0;
    out.length = det.length;
    out.width = det.width;
    out.height = det.height;
    out.source = SOURCE_LIDAR_ONLY;
    out.heading = det.heading_vcs;
    TransForm_simple_cluster(det, out);
    data_out.objs.push_back(out);
  }

  // 8. post process, clear globals
  PostProcess();
  return 0;
}


void MotManager::ID_Maintain_CT_RADAR(Radar2CamInfo &current_radarinfo,FusionTracker &track){
    int radar_id_old = -1;
    int trk_id_old = -1;
    int current_radar_id = current_radarinfo.radar_id;
    // radar_id比较与更新radar_info
    for (int j=0;j<radar_track_id.size();j++){
        int radar_id_old = radar_track_id[j].first.radar_id;
        if(current_radar_id == radar_id_old ){
            // 比较新的radarinfo和旧的radarinfo的距离
            double dis_center_ = sqrt(pow(current_radarinfo.x-radar_track_id[j].first.x,2)+pow(current_radarinfo.y-radar_track_id[j].first.y,2));
            if(dis_center_ < 10){
                //满足条件则更新radar_info
                radar_track_id[j].first = current_radarinfo;
                trk_id_old = radar_track_id[j].second;
                break;
            }
            
        }
    }
    if(trk_id_old > -1){

            track.trk.id = trk_id_old;
        
    }else{
        // id记录
        radar_track_id.push_back(std::pair<Radar2CamInfo,int>(current_radarinfo,track.trk.id));
    }
    if(track.trk.radarInfo.matched_vision>0){//type 记录
        radar_track_type.push_back(std::pair<int,int>(track.trk.radarInfo.radar_id,track.trk.type));
    }
}


int MotManager::Process(const ThreadProfile &profile_lidar,
                        const std::vector<ObjectDetection_> &detections,
                        const ThreadProfile &profile_dor,
                        const DorDetection_ &dor_dets,
                        FusionDataType_ &data_out) {
  AINFO << "only_simple_cluster: " << only_simple_cluster;
  if (only_simple_cluster) {
    Process_simple_cluster(profile_lidar, detections, profile_dor,dor_dets, data_out);
  } else {
    // 1. detections preprocess
    Preprocess(profile_lidar, detections, profile_dor,dor_dets);
    /***log internal info***/
    p_cnt_++; if (p_cnt_ > 1e6) {p_cnt_ = 0;}
    // logs_.timestamp_ = pstart_time_;
    // logs_.p_cnt_ = p_cnt_;
    // logs_.locpos_ = locpos_;
    // logs_.LoggingProfile(profile_lidar, profile_dor, lidar_frame_time_);
    // logs_.LoggingRawInputs(detections_lidar_, detections_radar_, detections_vision_);
    // viz_.VisualizeInputs(detections_lidar_, "lidar");
    // viz_.VisualizeInputs(detections_radar_, "radar");

    // remove false detections
    RemoveFalseDetections(detections_lidar_);
    RemoveFalseDetections(detections_radar_);

    // viz_.VisualizeInputs(detections_lidar_, "lidar");

    // 2: time predict for all tracks
    PredictTracks();

    // 3. match detections to tracks
    std::vector<ObjectDetection_> no_match_dets_lidar;
    MatchLidarToTracks(detections_lidar_, no_match_dets_lidar);

    // AINFO << "[MOT MANAGER] Before MatchRadarToTracks detections_radar_ size " 
    //           <<  detections_radar_.size();
    std::vector<ObjectDetection_> no_match_dets_radar;
    MatchRadarToTracks(detections_radar_, no_match_dets_radar);
//    AINFO << "[MOT MANAGER] After MatchRadarToTracks detections_radar_ size " 
//               <<  detections_radar_.size();

    std::vector<ObjectDetection_> no_match_dets_vision;
    MatchVisionToTracks(detections_vision_, no_match_dets_vision);

    // 4. update fusion tracks
    for (auto &track : fusion_tracks_) {
      // 判断radar-id,// 基于CT radar的id追踪进行id生成时的不切换,fusion-track只做修正，针对的是一开始生成的纯lidar - track
      if(track.matched_radar_dets_.size()>0){
          for(int i=0;i<track.matched_radar_dets_.size();i++){
            // id保持
            ID_Maintain_CT_RADAR(track.matched_radar_dets_[i].radarInfo,track);
          }
      }else if(track.trk.radarInfo.radar_id > -1){
            ID_Maintain_CT_RADAR(track.trk.radarInfo,track);
      }
      // AINFO << " [update fusion] ";
      // AINFO << "MRS "<<  track.matched_radar_dets_.size() 
      // << " |  MLS " <<track.matched_lidar_dets_.size() 
      // << " |  MVS " <<track.matched_vision_dets_.size() 
      // << " | id " << track.trk.id 
      // << " | radar id " << track.trk.radarInfo.radar_id
      // << " | track.trk.x_center_vcs "<< track.trk.x_center_vcs 
      // << " | track.trk.y_center_vcs " <<track.trk.y_center_vcs 
      // << " | track.trk.radarInfo.x " <<track.trk.radarInfo.x
      //;
      track.UpdateFusionTrack(pstart_time_, locpos_);
      // AINFO<<"out_id:"<<track.trk.x_center_vcs<<","<<track.trk.y_center_vcs<<" heding:"<<track.trk.heading_vcs;
    }
    // AINFO << " ========= [update fusion] TrackLifeManagement(); ";
    TrackLifeManagement();

    // 5. update fake tracks
    for (auto &track : fake_tracks_) {
      // 判断radar-id,// 基于CT radar的id追踪进行id生成时的不切换,fake - track做修正和记录
      if(track.matched_radar_dets_.size()>0){
          for(int i=0;i<track.matched_radar_dets_.size();i++){
            // id保持
            ID_Maintain_CT_RADAR(track.matched_radar_dets_[i].radarInfo,track);
          }
      }else if(track.trk.radarInfo.radar_id > -1){
            // id保持判断
            ID_Maintain_CT_RADAR(track.trk.radarInfo,track);
      }
      // AINFO << " [update fake] ";
      // AINFO << "MRS "<<  track.matched_radar_dets_.size() 
      // << " |  MLS " <<track.matched_lidar_dets_.size() 
      // << " |  MVS " <<track.matched_vision_dets_.size() 
      // << " | id " << track.trk.id 
      // << " | radar id " << track.trk.radarInfo.radar_id
      // << " | track.trk.x_center_vcs "<< track.trk.x_center_vcs 
      // << " | track.trk.y_center_vcs " <<track.trk.y_center_vcs 
      // << " | track.trk.radarInfo.x " <<track.trk.radarInfo.x
      //;
      track.UpdateFakeTrack(pstart_time_, locpos_);
    }
    FakeTrackLifeManagement();

    // 6. create new fake track from no match detections
    InitNewFakeTrack(no_match_dets_lidar, no_match_dets_radar);

    /***log internal info***/
    // for (const auto& track : fake_tracks_) {
    //   // logs_.LoggingMatchedDetections(track.trk, track.matched_lidar_dets_, "fake-lidar");
    //   // logs_.LoggingMatchedDetections(track.trk, track.matched_radar_dets_, "fake-radar");
    //   // logs_.LoggingMatchedDetections(track.trk, track.matched_vision_dets_, "fake-vision");
    //   logs_.LoggingFusionTracks(track.trk,
    //                             track.matched_lidar_dets_,
    //                             track.matched_radar_dets_,
    //                             track.matched_vision_dets_,
    //                             "fake-track");
    //   // logs_.LoggingBBoxVelocity(track.trk, "fake-track");
    // }
    // for (const auto& track : fusion_tracks_) {
    //   // logs_.LoggingMatchedDetections(track.trk, track.matched_lidar_dets_, "lidar");
    //   // logs_.LoggingMatchedDetections(track.trk, track.matched_radar_dets_, "radar");
    //   // logs_.LoggingMatchedDetections(track.trk, track.matched_vision_dets_, "vision");
    //   logs_.LoggingFusionTracks(track.trk,
    //                             track.matched_lidar_dets_,
    //                             track.matched_radar_dets_,
    //                             track.matched_vision_dets_,
    //                             "track");
    //   // logs_.LoggingBBoxVelocity(track.trk, "track");
    // }
    // std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now(), end;
    // 7. fill in data out
    FillInDataOutput(data_out);
    // end = std::chrono::high_resolution_clock::now();
    // AINFO<<"FillInDataOutput:"<<std::fixed<< (std::chrono::duration<double>(end - start).count())*1e3 ;
    /***log internal info***/
    // for (const auto& out : data_out.objs) {
    //   logs_.LoggingPerceptionOutput(data_out.time, out);
    // }
    // logs_.PublishFusionDebug(data_out.time);

    // 8. post process, clear globals
    PostProcess();
  }
  return 0;
}

void MotManager::MatchRadarToTracks(std::vector<ObjectDetection_> &radarDets,
                                    std::vector<ObjectDetection_> &no_match_radarDets) {
   
//   int count = 0;                              
  // step1: track-radarDet id 关联
  for (auto& track : fusion_tracks_) {
    auto radar_id = track.trk.radarInfo.radar_id;
    auto radar_age = track.trk.radarInfo.radar_age;
    auto vision_id = track.trk.radarInfo.visionobject_id;
    if(radar_id > -1){//如果该trk为radar的检测结果的话
        // AINFO << "[MatchRadarToTracks] Before id match fusion_tracks "  << " | radar id  " << radar_id <<" | radar age  " << radar_age << " | vision id  " << vision_id << " | track_id  " << track.trk.id <<" | track.trk.radarInfo.x"<< track.trk.radarInfo.x; 
        if (track.matched_radar_dets_.size() == 0) {//如果该track没有匹配上新的radar结果的话
        std::vector<ObjectDetection_>::iterator it;
        for (it = radarDets.begin(); it != radarDets.end();) {
            // AINFO << "[MatchRadarToTracks] id match detections " << "  | radar id  " << (*it).radarInfo.radar_id <<" | radar age   " << (*it).radarInfo.radar_age << " | vision id   " << (*it).radarInfo.visionobject_id << " | x   "<< (*it).radarInfo.x<< "  | source   "<< (*it).source ;     
            // 距离判断
            bool match_found  = false;
            // float det_dist = sqrt(pow((*it).radarInfo.x, 2) + pow((*it).radarInfo.y, 2));
            // float delta_dist = fabs(track_dist - det_dist);
            float delta_dist = sqrt(pow((*it).radarInfo.x-track.trk.radarInfo.x,2) + pow((*it).radarInfo.y-track.trk.radarInfo.y,2) );
            bool dist_valid = false;
            if (IsPedestrainOrCyclist(track.trk.type) && delta_dist <= 2.5) {
                dist_valid = true;
            }
            if (IsCarOrTruck(track.trk.type) && delta_dist <= 5.0) {
                dist_valid = true;
            }
            else if(delta_dist <=5.0){
                dist_valid = true;
            }
            // AINFO << "[MatchRadarToTracks] fusion id matching distance : " << delta_dist; 
            // id匹配 + 距离匹配 + source 判断
            if (radar_id == (*it).radarInfo.radar_id && dist_valid == true) {
            //   it->match_method = MM_RADAR_ID;
            track.matched_radar_dets_.emplace_back(*it);
            // AINFO << "[MatchRadarToTracks] After id match fusion_tracks "  << "  | radar id  " << (*it).radarInfo.radar_id; 
            it = radarDets.erase(it);
            } else {
            it++;
            //   AINFO << "[MatchRadarToTracks] no id match fusion_tracks "; 
            }


        }

        }
    }

    // count ++ ;
  }

  for (auto& track : fake_tracks_) {
    auto radar_id = track.trk.radarInfo.radar_id;
    auto radar_age = track.trk.radarInfo.radar_age;
    auto vision_id = track.trk.radarInfo.visionobject_id;
    if(radar_id > -1){//如果该结果为radar的检测结果的话
        // AINFO << "[MatchRadarToTracks] Before id match fake_tracks "  << " | radar id  " << radar_id <<" | radar age  " << radar_age << " | vision id  " << vision_id << " | track_id  " << track.trk.id << " | track.trk.radarInfo.x"<< track.trk.radarInfo.x; 
        
        // float track_dist = sqrt(pow(track.trk.radarInfo.x, 2) + pow(track.trk.radarInfo.y, 2));
        if (track.matched_radar_dets_.size() == 0) {
        std::vector<ObjectDetection_>::iterator it;
        for (it = radarDets.begin(); it != radarDets.end();) {
            // AINFO << "[MatchRadarToTracks] id match detections " << "  | radar id  " << (*it).radarInfo.radar_id <<" | radar age   " << (*it).radarInfo.radar_age << " | vision id   " << (*it).radarInfo.visionobject_id << " | x   "<< (*it).radarInfo.x<< "  | source   "<< (*it).source ;   
            // 距离判断
            bool match_found  = false;
            // float det_dist = sqrt(pow((*it).radarInfo.x, 2) + pow((*it).radarInfo.y, 2));
            // float delta_dist = fabs(track_dist - det_dist);
            float delta_dist = sqrt(pow((*it).radarInfo.x-track.trk.radarInfo.x,2) + pow((*it).radarInfo.y-track.trk.radarInfo.y,2) );
            bool dist_valid = false;
            if (IsPedestrainOrCyclist(track.trk.type) && delta_dist <= 2.5) {
                dist_valid = true;
            }
            if (IsCarOrTruck(track.trk.type) && delta_dist <=5.0) {
                dist_valid = true;
            }
            else if(delta_dist <=5.0){
                dist_valid = true;
            }
            // AINFO << "[MatchRadarToTracks] fake id matching distance  " << delta_dist; 
            // id匹配 + 距离匹配 + source 判断
            if (radar_id == (*it).radarInfo.radar_id && dist_valid == true ) {
            //   it->match_method = MM_RADAR_ID;
            track.matched_radar_dets_.emplace_back(*it);
            //   AINFO << "[MatchRadarToTracks] After id match fake_tracks " << count << "  radar id | " <<(*it).radarInfo.radar_id; 
            it = radarDets.erase(it);
            } else {
            //   AINFO << "[MatchRadarToTracks] no id match fake_tracks "; 
            it++;
            }
        }
        }
    }
  }

  // step2: track-radarDet gnn 关联
  // match unmatched detections to tracks
  fusion_track_matcher_->Matching(fusion_tracks_, radarDets);
  std::vector<int> trks_status;
  std::vector<int> dets_status;
  fusion_track_matcher_->getMatchResult(trks_status, dets_status);
//   AINFO << "[MatchRadarToTracks] fake_tracks matched_radar_dets "; 
  PushMatchedDetectionToTrack(trks_status, fusion_tracks_, radarDets);
  std::vector<ObjectDetection_> unmatched_dets;
  GetUnmatchedDetections(dets_status, radarDets, unmatched_dets);
//   AINFO << "[MatchRadarToTracks] GetUnmatchedDetections fake_tracks unmatched_dets size " << unmatched_dets.size(); 
  // match unmatched detections to fake tracks
  fake_track_matcher_->Matching(fake_tracks_, unmatched_dets);
  trks_status.clear();
  dets_status.clear();
  fake_track_matcher_->getMatchResult(trks_status, dets_status);
//   AINFO << "[MatchRadarToTracks] fake_tracks matched_radar_dets "; 
  PushMatchedDetectionToTrack(trks_status, fake_tracks_, unmatched_dets);
  GetUnmatchedDetections(dets_status, unmatched_dets, no_match_radarDets);
//   AINFO << "[MatchRadarToTracks] GetUnmatchedDetections fake_tracks no_matched_dets size " << no_match_radarDets.size(); 
}

void MotManager::MatchLidarToTracks(std::vector<ObjectDetection_> &detections,
                                    std::vector<ObjectDetection_> &no_match_dets) {
  MatchingWithId(detections);

  // match detections to tracks
  fusion_track_matcher_->Matching(fusion_tracks_, detections);
  std::vector<int> trks_status;
  std::vector<int> dets_status;
  fusion_track_matcher_->getMatchResult(trks_status, dets_status);
  PushMatchedDetectionToTrack(trks_status, fusion_tracks_, detections);
  std::vector<ObjectDetection_> unmatched_dets;
  GetUnmatchedDetections(dets_status, detections, unmatched_dets);

  // match unmatched detections to fake tracks
  fake_track_matcher_->Matching(fake_tracks_, unmatched_dets);
  trks_status.clear();
  dets_status.clear();
  fake_track_matcher_->getMatchResult(trks_status, dets_status);
  PushMatchedDetectionToTrack(trks_status, fake_tracks_, unmatched_dets);
  GetUnmatchedDetections(dets_status, unmatched_dets, no_match_dets);
}

void MotManager::MatchVisionToTracks(std::vector<ObjectDetection_> &detections,
                                     std::vector<ObjectDetection_> &no_match_dets) {
  MatchingWithId(detections);
  no_match_dets = detections;

  // 2022-07-12: 利用未匹配的视觉结果直接生成新目标
  for (const auto& det : no_match_dets) {
    ObjectTrack trk;
    trk.system_time = det.system_time;
    trk.lidar_received_time = 0;

    trk.x = det.x_dr;
    trk.y = det.y_dr;
    trk.vx = det.vx_dr;
    trk.vy = det.vy_dr;
    trk.heading = det.heading_dr;
    trk.x_center_vcs = det.x_vcs;
    trk.y_center_vcs = det.y_vcs;
    trk.heading_vcs = det.heading_vcs;
    trk.heading_source = "vision";

    trk.length = det.length;
    trk.width = det.width;
    trk.height = det.height;
    trk.length_predict = det.length;
    trk.width_predict = det.width;

    trk.age = 0;
    trk.type = det.type;
    trk.confidence = 1.0;
    trk.fuse_source = SOURCE_VISION_ONLY;
    trk.fuse_point = FUSE_POINT_BOTTOM;
    bbox_.UpdateBBoxBuffer(trk, det);
    trk.l2cInfo = det.l2cInfo;
    trk.is_l2c_updated = true;
    trk.updated = false;

    FusionTracker new_track(trk);
    new_track.matched_vision_dets_.push_back(det);
    fusion_tracks_.push_back(new_track);
  }
}

void MotManager::MatchingWithId(std::vector<ObjectDetection_> &detections) {
  for (auto& track : fusion_tracks_) {
    // if (track.matched_lidar_dets_.size()) {
    //   continue;
    // }
    auto l2c_objs = track.trk.l2cInfo.objs;
    if (l2c_objs.size() == 1) {
      int track_id = l2c_objs[0].track_id;
      int track_age = l2c_objs[0].track_age;
      float track_dist = sqrt(pow(l2c_objs[0].x, 2) + pow(l2c_objs[0].y, 2));

      std::vector<ObjectDetection_>::iterator it;
      for (it = detections.begin(); it != detections.end();) {
        bool match_found  = false;
        for (const auto& cluster : it->l2cInfo.objs) {
          // need to check distance
          float det_dist = sqrt(pow(cluster.x, 2) + pow(cluster.y, 2));
          float delta_dist = fabs(track_dist - det_dist);
          bool dist_valid = false;
          if (IsPedestrainOrCyclist(track.trk.type) && delta_dist <= 1.5) {
            dist_valid = true;
          }
          if (IsCarOrTruck(track.trk.type) && delta_dist <= 3.0) {
            dist_valid = true;
          }
          if ( (track.trk.type == SUBULATE_TYPE ||track.trk.type == ROADBLOCK_TYPE || track.trk.type == ANTITANK_TYPE || track.trk.type == WATERFENCE_TYPE ) && delta_dist <= 1.0) {
            dist_valid = true;
          }
          if (track_id == cluster.track_id
              && (cluster.track_age - track_age) <= 5
              && dist_valid == true) {
            match_found = true;
            break;
          }
        }
        if (match_found == true) {
          if (((*it).source & SOURCE_MASK) == LIDAR_SOURCE) {
            track.matched_lidar_dets_.emplace_back(*it);
          } else if (((*it).source & SOURCE_MASK) == VISION_SOURCE) {
            track.matched_vision_dets_.emplace_back(*it);
          }
          it = detections.erase(it);
        } else {
          it++;
        }
      }

    } else if (track.trk.radarInfo.radar_id > -1){//如果该track含有radar的结果的话
      std::vector<ObjectDetection_>::iterator it;
      for (it = detections.begin(); it != detections.end();) {
        double dist_ = sqrt(pow(track.trk.radarInfo.x-(*it).x_vcs,2) + pow(track.trk.radarInfo.y-(*it).y_vcs,2));
        if(dist_ < 6.0){
          if (((*it).source & SOURCE_MASK) == LIDAR_SOURCE) {
            track.matched_lidar_dets_.emplace_back(*it);
          } else if (((*it).source & SOURCE_MASK) == VISION_SOURCE) {
            track.matched_vision_dets_.emplace_back(*it);
          }
          it = detections.erase(it);
          AINFO << " +++++++++ matching lidar or vision to radar ";
        } else {
          it++;
        }
      }
    }
    else if (track.trk.is_bl_cluster){
        // AINFO << " -------- TEST : Matching Bl cluster ----------- "  
        //           << " x : "<<track.trk.x_center_vcs
        //           << " | y : " << track.trk.y_center_vcs 
        //          ;
        std::vector<ObjectDetection_>::iterator it;
        for (it = detections.begin(); it != detections.end();) {
            bool match_found  = false;
            if(it->is_bl_cluster || ((*it).source & SOURCE_MASK) == RADAR_SOURCE){
                // //宽和高差不多的话匹配距离
                // bool width_check = fabs(it->width - track.trk.width) < 3;
                // bool height_check = fabs(it->height - track.trk.height) < 3;
                float center_dis = sqrt(pow(it->x_vcs - track.trk.x_center_vcs, 2) + pow(it->y_vcs - track.trk.y_center_vcs, 2));
                bool dist_check ;
                dist_check = fabs(center_dis) < 6.0;
                // if(((*it).source & SOURCE_MASK) == RADAR_SOURCE){
                //     dist_check = fabs(center_dis) < 6.0;
                //     width_check = true;
                //     height_check = true;
                // }
                // AINFO << " det x: " << it->x_vcs << "| y: "<<it->y_vcs
                //          << " | det width_check: " << fabs(it->width - track.trk.width)
                //          << " | det height_check: " << fabs(it->height - track.trk.height)
                //          << " | det dist_check: " << center_dis
                //         ;
                if(dist_check){
                    match_found = true;
                    AINFO << " FOUND BL CLUSTER ";
                    break;
                }
            }  
            if (match_found == true) {
                if (((*it).source & SOURCE_MASK) == LIDAR_SOURCE) {
                    track.matched_lidar_dets_.emplace_back(*it);
                } else if (((*it).source & SOURCE_MASK) == VISION_SOURCE) {
                    track.matched_vision_dets_.emplace_back(*it);
                } else if (((*it).source & SOURCE_MASK) == RADAR_SOURCE){
                    track.matched_radar_dets_.emplace_back(*it);
                }
                    it = detections.erase(it);
            } else {
                it++;
            }  
        }
    }


  }
//   for (auto& track : fake_tracks_) {
//     // if (track.matched_lidar_dets_.size()) {
//     //   continue;
//     // }
//     auto l2c_objs = track.trk.l2cInfo.objs;
//     if (l2c_objs.size() == 1) {
//       int track_id = l2c_objs[0].track_id;
//       int track_age = l2c_objs[0].track_age;
//       float track_dist = sqrt(pow(l2c_objs[0].x, 2) + pow(l2c_objs[0].y, 2));

//       std::vector<ObjectDetection_>::iterator it;
//       for (it = detections.begin(); it != detections.end();) {
//         bool match_found  = false;
//         for (const auto& cluster : it->l2cInfo.objs) {
//           // need to check distance
//           float det_dist = sqrt(pow(cluster.x, 2) + pow(cluster.y, 2));
//           float delta_dist = fabs(track_dist - det_dist);
//           bool dist_valid = false;
//           if (IsPedestrainOrCyclist(track.trk.type) && delta_dist <= 1.5) {
//             dist_valid = true;
//           }
//           if (IsCarOrTruck(track.trk.type) && delta_dist <= 3.0) {
//             dist_valid = true;
//           }
//           if ( (track.trk.type == SUBULATE_TYPE ||track.trk.type == ROADBLOCK_TYPE || track.trk.type == ANTITANK_TYPE || track.trk.type == WATERFENCE_TYPE ) && delta_dist <= 1.0) {
//             dist_valid = true;
//           }
//           if (track_id == cluster.track_id
//               && (cluster.track_age - track_age) <= 5
//               && dist_valid == true) {
//             match_found = true;
//             break;
//           }
//         }
//         if (match_found == true) {
//           if (((*it).source & SOURCE_MASK) == LIDAR_SOURCE) {
//             track.matched_lidar_dets_.emplace_back(*it);
//           } else if (((*it).source & SOURCE_MASK) == VISION_SOURCE) {
//             track.matched_vision_dets_.emplace_back(*it);
//           }
//           it = detections.erase(it);
//         } else {
//           it++;
//         }
//       }

//     }else if (track.trk.radarInfo.radar_id > 0){//如果该track含有radar的结果的话
//       std::vector<ObjectDetection_>::iterator it;
//       for (it = detections.begin(); it != detections.end();) {
//         double dist_ = sqrt(pow(track.trk.radarInfo.x-(*it).x_vcs,2) + pow(track.trk.radarInfo.y-(*it).y_vcs,2));
//         if(dist_ < 3.0){
//           if (((*it).source & SOURCE_MASK) == LIDAR_SOURCE) {
//             track.matched_lidar_dets_.emplace_back(*it);
//           } else if (((*it).source & SOURCE_MASK) == VISION_SOURCE) {
//             track.matched_vision_dets_.emplace_back(*it);
//           }
//           it = detections.erase(it);
//           AINFO << " +++++++++ matching lidar or vision to radar fake ";
//         } else {
//           it++;
//         }
//       }
//     }

//   }
}

void MotManager::PushMatchedDetectionToTrack(const std::vector<int> trks_status,
    std::vector<FusionTracker> &tracks,
    const std::vector<ObjectDetection_> &detections) {
  /// <0,not matched; >=0,target id
  for (size_t i = 0; i < trks_status.size(); ++i) {
    if (trks_status[i] >= 0) {
      auto matched_det = detections[trks_status[i]];
      if ((matched_det.source & SOURCE_MASK) == LIDAR_SOURCE) {
        tracks[i].matched_lidar_dets_.push_back(matched_det);
      } else if ((matched_det.source & SOURCE_MASK) == RADAR_SOURCE) {
        tracks[i].matched_radar_dets_.push_back(matched_det);
      } else if ((matched_det.source & SOURCE_MASK) == VISION_SOURCE) {
        tracks[i].matched_vision_dets_.push_back(matched_det);
      }
    }
  }
}

void MotManager::GetUnmatchedDetections(const std::vector<int> dets_status,
                                        const std::vector<ObjectDetection_> &detections,
                                        std::vector<ObjectDetection_> &unmatched_dets) {
  /// <0,not matched; >=0,target id
  for (size_t i = 0; i < dets_status.size(); ++i) {
    auto det = detections[i];
    if (dets_status[i] < 0) {
      unmatched_dets.push_back(det);
    }
  }
}

void MotManager::TrackLifeManagement() {
  for (int i = fusion_tracks_.size() - 1; i >= 0; --i) {
    fusion_tracks_[i].trk.age++;
    if (fusion_tracks_[i].trk.updated == false) {
      //modify 强化小车用
      if (perception_mode == 0) {
        if (fusion_tracks_[i].trk.type == PEDESTRIAN_TYPE ) {
          fusion_tracks_[i].trk.confidence -= 0.08;
        }else if(IsCarOrTruck(fusion_tracks_[i].trk.type )){ // 增加车类目标的存在时长
          fusion_tracks_[i].trk.confidence -= 0.08;
        }else if(fusion_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY || fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_RADAR ){//增加毫米波结果的存在时长
            fusion_tracks_[i].trk.confidence -= 0.1;
        }else {
          fusion_tracks_[i].trk.confidence -= 0.2;
        }
      }else{
        if(IsCarOrTruck(fusion_tracks_[i].trk.type)){ // 增加车类目标的存在时长
            fusion_tracks_[i].trk.confidence -= 0.08;  
        }else if(fusion_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY || fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_RADAR ){//增加毫米波结果的存在时长
            fusion_tracks_[i].trk.confidence -= 0.1;
        } else{
            fusion_tracks_[i].trk.confidence -= 0.2;    
        } 
      }
      fusion_tracks_[i].trk.cells.clear();//gqp 20240926 移除飘飞的空目标
    }else {
      if (fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_VISION
          || fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_VISION_RADAR) {
        fusion_tracks_[i].trk.confidence += 0.4;
      }else if(IsCarOrTruck(fusion_tracks_[i].trk.type )){ // 增加车类目标的存在时长
          fusion_tracks_[i].trk.confidence += 0.3;
      }else if (fusion_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY ||       fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_RADAR ){//增加毫米波结果的存在时长
        fusion_tracks_[i].trk.confidence += 0.3;
      }else{
         fusion_tracks_[i].trk.confidence += 0.2; 
      }
    }
    fusion_tracks_[i].trk.confidence = fusion_tracks_[i].trk.confidence < 1.0 ? fusion_tracks_[i].trk.confidence : 1.0;
    fusion_tracks_[i].trk.updated = false;

    float DELETE_TRK_MAX_CONFI = 0.4;
    if (fusion_tracks_[i].trk.type == UNKNOWN_TYPE) {
      DELETE_TRK_MAX_CONFI = 0.6;
      if(fusion_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY || fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_RADAR){
          DELETE_TRK_MAX_CONFI = 0.4;
      }
    } 
    if(fusion_tracks_[i].trk.lidar_fake2vision>6){
      fusion_tracks_[i].trk.confidence=0;//杀掉只有激光，长时间没有视觉的目标
    }
    //add lxd 强化小车用
    if (perception_mode == 0) {
      if (fusion_tracks_[i].trk.type == PEDESTRIAN_TYPE) {
        DELETE_TRK_MAX_CONFI = 0.3;
      }
    }
    if (IsCarOrTruck(fusion_tracks_[i].trk.type )) {
        DELETE_TRK_MAX_CONFI = 0.3;
    }
    if (fusion_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY ||       fusion_tracks_[i].trk.fuse_source == SOURCE_LIDAR_RADAR) { // 增加毫米波结果的存在时长
        DELETE_TRK_MAX_CONFI = 0.3;
    }
    if (fusion_tracks_[i].trk.confidence <= DELETE_TRK_MAX_CONFI) {
      // if(fusion_tracks_[i].trk.x_center_vcs<20 && fusion_tracks_[i].trk.y_center_vcs<0.5 && fusion_tracks_[i].trk.y_center_vcs>-5){
      //   AINFO<<"gqp TrackLifeManagement erase:x:"<<fusion_tracks_[i].trk.x_center_vcs<<" y:"<<fusion_tracks_[i].trk.y_center_vcs
      //     <<" age:"<<fusion_tracks_[i].trk.age<<" fuse_source:"<<fusion_tracks_[i].trk.fuse_source<<" confidence:"<<fusion_tracks_[i].trk.confidence
      //     <<" type:"<<fusion_tracks_[i].trk.type<<" perception_mode:"<<perception_mode<<" DELETE_TRK_MAX_CONFI:"<<DELETE_TRK_MAX_CONFI
      //     ;
      // }
      fusion_tracks_.erase(fusion_tracks_.begin() + i);

    }
  }
}

void MotManager::FakeTrackLifeManagement() {
  for (int i = fake_tracks_.size() - 1; i >= 0; --i) {
    fake_tracks_[i].trk.age++;
    if (fake_tracks_[i].trk.updated == false) {
        fake_tracks_[i].trk.confidence -= 0.1;
    } else {
      if (fake_tracks_[i].trk.fuse_source == SOURCE_LIDAR_VISION
          || fake_tracks_[i].trk.fuse_source == SOURCE_LIDAR_VISION_RADAR) {
        fake_tracks_[i].trk.confidence += 0.4;
      } else if (fake_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY || fake_tracks_[i].trk.fuse_source == SOURCE_LIDAR_RADAR){
        fake_tracks_[i].trk.confidence += 0.3;
      } else{
        fake_tracks_[i].trk.confidence += 0.2;  
      }
    }
    fake_tracks_[i].trk.confidence = fake_tracks_[i].trk.confidence < 1.0 ? fake_tracks_[i].trk.confidence : 1.0;
    fake_tracks_[i].trk.updated = false;

    if (fake_tracks_[i].trk.confidence <= 0.1 || fake_tracks_[i].trk.age >= 10) {
      fake_tracks_.erase(fake_tracks_.begin() + i);
      continue;
    }
  }
  // transfer fake track to fusion track
  TransferFakeTrackToFusionTrack();
}


void MotManager::TransferFakeTrackToFusionTrack() {
  for (int i = fake_tracks_.size() - 1; i >= 0; --i) {
    float NEW_TRK_MIN_CONFI = 0.6;
    if (fake_tracks_[i].trk.type == UNKNOWN_TYPE) {
      NEW_TRK_MIN_CONFI = 0.9;
      if(fake_tracks_[i].trk.fuse_source == SOURCE_RADAR_ONLY){
          NEW_TRK_MIN_CONFI = 0.6;
      }
    }
    // modify for bl_cluster
    if (fake_tracks_[i].trk.type == UNKNOWN_TYPE && fake_tracks_[i].trk.is_bl_cluster) {
      NEW_TRK_MIN_CONFI = 0.6;
    }

    //modify for car type
    if (IsCarOrTruck(fake_tracks_[i].trk.type)) {
      NEW_TRK_MIN_CONFI = 0.5;
    }
    if (fake_tracks_[i].trk.confidence >= NEW_TRK_MIN_CONFI) {
      // get new velocity
      float vx = 0; float vy = 0;
      if (fake_tracks_[i].matched_radar_dets_.size() > 0
          && (fake_tracks_[i].trk.type == CAR_TYPE || fake_tracks_[i].trk.type == TRUCK_TYPE)) {
        float sum_vx = 0; float sum_vy = 0;
        for (auto &det : fake_tracks_[i].matched_radar_dets_) {
          sum_vx += det.vx_dr;
          sum_vy += det.vy_dr;
        }
        vx = sum_vx / fake_tracks_[i].matched_radar_dets_.size();
        vy = sum_vy / fake_tracks_[i].matched_radar_dets_.size();
      } else {
        vx = fake_tracks_[i].trk.vx_bbox_dr;
        vy = fake_tracks_[i].trk.vy_bbox_dr;
        if (fake_tracks_[i].trk.type == SUBULATE_TYPE || fake_tracks_[i].trk.type == ROADBLOCK_TYPE || fake_tracks_[i].trk.type == ANTITANK_TYPE || fake_tracks_[i].trk.type == UNKNOWN_UNMOVABLE_TYPE || fake_tracks_[i].trk.type == WATERFENCE_TYPE) { //modift by lxd
          vx = 0;
          vy = 0;
        }
      }
      fake_tracks_[i].trk.vx = vx;
      fake_tracks_[i].trk.vy = vy;
      fake_tracks_[i].trk.vx_bbox_dr = vx;
      fake_tracks_[i].trk.vy_bbox_dr = vy;
      fake_tracks_[i].trk.age = 0;
      fake_tracks_[i].trk.rms_heading.mean = atan2(vy, vx) / 3.14 * 180;
      fake_tracks_[i].InitFilterParams();
      fake_tracks_[i].ClearMatchedDetections();
      fusion_tracks_.push_back(fake_tracks_[i]);
      fake_tracks_.erase(fake_tracks_.begin() + i);
      continue;
    }
  }
}

void MotManager::InitNewFakeTrack(const std::vector<ObjectDetection_> lidarDets,
                                  const std::vector<ObjectDetection_> radarDets) {
  // creat new fake track from unmatched detections
  // lidar dets
  std::vector<ObjectDetection_> radarDets_ = radarDets; 
  for (const auto& det : lidarDets) {
    ObjectTrack trk;
    trk.system_time = det.system_time;
    trk.lidar_received_time = det.received_time;
    auto prev_fuse_point = trk.fuse_point;
    auto trk_type = trk.type;
    auto trk_heading_vcs = trk.heading_vcs;
    trk.is_top_bottom_reversed = false;
    bbox_.GetAnchorPoint(prev_fuse_point, trk_type, trk_heading_vcs, det, trk.is_top_bottom_reversed,
                         trk.fuse_point, trk.x, trk.y);
    trk.vx = 0;
    trk.vy = 0;
    trk.heading = det.heading_dr;
    trk.x_center_vcs = det.x_vcs;
    trk.y_center_vcs = det.y_vcs;
    trk.heading_vcs = det.heading_vcs;
    trk.heading_source = "lidar";

    trk.length = det.length;
    trk.width = det.width;
    trk.height = det.height;
    trk.length_predict = det.length;
    trk.width_predict = det.width;

    trk.age = 0;
    trk.type = det.type;
    trk.type_confidence += 1;
    trk.confidence = 0.2;
    trk.fuse_source = SOURCE_LIDAR_ONLY;
    if (det.l2cInfo.totalCamObjs >= 1) {
      int max_pts = 0;
      for (const auto &obj : det.l2cInfo.objs) {
        auto pts_num = obj.pointsNum;
        if (pts_num > max_pts) {
          trk.l2cInfo = det.l2cInfo;
          trk.l2cInfo.totalCamObjs = 1;
          trk.l2cInfo.objs.clear();
          trk.l2cInfo.objs.push_back(obj);
          max_pts = pts_num;
        }
      }
      trk.is_l2c_updated = true;
      trk.fuse_source = SOURCE_LIDAR_VISION;
      trk.confidence = 0.4;
      // for roadblocks
      if (det.type == UNKNOWN_UNMOVABLE_TYPE || det.type == SUBULATE_TYPE || det.type == ROADBLOCK_TYPE || det.type == ANTITANK_TYPE || det.type == WATERFENCE_TYPE ) {
        trk.confidence = 1.0;
      }
    }

    trk.cells = det.cells_vcs;
    bbox_.UpdateBBoxBuffer(trk, det);
    trk.updated = false;
    trk.det_in_img = det.det_in_img;
    //add by lxd
    trk.img_box_x = det.img_box_x;
    trk.img_box_y = det.img_box_y;
    trk.img_box_w = det.img_box_w;
    trk.img_box_h = det.img_box_h;
    trk.img_width = det.img_width;
    trk.img_height = det.img_height;
    trk.img_camId = det.img_camId;
    trk.is_bl_cluster = det.is_bl_cluster;
    if(config_.track_vision_must){// 临时处理措施：用于避免视觉目标迁移到纯激光目标后，导致纯激光目标持续跟踪
      trk.lidar_fake2vision=1;//设置为生效（>0）
    }

    FusionTracker new_fake_track(trk);
    new_fake_track.matched_lidar_dets_.push_back(det);
    // 判断当前fake帧radar检测结果和lidar检测结果是否重合
    std::vector<ObjectDetection_>::iterator it;
    for (it = radarDets_.begin(); it != radarDets_.end();) {
        if (fabs((*it).y_vcs <= 20)) {
            double dist_radar_lidar = sqrt(pow(trk.x_center_vcs-(*it).radarInfo.x,2) + pow(trk.y_center_vcs-(*it).radarInfo.y,2));
            if (dist_radar_lidar<6.0) {
                trk.radarInfo = det.radarInfo; //将radar信息融合给当前的lidar - trk
                new_fake_track.matched_radar_dets_.push_back(det);
                AINFO << "[fusion lidar - Radar tracks]" 
                            << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id 
                            << " | trk.x_center_vcs " << trk.x_center_vcs 
                            << " | trk.y_center_vcs " << trk.y_center_vcs
                            << " | trk.radarInfo.x " << trk.radarInfo.x
                            << " | trk.radarInfo.y " << trk.radarInfo.y
                           ;
                it = radarDets_.erase(it);
            } else {
                it++;
            }
        }
    }
    // AINFO << "[Generate New Lidar fake tracks]" 
    //             << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id 
    //             << " | track id " << trk.id
    //             << " | new_fake_track.trk.id " <<  new_fake_track.trk.id
    //             << " | trk.x_center_vcs " << trk.x_center_vcs 
    //             << " | trk.y_center_vcs " << trk.y_center_vcs
    //             << " | trk.radarInfo.x " << trk.radarInfo.x
    //             << " | trk.radarInfo.y " << trk.radarInfo.y
    //            ;
    fake_tracks_.push_back(new_fake_track);
  }

  // radar dets add by hhl : 根据radar检测结果生成新的track(筛选后)
    for (const auto& det : radarDets_) {
        if (fabs(det.y_vcs <= 20)) {
        ObjectTrack trk;
        trk.system_time = det.system_time;
        trk.lidar_received_time = det.received_time;
        trk.x = det.x_dr;
        trk.y = det.y_dr;
        trk.x_center_vcs = det.radarInfo.x;
        trk.y_center_vcs = det.radarInfo.y;
        // trk.vx = det.vx_dr;
        // trk.vy = det.vy_dr;
        trk.vx = 1.0;
        trk.vy = 1.0;
        trk.heading = det.heading_dr;
        trk.heading_vcs = det.heading_vcs;
        trk.heading_source = "radar";
        trk.length = det.length;
        trk.width = det.width > 0 ? det.width : 1.8;
        trk.length = det.length > 0 ? det.length : 4.4;
        trk.length_predict = trk.length;
        trk.width_predict = trk.width;
        trk.age = 0;
        trk.type = det.type;
        trk.type_confidence += 1;
        trk.confidence = 0.9;
        trk.fuse_source = SOURCE_RADAR_ONLY;
        trk.fuse_point = FUSE_POINT_BOTTOM;
        //   bbox_.GetBBoxCenter(trk, det.radarInfo.x, det.radarInfo.y);
        bbox_.UpdateBBoxBuffer(trk, det);

        trk.updated = false;
        trk.radarInfo = det.radarInfo;
        FusionTracker new_fake_track(trk);
        // 基于CT radar的id追踪进行id生成时的不切换
        ID_Maintain_CT_RADAR(new_fake_track.trk.radarInfo,new_fake_track);
        new_fake_track.matched_radar_dets_.push_back(det);
        fake_tracks_.push_back(new_fake_track);
        AINFO << "[Generate New Radar fake tracks]" 
                    << " | trk.radarInfo.radar_id " << trk.radarInfo.radar_id 
                    << " | track id " << trk.id
                    << " | new_fake_track.trk.id " <<  new_fake_track.trk.id
                    << " | trk.x_center_vcs " << trk.x_center_vcs 
                    << " | trk.y_center_vcs " << trk.y_center_vcs
                    << " | trk.radarInfo.x " << trk.radarInfo.x
                    << " | trk.radarInfo.y " << trk.radarInfo.y
                   ;
        }
  }
  // radar dets
  // for (const auto& det : radarDets) {
  //   auto x_min = config_.fake_track_config.radar_create_x_min;
  //   auto x_max = config_.fake_track_config.radar_create_x_max;
  //   auto y_min = config_.fake_track_config.radar_create_y_min;
  //   auto y_max = config_.fake_track_config.radar_create_y_max;
  //   if (fabs(det.x_vcs) > x_max || fabs(det.x_vcs) < x_min
  //       || fabs(det.y_vcs) > y_max || fabs(det.y_vcs) < y_min) {
  //     continue;
  //   }
  //   ObjectTrack trk;
  //   trk.system_time = det.system_time;
  //   trk.lidar_received_time = 0;

  //   trk.x = det.x_dr;
  //   trk.y = det.y_dr;
  //   trk.vx = det.vx_dr;
  //   trk.vy = det.vy_dr;
  //   trk.heading = det.heading_dr;
  //   trk.x_center_vcs = det.x_vcs;
  //   trk.y_center_vcs = det.y_vcs;
  //   trk.heading_vcs = det.heading_vcs;
  //   trk.heading_source = "radar";

  //   trk.length = det.length;
  //   trk.width = det.width;
  //   trk.height = det.height;
  //   trk.length_predict = det.length;
  //   trk.width_predict = det.width;

  //   trk.age = 0;
  //   trk.type = det.type;
  //   trk.confidence = 1.0;
  //   trk.fuse_source = SOURCE_RADAR_ONLY;
  //   if (det.x_vcs >= 0) {
  //     trk.fuse_point = FUSE_POINT_BOTTOM;
  //   } else {
  //     trk.fuse_point = FUSE_POINT_TOP;
  //   }

  //   bbox_.UpdateBBoxBuffer(trk, det);
  //   trk.updated = false;

  //   FusionTracker new_fake_track(trk);
  //   new_fake_track.matched_radar_dets_.push_back(det);
  //   fake_tracks_.push_back(new_fake_track);
  // }
}


void MotManager::Preprocess(const ThreadProfile &profile_lidar,
                            const std::vector<ObjectDetection_> &detections,
                            const ThreadProfile &profile_dor,
                            const DorDetection_ &dor_dets) {
  AINFO<<"mot input:"<<detections.size();
  for (const auto &det : detections) {
    switch (det.source & SOURCE_MASK) {
    case LIDAR_SOURCE:
      detections_lidar_.push_back(det);
      break;
    case RADAR_SOURCE:
      detections_radar_.push_back(det);
      break;
    case VISION_SOURCE:
      detections_vision_.push_back(det);
      break;
    default:
      break;
    }
  }
  AINFO << "[MOT MANAGER] Before Preprocess detections_lidar_ size" 
              <<  detections_lidar_.size();
  AINFO << "[MOT MANAGER] Before Preprocess detections_radar_ size" 
              <<  detections_radar_.size();
  AINFO << "[MOT MANAGER] Before Preprocess detections_vision_ size" 
              <<  detections_vision_.size();
  GetObjToBoundaryInfo(detections_lidar_);
  GetObjToBoundaryInfo(detections_radar_);
  GetObjToBoundaryInfo(detections_vision_);

  // pre-process lidar detections
  for (auto &det : detections_lidar_) {

    // 修改误检测车辆类别
    if(IsCarOrTruck(det.type) && det.is_bl_cluster){
        if(det.width < 0.6 || det.length < 0.6 || det.height < 0.6){
            det.type = UNKNOWN_TYPE;
        }
    }

    // add by hhl: filter car obj where length/width >= 6 or width /length >=6
    if(IsCarOrTruck(det.type)){
        if ((det.width / det.length) >= 6.0 || (det.length / det.width) >= 6.0 ){
            det.isTrueObj = false;
        }
        if(det.length > 11 || det.width > 11 || det.height > 11){
            det.isTrueObj = false;
        }
    }
    
    // add by hhl: filter in car weird obstacle
    if (fabs(det.x_vcs)<0.1 && fabs(det.y_vcs)<0.1){
        det.isTrueObj = false;
    }


    // check unknow objects status
    if (det.type == UNKNOWN_TYPE && perception_mode == 1) {
      if ((det.width / det.length) > 10.0 || (det.width / det.length) < 0.1
          || det.width > 10 || det.length > 30) {
        det.isTrueObj = false;
      }
    }

    // bl unknown_type ---> person
    // if (det.type == UNKNOWN_TYPE && det.is_bl_cluster ) {
    //   if (det.width > 0.1 && det.width < 0.5 && det.length > 0.1 && det.length < 0.5 && det.height > 0.4 && det.height < 2.0 && det.vx_vcs <= 5 && det.vy_vcs <= 5) {
    //     det.type = PEDESTRIAN_TYPE;
    //   }
    // }

    // use l2c info to fix top lidar PEDESTRIAN_TYPE dets in view range (&& !det.is_bl_cluster)
    if(vision_person_filter_){
        // if(det.type == PEDESTRIAN_TYPE){
        //     AINFO << " ++++++++ > person det : "
        //               << " | x : " << det.x_vcs
        //               << " | y : "<<det.y_vcs
        //               << " | det.det_in_img[0] " << det.det_in_img[0]
        //               << " | det.det_in_img[1] " << det.det_in_img[1]
        //               << " | det.det_in_img[2] " << det.det_in_img[2]
        //               << "det.l2cInfo.totalCamObjs  | : " << det.l2cInfo.totalCamObjs 
        //              ;
        // }
        if((det.det_in_img[0] == 1 ||det.det_in_img[1] == 1 || det.det_in_img[2] == 1) && det.type == PEDESTRIAN_TYPE  && det.l2cInfo.totalCamObjs == 0){ // 主激光雷达检测行人结果，且无反投影视觉融合信息，表明该感知目标没有视觉属性，则将该检测结果类别修改为unknown
            // AINFO << "mot manager change det type | "
            //         << det.type;
            // for (int i=0;i<det.l2cInfo.objs.size();i++){
            //     AINFO << "mot manager change det l2c norm type | "
            //             << det.l2cInfo.objs[i].norm_type
            //             ;
            // }
            det.type = UNKNOWN_TYPE;
            // AINFO<<"Reset person type to unkown";
        }
    }

    // bl person-check
    if (det.type == PEDESTRIAN_TYPE && det.is_bl_cluster ) {
      if (det.width <= 0.1 || det.width >= 2.5 || det.length <= 0.1 || det.length >= 2.5 || det.height <= 0.4 || det.height >= 2.0 || det.vx_vcs > 7 || det.vy_vcs > 7) {
        det.isTrueObj = false;
        // AINFO<<"Reset person type to non-object";
      }
    }

    // remove object in car area
    if(det.is_bl_cluster){
        if( fabs(det.x_vcs) < 1.5 && fabs(det.y_vcs) < 1.5 ){
            det.isTrueObj = false;
            // AINFO<<"Reset person type to22 non-object";
        }
    }


    // AINFO  << "------------------------------INFO:" << det.lidar2semantic.is_valid <<
    //        " |total_cloud_pts: " << det.lidar2semantic.total_cloud_pts <<
    //        " |det.y_vcs: " << det.y_vcs <<
    //        " |det.x_vcs: " << det.x_vcs;
    // use semantic segmentation results to filter noise or bring back object
    if (det.lidar2semantic.is_valid && det.lidar2semantic.total_cloud_pts > 0
        && fabs(det.y_vcs) <= 15 && det.x_vcs >= 5) {
      auto l2s = det.lidar2semantic;

      // int total_ignored_pts = l2s.background_pts;
      // if ( total_ignored_pts >= l2s.total_cloud_pts * 0.85
      //      && l2s.roadblock_pts == 0) {
      //   det.lidar2semantic.isTrueObj = false;
      //   det.isTrueObj = false;
      // }

      if (det.isTrueObj == true && IsPedestrainOrCyclist(det.type)) {
        if (l2s.roaduser_other_pts == 0 &&
            (l2s.background_pts > l2s.total_cloud_pts * 0.85 || l2s.road_curb_pts > l2s.total_cloud_pts * 0.85)) {
          det.lidar2semantic.isTrueObj = false;
          det.isTrueObj = false;
          // AINFO<<"Reset person type IsPedestrainOrCyclist non-object";
        } else if (l2s.roaduser_other_pts == 0 && l2s.roadblock_pts > l2s.total_cloud_pts * 0.85) {
          det.type = UNKNOWN_UNMOVABLE_TYPE;
          // AINFO<<"Reset person type IsPedestrainOrCyclist22 non-object";
        }
      }

      if (det.isTrueObj == true && IsCarOrTruck(det.type)) {
        if (l2s.roaduser_car_pts == 0 && (l2s.background_pts > l2s.total_cloud_pts * 0.85 || l2s.road_curb_pts > l2s.total_cloud_pts * 0.85)) {
          det.lidar2semantic.isTrueObj = false;
          det.isTrueObj = false;
        } else if (l2s.roaduser_car_pts == 0 && l2s.roadblock_pts > l2s.total_cloud_pts * 0.85) {
          det.type = UNKNOWN_UNMOVABLE_TYPE;
        }
      }

      int total_roaduser_pts = l2s.roaduser_car_pts + l2s.roaduser_other_pts;
      if (l2s.roadblock_pts >= l2s.total_cloud_pts * 0.75
          || ((total_roaduser_pts <= l2s.total_cloud_pts * 0.05)
              && (l2s.roadblock_pts >= l2s.total_cloud_pts * 0.5))) {
        det.type = UNKNOWN_UNMOVABLE_TYPE;
        // AINFO<<"Reset person type UNKNOWN_UNMOVABLE_TYPE non-object";
        if (det.isTrueObj == false) {
          det.lidar2semantic.isTrueObj = true;
          det.isTrueObj = true;
        }
      }
    }


    //  use image objects to bring some objects alive
    if (profile_lidar.module_activated == true) {
      det.l2cInfo.is_valid = true;
      if (det.isTrueObj == false && det.l2cInfo.totalCamObjs > 0) {
        det.isTrueObj = true;
        // AINFO<<"Reset person type isTrueObj";
      }
      if (det.l2cInfo.totalCamObjs == 1) {
        auto camObj = det.l2cInfo.objs[0];

        // //add by lxd 20211121
        //  det.type = camObj.norm_type;//以视觉类别为主-add by lxd 20211126
        //  if (det.type == det.l2cInfo.objs[0].norm_type) {
        //    det.heading_vcs = det.l2cInfo.objs[0].heading;
        //  }
      }
    }



  } // end for

    // fusion of top-lidar result and blind-lidar result
    std::vector<std::pair<int,int>> top_bl_fusion;
    for (size_t obj_id = 0; obj_id < detections_lidar_.size(); ++obj_id) {
        for (size_t obj_plus = obj_id; obj_plus < detections_lidar_.size(); ++obj_plus){
            if(obj_id != obj_plus ){
                // x / y check
                if(fabs(detections_lidar_[obj_id].x_vcs - detections_lidar_[obj_plus].x_vcs)<0.5 && fabs(detections_lidar_[obj_id].y_vcs - detections_lidar_[obj_plus].y_vcs)<0.5 ){
                    // AINFO<< "Same object at x : " << detections_lidar_[obj_id].x_vcs;
                    // 主雷达检测结果在first，补盲雷达聚类结果在second
                    if(detections_lidar_[obj_id].is_bl_cluster ==false){
                        top_bl_fusion.push_back(std::pair<int,int>(obj_id,obj_plus));
                        break;       
                    }
                    else if(detections_lidar_[obj_plus].is_bl_cluster ==false){
                        top_bl_fusion.push_back(std::pair<int,int>(obj_plus,obj_id));
                        break;       
                    }
                } 
                }
        }
    }
    for (size_t obj_id = 0; obj_id < top_bl_fusion.size(); ++obj_id){
        //type-change
        auto &det_first = detections_lidar_[top_bl_fusion[obj_id].first];
        auto &det_second = detections_lidar_[top_bl_fusion[obj_id].second];
        if(det_first.type != det_second.type){
            // 长宽高判断基本一致再进行融合 目前针对行人做融合
            if (fabs(det_first.width - det_second.width)<0.6 && fabs(det_first.length - det_second.length)<0.6 && fabs(det_first.height - det_second.height)<0.6){
                if(det_first.type == PEDESTRIAN_TYPE && det_second.type == UNKNOWN_TYPE){
                    det_second.type = det_first.type;
                }
                if(det_first.type == UNKNOWN_TYPE && det_second.type == PEDESTRIAN_TYPE){
                    det_first.type = det_second.type;
                }
            }
            // 宽度和高度判断基本一致再进行融合 针对车辆也做融合
            if (fabs(det_first.width - det_second.width)<0.6 && fabs(det_first.height - det_second.height)<0.6){
                if(IsCarOrTruck(det_first.type) && det_second.type == UNKNOWN_TYPE){
                    det_second.type = det_first.type;
                }
                if(det_first.type == UNKNOWN_TYPE && IsCarOrTruck(det_second.type)){
                    det_first.type = det_second.type;
                }
            }
        }else{
            continue;
        }
    }
    
    // save only one object and change is_bl_cluster property
    std::vector<int> top_bl_fusion_num(detections_lidar_.size(),1);
    for(size_t obj_id = 0; obj_id < top_bl_fusion.size(); ++obj_id){
        top_bl_fusion_num[top_bl_fusion[obj_id].second] = 0;
    }
    
    for (size_t obj_id = 0; obj_id < detections_lidar_.size(); ++obj_id){
        if(top_bl_fusion_num[obj_id] > 0){
            detections_lidar_new.push_back(detections_lidar_[obj_id]);
        }
    }
    detections_lidar_.clear();
    detections_lidar_ = detections_lidar_new;

//   // pre-process radar detections
//   for (auto &det : detections_radar_) {
//     if (det.geoInfo.distance < 0.5) {
//       det.isTrueObj = false;
//     }
//   }

    //  add by hhl： 范围滤除
    int radar_count = 0 ;
    for (auto &det : detections_radar_) {
        if ( fabs(det.x_vcs) >= 120 || fabs(det.y_vcs) >= 30) {
            det.isTrueObj = false;
            radar_count ++ ;
        }
    }

  VCS2DRTransForm(detections_lidar_);
  VCS2DRTransForm(detections_radar_);
  VCS2DRTransForm(detections_vision_);

  // associate static points to lidar detections
//   const int MAX_POINTS = 300;
//   if (dor_dets.cloudPtr_static != nullptr) {
//     for (const auto& p : dor_dets.cloudPtr_static->points) {
//       for (auto &det : detections_lidar_) {
//         if (det.x_vcs < 0 || det.static_points_num >= MAX_POINTS) { continue; }
//         pcl::PointCloud<pcl::PointXYZI> poly;
//         pcl::PointCloud<pcl::PointXYZI> pts;
//         pts.x = det.lidar_bbox_vcs.geoPoints[TOP_LEFT].x;
//         pts.y = det.lidar_bbox_vcs.geoPoints[TOP_LEFT].y;
//         poly.points.push_back(pts);
//         pts.x = det.lidar_bbox_vcs.geoPoints[TOP_RIGHT].x;
//         pts.y = det.lidar_bbox_vcs.geoPoints[TOP_RIGHT].y;
//         poly.points.push_back(pts);
//         pts.x = det.lidar_bbox_vcs.geoPoints[BOTTOM_RIGHT].x;
//         pts.y = det.lidar_bbox_vcs.geoPoints[BOTTOM_RIGHT].y;
//         poly.points.push_back(pts);
//         pts.x = det.lidar_bbox_vcs.geoPoints[BOTTOM_LEFT].x;
//         pts.y = det.lidar_bbox_vcs.geoPoints[BOTTOM_LEFT].y;
//         poly.points.push_back(pts);
//         if (CheckPointInPolygon<pcl::PointXYZ>(p, poly)) {
//           // AINFO  << "inside poly :" << p.x << "," << p.y;
//           det.static_points_num++;
//         }
//       }
//     }
//   }

  const int MIN_POINTS_VEHICLE = 150;
  const int MIN_POINTS_OTHER = 50;
  for (auto &det : detections_lidar_) {
    det.is_dor_valid = profile_dor.module_activated;
    if (IsCarOrTruck(det.type)) {
      if (det.static_points_num >= MIN_POINTS_VEHICLE) {
        det.is_static = true;
      }
    } else {
      if (det.static_points_num >= MIN_POINTS_OTHER) {
        det.is_static = true;
      }
    }
  }
  // end pre-process
  return;
}

void MotManager::RecalcObjCenterAndShape(ObjectDetection_ &det) {
  pcl::PointCloud<pcl::PointXYZ>::Ptr clouds(new pcl::PointCloud<pcl::PointXYZ>);
  for (const auto& cell : det.cells_vcs) {
    pcl::PointXYZ p_xyz;
    p_xyz.x = cell.x;
    p_xyz.y = cell.y;
    p_xyz.z = 0;
    clouds->points.emplace_back(p_xyz);
  }
  if (det.cells_vcs.size()) {
    pcl::MomentOfInertiaEstimation <pcl::PointXYZ> feature_extractor;
    feature_extractor.setInputCloud(clouds);
    feature_extractor.compute();
    pcl::PointXYZ min_point_OBB;
    pcl::PointXYZ max_point_OBB;
    pcl::PointXYZ position_OBB;
    Eigen::Matrix3f rotational_matrix_OBB;
    feature_extractor.getOBB(min_point_OBB, max_point_OBB, position_OBB, rotational_matrix_OBB);
    det.x_vcs = position_OBB.x;
    det.y_vcs = position_OBB.y;
    float length = max_point_OBB.x - min_point_OBB.x;
    float width = max_point_OBB.y - min_point_OBB.y;
    Eigen::Vector3f rot3f = rotational_matrix_OBB.eulerAngles(2, 1, 0);
    float heading_vcs = rot3f[0] / 3.14 * 180.0;
    AINFO  << "zr-debug: old = " << det.length << "," << det.width << "," << det.heading_vcs
           << ", new = " << length << "," << width << "," << heading_vcs;
    det.length = length >= 0.2 ? length : 0.2;
    det.width = width >= 0.2 ? width : 0.2;
    det.heading_vcs = heading_vcs;
  }
}


template <class T>
bool MotManager::CheckPointInPolygon(const T & pt,
                                     const pcl::PointCloud<pcl::PointXYZI> & bounding_polygon) {
  if (bounding_polygon.points.empty()) return false;
  int counter = 0;
  int i;
  double xinters;
  pcl::PointXYZI p1;
  pcl::PointXYZI p2;
  int N = bounding_polygon.points.size();
  p1 = bounding_polygon.points.at(0);
  for (i = 1; i <= N; i++) {
    p2 = bounding_polygon.points.at(i % N);
    if (pt.y > std::min<float>(p1.y, p2.y)) {
      if (pt.y <= std::max<float>(p1.y, p2.y)) {
        if (pt.x <= std::max<float>(p1.x, p2.x)) {
          if (p1.y != p2.y) {
            xinters = (pt.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
            if (p1.x == p2.x || pt.x <= xinters) counter++;
          }
        }
      }
    }
    p1 = p2;
  }
  if (counter % 2 == 0)
    return false;
  else
    return true;
}


void MotManager::GetObjToBoundaryInfo(std::vector<ObjectDetection_> &detections) {
  for (auto &det : detections) {
    double distance = 0.0;
    if (PointDistanceToFreeSpace(det.x_vcs, det.y_vcs, distance)) {
      det.geoInfo.isvalid = true;
      det.geoInfo.distance = distance;
    }
  }
}

void MotManager::RemoveFalseDetections(std::vector<ObjectDetection_> &detections) {
  auto iter = detections.begin();
  for (; iter != detections.end(); ) {
    if (iter->isTrueObj == false) {
      AINFO  << "------------Fusion: erase obj: " << iter->x_vcs << " | " << iter->y_vcs << " | " << iter->type;
      detections.erase(iter);
    } else {
      iter ++;
    }
  }
}

void MotManager::VCS2DRTransForm(std::vector<ObjectDetection_> &detections) {
  // calculate object dr position
  for (auto &obj : detections) {
    Eigen::Vector3d obj_vcs_pos(obj.x_vcs, obj.y_vcs, 0);
    auto obj_dr_pos = own_dr_rotation_ * obj_vcs_pos + own_dr_pos_;

    Eigen::Vector3d obj_vcs_vel(obj.vx_vcs, obj.vy_vcs, 0);
    Eigen::Vector3d own_dr_vel(locpos_.speed - locpos_.yawrate * DEG2RAD * obj_vcs_pos[1],
                               locpos_.yawrate * DEG2RAD * obj_vcs_pos[0],
                               0);
    auto obj_dr_vel = own_dr_rotation_ * (obj_vcs_vel + own_dr_vel);

    // fill in tf data
    obj.x_dr = obj_dr_pos[0];
    obj.y_dr = obj_dr_pos[1];
    if ((obj.source & SOURCE_MASK) == RADAR_SOURCE) {
    //   obj.vx_dr = obj_dr_vel[0];
    //   obj.vy_dr = obj_dr_vel[1];
    //   obj.heading_dr = atan2(obj.vy_dr, obj.vx_dr) * RAD2DEG;
    //   float obj_vcs_heading = obj.heading_dr - locpos_.dr_yaw;
    //   obj.heading_vcs = NormalizeHeading(obj_vcs_heading);
      obj.heading_vcs = 0;
    } else if ((obj.source & SOURCE_MASK) == LIDAR_SOURCE) {
      float obj_dr_heading = obj.heading_vcs + locpos_.dr_yaw;
    //   float obj_dr_heading = obj.heading_vcs;
      obj.heading_dr = NormalizeHeading(obj_dr_heading);
    } else if ((obj.source & SOURCE_MASK) == VISION_SOURCE) {
      if (obj.l2cInfo.totalCamObjs == 1
          && obj.l2cInfo.is_cluster_obj == true
          && obj.l2cInfo.objs[0].v_valid == true) {
        obj.vx_dr = obj.l2cInfo.objs[0].vx;
        obj.vy_dr = obj.l2cInfo.objs[0].vy;
        obj.heading_dr = atan2(obj.vy_dr, obj.vx_dr) * RAD2DEG;
        float obj_vcs_heading = obj.heading_dr - locpos_.dr_yaw;
        // obj_vcs_heading = obj.heading_vcs;
        obj.heading_vcs = NormalizeHeading(obj_vcs_heading);
        obj.vision_velo_valid = true;
      }
    }

    // 2021-04-28, zr-add
    // process lidar detection
    if (obj.x_vcs >= 0 && obj.l2cInfo.totalCamObjs >= 1 ) {
      // sort the camobjs by track age
      auto clusterObj = obj.l2cInfo.objs[0];
      int trk_age = clusterObj.track_age;
      for (const auto& it : obj.l2cInfo.objs) {
        if (it.track_age > trk_age) {
          clusterObj = it;
          trk_age = it.track_age;
        }
      }
      if ((clusterObj.orientation_id == 3 || clusterObj.orientation_id == 4 || clusterObj.orientation_id == 5)
          && (clusterObj.norm_type == CAR_TYPE || clusterObj.norm_type == TRUCK_TYPE)) {
        // case 1: process unknown types
        if ((obj.type == UNKNOWN_TYPE || obj.type == SUBULATE_TYPE || obj.type == ROADBLOCK_TYPE || obj.type == ANTITANK_TYPE) && fabs(obj.y_vcs) <= 2.5) {
          obj.heading_vcs = 0;
          obj.type = clusterObj.norm_type;
          // use clustered position
          if (obj.x_vcs >= 80) {
            obj.x_vcs = clusterObj.x + obj.length / 2;
            obj.y_vcs = clusterObj.y;
            obj.l2cInfo.is_cluster_obj = true;
          }
        }
      }
    }

    // float obj_dr_heading = obj.heading_vcs + locpos_.dr_yaw;
    // obj.heading_dr = NormalizeHeading(obj_dr_heading);

    if ((obj.source & SOURCE_MASK) == LIDAR_SOURCE) {
      // geo point in vsc
      obj.lidar_bbox_vcs.timestamp = pstart_time_;
      obj.lidar_bbox_vcs.geoPoints =
        bbox_.GetLidarBoundingBox(obj.x_vcs, obj.y_vcs, obj.length, obj.width, obj.heading_vcs);
      obj.lidar_bbox_vcs.fusion_point = obj.lidar_bbox_vcs.geoPoints[CENTER];

      // geo points in dr
      obj.lidar_bbox_dr.timestamp = pstart_time_;
      for (int i = 0; i < 9; i++) {
        obj.lidar_bbox_dr.geoPoints[i].isvalid = true;
        Eigen::Vector3d obj_vcs_pos(obj.lidar_bbox_vcs.geoPoints[i].x,
                                    obj.lidar_bbox_vcs.geoPoints[i].y,
                                    0);
        auto obj_dr_pos = own_dr_rotation_ * obj_vcs_pos + own_dr_pos_;
        obj.lidar_bbox_dr.geoPoints[i].x = obj_dr_pos[0];
        obj.lidar_bbox_dr.geoPoints[i].y = obj_dr_pos[1];
      }
      obj.lidar_bbox_dr.fusion_point = obj.lidar_bbox_dr.geoPoints[CENTER];
    }

    // 20210928: calculate cell xg yg
    if ((obj.source & SOURCE_MASK) == LIDAR_SOURCE) {
      // 20211013, temp fix: lidar cellsize
      if (obj.cells_vcs.size() <= 2) {
        // AINFO  << "warning: lidarobj cellsize = " << obj.cells_vcs.size()
        //        << ", x|y = " << obj.x_vcs << "," << obj.y_vcs
        //        << ", length|width = " << obj.length << "," << obj.width;
        obj.cells_vcs.clear();
        OccupyCell_ cell;
        cell.x = obj.lidar_bbox_vcs.geoPoints[TOP_LEFT].x;
        cell.y = obj.lidar_bbox_vcs.geoPoints[TOP_LEFT].y;
        obj.cells_vcs.push_back(cell);
        cell.x = obj.lidar_bbox_vcs.geoPoints[TOP_RIGHT].x;
        cell.y = obj.lidar_bbox_vcs.geoPoints[TOP_RIGHT].y;
        obj.cells_vcs.push_back(cell);
        cell.x = obj.lidar_bbox_vcs.geoPoints[BOTTOM_LEFT].x;
        cell.y = obj.lidar_bbox_vcs.geoPoints[BOTTOM_LEFT].y;
        obj.cells_vcs.push_back(cell);
        cell.x = obj.lidar_bbox_vcs.geoPoints[BOTTOM_RIGHT].x;
        cell.y = obj.lidar_bbox_vcs.geoPoints[BOTTOM_RIGHT].y;
        obj.cells_vcs.push_back(cell);
      }
      for (auto &cell : obj.cells_vcs) {
        Eigen::Vector3d cell_rel_pos(cell.x, cell.y, 0);
        auto cell_utm_pos = own_utm_rotation_ * cell_rel_pos + own_utm_pos_;
        cell.xg = cell_utm_pos[0];
        cell.yg = cell_utm_pos[1];
      }
    }
  } // end for
}

void MotManager::PredictTracks() {
  for (auto &track : fusion_tracks_) {
    // if(track.trk.x_center_vcs<20 && track.trk.y_center_vcs<0.5 && track.trk.y_center_vcs>-5){
    //   AINFO<<"gqp PredictTracks before:x:"<<track.trk.x_center_vcs<<" y:"<<track.trk.y_center_vcs
    //     <<" age:"<<track.trk.age<<" fuse_source:"<<track.trk.fuse_source<<" confidence:"<<track.trk.confidence
    //     <<" type:"<<track.trk.type
    //     ;
    // }
    track.PredictFusionTrack();
    // Eigen::Vector3d obj_dr_pos(track.trk.x, track.trk.y, 0);
    // auto obj_vcs_pos = own_dr_rotation_inverse_ * (obj_dr_pos - own_dr_pos_);
    // bbox_.GetBBoxCenter(track.trk, obj_vcs_pos[0], obj_vcs_pos[1]);
  }
}

void MotManager::FillInDataOutput(FusionDataType_ & data_out) {
  // copy fusion track to output
  data_out.objs.clear();
  data_out.cells.clear();
  data_out.time = lidar_frame_time_;
  track_ids.clear();
  std::vector<FusionObject_> out_last_frame_tmp = out_last_frame;
  out_last_frame.clear();
  for (auto &track : fusion_tracks_) {
    FusionObject_ out;
    out.id = track.trk.id;
    track_ids.push_back(track.trk.id);
    out.type = track.trk.type;

    //   out.type = UNKNOWN_TYPE;
    // }
    // 水马等静态障碍物输出为unknown类别
    //remove by lxd
    // if (track.trk.type == UNKNOWN_UNMOVABLE_TYPE) {
    //   out.type = UNKNOWN_TYPE;
    // }
    out.age = track.trk.age;
    out.length = track.trk.length_predict;
    out.width = track.trk.width_predict;
    out.height = track.trk.height;
    out.dr_heading = track.trk.heading; // under DR
    out.heading = track.trk.heading_vcs;
    out.confidence = track.trk.confidence;
    out.source = track.trk.fuse_source;
    // 2021-05-13: use this field to indicate static/dynamic status
    out.velocitystatus = track.trk.is_trk_static;

    out.det_in_img = track.trk.det_in_img;
    //add by lxd 2021-11-16
    out.img_box_x = track.trk.img_box_x;
    out.img_box_y = track.trk.img_box_y;
    out.img_box_w = track.trk.img_box_w;
    out.img_box_h = track.trk.img_box_h;
    // out.img_width = track.trk.img_width;
    // out.img_height = track.trk.img_height;
    out.img_camId = track.trk.img_camId;
    // out.l2cInfo = track.trk.l2cInfo;
    //add by lxd
    // 使用长宽高的均值作为输出，若此次均值与上次均值波动小于阈值则使用上次均值
    if (steady_size_) {
      if (id_length[track.trk.id].size() > 100) {
        mean_length = accumulate(id_length[track.trk.id].begin(), id_length[track.trk.id].end(), 0.0) / id_length[track.trk.id].size();
        mean_width =  accumulate(id_width[track.trk.id].begin(), id_width[track.trk.id].end(), 0.0)   / id_width[track.trk.id].size();
        mean_height = accumulate(id_height[track.trk.id].begin(), id_height[track.trk.id].end(), 0.0) / id_height[track.trk.id].size();

        out.length = mean_length;
        out.width = mean_width;
        out.height = mean_height;
        int erase_num = 1;
        id_length[track.trk.id].erase(id_length[track.trk.id].begin(), id_length[track.trk.id].begin() + erase_num);
        id_width[track.trk.id].erase(id_width[track.trk.id].begin(), id_width[track.trk.id].begin() + erase_num);
        id_height[track.trk.id].erase(id_height[track.trk.id].begin(), id_height[track.trk.id].begin() + erase_num);
      }

      id_length[track.trk.id].push_back(track.trk.length_predict);
      id_width[track.trk.id].push_back(track.trk.width_predict);
      id_height[track.trk.id].push_back(track.trk.height);
    }
    // std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now(), end;
    TransForm(track.trk, out);
    // end = std::chrono::high_resolution_clock::now();
    // AINFO << "TransForm : " <<std::fixed<< (std::chrono::duration<double>(end - start).count())*1e3;
    ObstacleSizeFilter(track.trk,out);
    std::string trk_heading_source = track.trk.heading_source;
    // HeadingUtmOpimize(out,out_last_frame_tmp,trk_heading_source);
    // HeadingUtmOpimizeNew(out,out_last_frame_tmp,trk_heading_source);
    // 融合上视觉的bl_cluster的heading置0
    // if(track.trk.is_bl_cluster){
    //     out.heading = 0;
    // }
    //静止的车辆障碍物搜寻是否存在匹配的上一帧障碍物
    // StaticCarOptimize(track.trk, out,out_last_frame_tmp);

    // 如果有radar匹配结果，类别保持和长宽高重定义
    for(int i=0;i<radar_track_type.size();i++){
        if(track.trk.radarInfo.radar_id == radar_track_type[i].first){
            out.type = radar_track_type[i].second;
            if(IsCarOrTruck(out.type)){
                out.length = 4.0;
                out.width = 2.0;
                out.height = 2.0;
            }else if (out.type == PEDESTRIAN_TYPE){
                out.length = 0.8;
                out.width = 0.8;
                out.height = 2.0;
            }else{
                out.length = 1.0;
                out.width = 1.0;
                out.height = 1.0;
            }
        }
    }
    
    data_out.objs.push_back(out);
    out_last_frame.emplace_back(out);

  }
  if (steady_size_) {
    //删除未在跟踪列表的id
    steady_ids.clear();
    std::map<uint32_t, std::vector<float>>::iterator  it_;
    for (it_ = id_length.begin(); it_ != id_length.end(); it_++) {
      steady_ids.push_back(it_->first);
    }
    for (auto &e_id : steady_ids) {
      if (std::find(track_ids.begin(), track_ids.end(), e_id) == track_ids.end()) {
        id_length.erase(e_id);
        id_width.erase(e_id);
        id_height.erase(e_id);
      }
    }
  }
}

void MotManager::ObstacleSizeFilter(ObjectTrack & trk,FusionObject_ &out){
    // if (trk.type == PEDESTRIAN_TYPE && out.speed<0.5 && trk.is_bl_cluster) {
    //     out.type = UNKNOWN_TYPE;
    // }
    if (trk.type == PEDESTRIAN_TYPE && trk.is_bl_cluster) {//补盲行人目标尺寸筛选
        if (out.width > 2.0 || out.length > 2.0 ||out.width+out.length <0.4 || out.width+out.length>2.8){
            out.type = UNKNOWN_TYPE;
        }
        if(out.speed > 4){
            out.type = UNKNOWN_TYPE;
        }
    }
    if (trk.type == PEDESTRIAN_TYPE ) {//行人目标尺寸筛选
        if (out.width > 1.0 || out.height > 2.0 || out.length > 1.0 || out.width < 0.2 || out.length < 0.2 || out.height < 0.3 ){
            out.type = UNKNOWN_TYPE;
        }
    }
    if (trk.type == CYCLIST_TYPE ) { //骑行者目标尺寸筛选
        if (out.width > 2.0 || out.height > 2.0 || out.length > 2.0 || out.width < 0.3 || out.height < 0.4 || out.length < 0.3)
        out.type = UNKNOWN_TYPE;
    }
    if (IsCarOrTruck(trk.type)) { //车辆目标尺寸筛选
        if ((out.width / out.length) >= 6.0 || (out.length / out.width) >= 6.0 ){
            out.type = UNKNOWN_TYPE;
        }
        if(out.length > 12 || out.width > 12 || out.height > 12){
            out.type = UNKNOWN_TYPE;
        }
    }


}
void MotManager::StaticCarOptimize(ObjectTrack & trk,FusionObject_ &out,std::vector<FusionObject_> &out_last_frame_tmp){
    if (IsCarOrTruck(trk.type) && out.speed<1.0 ) { 
        for(const auto out_old:out_last_frame_tmp){
            double dist_check = sqrt(pow(out_old.xabs - out.xabs ,2)+pow(out_old.yabs - out.yabs ,2));
            // if(out.id == out_old.id  && dist_check < 1.0 && out_old.speed < 0.5){
            if(out.id == out_old.id  && abs(out_old.xabs - out.xabs) < 1.0 && abs(out_old.yabs - out.yabs) < 1.0 && out_old.speed < 1.0){
                out.xabs = 0.8 * out_old.xabs + 0.2 * out.xabs ; 
                out.yabs = 0.8 * out_old.yabs + 0.2 * out.yabs ;
                Eigen::Vector3d obj_utm_pos_tmp(out.xabs,out.yabs,0);
                auto obj_vcs_pos_new = own_utm_rotation_.inverse() * (obj_utm_pos_tmp - own_utm_pos_);
                if(sqrt(pow(obj_vcs_pos_new[0] - out_old.x ,2)+pow(obj_vcs_pos_new[1] - out_old.y ,2)) < 1.0){ //相对坐标不应该很远，避免定位跳动导致的问题
                    out.x = obj_vcs_pos_new[0];
                    out.y = obj_vcs_pos_new[1];
                    trk.x_center_vcs = obj_vcs_pos_new[0];
                    trk.y_center_vcs = obj_vcs_pos_new[1];
                    trk.bbox_dr_buffer.back().fusion_point.x = obj_vcs_pos_new[0];
                    trk.bbox_dr_buffer.back().fusion_point.y = obj_vcs_pos_new[1];
                    // calculate object utm position and velocity
                    // AINFO << " ============== use last frame static obstacle id : " 
                    //         << out_old.id ;
                    // AINFO << " | out o old " << out_old.x
                    //         << " | out y old " << out_old.y 
                    //         << " | out speed  old "<< out_old.speed 
                    //         << " | out heading  old "<< out_old.heading 
                    //         ;
                    // AINFO << " | out new x "<< obj_vcs_pos_new[0]
                    //         << " | out new y "<< obj_vcs_pos_new[1]
                    //         << " | out new speed  "<< out.speed 
                    //         << " | out new heading  "<< out.heading 
                    //         ;
                    break;

                }
            }
        }
    }
}

void MotManager::HeadingUtmOpimize(FusionObject_ &out,std::vector<FusionObject_> &out_last_frame_tmp,std::string &trk_heading_source){
    // 航向优化,id相同，较大的航向跳动采用历史航向，较小跳动的航向采用加权航向
    for(const auto out_old:out_last_frame_tmp){
        if(IsCarOrTruck(out.type) && out.id == out_old.id){
            // 存在视觉航向采用视觉航向
            bool vision_heading = (trk_heading_source.find("vision") != string::npos);
            if(vision_heading){return;}
            if(out_old.heading * out.heading < 0 ){
                // 1. 抖动跨越边界
                if((abs(out_old.heading)+abs(out.heading)>4 && abs(out_old.heading)+abs(out.heading) < 20)||(abs(out_old.heading)+abs(out.heading)>356 && abs(out_old.heading)+abs(out.heading) < 360)){ 
                    AINFO << " +++++ - - - - heading cross border : weighted | id : " << out.id 
                    << " det heading "<< out.heading;
                    if (abs(out_old.heading )> 160 && abs(out_old.heading ) < 180){
                        if(out.speed < 0.8 ){//静止更相信历史航向
                        out.heading = 0.9 * out_old.heading - 0.1* out.heading;      
                        }else{
                        out.heading = 0.5 * out_old.heading - 0.5* out.heading;   
                        } 
                    }else if (abs(out_old.heading )<20 && abs(out_old.heading ) >0){
                        if(out.speed < 0.8 ){//静止更相信历史航向
                        out.heading = 0.9 * out_old.heading + 0.1* out.heading;      
                        }else{
                        out.heading = 0.5 * out_old.heading + 0.5* out.heading;   
                        } 
                    }

                    AINFO << "after processing"
                      << " old " << out_old.heading
                      << " new " << out.heading
                     ; 
                }
                // 2. 突变180度
                else if((abs(out_old.heading)+abs(out.heading)>150 && abs(out_old.heading)+abs(out.heading) < 210)){ 
                    AINFO << " +++++ - - - - heading reverse : +-180 | id :  " << out.id << " det heading "<< out.heading;
                    if(out_old.heading < 0){out.heading = out.heading - 180;}
                    else{out.heading = out.heading + 180;}
                    AINFO << " after processing " << out.id 
                    << " old " << out_old.heading
                    << " new " << out.heading
                   ;
                }
                // 3. 抖动较大 航向错误
                else if((abs(out_old.heading)+abs(out.heading)>20 && abs(out_old.heading)+abs(out.heading) < 150)||(abs(out_old.heading)+abs(out.heading)>210 && abs(out_old.heading)+abs(out.heading) < 340)){
                    AINFO << " +++++ - - - - heading wrong : use old | id : " << out.id << " old " << out_old.heading
                      << " new " << out.heading;
                    out.heading = out_old.heading;
                }    
            }
            if(out_old.heading * out.heading > 0 ){
                //正常的抖动范围根据速度speed和转弯半径R（5m）来算 
                // theta = speedx0.1x180/(pi^2xR）
                // double theta =(out_old.speed * 0.1 * 180)/(pow(3.1415926,2)*5);
                // AINFO << " max heading theta = " << theta;
                //  1. 航向抖动较小，采用融合航向
                if(abs(abs(out.heading) - abs(out_old.heading))>4.0 && abs(abs(out.heading) - abs(out_old.heading))<20.0){
                    AINFO << " +++++ +++++ heading bouncing : weighted | id : " << out.id << " det heading "<< out.heading;
                    if(out.speed < 0.8 ){//静止更相信历史航向
                      out.heading = 0.9 * out_old.heading + 0.1* out.heading;      
                    }else{ //动态更相信检测航向
                      out.heading = 0.5 * out_old.heading + 0.5* out.heading;   
                    }    
                    AINFO << " after processing " 
                    << " old " << out_old.heading
                    << " new " << out.heading; 
                }
                // 2.突变180度左右
                else if(abs(abs(out.heading) - abs(out_old.heading)) > 150){ 
                    AINFO << " +++++ +++++ heading reverse : +-180 | id : " << out.id 
                    << " det heading "<< out.heading;
                    if(out_old.heading < 0){out.heading = - out.heading - 180;}
                    else{out.heading = out.heading + 180;}
                    AINFO << " after processing " 
                    << " old " << out_old.heading
                    << " new " << out.heading;
                }
                // 3. 航向错误
                else if(abs(abs(out.heading) - abs(out_old.heading)) > 20 && abs(abs(out.heading) - abs(out_old.heading)) <150 ) {
                    AINFO << " +++++ ++++ heading wrong : use old | id : " << out.id;
                    out.heading =   out_old.heading;
                }     
            }
        }
    }
    out.heading = NormalizeHeading(out.heading);
}

void MotManager::HeadingUtmOpimizeNew(FusionObject_ &out,std::vector<FusionObject_> &out_last_frame_tmp,std::string &trk_heading_source){
    // 航向优化,id相同，较大的航向跳动采用历史航向，较小跳动的航向采用加权航向
    for(const auto out_old:out_last_frame_tmp){
        if(IsCarOrTruck(out.type) && out.id == out_old.id){
            // 存在视觉航向采用视觉航向
            bool vision_heading = (trk_heading_source.find("vision") != string::npos);
            if(vision_heading){return;}
            double out_old_heading_tmp = out_old.heading + 180;
            double out_heading_tmp = out.heading + 180;
            double difference = abs(out_old_heading_tmp - out_heading_tmp);
            bool cross_border = false;
            if (difference > 210){
                cross_border = true;
                difference = abs(difference - 360); 
            }   
            // 轻微抖动
            if ( difference>4 && difference<20){
                AINFO << " ++++++++++++ heading bouncing : weighted | id : " << out.id 
                << " out_old_heading_tmp "<< out_old_heading_tmp 
                << " out_heading_tmp "<<out_heading_tmp
                << " cross_border "<<cross_border
               ;
                if (!cross_border){
                    if(out.speed < 0.8 ){//静止更相信历史航向
                    out.heading = 0.9*out_old_heading_tmp + 0.1* out_heading_tmp - 180;      
                    }else{
                    out.heading = 0.5*out_old_heading_tmp + 0.5* out_heading_tmp - 180;   
                    } 
                }else{
                    out.heading = 0.5*out_old_heading_tmp + 0.5* out_heading_tmp;   
                    if(out.heading > 180){
                        out.heading = out.heading - 360;
                    }
                }

                AINFO << " after processing " 
                << " new " << out.heading;
            }
            // 抖动过大
            if ( difference>20 && difference<150){
                AINFO << " +++++++++++++++++++++++++ wrong " << out.id 
                << " out_old_heading_tmp "<< out_old_heading_tmp 
                << " out_heading_tmp "<<out_heading_tmp
               ;
                out.heading =  out_old_heading_tmp - 180;
            }
            //
            // 跳变180度
            if ( difference>150 && difference<210){
                AINFO << " +++++++++++++++++++++++ reverse " << out.id 
                << " out_old_heading_tmp "<< out_old_heading_tmp 
                << " out_heading_tmp "<<out_heading_tmp
               ;
                if(out_old_heading_tmp >= out_heading_tmp){
                    out.heading =  out_heading_tmp ;
                }else{
                    out.heading =  out_heading_tmp - 360;
                }
                AINFO << " after processing " 
                << " new " << out.heading;
            }
            
        }
    }
    out.heading = NormalizeHeading(out.heading);
}

void MotManager::TransForm(ObjectTrack & trk, FusionObject_ & out) {
  // output predict obj center
  float dt = 0.1;
  if (IsTrkUpdated(trk.fuse_source) == false) {
    trk.x_center_vcs += dt * trk.vx_vcs;
    trk.y_center_vcs += dt * trk.vy_vcs;
    // trk.z_center_vcs += dt * trk.vy_vcs;
  }
  Eigen::Vector3d obj_vcs_pos(trk.x_center_vcs, trk.y_center_vcs, trk.z_center_vcs);

  // calculate object vcs velocity
  float dr_vx = trk.vx;
  float dr_vy = trk.vy;

  // 2021-07-26: use stablized heading for pedstrain and cyclist
  if (trk.type == PEDESTRIAN_TYPE || trk.type == CYCLIST_TYPE) {
    float spd = sqrt(pow(dr_vx, 2) + pow(dr_vy, 2));
    dr_vx = spd * cos(trk.rms_heading.mean / 180 * 3.14);
    dr_vy = spd * sin(trk.rms_heading.mean / 180 * 3.14);
  }

  Eigen::Vector3d obj_dr_vel(dr_vx, dr_vy, 0);
  Eigen::Vector3d own_dr_vel(locpos_.speed - locpos_.yawrate * DEG2RAD * obj_vcs_pos[1],
                             locpos_.yawrate * DEG2RAD * obj_vcs_pos[0],
                             0);
  auto obj_vcs_vel = own_dr_rotation_inverse_ * obj_dr_vel - own_dr_vel;
  out.x = obj_vcs_pos[0];
  out.y = obj_vcs_pos[1];
  out.z = obj_vcs_pos[2];
  out.vxrel = obj_vcs_vel[0];
  out.vyrel = obj_vcs_vel[1];
  trk.vx_vcs = obj_vcs_vel[0];
  trk.vy_vcs = obj_vcs_vel[1];

  // calculate object utm position and velocity
  auto obj_utm_pos = own_utm_rotation_ * obj_vcs_pos + own_utm_pos_;
  auto obj_utm_vel = own_utm_rotation_ * own_dr_rotation_inverse_ * obj_dr_vel;
  out.xabs = obj_utm_pos[0];
  out.yabs = obj_utm_pos[1];
  out.vxabs = obj_utm_vel[0];
  out.vyabs = obj_utm_vel[1];
  out.speed = sqrt(out.vxabs * out.vxabs + out.vyabs * out.vyabs);

  // convert to heading under UTM
  float utm_heading = out.heading - locpos_.dr_yaw + locpos_.angle;
  // out.heading_vcs = out.heading - locpos_.dr_yaw;
  // out.heading = NormalizeHeading(utm_heading);

  // 2021-09-15, if there is no lidar, fill cells based on previous frame
  if (IsTrkUpdated(trk.fuse_source) == false) {
    float vxabs = out.vxabs;
    float vyabs = out.vyabs;
    if (trk.is_trk_static == 1) {
      vxabs = 0;
      vyabs = 0;
    }
    // predict
    for (auto &cell : trk.cells) {
      cell.xg += vxabs * dt;
      cell.yg += vyabs * dt;
      Eigen::Vector3d cell_utm_pos(cell.xg, cell.yg, 0);
      auto cell_rel_pos = own_utm_rotation_inverse_ * (cell_utm_pos - own_utm_pos_);
      cell.x = cell_rel_pos[0];
      cell.y = cell_rel_pos[1];
    }
  }

  for (auto &cell : trk.cells) {
    Eigen::Vector3d cell_rel_pos(cell.x, cell.y, 0);
    auto cell_utm_pos = own_utm_rotation_ * cell_rel_pos + own_utm_pos_;
    OccupyCell_ cell_utm;
    cell_utm.idc = 0;
    cell_utm.x = cell.x;
    cell_utm.y = cell.y;
    cell_utm.xg = cell_utm_pos[0];
    cell_utm.yg = cell_utm_pos[1];
    out.cells.push_back(cell_utm);
    cell = cell_utm;
  }

  // if there is no lidar, fill obj corner
  // if (trk.cells.size() == 0) {
  //   auto geoPoints = bbox_.GetLidarBoundingBox(trk.x_center_vcs, trk.y_center_vcs,
  //                    trk.length, trk.width, trk.heading_vcs);
  //   for (int idx = 0; idx < 9; idx++) {
  //     if (idx == TOP_LEFT || idx == TOP_RIGHT || idx == BOTTOM_LEFT || idx == BOTTOM_RIGHT) {
  //       Eigen::Vector3d cell_rel_pos(geoPoints[idx].x, geoPoints[idx].y, 0);
  //       auto cell_utm_pos = own_utm_rotation_ * cell_rel_pos + own_utm_pos_;
  //       OccupyCell_ cell_utm;
  //       cell_utm.idc = 0;
  //       cell_utm.x = cell_rel_pos[0];
  //       cell_utm.y = cell_rel_pos[1];
  //       cell_utm.xg = cell_utm_pos[0];
  //       cell_utm.yg = cell_utm_pos[1];
  //       out.cells.push_back(cell_utm);
  //     }
  //   }
  // }

}


void MotManager::TransForm_simple_cluster(ObjectDetection_ det, FusionObject_ & out) {
  // output predict obj center

  Eigen::Vector3d obj_vcs_pos(det.x_vcs, det.y_vcs, 0);

  out.x = obj_vcs_pos[0];
  out.y = obj_vcs_pos[1];


  // calculate object utm position and velocity
  auto obj_utm_pos = own_utm_rotation_ * obj_vcs_pos + own_utm_pos_;
  out.xabs = obj_utm_pos[0];
  out.yabs = obj_utm_pos[1];
  float utm_heading = out.heading - locpos_.dr_yaw + locpos_.angle;
  out.heading = NormalizeHeading(utm_heading);
  for (auto &cell : det.cells_vcs) {
    Eigen::Vector3d cell_rel_pos(cell.x, cell.y, 0);
    auto cell_utm_pos = own_utm_rotation_ * cell_rel_pos + own_utm_pos_;
    OccupyCell_ cell_utm;
    cell_utm.idc = 0;
    cell_utm.x = cell.x;
    cell_utm.y = cell.y;
    cell_utm.xg = cell_utm_pos[0];
    cell_utm.yg = cell_utm_pos[1];
    out.cells.push_back(cell_utm);
    cell = cell_utm;
  }

}

void MotManager::PostProcess() {
  // clear detections inputs
  detections_lidar_.clear();
  detections_lidar_new.clear();
  detections_radar_.clear();
  detections_vision_.clear();
  for (auto &track : fusion_tracks_) {
    track.ClearMatchedDetections();
  }
  for (auto &track : fake_tracks_) {
    track.matched_lidar_dets_.clear();
    track.matched_radar_dets_.clear();
    track.matched_vision_dets_.clear();
  }
}

bool MotManager::PointDistanceToFreeSpace(const double&x_vcs, const double&y_vcs, double & distance) {
  return false; //add by lxd 需要地图才行
  /*
  // double check
  if (bmp_ == nullptr) {
    bmp_ = avos::map::MapLoader::GetBasemapPtr();
    if (bmp_ == nullptr) {
      AERROR << "Fusion: Get BaseMap failed.";
      return false;
    }
  }
  // zr-debug
  return false;
  PointGCCS pgccs{locpos_.xg, locpos_.yg, locpos_.angle};
  PointVCS pvcs{x_vcs, y_vcs, 0.0};

  distance = bmp_->DistanceToFreeSpace(pgccs, pvcs);
  // AINFO  << "DistanceToFreeSpace: " << x_vcs << ", " << y_vcs
  //           << ", distance: " << distance;
  return true;
  */
}


}
}  // namespace fusion
}  // namespace avos



