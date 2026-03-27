#pragma once
#include "perception_fusion_common/Xlidar_mot/munkres/munkres.h"
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/matching/base_matching.h"
#include "perception_fusion_common/Xlidar_mot/matching/lite_cost.h"

namespace avos {
namespace perception {
namespace xlidarMOT{
class GNNMatching : public BaseMatching {
public:
  GNNMatching();
  virtual ~GNNMatching();

  virtual int Init(MatchingConfigType config);
  virtual int Matching(std::vector<FusionTracker> &tracks,
                       std::vector<ObjectDetection_> &dets);
  /// <0,not matched; >=0,target id
  virtual int getMatchResult(std::vector<int> &tracks_matched,
                             std::vector<int> &dets_matched);
  virtual std::string Name()
  {
    return "GNN Matching";
  }

private:
  MatchingConfigType config_;
  std::vector<int> dets_matched_;
  std::vector<int> tracks_matched_;
  BaseCost* cost_ptr_;
  Numerical num_;
};
}
} //namespace avos
} //namespace perception
