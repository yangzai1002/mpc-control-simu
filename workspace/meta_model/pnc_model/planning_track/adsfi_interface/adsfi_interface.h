
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

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

#include "publisher.h"
#include "core/types.h"
#include "../Emplanner/planner_interface.h"

using namespace localplanner::interface;
//using namespace ara::adsfi;

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
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr);

	int Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &plan_result_ptr);
	void coverterLoc(std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr,localplanner::interface::Location & out);
	//int Process(std::shared_ptr<ara::adsfi::PlanningStatus> &ptr);
	void coverterGridMap(std::shared_ptr<ara::adsfi::PerceptionStaticEnv>& map_ptr,std::vector<double>& occupancy_grid);

	BASE_TEMPLATE_FUNCION

private:

    struct SimObs { double x, y, w, h; };

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
	

	std::shared_ptr<localplanner::interface::PlannerInterface> local_planner_ptr;  // 仿真定位信息
	std::vector<GlobalPathPoint> interface_global_path;

	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_bus_command_;
	ThreadSafeBuffer<ara::adsfi::PerceptionStaticEnv> safebuf_Perception_grid_map;
	ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_vehicle_info_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_location_info_;

	// ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_sim_bus_command_;
	// ThreadSafeBuffer<ara::adsfi::MsgHafFusionOutArray> safebuf_sim_fusion_objarray_;
	// ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_sim_vehicle_info_;
	// ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_sim_location_info_;
	// ThreadSafeBuffer<int> safebuf_flag_info_;

	// 初始化仿真数据
	void InitSimulationData();
	
	// 路径可视化函数
	//void VisualizePaths(const PlanningTrajectory& final_trajectory,
	 //                   const avos::planning::Paths& casadi_path,
	 //                   const avos::planning::Paths& behavior_path);
	void VisualizeFinalPaths(const std::vector<PathPoint>& output_trajectory);
	void VisualizeOriginPaths();
	void VisualizeObstacles(const std::vector<SimObs>& sim_obstalces);
	void VisualGridMap(const std::vector<double> &occupancy_grid,Location sensor_loc);
	mdc::visual::Publisher current_pos_Pub = mdc::visual::Publisher::Advertise
						<mdc::visual::Marker>(ara::core::String("local_plan_current_pos"));
	mdc::visual::Publisher local_plan_trajPub = mdc::visual::Publisher::Advertise
						<mdc::visual::Marker>(ara::core::String("local_plan_trajectory"));
	mdc::visual::Publisher local_plan_obsPub = mdc::visual::Publisher::Advertise<mdc::visual::PointCloud2>
	                           (ara::core::String("local_plan_obstalces"));
	mdc::visual::Publisher gridPub = mdc::visual::Publisher::Advertise<mdc::visual::OccupancyGrid>
								(ara::core::String("plan_grid_map"));

	std::shared_ptr<int> flag_ptr;

	double origin_utm_x = 0.0;
	double origin_utm_y = 0.0;
	double plan_last_time = 0.0;



};
#endif








