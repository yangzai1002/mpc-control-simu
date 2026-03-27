
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

// 使用工具链的 MsgPoint3D 替代本地 Vector3，避免与 impl_type_vector3.h 冲突
#include "ara/adsfi/impl_type_msgpoint3d.h"
// 阻止工具链的 impl_type_vector3.h 被包含
#ifndef IMPL_TYPE_VECTOR3_H
#define IMPL_TYPE_VECTOR3_H
#endif
using Vector3 = ara::adsfi::MsgPoint3D;

#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "PlanningTrajectoryPoint.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "svsPointMsg.h"
#include "LatcontrolDebugStruct.h"
#include "ara/adsfi/impl_type_vehiclelatcontrol.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "LoncontrolDebugStruct.h"
#include "PlanningTrajectory.h"
#include "PlanningTarget.h"
#include "PerceptionFinalObjects.h"
// 使用工具链的 MsgPoint3DVec 替代本地 svVector3
#include "ara/adsfi/impl_type_msgpoint3dvec.h"
using svVector3 = ara::adsfi::MsgPoint3DVec;
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_businesstaskpoints.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"

#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

#include "common/time/node_time.h"
#include "custom_stack.h"
#include "faulthandle_api.hpp"
//#include "filter_iir.h"
#include "map_util/geometry_tools.hpp"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "common/shm_util/shm_transmitter.h"

// 注意：不要使用 using namespace ara::adsfi，避免与本地类型冲突

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
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessTaskPoints> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<PlanningTarget> &ptr);
	// /tpsemanticmap 和 /tpterrainmap 数据接收
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr);

	int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &planning_result_ptr);
	int Process(const std::string &name, std::shared_ptr<ara::adsfi::PlanningStatus> &ptr);
	int Process(const std::string &name, std::shared_ptr<svVector3> &ptr);

	BASE_TEMPLATE_FUNCION

private:

	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_bus_command_;
	ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_fusion_objarray_;
	ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_vehicle_info_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_location_info_;
	ThreadSafeBuffer<ara::adsfi::BusinessTaskPoints> safebuf_task_points_;
	ThreadSafeBuffer<PlanningTarget> safebuf_planning_target_;
	// /tpsemanticmap 和 /tpterrainmap 数据缓存
	ThreadSafeBuffer<ara::adsfi::PerceptionStaticEnv> safebuf_semantic_map_;
	ThreadSafeBuffer<ara::adsfi::PerceptionStaticEnv> safebuf_terrain_map_;

	// 互斥锁与输出缓存
	std::mutex process_data_mutex_;
	bool has_valid_output_cache_ = false; // 标记缓存中是否有合法的计算结果
	ara::adsfi::PlanningStatus cached_planning_status_;

	// 仿真模式相关
	bool simulation_mode_;  // 是否启用仿真模式
	std::shared_ptr<ara::adsfi::BusinessCommand> sim_bus_command_;  // 仿真业务命令
	std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> sim_fusion_objarray_;  // 仿真融合目标数组
	std::shared_ptr<ara::adsfi::VehicleInformation> sim_vehicle_info_;  // 仿真车辆信息
	std::shared_ptr<ara::adsfi::MsgHafLocation> sim_location_info_;  // 仿真定位信息

	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_sim_bus_command_;
	ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_sim_fusion_objarray_;
	ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_sim_vehicle_info_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_sim_location_info_;
	ThreadSafeBuffer<int> safebuf_flag_info_;

	// 初始化仿真数据
	void InitSimulationData();
	
	// 路径可视化函数
	void VisualizePaths(const PlanningTrajectory& final_trajectory,
	                    const avos::planning::Paths& frenet_path,
	                    const avos::planning::Paths& behavior_path);

	// 轨迹格式转换函数
	void ConvertTrajectoryToMsgHafEgoTrajectory(const PlanningTrajectory& in, 
	                                             ara::adsfi::MsgHafEgoTrajectory& out);

	std::shared_ptr<int> flag_ptr;

};
#endif
