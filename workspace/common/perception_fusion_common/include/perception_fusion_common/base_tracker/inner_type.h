#pragma once

#include "perception_fusion_common/common/lidarvisioncommon.h"
#include "perception_fusion_common/base_tracker/code_check.h"


namespace avos {
namespace perception {






class DetObject {
public:
	DetObject() {}
	~DetObject() {}

	double trigger_stamp = -1;
	int camid = -1;
	int dltype_det2d = -1; // useless
	float conf2d = 0.0;
	float conf3d = 0.0;
	cv::Rect2f box2d;
	bool crop_h30 = false;
	bool is3d = false;
	FusionObjType type = FusionObjType::FUS_UNDEFINED;
	PosSource pos_source = PosSource::UNDEFINED;

	int htflag = -1; // -1:none 0:head 1:tail 
	cv::Rect2f htbox2d; // head or tail box2d 

	double length = 0.0;
	double width = 0.0;
	double height = 0.0;

	double x_vcs_det = 0.0; // 目标中心点
	double y_vcs_det = 0.0;
	double z_vcs_det = 0.0;
	double heading_vcs_det = 0.0;

	double x_dr_det = 0.0;
	double y_dr_det = 0.0;
	double z_dr_det = 0.0;
	double heading_dr_det = 0.0;

	std::vector<cv::Point2f> seg_img_pts;
	std::vector<cv::Point3f> seg_ground_pts;
};



class TrackObject : public DetObject {
public:
	TrackObject() {}
	~TrackObject() {}
	TrackObject(const DetObject& det) : DetObject(det) {
		x_vcs = det.x_vcs_det;
		y_vcs = det.y_vcs_det;
		z_vcs = det.z_vcs_det;
		heading_vcs = det.heading_vcs_det;
		x_dr = det.x_dr_det;
		y_dr = det.y_dr_det;
		z_dr = det.z_dr_det;
		heading_dr = det.heading_dr_det;
	}

	int trk_id = -1;
	int trk_age = 0; // 每个tracker内部的age
	int trk_guess_age = 0;

	double vx_vcs = 0.0;
	double vy_vcs = 0.0;
	double vx_ground = 0.0; // vx_dr旋转到自车方向
	double vy_ground = 0.0;

	double vx_dr = 0.0;
	double vy_dr = 0.0;
	double vz_dr = 0.0;
	double speed_dr = 0.0;

	double x_vcs = 0;
	double y_vcs = 0;
	double z_vcs = 0;
	double heading_vcs = 0;

	double x_dr = 0;
	double y_dr = 0;
	double z_dr = 0;
	double heading_dr = 0;

	bool matched = false;
};




// 用于从Global Obj更新的输入结构体
typedef struct RefObject {
  int trk_age = 0;
  int trk_guess_age = 0;
  double length = 0.0;
  double width = 0.0;
  double x_vcs = 0.0;
  double y_vcs = 0.0;
  double heading_vcs = 0.0;
  double speed_dr = 0;
  double kal_v = 0;
  double kal_a = 0;
  double kal_theta = 0;
} RefObject;





}
}



