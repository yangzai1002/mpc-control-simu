#pragma once

#include <vector>
#include <Eigen/Dense>
#include "perception_fusion_common/Xlidar_mot/common/common_type.h"
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
// #include "algorithm/fusion/logging/logging.h"

namespace avos {
namespace perception {
namespace xlidarMOT{

class BBoxCalculator {
public:
	BBoxCalculator();
	~BBoxCalculator();

	std::vector<GeoPoint> GetLidarBoundingBox(const double x, const double y,
	        const float length, const float width, const float heading);

	void GetAnchorPoint(const int prev_fuse_point, const int trk_type, const float trk_heading_vcs,
	                    const ObjectDetection_ &det, const bool is_top_bottom_reversed,
	                    int &fuse_point, double &x, double &y);

	void UpdateBBoxBuffer(ObjectTrack &trk,
	                      const ObjectDetection_ &det);

	void CalculateBBoxVelocity(ObjectTrack &trk);

	void GetPredictBBoxCenter(ObjectTrack &trk);

	void GetBBoxCenter(ObjectTrack &trk, const float &x_vcs, const float &y_vcs);

	bool IsCarTruck(const int &det_type, const int &trk_type);

private:


};
}
}
}

