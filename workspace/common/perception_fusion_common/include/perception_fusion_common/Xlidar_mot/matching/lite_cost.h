#pragma once
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/matching/base_cost.h"


namespace avos {
namespace perception {
namespace xlidarMOT{
class LiteCost: public BaseCost
{
public:
	LiteCost();
	virtual ~LiteCost();
	virtual std::string Name();
	virtual int Init(CostConfigType config);
	virtual int Cost(avos::perception::xfusion_new::Matrix<double> &mat,
	                 const std::vector<FusionTracker> &tracks,
	                 const std::vector<ObjectDetection_> &dets);
	virtual bool Gating(const FusionTracker& track,
	                    const ObjectDetection_& det);
private:
	CostConfigType config_;
};
}
}//namespace avos
}//namespace perception

