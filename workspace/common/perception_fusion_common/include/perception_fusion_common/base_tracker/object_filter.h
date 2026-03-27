#pragma once

#include "perception_fusion_common/radartracker/radar_common.h"
#include "perception_fusion_common/common/imu_message.h"
#include "inner_type.h"


namespace avos {
namespace perception {


bool inline IsHeadingVertical(const float & heading_vcs_deg) {
  if ((heading_vcs_deg >= -60 && heading_vcs_deg <= 60)
      || (heading_vcs_deg >= -180 && heading_vcs_deg <= -120)
      || (heading_vcs_deg >= 120 && heading_vcs_deg <= 180)) {
    return true;
  }
  return false;
};


typedef struct KalmanStateV {
  KalmanStateV() {
    H = cv::Matx<double,3,5>::zeros();
    H(0,0) = 1;
    H(1,1) = 1;
    H(2,2) = 1;
  }
  cv::Vec3d observe; // x, y, theta in dr, theta in rad
  double x=0, y=0, theta=0, v=0, a=0, w=0; // state in dr, theta in rad
  cv::Matx<double,5,5> P,Q;
  cv::Matx33d R;
  cv::Matx<double,3,5> H;
  cv::Matx<double,5,3> K;
  cv::Matx33d S;
  double x_prev=0, y_prev=0, theta_prev=0, v_prev=0, a_prev=0, w_prev; // prev state for abnormal judge
  int bad_count = 0;
  bool inited = false;
  bool inited_with_radar = false;
  int source_mode = 0;
}KalmanStateV;

struct ObjectType
{
  	FusionObjType type = FusionObjType::FUS_UNDEFINED;
    int type_cnt = 0;
};

class OObject : public TrackObject
{
public:
	OObject() {}
	OObject(const TrackObject& det) : TrackObject(det) {}
	~OObject() {}

	void of_predect_2d(float shake_y);
	void of_update_2d(TrackObject& det);
	void of_fake_update_2d();

	int of_predict(const LocationType &locdr, float Tperiod);
	int of_update(TrackObject& det, bool isinit, const LocationType &locdr, float Tperiod, const RadarTrackMsg &radartracks);
	void of_fake_update();
	
	void of_initFromGlobalObj(const std::vector<RefObject> &globalobjs);
	void of_initFromRadar(const RadarTrackMsg &radartracks);

  KalmanStateV kal;
  int tracktype = 0; // 1:vision 2:lidarobj 3:bev vision ...
  cv::RotatedRect match_gate; // 仅用于可视化，前后帧跟踪匹配门限
  cv::Rect2f box2d_predict; // 仅可视化
  ObjectType trk_type;
  // // bool matched = false; // 前后帧跟踪匹配
  // bool blocked = false; // 是否被遮挡
  // bool ignore = false; // 被护栏遮挡的目标 暂时没用到
  // float wheel_s = 0;
  // float wheel_t = 0;

private:
  std::queue<TrackObject> _history_det;
	void _setQ();
	void _setObserve(TrackObject &det, const LocationType &locdr, float Tperiod);
	void _fuseObserveWithRadar(const LocationType &locdr, float Tperiod, const RadarTrackMsg &radartracks);
	void _coorTransform(const LocationType &locdr);
  void of_update_2d_type(TrackObject& det);

  
};










}
}

