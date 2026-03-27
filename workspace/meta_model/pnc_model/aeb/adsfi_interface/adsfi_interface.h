#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"

#include "ara/adsfi/impl_type_vehicleactcontrol.h"   
#include "ara/adsfi/impl_type_vehicleinformation.h"  
#include "ara/adsfi/impl_type_businesscommand.h"         
#include "PlanningTrajectory.h"  
#include "ara/adsfi/impl_type_perceptionstaticenv.h" 
#include "ara/adsfi/impl_type_faultdata.h"   
#include "ara/adsfi/impl_type_planningstatus.h"
#include "PerceptionFinalObjects.h"

#include "include/trajectory_safety_checker.h"
#include "include/lookuptable.h"
#include "include/aeb_flags.h"
#include "include/aeb_types.h"

#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "common/time/node_time.h"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "base_control/lat/latcontrol_header.h"

class AdsfiInterface: public BaseAdsfiInterface
{
public:
    AdsfiInterface() : simulation_mode_(false) {}
    ~AdsfiInterface() {}

    void Init();
    void DeInit();

    // ================== 输入回调 (Callbacks) ==================
    // 对应 XAEBControl_soa.cpp 中的 9 个 Recv
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &ptr);  
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr);   
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr);  
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);                 
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr);  
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr);            
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &ptr);            

    // ================== 输出处理 (Process) ==================
    
    // Process 1: 对应 U-9 GenericDataSend(m_0_1) -> 车辆控制指令 (主处理函数)
    int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleActControl> &control_ptr);
    // Process 2: 对应 U-4 GenericDataSend(m_0_0) -> 规划/预测轨迹 (副处理函数)
	int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &traj_ptr);

    BASE_TEMPLATE_FUNCION

private:
    // --- 线程安全缓冲区 ---
	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_bus_cmd_;
    ThreadSafeBuffer<ara::adsfi::VehicleActControl> safebuf_input_ctrl_;
    ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_veh_info_;
    ThreadSafeBuffer<PlanningTrajectory> safebuf_input_path_;
    ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_loc_;
    ThreadSafeBuffer<ara::adsfi::PerceptionStaticEnv> safebuf_static_env_;
    ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_fusion_objs_;
    ThreadSafeBuffer<ara::adsfi::FaultData> safebuf_fault_data_;

    ThreadSafeBuffer<PlanningTrajectory> safebuf_output_traj_;

    // --- 线程同步 ---
    // 用于通知 Process 2 数据已准备好
    ThreadSafeBuffer<int> safebuf_flag_info_;
    std::shared_ptr<int> flag_ptr_;

    // --- 仿真相关 ---
    bool simulation_mode_;
    void InitSimulationData();
    // 仿真预设数据
	std::shared_ptr<ara::adsfi::VehicleActControl> sim_input_ctrl_;
    std::shared_ptr<ara::adsfi::VehicleInformation> sim_veh_info_;
    std::shared_ptr<ara::adsfi::MsgHafLocation> sim_loc_;
    std::shared_ptr<ara::adsfi::BusinessCommand> sim_bus_cmd_;

	// --- AEB 核心算法实例 ---
    bool is_aeb_init_ = false;
    // 使用静态成员变量保存配置，避免 Process 中重复加载
    static LatcontrollerInterfaceConfig lat_config;
            
    // --- 辅助逻辑 ---
    bool AebFaultDiagnosis(const ara::adsfi::MsgHafLocation& loc, 
    		const PerceptionFinalObjects& objs, const ara::adsfi::VehicleInformation& veh);
    void AdaptLocalizationByMode(const int business_mode, 
                                    const ara::adsfi::MsgHafLocation& raw_loc, 
                                    ara::adsfi::MsgHafLocation& adapted_loc);
};
#endif