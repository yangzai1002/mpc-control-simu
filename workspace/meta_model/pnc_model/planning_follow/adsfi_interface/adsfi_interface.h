#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "PlanningTrajectoryPoint.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "svsPointMsg.h"
#include "LatcontrolDebugStruct.h"
#include "ara/adsfi/impl_type_vehiclelatcontrol.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "LoncontrolDebugStruct.h"
#include "PlanningTrajectory.h"
#include "PerceptionTargetObj.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"

#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "common/time/node_time.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
#include "filter_iir.h"
#include "map_util/geometry_tools.hpp"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "common/shm_util/shm_transmitter.h"

class AdsfiInterface: public BaseAdsfiInterface
{
public:
    AdsfiInterface() : simulation_mode_(false) {
    }
    ~AdsfiInterface() {
    }

    // 初始化与反初始化
    void Init();
    void DeInit();

    // --- 回调函数：接收外部输入 ---
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr);

    // --- 核心处理函数 ---
    // 输入：内部处理后的结果，通过引用传出给 planning_result_ptr
    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &planning_result_ptr);
    // 用于处理规划状态的共享内存逻辑
    int Process(const std::string &name, std::shared_ptr<ara::adsfi::PlanningStatus> &ptr);

    BASE_TEMPLATE_FUNCION

private:
    // --- 线程安全缓冲区 ---
    ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_bus_command_;
    ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_fusion_out_array_;
    ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_target_array_;
    ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_vehicle_info_;
    ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_location_info_;
    ThreadSafeBuffer<ara::adsfi::PerceptionStaticEnv> safebuf_static_env_;

    // 互斥锁与输出快照缓存
    std::mutex process_data_mutex_;
    bool has_valid_output_cache_ = false; // 标记缓存中是否有合法的计算结果
    ara::adsfi::PlanningStatus cached_planning_status_;

    // --- 仿真模式相关 ---
    bool simulation_mode_;  // 是否启用仿真模式
    std::shared_ptr<ara::adsfi::BusinessCommand> sim_bus_command_;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> sim_fusion_objarray_;
    std::shared_ptr<ara::adsfi::VehicleInformation> sim_vehicle_info_;
    std::shared_ptr<ara::adsfi::MsgHafLocation> sim_location_info_;

    ThreadSafeBuffer<int> safebuf_flag_info_;
    std::shared_ptr<int> flag_ptr_;

    // 初始化仿真数据
    void InitSimulationData();
    
    // 路径可视化函数 (用于mviz调试)
    void VisualizePaths(const PlanningTrajectory& final_trajectory,
                        const avos::planning::Paths& inner_path,
                        const avos::planning::Paths& debug_path);
};
#endif