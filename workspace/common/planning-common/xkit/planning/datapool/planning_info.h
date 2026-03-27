#ifndef DATAPOOL_INCLUDE_PLANNINGINFO_H_
#define DATAPOOL_INCLUDE_PLANNINGINFO_H_

#include "xkit/planning/datapool/speedplan_typedef.h"
#include "xkit/planning/datapool/task_struct.h"

namespace avos {
namespace planning {

struct TaskInfo {
    std::string task_status;
    TaskInfo() {
        task_status = "none";
    }

};

struct RemoteControlInfo {
    bool remote_ctl_flag;
    bool last_remote_ctl_flag;
    bool is_idle_remote_;
    bool remote_ctl_stop;
    int remote_ctl_mode;
    int remote_gear;
    double remote_steering_angle;
    double remote_acceleration;
    double remote_brake;

    RemoteControlInfo() {
        Reset();
    }

    void Reset() {
        remote_ctl_flag = false;
        last_remote_ctl_flag = false;
        is_idle_remote_ = false;
        remote_ctl_stop = false;
        remote_ctl_mode = 0;
        remote_gear = 0;
        remote_steering_angle = 0.0;
        remote_acceleration = 0.0;
        remote_brake = 0.0;
    }
};

struct PathplanInfo {
    int avoid_flag;
    bool pedestrian_avoid_flag;
    bool run_real_time;
    bool track_enable_dr;
    double start_xg, start_yg;

    RemoteControlInfo remote_control_info;

    PathplanInfo() {
        Reset();
    }

    void Reset() {
        avoid_flag = 1;
        pedestrian_avoid_flag = false;
        run_real_time = false;
        track_enable_dr = false;
        start_xg = start_yg = 0.0;
    }
};

struct FollowInfo {
    int follow_mode = -1;
    int follow_type = -1;
    bool is_enable = false;
    int follow_id = -1;
    int task_status = -1;
    bool avoid_flag = false;
    double follow_min_dis{0.0};
};

struct PlanningInfo {
    PathplanInfo pathplan_info;
    SpeedplanInfo speedplan_info;
    TaskInfo task_info;
    TrackTaskPoint track_task_point;
    FollowInfo follow_info;
    // MSGPACK_DEFINE(pathplan_info, speedplan_info, task_info,
    // track_task_point); MSGHANDLE_PARAKLIST_DEFINE("pathplan_info",
    // "speedplan_info", "task_info",
    //                            "track_task_point");
    // MSGHANDLE_PARAVLIST_DEFINE(pathplan_info, speedplan_info, task_info,
    //                            track_task_point);

};
} // namespace planning
} // namespace avos

#endif
