#include "log/log.h"
#include "perception_fusion_common/Xlidar_mot/tracker/base_tracker.h"
namespace avos {
namespace perception {
namespace xlidarMOT{
uint32_t BaseTracker::static_tracker_id_ = 0;

BaseTracker::BaseTracker(const ObjectTrack& obj) {
  trk = obj;
  if (++static_tracker_id_ < 0 || static_tracker_id_ > MAX_ID) {
    static_tracker_id_ = 0;
  }
  trk.id = static_tracker_id_;
}

BaseTracker::~BaseTracker() {
}

int BaseTracker::Init(TrackerConfigType config) {
  config_ = config;
  X_.resize(Num_State);
  X_(0) = trk.x;
  X_(1) = trk.y;
  X_(2) = sqrt(pow(trk.vx, 2) + pow(trk.vy, 2));
  X_(3) = atan2(trk.vy, trk.vx);
  X_(4) = 0;
  X_(5) = 0;
  P0_.resize ( Num_State, Num_State );
  P0_ ( 0, 0 ) = config_.P0_0;
  P0_ ( 1, 1 ) = config_.P0_1;
  P0_ ( 2, 2 ) = config_.P0_2;
  P0_ ( 3, 3 ) = config_.P0_3;
  P0_ ( 4, 4 ) = config_.P0_4;
  P0_ ( 5, 5 ) = config_.P0_5;

  kalman.init(X_, P0_);

  return 0;
}

void BaseTracker::SetT(double T)
{
  period_ = T;
}

int BaseTracker::MessureUpdate(MyVector new_R, MyVector new_Z) {
  if (new_R.size() != Num_Measure ||
      new_Z.size() != Num_Measure) {
    return 1;
  }
  kalman.resetR ( new_R );
  kalman.measureUpdateStep ( new_Z );
  X_ = kalman.getX();
  trk.x = X_( 0 );
  trk.y = X_( 1 );
  trk.vx = X_( 2 ) * cos(X_( 3 ));
  trk.vy = X_( 2 ) * sin(X_( 3 ));
  return 0;
}

int BaseTracker::Predict(const float time_step) {
  kalman.resetPeriod(time_step);
  MyVector _u( Num_Measure, 0.0 );
  kalman.timeUpdateStep ( _u );
  X_ = kalman.getX();
  trk.x = X_( 0 );
  trk.y = X_( 1 );
  trk.vx = X_( 2 ) * cos(X_( 3));
  trk.vy = X_( 2 ) * sin(X_( 3));
  return 0;
}

ObjectTrack& BaseTracker::getResult() {
  return trk;
}

}
}
}
