#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include "custom_stack.h"
#include "log/avos_node.h"
#include "ap_log/ap_log_interface.h"

#include "common_header/geotool.h"
//#include "BlockPerceptionData.h"
//#include "XBehaviorArrive.h"
// #include "XBridge.h"
// #include "XBridgeArrive.h"
// #include "XBusinessArrive.h"
// #include "XConfigParamReader.h"
// #include "XEnvironmentalCognition.h"
// #include "XFrenetCilqrPathPlanner.h"
// // #include "XMinimumJerk.h"
// #include "XSpeedPlanArrive.h"
#include "mviz_interface/mviz_interface.h"
// svVector3 已在 adsfi_interface.h 中通过 using 别名定义为 MsgPoint3DVec
#include "base_control/lat/latcontrol_header.h"

void AdsfiInterface::Init()  
{
	SetScheduleInfo("timmer", 100);
	// ApLogInterface::Instance()->Init("planning_arrive_open");

	ApInfo("planning_arrive_open") << "init planning_arrive_open";
	
	// 初始化共享内存写入器，用于在进程间共享 PlanningStatus
	// int ret = planning_status_shm_.InitWriter("IdpPlanningStatus_arrive_shm", 10);
	// if (ret != 0) {
	// 	ApInfo("planning_arrive_open") << "Failed to init planning status shared memory writer, ret = " << ret;
	// } else {
	// 	ApInfo("planning_arrive_open") << "Successfully initialized planning status shared memory writer";
	// }
	
	// 初始化仿真模式（默认关闭，可通过环境变量或配置开启）
	if (simulation_mode_) {
		ApInfo("planning_arrive_open") << "Simulation mode enabled - initializing simulation data";
		InitSimulationData();
	} else {
		ApInfo("planning_arrive_open") << "Normal mode - waiting for external inputs";
	}
	flag_ptr = std::make_shared<int>();
}

void AdsfiInterface::DeInit()  
{
	ApInfo("planning_arrive_open") << "deinit planning_arrive_open";
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) 
{
	std::string tagstr = "callback_point_size=" + std::to_string(ptr->business_path.points.size());
	AVOS_CODETAG_ADV1(tagstr.c_str(),0,0);
	ApInfo("planning_arrive_open") << "BusinessCommand callback:" << ptr->header.seq << ", points size = " << ptr->business_path.points.size();
	safebuf_bus_command_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr) 
{
	ApInfo("planning_arrive_open") << "FusionObjectArray callback:" << ptr->seq;
	safebuf_fusion_objarray_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr) 
{
	ApInfo("planning_arrive_open") << "IdpVehicleInformation callback:" << ptr->header.seq;
	safebuf_vehicle_info_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) 
{
	ApInfo("planning_arrive_open") << "LocationInfo callback:" << ptr->header.seq;
	safebuf_location_info_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessTaskPoints> &ptr) 
{
	ApInfo("planning_arrive_open") << "BusinessTaskPoints callback";
	safebuf_task_points_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<PlanningTarget> &ptr) 
{
	ApInfo("planning_arrive_open") << "PlanningTarget callback";
	safebuf_planning_target_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr) 
{
	// 根据 name 区分 /tpsemanticmap 和 /tpterrainmap
	if (name.find("semantic") != std::string::npos || name.find("tpsemanticmap") != std::string::npos)
	{
		ApInfo("planning_arrive_open") << "PerceptionStaticEnv (semantic map) callback: seq=" << ptr->header.seq;
		safebuf_semantic_map_.Set(ptr);
	}
	else if (name.find("terrain") != std::string::npos || name.find("tpterrainmap") != std::string::npos)
	{
		ApInfo("planning_arrive_open") << "PerceptionStaticEnv (terrain map) callback: seq=" << ptr->header.seq;
		safebuf_terrain_map_.Set(ptr);
	}
	else
	{
		ApWarn("planning_arrive_open") << "PerceptionStaticEnv callback (unknown type): name=" << name << ", seq=" << ptr->header.seq;
		safebuf_semantic_map_.Set(ptr);
	}
}

// 将内部 PlanningTrajectory 转换为 MsgHafEgoTrajectory
void AdsfiInterface::ConvertTrajectoryToMsgHafEgoTrajectory(const PlanningTrajectory& in, 
                                                            ara::adsfi::MsgHafEgoTrajectory& out)
{
	// 1. 初始化智能指针
	// out = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>();

	// 2. 转换头部信息
	out.egoTrajectoryHeader.timestamp = DoubleToHafTime(in.header.time);
	out.egoTrajectoryHeader.seq = static_cast<uint32_t>(in.header.seq);
	out.egoTrajectoryHeader.frameID = in.header.frame_id;

	// 3. 准备容器
	size_t point_size = in.trajectory.size();
	out.trajectoryPoints.clear();
	out.trajectoryPoints.reserve(point_size);
	out.wayPoints.clear();
	out.wayPoints.reserve(point_size);

	// 4. 设置挡位信息
	// if(!in.trajectory.empty()) {
	// 	if(in.trajectory.front().type == 1) {
	// 		out->gear = 4;
	// 	}
	// 	else {
	// 		out->gear = in.trajectory.back().direction;
	// 	}
	// } else {
	// 	out->gear = 0;
	// }
	if(!in.trajectory.empty()) {
		if(in.trajectory.front().type == 1) {
			out.gear = 4;  // C: 中心转向
		} else {
			switch(in.trajectory.back().direction) {
				case 0:  out.gear = 3; break;  // D: 前进
				case 1:  out.gear = 1; break;  // R: 倒车
				default: out.gear = 0; break;  // P: 异常兜底
			}
		}
	} else {
		out.gear = 0;  // P: 无轨迹时驻车
	}

	// 5. 遍历并转换轨迹点
	for (const auto& src_pt : in.trajectory) {
		ara::adsfi::MsgHafTrajectoryPoint dst_pt;

		// --- 填充 Header (复用外层 Header) ---
		dst_pt.header = out.egoTrajectoryHeader;
		dst_pt.wayPoint.header = out.egoTrajectoryHeader;

		// --- 填充物理/运动学属性 ---
		dst_pt.speed = static_cast<double>(src_pt.velocity);
		dst_pt.acc = static_cast<double>(src_pt.a);
		dst_pt.timeRelative = static_cast<double>(src_pt.t);

		// --- 填充 WayPoint 信息 ---
		// 车体坐标系
		dst_pt.wayPoint.x = static_cast<double>(src_pt.x);
		dst_pt.wayPoint.y = static_cast<double>(src_pt.y);
		dst_pt.wayPoint.angle = static_cast<double>(src_pt.angle);

		// 绝对坐标 (UTM/Global)
		dst_pt.wayPoint.xg = src_pt.xg;
		dst_pt.wayPoint.yg = src_pt.yg;

		// 路径属性
		dst_pt.wayPoint.theta = static_cast<double>(src_pt.angleglobal);
		dst_pt.wayPoint.curvature = static_cast<double>(src_pt.curvature);
		dst_pt.wayPoint.s = static_cast<double>(src_pt.length);

		// 推入向量
		out.trajectoryPoints.push_back(dst_pt);
		out.wayPoints.push_back(dst_pt.wayPoint);
	}
}

// 从 MsgHafLocation 提取时间戳（秒）
static double GetLocationTimestamp(const std::shared_ptr<ara::adsfi::MsgHafLocation>& loc_ptr)
{
	// MsgHafHeader 使用 timestamp 成员（MsgHafTime 类型）
	return static_cast<double>(loc_ptr->header.timestamp.sec) 
	       + static_cast<double>(loc_ptr->header.timestamp.nsec) * 1e-9;
}

// 将融合障碍物数组转换为 PerceptionFinalObjects
void coverter(std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_objarray_ptr, PerceptionFinalObjects & out)
{
	out.Clear();
	
	// 设置头信息 (CommonHeader.time 是 double 类型)
	out.header.time = static_cast<double>(fusion_objarray_ptr->stamp.sec) 
	                  + static_cast<double>(fusion_objarray_ptr->stamp.nsec) * 1e-9;
	out.header.seq = fusion_objarray_ptr->seq;
	out.header.data_name = fusion_objarray_ptr->frameID;
	out.send_stamp = out.header.time;
	out.start_stamp = out.send_stamp;
	
	// 转换障碍物列表
	out.objs.clear();
	for (size_t i = 0; i < fusion_objarray_ptr->fusionOut.size(); i++)
	{
		const auto& src = fusion_objarray_ptr->fusionOut[i];
		PerceptionObject obj;
		obj.Clear();
		
		obj.id = src.objectID;
		obj.type = static_cast<int>(src.cs);
		obj.age = 0;  // MsgHafFusionOut 无此字段
		obj.fusion_source = src.fusionType;
		obj.motion_state = 0;  // 需根据速度判断
		
		// 相对坐标信息
		obj.vcs_info.x = static_cast<float>(src.rect.center.x);
		obj.vcs_info.y = static_cast<float>(src.rect.center.y);
		obj.vcs_info.z = static_cast<float>(src.rect.center.z);
		obj.vcs_info.heading = src.rect.orientation;  // yaw (MsgHafRect3D uses orientation)
		obj.vcs_info.vx = src.velocity.x;
		obj.vcs_info.vy = src.velocity.y;
		
		// 目标尺寸
		obj.obj_size.length = static_cast<float>(src.rect.size.x);
		obj.obj_size.width = static_cast<float>(src.rect.size.y);
		obj.obj_size.height = static_cast<float>(src.rect.size.z);
		
		// 绝对坐标信息
		obj.dr_info.xg = static_cast<float>(src.absRect.center.x);
		obj.dr_info.yg = static_cast<float>(src.absRect.center.y);
		obj.dr_info.heading = src.absRect.orientation;
		obj.dr_info.vxg = src.absVelocity.x;
		obj.dr_info.vyg = src.absVelocity.y;
		obj.dr_info.speed = std::sqrt(src.absVelocity.x * src.absVelocity.x + src.absVelocity.y * src.absVelocity.y);
		
		// 车灯状态
		obj.carlight = src.blinkerFlag;
		
		out.objs.push_back(obj);
	}
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &planning_result_ptr) 
{
// 	ApInfo("planning_arrive_open") << "PROCESS PLANNING_ARRIVE_OPEN";

// 	std::shared_ptr<ara::adsfi::BusinessCommand> bus_command_ptr;
// 	std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_objarray_ptr;
// 	std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr;
// 	std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr;
// 	std::shared_ptr<ara::adsfi::BusinessTaskPoints> task_points_ptr;
// 	std::shared_ptr<PlanningTarget> planning_target_ptr;
// 	std::shared_ptr<ara::adsfi::PerceptionStaticEnv> semantic_map_ptr;
// 	std::shared_ptr<ara::adsfi::PerceptionStaticEnv> terrain_map_ptr;
	
// 	bool bGet_bus_cmd = safebuf_bus_command_.Get(bus_command_ptr);
// 	bool bGet_fus_obj = safebuf_fusion_objarray_.Get(fusion_objarray_ptr);
// 	bool bGet_veh_inf = safebuf_vehicle_info_.Get(vehicle_info_ptr);
// 	bool bGet_loc_inf = safebuf_location_info_.Get(loc_ptr);
// 	bool bGet_task_pts = safebuf_task_points_.Get(task_points_ptr);
// 	bool bGet_planning_target = safebuf_planning_target_.Get(planning_target_ptr);
// 	bool bGet_semantic_map = safebuf_semantic_map_.Get(semantic_map_ptr);
// 	bool bGet_terrain_map = safebuf_terrain_map_.Get(terrain_map_ptr);
// 	// 先获取定位信息
// 	if(bGet_loc_inf == false)
// 	{
// 		if(simulation_mode_)
// 		{
// 			loc_ptr = sim_location_info_;
// 			if(!loc_ptr)
// 			{
// 				ApWarn("planning_arrive_open") << "Simulation mode: LocationInfo not initialized";
// 				return -1;
// 			}
// 			ApInfo("planning_arrive_open") << "Simulation mode: using preset LocationInfo";
// 		}
// 		else
// 		{
// 			ApInfo("planning_arrive_open") << "Process error: LocationInfo has no new data";
// 			return -1;
// 		}
// 	}
	
// 	// 定位信息 (直接使用 MsgHafLocation，XBridge 会转换为 LocalizationData)
// 	static ara::adsfi::MsgHafLocation m_10_0;
// 	m_10_0 = *loc_ptr;

// 	// 获取业务命令
// 	if(bGet_bus_cmd == false)
// 	{
// 		if(!simulation_mode_)
// 		{
// 			ApInfo("planning_arrive_open") << "Process error: BusinessCommand has no new data";
// 			return -1;
// 		}
// 		ApInfo("planning_arrive_open") << "Simulation mode: BusinessCommand has no new data, creating default 100m trajectory";
// 		// 外部没有输入时，根据当前定位信息创建一条100米长的直线轨迹
// 		bus_command_ptr = std::make_shared<ara::adsfi::BusinessCommand>();
// 		bus_command_ptr->header.seq = 0;
// 		double now_sec = avos::common::NodeTime::Now().ToSecond();
// 		bus_command_ptr->header.time.sec = static_cast<int32_t>(now_sec);
// 		bus_command_ptr->header.time.nsec = static_cast<int32_t>((now_sec - bus_command_ptr->header.time.sec) * 1e9);
// 		bus_command_ptr->param.command = 1;
// 		bus_command_ptr->param.max_speed = 10.0;
// 		bus_command_ptr->business_mode = 1;
// 		bus_command_ptr->param.task_avoid_mode = 0;
		
// 		// 业务轨迹使用 UTM 坐标系
// 		// 从当前定位的 UTM 坐标开始，生成 100m 直线轨迹
// 		avos::vectormap::GeoTool geotool;
// 		avos::vectormap::PointGPS gps_point(loc_ptr->llh.x, loc_ptr->llh.y, 0.0);
// 		avos::vectormap::PointGCCS current_pos;
// 		geotool.GPS2GCCS(gps_point, current_pos);
		
// 		const double trajectory_length = 100.0;
// 		const double point_interval = 1.0;
// 		const int num_points = static_cast<int>(trajectory_length / point_interval) + 1;
		
// 		bus_command_ptr->business_path.points.clear();
// 		bus_command_ptr->business_path.length = trajectory_length;
		
// 		// 使用 UTM 坐标系的航向角
// 		double yaw_rad = current_pos.angle * M_PI / 180.0;
		
// 		for (int i = 0; i < num_points; ++i) {
// 			ara::adsfi::BusinessPoint point;
// 			double distance = i * point_interval;
			
// 			// 全局坐标使用 UTM
// 			point.xg = current_pos.xg + distance * cos(yaw_rad);
// 			point.yg = current_pos.yg + distance * sin(yaw_rad);
// 			// 局部坐标（相对于车辆当前位置）
// 			point.x = distance * cos(yaw_rad);
// 			point.y = distance * sin(yaw_rad);
// 			point.heading = current_pos.angle;
// 			point.angle = 0.0;
// 			point.direction = 0;
// 			point.index = i;
			
// 			bus_command_ptr->business_path.points.push_back(point);
// 		}
		
// 		ApInfo("planning_arrive_open") << "Created default trajectory with " << bus_command_ptr->business_path.points.size() 
// 		          << " points, length: " << trajectory_length << "m";
// 	}
	
// 	// 获取融合障碍物数组
// 	if(bGet_fus_obj == false)
// 	{
// 		fusion_objarray_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
// 		fusion_objarray_ptr->seq = 0;
// 		fusion_objarray_ptr->stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
// 		fusion_objarray_ptr->stamp.nsec = 0;
// 		fusion_objarray_ptr->frameID = "default";
// 		fusion_objarray_ptr->fusionOut.clear();
// 		if(simulation_mode_)
// 		{
// 			ApInfo("planning_arrive_open") << "Simulation mode: using empty FusionObjectArray";
// 		}
// 		else
// 		{
// 			ApWarn("planning_arrive_open") << "Process warning: FusionObjectArray has no new data, using empty array";
// 		}
// 	}
	
// 	// 获取车辆信息
// 	if(bGet_veh_inf == false)
// 	{
// 		if(simulation_mode_)
// 		{
// 			vehicle_info_ptr = sim_vehicle_info_;
// 			if(!vehicle_info_ptr)
// 			{
// 				vehicle_info_ptr = std::make_shared<ara::adsfi::VehicleInformation>();
// 				vehicle_info_ptr->header.seq = 0;
// 				double veh_now_sec = avos::common::NodeTime::Now().ToSecond();
// 				vehicle_info_ptr->header.time.sec = static_cast<int32_t>(veh_now_sec);
// 				vehicle_info_ptr->header.time.nsec = static_cast<int32_t>((veh_now_sec - vehicle_info_ptr->header.time.sec) * 1e9);
// 				vehicle_info_ptr->vehicle_act_state.speed = 0.0;
// 				vehicle_info_ptr->vehicle_act_state.steer_angle = 0.0;
// 				vehicle_info_ptr->vehicle_act_state.acceleration = 0.0;
// 				vehicle_info_ptr->vehicle_act_state.drive_mode = 0;
// 				vehicle_info_ptr->vehicle_act_state.shift_position = 0;
// 			}
// 			ApInfo("planning_arrive_open") << "Simulation mode: using preset VehicleInformation";
// 		}
// 		else
// 		{
// 			ApInfo("planning_arrive_open") << "Process error: IdpVehicleInformation has no new data";
// 			return -1;
// 		}
// 	}

// 	// 初始化任务点
// 	if(!task_points_ptr)
// 	{
// 		task_points_ptr = std::make_shared<ara::adsfi::BusinessTaskPoints>();
// 	}

// 	// 初始化规划目标
// 	if(!planning_target_ptr)
// 	{
// 		planning_target_ptr = std::make_shared<PlanningTarget>();
// 	}
// 	std::lock_guard<std::mutex> lock(process_data_mutex_);

// 	// ========== 静态变量声明（对应老架构的变量）==========
// 	static ara::adsfi::BusinessCommand m_13_0;              // BusinessCommand
// 	m_13_0 = *bus_command_ptr;
// 	static ara::adsfi::BusinessTaskPoints m_14_0;           // BusinessTaskPoints
// 	if(task_points_ptr) m_14_0 = *task_points_ptr;
// 	static PerceptionFinalObjects m_22_1;                   // PerceptionFinalObjects (output of BlockPerceptionData)
// 	static PerceptionFinalObjects m_9_0;                    // PerceptionFinalObjects (input from fusion_objarray)
// 	static ara::adsfi::PerceptionStaticEnv m_11_0;          // PerceptionStaticEnv (input from /tpsemanticmap)
	
// 	// 将 fusion_objarray 转换为 PerceptionFinalObjects 作为 m_9_0 输入
// 	coverter(fusion_objarray_ptr, m_9_0);
// 	ApInfo("planning_arrive_open") << "Converted FusionOutArray to PerceptionFinalObjects, objs size=" << m_9_0.objs.size();
	
// 	// 使用 /tpsemanticmap 数据作为 m_11_0
// 	if(bGet_semantic_map && semantic_map_ptr)
// 	{
// 		m_11_0 = *semantic_map_ptr;
// 		ApInfo("planning_arrive_open") << "Using /tpsemanticmap data, seq=" << m_11_0.header.seq;
// 	}
// 	else
// 	{
// 		// 无语义地图数据时清空
// 		m_11_0 = ara::adsfi::PerceptionStaticEnv();
// 		ApWarn("planning_arrive_open") << "No /tpsemanticmap data, using empty PerceptionStaticEnv";
// 	}
	
// 	// 地形地图数据暂时只接收不使用
// 	(void)bGet_terrain_map;
// 	(void)terrain_map_ptr;
// 	static ara::adsfi::PerceptionStaticEnv m_22_0;          // PerceptionStaticEnv (output of BlockPerceptionData)
// 	static ara::adsfi::PlanningStatus m_2_2;                // PlanningStatus
// 	static PlanningTarget m_16_0;                           // PlanningTarget
// 	if(planning_target_ptr) m_16_0 = *planning_target_ptr;
// 	static PlanningTrajectory m_21_0;                       // PlanningTrajectory (output of XMinimumJerk)
// 	static PlanningTrajectory m_7_1;                        // PlanningTrajectory (output of XSpeedPlanArrive)
// 	// static ara::adsfi::MsgHafLocation m_10_0;            // MsgHafLocation (已在前面赋值)
// 	static ara::adsfi::VehicleInformation m_12_0;           // VehicleInformation
// 	m_12_0 = *vehicle_info_ptr;
	
// 	// 内部处理变量
// 	static avos::planning::ConfigInfo m_8_0;                // 配置信息
// 	static avos::planning::InnerPath m_1_0;                 // 内部路径
// 	static avos::planning::LocalizationData m_0_1;          // 定位数据
// 	static avos::planning::Paths m_21_1;                    // 路径 (XMinimumJerk output)
// 	static avos::planning::Paths m_2_1;                     // 路径 (XBusinessArrive output)
// 	static avos::planning::Paths m_3_2;                     // 路径 (XEnvironmentalCognition output)
// 	static avos::planning::Paths m_4_2;                     // 路径 (XBehaviorArrive output)
// 	static avos::planning::Paths m_6_0;                     // 路径 (XFrenetCilqrPathPlanner output)
// 	static avos::planning::Paths m_7_0;                     // 路径 (XSpeedPlanArrive input/output)
// 	static avos::planning::Perception m_0_0;                // 感知数据
// 	static avos::planning::PlanningInfo m_2_0;              // 规划信息 (XBusinessArrive output)
// 	static avos::planning::PlanningInfo m_3_1;              // 规划信息 (XEnvironmentalCognition output)
// 	static avos::planning::PlanningInfo m_4_1;              // 规划信息 (XBehaviorArrive output)
// 	static avos::planning::TrackTaskPoint m_1_1;            // 跟踪任务点
// 	static avos::planning::UnstructEnv m_3_0;               // 非结构化环境 (XEnvironmentalCognition output)
// 	static avos::planning::UnstructEnv m_4_0;               // 非结构化环境 (XBehaviorArrive input)
// 	static avos::planning::VehicleStatus m_0_2;             // 车辆状态
// 	static svVector3 m_3_3;                                 // 可视化对象 (XEnvironmentalCognition output)

// 	// ========== 函数调用顺序（按照老架构 Xarrive_open_filed 的顺序）==========
	
// 	// 1. 读取配置参数
// 	XConfigParamReader(m_8_0);
	
// 	// 2. 感知数据处理
// 	BlockPerceptionData(m_11_0, m_9_0, m_13_0, m_22_0, m_22_1);
	
// 	// 3. 数据桥接转换
// 	XBridge(m_13_0, m_10_0, m_22_0, m_22_1, m_12_0, m_0_0, m_0_1, m_0_2);
	
// 	// 4. Arrive模式桥接
// 	XBridgeArrive(m_0_1, m_13_0, m_1_0, m_14_0, m_1_1);
	
// 	// 5. Arrive业务处理
// 	XBusinessArrive(m_0_1, m_1_0, m_0_2, m_1_1, m_8_0, m_7_0, m_2_0, m_2_1, m_2_2);
	
// 	// 6. 环境认知
// 	XEnvironmentalCognition(m_0_1, m_0_0, m_0_2, m_8_0, m_2_0, m_2_1, m_4_0, m_16_0, m_3_0, m_3_1, m_3_2, m_3_3);
	
// 	// 7. Arrive行为规划
// 	XBehaviorArrive(m_0_1, m_0_2, m_8_0, m_3_0, m_3_1, m_3_2, m_4_0, m_4_1, m_4_2);
	
// 	// 8. Frenet坐标系CILQR路径规划
// 	XFrenetCilqrPathPlanner(m_0_1, m_0_0, m_0_2, m_8_0, m_4_0, m_4_2, m_4_1, m_6_0);
	
// 	// 9. 最小急动度平滑
// 	//XMinimumJerk(m_0_1, m_11_0, m_13_0, m_4_2, m_21_0, m_21_1);
	
// 	// 10. Arrive速度规划
// 	XSpeedPlanArrive(m_0_1, m_0_0, m_0_2, m_8_0, m_4_1, m_6_0, m_21_1, m_7_0, m_7_1);

// 	// ========== 输出转换 ==========
// 	// 1. 临时转换
// 	ara::adsfi::MsgHafEgoTrajectory temp_out_trajectory;
// 	ConvertTrajectoryToMsgHafEgoTrajectory(m_7_1, temp_out_trajectory);

// 	// 2. 更新类成员状态
// 	cached_planning_status_ = m_2_2;
// 	has_valid_output_cache_ = true;

// 	// 3. 内存安全隔离：堆区重新分配
// 	planning_result_ptr = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>(temp_out_trajectory);

// 	// 路径可视化
// 	VisualizePaths(m_7_1, m_6_0, m_4_2);

// 	safebuf_flag_info_.SetNotify(flag_ptr);
// 	return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::PlanningStatus> &ptr)
{
	// std::shared_ptr<int> temp;
	// safebuf_flag_info_.WaitForGet(temp); // 阻塞等待主线程发出唤醒信号
	
	// // 被唤醒后，第一时间加锁
	// std::lock_guard<std::mutex> lock(process_data_mutex_);
	
	// // 拿到锁后，检查是否有主线程合法写入的缓存数据
	// if (has_valid_output_cache_) {
	// 	// 分配全新内存，安全拷贝并返回
	// 	ptr = std::make_shared<ara::adsfi::PlanningStatus>(cached_planning_status_);
	// 	return 0;
	// } 
	
	ApInfo("planning_arrive_open") << "error publish PlanningStatus - no valid cache";
	return -1;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<svVector3> &ptr)
{
	// 用于发布可视化对象数据
	// 目前返回空实现，可根据需要扩展
	ptr = std::make_shared<svVector3>();
	return 0;
}

// 初始化仿真数据
void AdsfiInterface::InitSimulationData()
{
	// 初始化定位信息
	sim_location_info_ = std::make_shared<ara::adsfi::MsgHafLocation>();
	sim_location_info_->header.seq = 0;
	sim_location_info_->header.timestamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	sim_location_info_->header.timestamp.nsec = 0;
	sim_location_info_->header.frameID = "simulation";
	
	// 设置经纬度 (llh 字段)
	const double init_lon = 116.3974;  // 经度
	const double init_lat = 39.9093;   // 纬度
	sim_location_info_->llh.x = init_lon;
	sim_location_info_->llh.y = init_lat;
	sim_location_info_->llh.z = 0.0;
	
	// 使用 GeoTool 将经纬度转换为 UTM 坐标
	avos::vectormap::GeoTool geotool;
	avos::vectormap::PointGPS gps_point(init_lon, init_lat, 0.0);
	avos::vectormap::PointGCCS gccs_point;
	geotool.GPS2GCCS(gps_point, gccs_point);
	
	// 设置初始位置 (pose - UTM坐标)
	sim_location_info_->pose.pose.position.x = gccs_point.xg;
	sim_location_info_->pose.pose.position.y = gccs_point.yg;
	sim_location_info_->pose.pose.position.z = 0.0;
	
	// 设置初始姿态 (MsgPoint3D 只有 x, y, z，没有 w)
	sim_location_info_->pose.pose.orientation.x = 0.0;  // roll
	sim_location_info_->pose.pose.orientation.y = 0.0;  // pitch
	sim_location_info_->pose.pose.orientation.z = 90.0;  // yaw (朝北，与生成的轨迹方向一致)
	
	// 设置 DR 定位数据 (dr_pose - 弱定位模式下使用)
	// 在弱定位模式下，系统使用 dr_pose 而不是 pose
	// 为了与业务轨迹（UTM坐标）匹配，dr_pose 也设置为 UTM 坐标
	sim_location_info_->dr_pose.pose.position.x = 0.0;  // UTM X
	sim_location_info_->dr_pose.pose.position.y = 0.0;  // UTM Y
	sim_location_info_->dr_pose.pose.position.z = 0.0;
	sim_location_info_->dr_pose.pose.orientation.x = 0.0;
	sim_location_info_->dr_pose.pose.orientation.y = 0.0;
	sim_location_info_->dr_pose.pose.orientation.z = 0.0;  // yaw
	
	// 设置初始速度 (与车辆信息速度保持一致)
	const double init_speed = 1.0;  // m/s
	sim_location_info_->velocity.twist.linear.x = init_speed;
	sim_location_info_->velocity.twist.linear.y = 0.0;
	sim_location_info_->velocity.twist.linear.z = 0.0;
	sim_location_info_->velocity.twist.angular.x = 0.0;
	sim_location_info_->velocity.twist.angular.y = 0.0;
	sim_location_info_->velocity.twist.angular.z = 0.0;
	
	// 设置定位状态为有效
	sim_location_info_->locationState = 1;  // 定位有效
	sim_location_info_->v = init_speed;  // 速度
	/home/cxy/control-simulation/
	// 初始化车辆信息
	sim_vehicle_info_ = std::make_shared<ara::adsfi::VehicleInformation>();
	sim_vehicle_info_->header.seq = 0;
	double sim_veh_now = avos::common::NodeTime::Now().ToSecond();
	sim_vehicle_info_->header.time.sec = static_cast<int32_t>(sim_veh_now);
	sim_vehicle_info_->header.time.nsec = static_cast<int32_t>((sim_veh_now - sim_vehicle_info_->header.time.sec) * 1e9);
	// 设置初始速度为 1.0 m/s，避免因速度过低导致任务提前结束
	// (CheckTaskOver 条件: velocity < 0.1 && path.size() < 155 会导致任务结束)
	sim_vehicle_info_->vehicle_act_state.speed = 1.0;  // m/s
	sim_vehicle_info_->vehicle_act_state.steer_angle = 0.0;
	sim_vehicle_info_->vehicle_act_state.acceleration = 0.0;
	sim_vehicle_info_->vehicle_act_state.drive_mode = 1;  // 自动驾驶模式
	sim_vehicle_info_->vehicle_act_state.shift_position = 1;  // D 档
	
	// 初始化融合目标数组
	sim_fusion_objarray_ = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
	sim_fusion_objarray_->seq = 0;
	sim_fusion_objarray_->stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	sim_fusion_objarray_->stamp.nsec = 0;
	sim_fusion_objarray_->frameID = "simulation";
	sim_fusion_objarray_->fusionOut.clear();
	
	ApInfo("planning_arrive_open") << "Simulation data initialized:"
		<< " LLH(lon=" << sim_location_info_->llh.x << ", lat=" << sim_location_info_->llh.y << ")"
		<< " UTM(xg=" << sim_location_info_->pose.pose.position.x << ", yg=" << sim_location_info_->pose.pose.position.y << ")"
		<< " DR(xg=" << sim_location_info_->dr_pose.pose.position.x << ", yg=" << sim_location_info_->dr_pose.pose.position.y << ")"
		<< " heading=" << sim_location_info_->pose.pose.orientation.z
		<< " speed=" << sim_vehicle_info_->vehicle_act_state.speed;
}

// 路径可视化函数
void AdsfiInterface::VisualizePaths(const PlanningTrajectory& final_trajectory,
                                    const avos::planning::Paths& frenet_path,
                                    const avos::planning::Paths& behavior_path)
{
	// 定义路径点结构
	struct PathPoint {
		double x, y, z;
		double velocity;
	};
	
	// 使用点云方式可视化路径
	static RGBPointCloud<PathPoint> viz_final_path("planning/arrive_final_trajectory");
	static RGBPointCloud<PathPoint> viz_frenet_path("planning/arrive_frenet_path");
	static RGBPointCloud<Pat/home/cxy/control-simulation/hPoint> viz_behavior_path("planning/arrive_behavior_path");
	
	// 速度阈值
	const double UP_SPEED = 40.0 / 3.6;  // 40 km/h
	
	// 颜色函数：根据速度设置颜色
	auto setColorByVelocity = [UP_SPEED](void* p) -> float {
		PathPoint* point = static_cast<PathPoint*>(p);
		double velocity = point->velocity;
		
		if (fabs(velocity) <= 0.001) {
			return 1.0;  // 白色
		} else if (velocity < 0) {
			return 0.0;  // 黑色
		} else if (velocity >= UP_SPEED) {
			return 0.0;  // 红色
		} else if (velocity < UP_SPEED / 2.0) {
			return 1.0;  // 绿色到黄色
		} else if (velocity < UP_SPEED) {
			return 1.0 - (velocity - UP_SPEED / 2.0) / (UP_SPEED / 2.0);
		}
		return 0.0;
	};
	
	// 可视化最终轨迹
	viz_final_path.Reset();
	viz_final_path.SetRFunc([UP_SPEED](void* p) -> float {
		PathPoint* point = static_cast<PathPoint*>(p);
		double velocity = point->velocity;
		if (fabs(velocity) <= 0.001) return 1.0;
		if (velocity < 0) return 0.0;
		if (velocity >= UP_SPEED) return 1.0;
		if (velocity < UP_SPEED / 2.0) return velocity / (UP_SPEED / 2.0);
		return 1.0;
	});
	viz_final_path.SetGFunc(setColorByVelocity);
	viz_final_path.SetBFunc([](void* p) -> float { return 0.0; });
	
	for (size_t i = 0; i < final_trajectory.trajectory.size(); i++) {
		PathPoint point;
		point.x = final_trajectory.trajectory[i].x;
		point.y = final_trajectory.trajectory[i].y;
		point.z = 0.0;
		point.velocity = final_trajectory.trajectory[i].velocity;
		viz_final_path.inner_points.push_back(point);
	}
	if (!viz_final_path.inner_points.empty()) {
		viz_final_path.publish('y');  // 'y' = yellow color
	}
}
