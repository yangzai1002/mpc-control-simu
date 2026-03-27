#pragma once
#include <string>
#include <vector>
#include "perception_fusion_common/Xlidar_mot/common/common_type.h"
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/tracker/base_tracker.h"
#include "perception_fusion_common/Xlidar_mot/tracker/fusion_tracker.h"
#include "perception_fusion_common/Xlidar_mot/matching/base_cost.h"


namespace avos {
namespace perception {
namespace xlidarMOT{

class BaseMatching
{
public:
	// BaseMatching(){}
	virtual ~BaseMatching() {;}
	virtual std::string Name() = 0;
	virtual int Init(MatchingConfigType config) = 0 ;
	virtual int Matching(std::vector<FusionTracker> &tracks,
	                     std::vector<ObjectDetection_> &dets) = 0;
	/// <0,not matched; >=0,target id
	virtual int getMatchResult(std::vector<int> &tracks_matched,
	                           std::vector<int> &dets_matched) = 0;
};

}
}
}

