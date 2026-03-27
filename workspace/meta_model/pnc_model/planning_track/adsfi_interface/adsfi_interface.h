
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "PlanningTrajectoryPoint.h"
#include "adsfi/impl_type_planningresult.h"
#include "ara/adsfi/impl_type_businesscommand.h"

#include "ara/adsfi/impl_type_businesscommand.h"
#include "adsfi/impl_type_locationinfo.h"
#include "svsPointMsg.h"
#include "LatcontrolDebugStruct.h"
#include "ara/adsfi/impl_type_vehiclelatcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "LoncontrolDebugStruct.h"
#include "PlanningTrajectory.h"

#include "ara/adsfi/impl_type_planningstatus.h"

#include "ara/adsfi/impl_type_msghaffusionoutarray.h"

#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

// #include "base_control/lat/latcontrol_header.h"
// #include "basic_remote_common_typedef.h"
// #include "basic_remote_flags.h"
// #include "basic_remote_loncontrol_interface.h"
// #include "basic_remote_purepursuit_core.h"
#include "common/time/node_time.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
//#include "filter_iir.h"
#include "map_util/geometry_tools.hpp"
// #include "remote_safety_alert.h"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "common/shm_util/shm_transmitter.h"

using namespace ara::adsfi;

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() : simulation_mode_(false) {

	}
	~AdsfiInterface() {

	}

	void Init();

	void DeInit();


	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr);

	int Process(std::shared_ptr<adsfi::PlanningResult> &ptr);
	int Process(std::shared_ptr<ara::adsfi::PlanningStatus> &ptr);

	BASE_TEMPLATE_FUNCION

private:

	ThreadSafeQueue<std::shared_ptr<ara::adsfi::BusinessCommand>> buscommand_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::MsgHafFusionOutArray>> fusionobjarray_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::MsgHafLocation>> locationinfo_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::VehicleInformation>> vehicleinfo_deque;

	// 共享内存传输器，用于在进程间安全共享 m_25_2 (PlanningStatus)
	avos::common::ShmTransmiiter<ara::adsfi::PlanningStatus> planning_status_shm_;

	// 仿真模式相关
	bool simulation_mode_;  // 是否启用仿真模式
	std::shared_ptr<ara::adsfi::BusinessCommand> sim_bus_command_;  // 仿真业务命令
	std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> sim_fusion_objarray_;  // 仿真融合目标数组
	std::shared_ptr<ara::adsfi::VehicleInformation> sim_vehicle_info_;  // 仿真车辆信息
	std::shared_ptr<ara::adsfi::MsgHafLocation> sim_location_info_;  // 仿真定位信息
	

	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_bus_command_;
	ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_fusion_objarray_;
	ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_vehicle_info_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_location_info_;

	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_sim_bus_command_;
	ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_sim_fusion_objarray_;
	ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_sim_vehicle_info_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_sim_location_info_;
	ThreadSafeBuffer<int> safebuf_flag_info_;

	// 初始化仿真数据
	void InitSimulationData();
	
	// 路径可视化函数
	void VisualizePaths(const PlanningTrajectory& final_trajectory,
	                    const avos::planning::Paths& casadi_path,
	                    const avos::planning::Paths& behavior_path);



	std::shared_ptr<int> flag_ptr;

};
#endif








