#ifndef DATAPOOL_INCLUDE_TASK_INPUT_H_
#define DATAPOOL_INCLUDE_TASK_INPUT_H_

#include <iostream>
#include <string>
#include "xkit/planning/datapool/public_typedef.h"
#include "log/log.h"
#include "common/time/node_time.h"
#include "xkit/planning/datapool/task_struct.h"
namespace avos {
namespace planning {

//任务类型
enum class eMissionType {
  STOP = 0,
  ARRIVE = 1,
  FOLLOW = 2,
  TRACK = 3
};

//控制状态
enum class eCommand {
  DEFAULT = 0,
  COMMAND_STOP = 0, //任务结束
  COMMAND_START = 1//任务开始
};

enum class eEmergencyStatus {
  DEFAULT = 0,
  EMERGENCY_ACTION = 1, //急停
  EMERGENCY_UNACTION = 2 //解除急停
};

enum class eTaskInputStatus {
  DEFAULT = 0,
  NEW_TASK = 1,
  ABORTED_TASK = 2,
  STOP_TASK = 3
};

enum class eAutoDriveStatus {
  DEFAULT = 0,
  AUTO_DRIVE_ENTRY = 1,
  AUTO_DRIVE_EXIT = 2
};

// struct VehicleDriveStatus {
//   /**origin input**/
//   double timestamp;
//   double targetsangle;
//   double targetaccel;
//   double targetshiftposition;
//   double steer_angle; // 车辆实际转角
//   double realstrtorque;
//   double shiftlvlposition; // 车辆实际档位
//   double velocity; // 车辆车速（控制从底层接收），可能没有数据
//   int fault_level; // 故障等级（从业务端获取）
//   int epb_status; // 车辆实际epb状态
//   int control_switch_state;
//   double vehicle_accel; // 车辆实际加速度 
//   double brake_pressure;
//   int system_state; // 车辆系统状态（5:自动驾驶，7：拍停，4：遥控）
//   int control_mode;
//   bool control_emergency; // 障碍物急停，未赋值，需要和夏老师确认
//   bool aeb_emergency; // 不用
//   /**origin input**/

//   // eEmergencyStatus emergency_status;
//   // eRemoteCtrlStatus remote_ctrl_status;
//   eAutoDriveStatus auto_drive_status;
  
//   VehicleDriveStatus() {
//     Reset();
//   }
//   void Reset() {
//     timestamp = 0.0;
//     targetsangle = 0.0;
//     targetaccel = 0.0;
//     targetshiftposition = 0.0;
//     steer_angle = 0.0;
//     realstrtorque = 0.0;
//     shiftlvlposition = 0.0;
//     velocity = 0.0;
//     fault_level = 0;
//     epb_status = 0;
//     control_switch_state = 0;
//     vehicle_accel = 0.0;
//     brake_pressure = 0.0;
//     system_state = 0;
//     control_mode = 0;
//     control_emergency = false;
//     aeb_emergency = false;
//     // emergency_status = eEmergencyStatus::DEFAULT;
//     // remote_ctrl_status = eRemoteCtrlStatus::DEFAULT;
//     auto_drive_status = eAutoDriveStatus::DEFAULT;
//   }

//   bool IsSyetemException() {
//     return (fault_level == 2);
//   }
//   bool IsSyetemNormal() {
//     return (fault_level != 2);
//   }
//   // bool IsBtnEmergencyPressed() {
//   //   return (emergency_status == eEmergencyStatus::EMERGENCY_PRESSED);
//   // }
//   // bool IsBtnEmergencyUnpressed() {
//   //   return (emergency_status == eEmergencyStatus::EMERGENCY_UNPRESSED);
//   // }
//   // bool IsRemoteCtrlConnected() {
//   //   return (remote_ctrl_status == eRemoteCtrlStatus::REMOTE_CTRL_PRESSED);
//   // }
//   // bool IsRemoteCtrlUnconnected() {
//   //   return (remote_ctrl_status == eRemoteCtrlStatus::REMOTE_CTRL_UNPRESSED);
//   // }
//   void SetDriveStatus() {
//     // emergency_status = eEmergencyStatus::DEFAULT;
//     // remote_ctrl_status = eRemoteCtrlStatus::DEFAULT;
//     auto_drive_status = eAutoDriveStatus::DEFAULT;
//     if (system_state == 7) {
//       // emergency_status = eEmergencyStatus::EMERGENCY_PRESSED;
//       // remote_ctrl_status = eRemoteCtrlStatus::REMOTE_CTRL_UNPRESSED;
//       auto_drive_status = eAutoDriveStatus::AUTO_DRIVE_EXIT;
//     } else if (system_state == 4) {
//       // emergency_status = eEmergencyStatus::EMERGENCY_UNPRESSED;
//       // remote_ctrl_status = eRemoteCtrlStatus::REMOTE_CTRL_PRESSED;
//       auto_drive_status = eAutoDriveStatus::AUTO_DRIVE_EXIT;
//     } else if (system_state == 5) {
//       // emergency_status = eEmergencyStatus::EMERGENCY_UNPRESSED;
//       // remote_ctrl_status = eRemoteCtrlStatus::REMOTE_CTRL_UNPRESSED;
//       auto_drive_status = eAutoDriveStatus::AUTO_DRIVE_ENTRY;
//     } else {
//       // emergency_status = eEmergencyStatus::EMERGENCY_UNPRESSED;
//       // remote_ctrl_status = eRemoteCtrlStatus::REMOTE_CTRL_UNPRESSED;
//       auto_drive_status = eAutoDriveStatus::AUTO_DRIVE_EXIT;
//     }
//   }
// };

typedef struct TaskContent {
  /***origin input***/
  double time;
  int valid_count;
  bool is_valid;
  bool new_input;
  bool is_new_task;
  //任务类型
  eMissionType mission_type;
  eMissionType last_mission_type;
  eCommand command;
  TaskProperty task_property;
  //急停
  eEmergencyStatus emergency_status;
  /***origin input***/
  eCommand command_info;
  eTaskInputStatus task_state;  /*!< for judge new task or abandon task   0 default  1 new_task  2 abandon task 3 Parallel driver*/
  eSystemStatus system_state;
  //app输入的路径数据
  //跟随信息
  //内部感知数据
  // FollowTarget follow_inner_data;
  //外部感知数据
  // FollowTarget follow_app_input_data;
  //跟随看路径
  InnerPath follow_inner_paths_data;
  //自主通行数据
  //app输入数据
  InputPath arrive_input_data; 
  //app转换后的内部数据
  InnerPath arrive_inner_paths_data;
  //寻迹通行数据
  //app输入数据
  InputPath track_input_data;//输入数据
  //app转后的数据
  InnerPath track_inner_paths_data;
  //循迹任务点数据
  // TrackTaskPoint track_task_point;
  double new_task_time;

  //外部任务
  PayloadTaskInfo payload_task_info;

  //外部任务反馈
  PayloadTaskStatus payload_task_status;

  TaskContent() {
    time = 0;
    valid_count = 0;
    is_valid = false;
    new_input = false;
    is_new_task = false;
    Reset();
  }
  void Reset() {
    mission_type = eMissionType::STOP;
    command = eCommand::COMMAND_START;
    command_info = eCommand::COMMAND_START;
    emergency_status = eEmergencyStatus::EMERGENCY_UNACTION;
    task_property.Reset();
    // follow_inner_data.Reset();
    // follow_app_input_data.Reset();
    follow_inner_paths_data.Reset();
    arrive_input_data.Reset();
    track_input_data.Reset();
    arrive_inner_paths_data.Reset();
    track_inner_paths_data.Reset();
    // track_task_point.Reset();
    task_state = eTaskInputStatus::DEFAULT;
    system_state = eSystemStatus::DEFAULT;
    new_task_time = avos::common::NodeTime::Now().ToSecond();
  }
  bool CheckValid() {
    valid_count++;
    if (valid_count > TASK_RESUME_TIME) {
      valid_count = 0;
      is_valid = true;
      new_input = true;
    }
    return is_valid;
  }
  void ResetValid() {
    valid_count = 0;
    is_valid = false;
    new_input = false;
  }

  void SetNewTask() {
    task_state = eTaskInputStatus::NEW_TASK;
    new_task_time = avos::common::NodeTime::Now().ToSecond();
  }
  void SetAbortedTask() {
    task_state = eTaskInputStatus::ABORTED_TASK;
  }
  void ResetNewTask() {
    task_state = eTaskInputStatus::DEFAULT;
    new_task_time = avos::common::NodeTime::Now().ToSecond();
  }
  bool IsNewTask() {
    return (task_state == eTaskInputStatus::NEW_TASK);
  }
  void SetTaskStatus() {
    if (command == eCommand::COMMAND_START &&
        command_info != eCommand::COMMAND_START) {
      SetNewTask();
    }

    if (command == eCommand::COMMAND_STOP &&
        command_info != eCommand::COMMAND_STOP) {
      SetAbortedTask();
    }
    command_info = command;
  }
} TaskContent;


typedef struct VoronoiDiagramInfo {
  double width;
  double front_length;
  double back_length;
  double resolution;
  int size_x;
  int size_y;
  std::vector<double> voronoi_diagram;
  std::vector<std::pair<double, double>> voronoi_diagram_obj_coor;
  
  VoronoiDiagramInfo() {
    Reset();
  }


  void Reset() {
    width = 0.0;
    front_length = 0.0;
    back_length = 0.0;
    resolution = 0.0;
    size_x = 0;
    size_y = 0;
    voronoi_diagram.clear();
    voronoi_diagram_obj_coor.clear();
  }

} VoronoiDiagramInfo;

typedef struct FaultDecisionCmd {
  int fault_type;
  bool fault_cmd;
  
  FaultDecisionCmd() {
    Reset();
  }


  void Reset() {
    fault_type = 0;
    fault_cmd = false;
  }

} FaultDecisionCmd;

} // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_TASK_INPUT_H_
