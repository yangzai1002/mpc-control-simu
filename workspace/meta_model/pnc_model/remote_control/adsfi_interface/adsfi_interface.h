
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <memory>
#include <string>
#include "common/shm_util/shm_transmitter.h"
#include "PerceptionFinalObjects.h"
#include "PlanningTrajectory.h"
#include "PlanningTrajectoryPoint.h"
#include "adsfi/impl_type_locationinfo.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "base_control/lat/latcontrol_header.h"
#include "basic_remote_common_typedef.h"
#include "basic_remote_flags.h"
#include "basic_remote_loncontrol_interface.h"
#include "basic_remote_purepursuit_core.h"
#include "common/time/node_time.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
#include "filter_iir.h"
#include "map_util/geometry_tools.hpp"
#include "remote_safety_alert.h"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"

using namespace ara::adsfi;

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
    }
    ~AdsfiInterface() {
    }

    void Init();

    void DeInit();

    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppRemoteControl> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr);  
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &ptr);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleActControl> &ptr);
    int Process(const std::string &name, std::shared_ptr<ara::adsfi::PlanningStatus> &ptr);
    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr);

    void FillPurepursuitLatInput(LatcontrolInput &lat_input, VehicleParam &vehicle_param,
                                 const std::vector<PlanningTrajectoryPoint> &traj, const double xg, const double yg,
                                 const double yaw);

    void FillPidLonInput(LoncontrolInput &                                     lon_input,
                         const std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_param);

    int BasicRemote_GetZone(const double longitude);

    std::vector<PlanningTrajectoryPoint> Generate_TrackPath(
        std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr,
        std::shared_ptr<ara::adsfi::MsgHafLocation>     loc_ptr,
        std::shared_ptr<ara::adsfi::AppRemoteControl>   app_remote_ctrl_ptr,
        std::vector<PlanningTrajectoryPoint> &          origin_path);

private:
    int GetParam();

    std::vector<PlanningTrajectoryPoint> UpdatePathInfoByLatestLoc(
        const std::vector<PlanningTrajectoryPoint> &origin_path, const double xg, const double yg, const double yaw);

private:
    ThreadSafeBuffer<ara::adsfi::AppRemoteControl>    app_remote_buf_;
    ThreadSafeBuffer<ara::adsfi::VehicleInformation>  vehicleinfo_buf_;
    ThreadSafeBuffer<ara::adsfi::MsgHafLocation>      locationinfo_buf_;
    ThreadSafeBuffer<ara::adsfi::PerceptionStaticEnv> static_env_buf_;
    ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> fusion_buf_;
    ThreadSafeBuffer<ara::adsfi::FaultData>           fault_data_buf_;

    ThreadSafeBuffer<int>           safebuf_flag_planning_status_;
    ThreadSafeBuffer<int>           safebuf_flag_ego_trajectory_;
    std::shared_ptr<int>            flag_ptr_ = std::make_shared<int>(1);
    std::mutex                      output_data_mutex_;
    ara::adsfi::PlanningStatus      cached_planning_status_;
    ara::adsfi::MsgHafEgoTrajectory cached_ego_trajectory_;
    bool                            has_valid_output_cache_ = false;
    ara::adsfi::VehicleActControl   cached_vehicle_act_control_;
    bool                            has_valid_control_cache_ = false;

    bool                                  has_load_config_ = false;
    int                                   zero_speed_cmd_counter_ = 0;
    double                                desired_velocity, start_planning_xg, start_planning_yg;
    double                                start_planning_globalangle = 0.0;
    std::vector<PlanningTrajectoryPoint>  last_path, last_center_trajectory;
    LatcontrollerInterfaceConfig          lat_config;
    LatMath                               latmath_;
    LoncontrolDebugStruct                 lon_debug;
    LatcontrolDebugStruct                 lat_debug;
    std::unique_ptr<SecondOrderIIRFilter> filter_;
    long long                             remote_control_seq_ = 0;

    double process_start_time_ = 0.0;
    double disconnect_start_time_ = 0.0;
    int last_tick_ = 0;
    double loc_last_time_ = 0.0;
    bool hold_on_ = false;
    double track_start_t_ = 0.0;
    bool   track_timer_started_ = false;
    double app_steering_wheel_last_ = 0.0;
    std::vector<PlanningTrajectoryPoint> fixed_reference_path_;// 存储冻结的绝对坐标轨迹
    bool has_locked_path_ = false;// 标记是否已经锁定了轨迹

private:
    void PackAndNotifyOutput(const ara::adsfi::CommonHeader &control_header, int fcw_level, double speed_limit,
                             double desired_vel_ms, double target_angle, const PlanningTrajectory &center_traj,
                             const PlanningTrajectory &left_traj, const PlanningTrajectory &right_traj);
};
#endif
