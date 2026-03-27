#include "log/log.h"
#include "perception_fusion_common/Xlidar_mot/matching/gnn_matching.h"

namespace avos {
namespace perception {
namespace xlidarMOT{
GNNMatching::GNNMatching() {
  cost_ptr_ = nullptr;
}
GNNMatching::~GNNMatching() {
  if (cost_ptr_ != nullptr) {
    delete cost_ptr_;
  }
}
int GNNMatching::Init(MatchingConfigType config) {
  config_ = config;
  if (config.cost_option == 0) {
    cost_ptr_ = new LiteCost();
  } else {
    cost_ptr_ = new LiteCost();
  }
  cost_ptr_->Init(config.cost_param);
  return 0;
}

int GNNMatching::Matching(std::vector<FusionTracker> &tracks,
                          std::vector<ObjectDetection_> &dets) {
  // AINFO  << "into Matching trks :" << tracks.size();
  Munkres<double> munkres_;
  if (dets.size() <= 0 || tracks.size() <= 0) {
    if (dets.size() > 0) {
      dets_matched_ = std::vector<int>(dets.size(), -1);
    } else {
      dets_matched_.clear();
    }
    if (tracks.size() > 0) {
      tracks_matched_ = std::vector<int>(tracks.size(), -1);
    } else {
      tracks_matched_.clear();
    }
    return 1;
  }
  // if (1) {
  //   AINFO  << "Matching Dets:" << dets.size();
  //   for (int i = 0; i < dets.size(); ++i) {
  //     AINFO  << "[" << i << "]:" << "(" << dets[i].x_dr << "," << dets[i].y_dr << "," << dets[i].length << ") ";
  //   }
  //   AINFO ;
  //   AINFO  << "Matching trks:" << tracks.size();
  //   for (int i = 0; i < tracks.size(); ++i) {
  //     AINFO  << "[" << i << "]:" << "(" << tracks[i].trk.x << "," << tracks[i].trk.y << "," << tracks[i].trk.length << ") ";
  //   }
  //   AINFO ;
  // }
  dets_matched_ = std::vector<int>(dets.size(), -1);
  tracks_matched_ = std::vector<int>(tracks.size(), -1);
  avos::perception::xfusion_new::Matrix<double> cost;
  if (cost_ptr_ == nullptr) {
    AINFO  << "not inited!";
    exit(-1);
  }
  //计算track和det的cost，赋予cost
  //TODO:是否根据age大小进行优先级排序
  if (cost_ptr_->Cost(cost, tracks, dets) != 0) {
    return 1;
  }
  //匈牙利匹配
  
  Munkres<double> munkres_t;
  munkres_t.solve(cost);
  bool un_matched = true;
  for (int track_id = 0; track_id < tracks.size(); ++track_id) {
    un_matched = true;
    for (int det_id = 0; det_id < dets.size(); ++det_id) {
      // AINFO  << cost(track_id, det_id) << "  ";
      if (num_.Near(cost(track_id, det_id), 0.0) &&
          cost_ptr_->Gating(tracks[track_id], dets[det_id])) {
        tracks_matched_[track_id] = det_id;
        dets_matched_[det_id] = track_id;
      }
    }
    // AINFO ;
  }
  return 0;
}

int GNNMatching::getMatchResult(std::vector<int> &tracks_matched,
                                std::vector<int> &dets_matched) {
  /// <0,not matched; >=0,target id
  tracks_matched = tracks_matched_;
  dets_matched = dets_matched_;
  return 0;
}

}
} //namespace avos
} //namespace perception
