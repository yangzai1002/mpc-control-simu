#pragma once
#include "perception_fusion_common/Xlidar_mot/common/common_type.h"
#include "perception_fusion_common/Xlidar_mot/matrix/matrix.h"
#include "perception_fusion_common/Xlidar_mot/common/algorithm_type.h"
#include "perception_fusion_common/Xlidar_mot/filter/objfusekf.h"

namespace avos {
namespace perception {
namespace xlidarMOT{

#define   MAX_R       999999 //tracker max measure noise.
#define   MAX_ID      200000 //
class BaseTracker
{
public:
  BaseTracker(const ObjectTrack& obj);
  ~BaseTracker();
  int Init(TrackerConfigType config);
  void SetR(vector<double > R);
  void SetT(double T);
  int MessureUpdate(MyVector new_R, MyVector new_Z);
  int Predict(const float time_step);
  ObjectTrack& getResult();

  ObjectTrack trk;
  avos::perception::xlidarMOT::cObjfusEKF kalman;
  MyVector R;
  MyVector Q;

private:
  MyMatrix P0_;
  MyVector X_;
  double period_;
  TrackerConfigType config_;
  static uint32_t static_tracker_id_;


};

}
}
}

