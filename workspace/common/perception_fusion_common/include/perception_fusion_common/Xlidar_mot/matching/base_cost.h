#pragma once
#include <vector>
#include "perception_fusion_common/Xlidar_mot/common/common_type.h"
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/tracker/fusion_tracker.h"
#include "perception_fusion_common/Xlidar_mot/matrix/matrix.h"

namespace avos {
namespace perception {
namespace xlidarMOT{
class BaseCost
{
public:
	// BaseCost(){}
	virtual ~BaseCost() {;}
	virtual std::string Name() = 0;
	virtual int Init(CostConfigType config) = 0 ;
	virtual int Cost( avos::perception::xfusion_new::Matrix<double> &mat,
	                  const std::vector<FusionTracker> &tracks,
	                  const std::vector<ObjectDetection_> &dets) = 0;
	virtual bool Gating(const FusionTracker& track,
	                    const ObjectDetection_& det) = 0;
};

}
}
}
