#pragma once

#include "perception_fusion_common/common/imu_message.h"
#include "object_filter.h"
#include "inner_type.h"

namespace avos {
namespace perception {


typedef std::pair<size_t, size_t> MatchPair;



class OTracker
{
public:
	OTracker(int camid_) {
		camid = camid_;
		idpool = camid*1000; // 每个camera用1000个，[ camid*1000, camid*1000+1000 )
	}
	~OTracker() {}
	
	int obj_tracking(double stamp, const std::vector<DetObject> &input_dets, const LocationType &locdr, float shake_y, const RadarTrackMsg &radartracks, std::vector<OObject> &out_trackobj);

	void reset_tracker() {
		trackobj.clear();
	}

	void init_from_radar(const RadarTrackMsg &radartracks) {
    for(auto &trk: trackobj)
      trk.of_initFromRadar(radartracks);
	}

	// void init_from_GlobalObj(const std::vector<RefObject> &globalobjs) {
    // for(auto &trk: trackobj)
    //   trk.of_initFromGlobalObj(globalobjs);
	// }

	void get_trackobj_for_debug(std::vector<OObject> &trackobj_out) {
		trackobj_out = trackobj;
	}


private:

	void getMatchGate(OObject &trk, const LocationType &locdr);
	int tracking_bev_GNN(const std::vector<TrackObject> &dets, std::vector<MatchPair> &pairs);
	int tracking_img2d_GNN(const std::vector<TrackObject> &dets, std::vector<MatchPair> &pairs);

	int camid = -1;
	int idpool = -1;
	double trigger_stamp = -1;
	float Tperiod = 0.1;

	std::vector<OObject> trackobj;

};







}
}

