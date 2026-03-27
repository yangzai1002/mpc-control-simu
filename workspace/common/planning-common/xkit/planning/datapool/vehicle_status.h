#ifndef DATAPOOL_INCLUDE_CAN_INPUT_H_
#define DATAPOOL_INCLUDE_CAN_INPUT_H_

#include <iostream>
// #include "public_typedef.h"
// #include "adb_kernel/common/base/time/include/node_time.h"
namespace avos {
namespace planning {

struct VehicleStatus {
  double steer_angle;
  double speed;
  int shift_position;
  int epb_status;
  int current_drive_mode;
  int control_switch_state;
  double vehicle_accel;
  double break_pressure;
  int control_emergency;
  bool quit_auto_drive;
  VehicleStatus() {
    Reset();
  }
  void Reset() {
    steer_angle = 0.0;
    speed = 0.0;
    shift_position = 0;
    epb_status = 0;
    current_drive_mode = 0;
    control_switch_state = 0;
    vehicle_accel = 0.0;
    break_pressure = 0.0;
    control_emergency = 0;
    quit_auto_drive = false;
  }
};

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_CAN_INPUT_H_