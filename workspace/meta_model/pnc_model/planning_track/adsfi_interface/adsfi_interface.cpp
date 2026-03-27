#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include "custom_stack.h"
#include "log/avos_node.h"

#include "common_header/geotool.h"
#include "BlockPerceptionData.h"
#include  "XBehaviorComplexTrack.h"
#include "XBridge.h"
#include "XBridgeTrack.h"
#include "XBusinessTrack.h"
#include "XCasadiLocalOptimize.h"
#include "XConfigParamReader.h"
#include "XDynamicProgramming.h"
#include "XEnvironmentalCognition.h"
#include "XHybridAstar.h"
#include "XSpeedPlanTrack.h"
#include "XTurnAround.h"
#include "mviz_interface/mviz_interface.h"

void AdsfiInterface::Init()  
{
	SetScheduleInfo("timmer", 100);
	std::cout << "init" << std::endl;
	
	// 初始化共享内存写入器，用于在进程间共享 PlanningStatus
	// topic 名称用于生成唯一的共享内存 key
	int ret = planning_status_shm_.InitWriter("IdpPlanningStatus_shm", 10);
	if (ret != 0) {
		std::cout << "Failed to init planning status shared memory writer, ret = " << ret << std::endl;
	} else {
		std::cout << "Successfully initialized planning status shared memory writer" << std::endl;
	}
	
	// 初始化仿真模式（默认关闭，可通过环境变量或配置开启）
	
	if (simulation_mode_) {
		std::cout << "Simulation mode enabled - initializing simulation data" << std::endl;
		InitSimulationData();
	} else {
		std::cout << "Normal mode - waiting for external inputs" << std::endl;
	}
	flag_ptr = std::make_shared<int>();
}

void AdsfiInterface::DeInit()  
{
	std::cout << "deinit" << std::endl;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) 
{
	std::string tagstr = "callback_point_size=" + std::to_string(ptr->business_path.points.size());
	AVOS_CODETAG_ADV1(tagstr.c_str(),0,0);
	std::cout << "BusinessCommand  callback:" << ptr->header.seq << std::endl;
	AINFO<< "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSsss = " << ptr->business_path.points.size() << std::endl;
	// buscommand_deque.push(ptr);
	safebuf_bus_command_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr) 
{
	std::cout << "FusionObjectArray  callback:" << ptr->header.seq << std::endl;
	// fusionobjarray_deque.push(ptr);
	safebuf_fusion_objarray_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr) 
{
	std::cout << "IdpVehicleInformation  callback:" << ptr->header.seq << std::endl;
	// vehicleinfo_deque.push(ptr);
	safebuf_vehicle_info_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) 
{
	std::cout << "LocationInfo  callback:" << ptr->header.seq << std::endl;
	// locationinfo_deque.push(ptr);
	safebuf_location_info_.Set(ptr);
}

void coverter(std::shared_ptr<adsfi::PlanningResult> planning_result_ptr,PlanningTrajectory & out)
{
    //need add
    for (size_t i = 0; i < planning_result_ptr->trajectoryPoints.size(); i++)
    {

        out.trajectory[i].velocity = planning_result_ptr->trajectoryPoints[i].speed;
        out.trajectory[i].a = planning_result_ptr->trajectoryPoints[i].acc;
        out.trajectory[i].t = planning_result_ptr->trajectoryPoints[i].timeRelative;
        // out.trajectory[i].velocity = planning_result_ptr->trajectoryPoints[i].deltaAcc;
        out.trajectory[i].angle = planning_result_ptr->trajectoryPoints[i].steerAngle;

        out.trajectory[i].x = planning_result_ptr->trajectoryPoints[i].wayPoint.x;
        out.trajectory[i].y = planning_result_ptr->trajectoryPoints[i].wayPoint.y;

    }
}

//TODO
void coverter(std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr,ara::adsfi::IdpSensorLocFusion & out)
{
    //need add
    double loc_last_time = static_cast<double>(loc_ptr->header.stamp.sec) 
                                                    + static_cast<double>(loc_ptr->header.stamp.nsec) * 1e-9;
    out.header.time = loc_last_time;
	out.llh.lat = loc_ptr->pose.pose.position.x;
	out.llh.lon = loc_ptr->pose.pose.position.y;
	double heading = loc_ptr->pose.pose.orientation.z;
	avos::vectormap::GeoTool geotool;
	avos::vectormap::PointGPS gps_point(out.llh.lon, out.llh.lat, heading);
	avos::vectormap::PointGCCS loc;
	geotool.GPS2GCCS(gps_point, loc);
    out.pose.pose_euler.position.x = loc.xg;
    out.pose.pose_euler.position.y = loc.yg;
    // out.pose.pose_euler.rotation.yaw = loc.angle;
    out.pose.pose_euler.rotation.yaw = heading;
	AINFO << "loc.xg = " << loc.xg << ",loc.yg = " << loc.yg << ", out.pose.pose_euler.rotation.yaw = " << out.pose.pose_euler.rotation.yaw;
	out.pose.pose_euler.rotation.pitch = loc_ptr->pose.pose.orientation.y;
	out.pose.pose_euler.rotation.roll = loc_ptr->pose.pose.orientation.x;
	double speed = std::hypot(loc_ptr->velocity.twist.linear.x, loc_ptr->velocity.twist.linear.y);
	speed = std::hypot(speed, loc_ptr->velocity.twist.linear.z);
	out.v = speed;
	out.twist.angular_velocity.z = loc_ptr->velocity.twist.angular.z;
}

void coverter(std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_objarray_ptr,ara::adsfi::PerceptionStaticEnv & out)
{
    //need add
    double fusion_objarray_last_time = static_cast<double>(fusion_objarray_ptr->header.stamp.sec) 
                                                    + static_cast<double>(fusion_objarray_ptr->header.stamp.nsec) * 1e-9;
    out.header.time = fusion_objarray_last_time;
	out.header.data_name = fusion_objarray_ptr->header.frameId;
	out.semantic_objs.clear();
	for (size_t i = 0; i < fusion_objarray_ptr->object.size(); i++)
	{
		ara::adsfi::PerceptionSemanticObj perception_semantic_obj;
		perception_semantic_obj.x = fusion_objarray_ptr->object[i].rect3d.center.x;
		perception_semantic_obj.y = fusion_objarray_ptr->object[i].rect3d.center.y;
		perception_semantic_obj.z = fusion_objarray_ptr->object[i].rect3d.center.z;
		perception_semantic_obj.type = fusion_objarray_ptr->object[i].cls;
		//TODO
		perception_semantic_obj.obj_size.width = fusion_objarray_ptr->object[i].rect3d.size.x;
		perception_semantic_obj.obj_size.length = fusion_objarray_ptr->object[i].rect3d.size.y;
		perception_semantic_obj.obj_size.height = fusion_objarray_ptr->object[i].rect3d.size.z;
		// perception_semantic_obj.speed = fusion_objarray_ptr->object[i].velocity;
		ara::adsfi::PerceptionSemanticCell perception_semantic_cell;
		for (size_t j = 0; j < fusion_objarray_ptr->object[i].pointsInObject.size(); j++)
		{
			perception_semantic_cell.x = fusion_objarray_ptr->object[i].pointsInObject[j].x;
			perception_semantic_cell.y = fusion_objarray_ptr->object[i].pointsInObject[j].y;
			// perception_semantic_cell.Z = fusion_objarray_ptr->object[i].pointsInObject[j].z;
			perception_semantic_obj.cells.push_back(perception_semantic_cell);
		}
		out.semantic_objs.push_back(perception_semantic_obj);
	}
    
}

void coverter(const PlanningTrajectory & in, std::shared_ptr<adsfi::PlanningResult> &out)
{
    //need add
	out = std::make_shared<adsfi::PlanningResult>();
    out->header.stamp.sec = static_cast<int32_t>(in.header.time);
    out->header.stamp.nsec = static_cast<int32_t>((in.header.time - static_cast<int32_t>(in.header.time)) * 1e9);
	out->gear = 0;
	if(!in.trajectory.empty()) {
		out->gear = in.trajectory.back().direction;
	}
	
	// 先清空并调整大小，避免越界访问
	out->trajectoryPoints.clear();
	out->trajectoryPoints.resize(in.trajectory.size());
	
	for (size_t i = 0; i < in.trajectory.size(); i++)
    {
        out->trajectoryPoints[i].speed = in.trajectory[i].velocity;
		out->trajectoryPoints[i].acc = in.trajectory[i].a;
		out->trajectoryPoints[i].timeRelative = in.trajectory[i].t;
		// out->trajectoryPoints[i].steerAngle = in.trajectory[i].angle;
		out->trajectoryPoints[i].wayPoint.x = in.trajectory[i].xg;
		out->trajectoryPoints[i].wayPoint.y = in.trajectory[i].yg;
		// out->trajectoryPoints[i].wayPoint.z = in.trajectory[i].z;
		out->trajectoryPoints[i].wayPoint.theta = in.trajectory[i].angleglobal;
		out->trajectoryPoints[i].wayPoint.curvature = in.trajectory[i].curvature;
		out->trajectoryPoints[i].wayPoint.s = in.trajectory[i].length;
		// out->trajectoryPoints[i].wayPoint.source = in.trajectory[i].source;
    // out->trajectoryPoints[i].wayPoint.type = in.trajectory[i].type;
    // out->trajectoryPoints[i].wayPoint.type_description = in.trajectory[i].type_description;
    // out->trajectoryPoints[i].wayPoint.type_confidence = in.trajectory[i].type_confidence;
	}
}

int AdsfiInterface::Process(std::shared_ptr<adsfi::PlanningResult> &control_ptr) 
{
	AINFO << "PROCESS PLANNINGTRACK";

	std::shared_ptr<ara::adsfi::BusinessCommand> bus_command_ptr;
	std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_objarray_ptr;
	std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr;
	std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr;
	
	bool bGet_bus_cmd = safebuf_bus_command_.Get(bus_command_ptr);
	bool bGet_fus_obj = safebuf_fusion_objarray_.Get(fusion_objarray_ptr);
	bool bGet_veh_inf = safebuf_vehicle_info_.Get(vehicle_info_ptr);
	bool bGet_loc_inf = safebuf_location_info_.Get(loc_ptr);

	// 先获取定位信息，因为生成轨迹需要当前位置和航向
	// ret = locationinfo_deque.getLatestAndClear(loc_ptr);
	if(bGet_loc_inf == false)
	{
		if(simulation_mode_)
		{
			// 仿真模式下，使用预设的定位信息
			loc_ptr = sim_location_info_;
			if(!loc_ptr)
			{
				std::cout << "Simulation mode: LocationInfo not initialized\n";
				return -1;
			}
			std::cout << "Simulation mode: using preset LocationInfo\n";
		}
		else
		{
			std::cout << "Process error LocationInfo have not new data\n";
			return -1;
		}
	}
	static ara::adsfi::IdpSensorLocFusion m_9_0;
	//convert loc_ptr to m_9_0
	coverter(loc_ptr, m_9_0);

	// ret = buscommand_deque.getLatestAndClear(bus_command_ptr);
	if(bGet_bus_cmd == false)
	{
		if(!simulation_mode_)
		{
			std::cout << "Process error BusinessCommand have not new data, simulation mode is not enabled\n";
			return -1;
		}
		std::cout << "Simulation mode: BusinessCommand have not new data, creating default 100m trajectory\n";
		// 外部没有输入时，根据当前定位信息创建一条100米长的直线轨迹
		bus_command_ptr = std::make_shared<ara::adsfi::BusinessCommand>();
		bus_command_ptr->header.seq = 0;
		bus_command_ptr->header.time = avos::common::NodeTime::Now().ToSecond();
		bus_command_ptr->param.command = 1;  // 启动命令
		bus_command_ptr->param.max_speed = 10.0;  // 最大速度
		bus_command_ptr->business_mode = 1;
		bus_command_ptr->param.task_avoid_mode = 0; // 1 AVOID
		
		// 获取当前位置和航向（需要转换为GCCS坐标系）
		avos::vectormap::GeoTool geotool;
		avos::vectormap::PointGPS gps_point(loc_ptr->pose.pose.position.x, loc_ptr->pose.pose.position.y, 
		                                     loc_ptr->pose.pose.orientation.z);
		avos::vectormap::PointGCCS current_pos;
		geotool.GPS2GCCS(gps_point, current_pos);
		
		// 生成100米长的直线轨迹，每隔1米一个点，共101个点（起点+100个中间点）
		// 注意：XBridgeTrack检查要求相邻点之间的长度差不能超过2米，所以点间距设为1米
		const double trajectory_length = 100.0;  // 100米
		const double point_interval = 1.0;       // 每1米一个点，确保相邻点长度差在允许范围内
		const int num_points = static_cast<int>(trajectory_length / point_interval) + 1;  // 101个点
		
		bus_command_ptr->business_path.points.clear();
		bus_command_ptr->business_path.length = trajectory_length;
		
		double yaw_rad = current_pos.angle * M_PI / 180.0;  // 转换为弧度
		
		for (int i = 0; i < num_points; ++i) {
			ara::adsfi::IdpBusinessPoint point;
			double distance = i * point_interval;  // 距离起点的距离
			
			// 根据航向计算下一个点的GCCS坐标
			point.xg = current_pos.xg + distance * cos(yaw_rad);
			point.yg = current_pos.yg + distance * sin(yaw_rad);
			point.x = 0.0;  // VCS坐标，后续会转换
			point.y = 0.0;
			point.heading = current_pos.angle;  // 航向角（度）
			point.angle = 0.0;  // 相对角度
			point.direction = 0;  // 前进方向：0-前进
			point.index = i;
			// point.length = distance;
			
			bus_command_ptr->business_path.points.push_back(point);
		}
		
		std::cout << "Created default trajectory with " << bus_command_ptr->business_path.points.size() 
		          << " points, length: " << trajectory_length << "m" << std::endl;
	}
	
	// ret = fusionobjarray_deque.getLatestAndClear(fusion_objarray_ptr);
	if(bGet_fus_obj == false)
	{
		if(simulation_mode_)
		{
			// 仿真模式下，创建空的融合目标数组
			fusion_objarray_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
			fusion_objarray_ptr->header.seq = 0;
			fusion_objarray_ptr->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
			fusion_objarray_ptr->header.stamp.nsec = 0;
			fusion_objarray_ptr->header.frameId = "simulation";
			fusion_objarray_ptr->object.clear();  // 空数组，无障碍物
			std::cout << "Simulation mode: using empty FusionObjectArray\n";
		}
		else
		{
			fusion_objarray_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
			fusion_objarray_ptr->header.seq = 0;
			fusion_objarray_ptr->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
			fusion_objarray_ptr->header.stamp.nsec = 0;
			fusion_objarray_ptr->header.frameId = "simulation";
			fusion_objarray_ptr->object.clear();  // 空数组，无障碍物
			std::cout << "Process error FusionObjectArray have not new data\n";
			// return -1;
		}
	}
	// ret = vehicleinfo_deque.getLatestAndClear(vehicle_info_ptr);
	if(bGet_veh_inf == false)
	{
		if(simulation_mode_)
		{
			// 仿真模式下，使用预设的车辆信息
			vehicle_info_ptr = sim_vehicle_info_;
			if(!vehicle_info_ptr)
			{
				// 如果预设数据不存在，创建默认车辆信息
				vehicle_info_ptr = std::make_shared<ara::adsfi::VehicleInformation>();
				vehicle_info_ptr->header.seq = 0;
				vehicle_info_ptr->header.time = avos::common::NodeTime::Now().ToSecond();
				vehicle_info_ptr->vehicle_act_state.speed = 0.0;
				vehicle_info_ptr->vehicle_act_state.steer_angle = 0.0;
				vehicle_info_ptr->vehicle_act_state.acceleration = 0.0;
				// vehicle_info_ptr->vehicle_act_state.gear = 0;
				vehicle_info_ptr->vehicle_act_state.drive_mode = 0;
				vehicle_info_ptr->vehicle_act_state.shift_position = 0;
			}
			std::cout << "Simulation mode: using preset VehicleInformation\n";
		}
		else
		{
			std::cout << "Process error IdpVehicleInformation have not new data\n";
			return -1;
		}
	}

	static ara::adsfi::BusinessCommand m_16_0;
	m_16_0 = *bus_command_ptr;
	static ara::adsfi::IdpBusinessTaskPoints m_19_0;
	static ara::adsfi::IdpCurves m_13_1;
	static ara::adsfi::IdpPerceptionFinalObjects m_22_1;
	static ara::adsfi::IdpPerceptionFinalObjects m_26_0;
	static ara::adsfi::PerceptionStaticEnv m_22_0;
	static ara::adsfi::PerceptionStaticEnv m_27_0;
	static ara::adsfi::PerceptionStaticEnv m_6_0; //input 
	//convert fusion_objarray_ptr to m_6_0
	coverter(fusion_objarray_ptr, m_6_0);
	static ara::adsfi::PlanningStatus m_25_2;
	static PlanningTarget m_23_0;
	static PlanningTrajectory m_3_1;
	static ara::adsfi::VehicleInformation m_4_0;
	//convert vehicle_info_ptr to m_4_0
	m_4_0 = *vehicle_info_ptr;
	static avos::planning::ConfigInfo m_0_0;
	static avos::planning::InnerPath m_10_0;
	static avos::planning::LocalizationData m_5_1;
	static avos::planning::Paths m_12_2;
	static avos::planning::Paths m_13_0;
	static avos::planning::Paths m_17_0;
	static avos::planning::Paths m_24_2;
	static avos::planning::Paths m_25_1;
	static avos::planning::Paths m_2_0;
	static avos::planning::Paths m_3_0;
	static avos::planning::Paths m_7_0;
	static avos::planning::Perception m_5_0;
	static avos::planning::PlanningInfo m_12_1;
	static avos::planning::PlanningInfo m_24_1;
	static avos::planning::PlanningInfo m_25_0;
	static avos::planning::TrackTaskPoint m_10_1;
	static avos::planning::UnstructEnv m_12_0;
	static avos::planning::UnstructEnv m_24_0;
	static avos::planning::VehicleStatus m_5_2;
	static ara::adsfi::MsgPoint3DVec m_24_3;
	static ara::adsfi::MsgPoint3DVec m_7_1;

	XConfigParamReader (m_0_0);

	BlockPerceptionData (m_6_0,m_26_0,m_16_0,m_22_0,m_22_1);
	XBridge (m_16_0,m_9_0,m_22_0,m_22_1,m_4_0,m_5_0,m_5_1,m_5_2);
	XBridgeTrack (m_5_1,m_16_0,m_10_0,m_19_0,m_10_1);
	XBusinessTrack (m_5_1,m_10_0,m_5_2,m_10_1,m_0_0,m_3_0,m_25_0,m_25_1,m_25_2);
	XEnvironmentalCognition (m_5_1,m_5_0,m_5_2,m_0_0,m_25_0,m_25_1,m_12_0,m_23_0,m_24_0,m_24_1,m_24_2,m_24_3);
	XBehaviorComplexTrack (m_5_1,m_5_2,m_0_0,m_24_0,m_24_1,m_24_2,m_12_0,m_12_1,m_12_2);
	XHybridAstar (m_5_1,m_12_0,m_12_1,m_0_0,m_12_2,m_2_0);
	XDynamicProgramming (m_5_1,m_12_0,m_0_0,m_12_2,m_7_0,m_7_1);
	XCasadiLocalOptimize (m_5_1,m_5_0,m_5_2,m_0_0,m_12_0,m_12_2,m_13_0,m_13_1);
	XTurnAround (m_5_1,m_12_0,m_0_0,m_12_2,m_17_0);
	XSpeedPlanTrack (m_5_1,m_5_0,m_5_2,m_0_0,m_12_1,m_13_0,m_7_0,m_2_0,m_17_0,m_27_0,m_3_0,m_3_1);
	coverter(m_3_1, control_ptr);

	// 将更新后的 m_25_2 写入共享内存，供其他进程读取
	planning_status_shm_.SetData(m_25_2);XPidLonControl函数进行封装

	// 路径可视化
	VisualizePaths(m_3_1, m_13_0, m_12_2);
	// ThreadSafeBuffer<int> safebuf_flag_info_;

	safebuf_flag_info_.SetNotify(flag_ptr);
	return 0;
}

int AdsfiInterface::Process(std::shared_ptr<ara::adsfi::PlanningStatus> &ptr)
{
	// 从共享内存读取 PlanningStatus 数据
	ara::adsfi::PlanningStatus status;
	
	// 确保 ptr 不为空
	if (!ptr) {
		ptr = std::make_shared<ara::adsfi::PlanningStatus>();
	}
	
	// 如果这是第一次调用，尝试初始化 reader（如果还没有初始化）
	// 注意：如果已经在 Init() 中作为 writer 初始化，这里需要作为 reader 初始化
	// GetNewestData 会自动处理初始化，但第一次调用可能返回 false
	// 所以先尝试获取数据，如果失败且是首次调用，可以重试
	
	std::shared_ptr<int>  temp;
	safebuf_flag_info_.WaitForGet(temp);
	// 使用 GetNewestData 获取最新的数据
	// 如果共享内存还没有初始化（作为 reader），GetNewestData 会自动调用 InitReader
	if (planning_status_shm_.GetNewestData(status)) {
		// 将共享内存中的数据复制到 ptr
		*ptr = status;
		return 0;  // 成功获取数据
	} else {
		// 如果没有新数据可用，可能是：
		// 1. 数据还没有更新（这是正常的，返回 0）
		// 2. 共享内存未初始化（GetNewestData 会自动初始化，但第一次可能返回 false）
		// 3. 写入进程还没有运行
		// 返回 0 表示函数执行成功，但可能没有新数据
		AINFO << "error publish PlanningStatus";
		return -1;
	}
}

// 初始化仿真数据：给定初始定位和业务轨迹
void AdsfiInterface::InitSimulationData()
{
	// 初始化定位信息（给定初始定位）
	sim_location_info_ = std::make_shared<ara::adsfi::MsgHafLocation>();
	sim_location_info_->header.seq = 0;
	sim_location_info_->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	sim_location_info_->header.stamp.nsec = 0;
	sim_location_info_->header.frameId = "simulation";
	
	// 设置初始位置（示例：可根据项目需求修改）
	// 默认位置：经度116.3974，纬度39.9093（北京天安门附近）
	sim_location_info_->pose.pose.position.x = 116.3974;  // 经度
	sim_location_info_->pose.pose.position.y = 39.9093;   // 纬度
	sim_location_info_->pose.pose.position.z = 0.0;
	
	// 设置初始姿态（航向角0度，即正北方向）
	sim_location_info_->pose.pose.orientation.x = 0.0;  // roll
	sim_location_info_->pose.pose.orientation.y = 0.0;  // pitch
	sim_location_info_->pose.pose.orientation.z = 0.0;  // yaw (heading)
	sim_location_info_->pose.pose.orientation.w = 1.0;
	
	// 设置初始速度
	sim_location_info_->velocity.twist.linear.x = 0.0;
	sim_location_info_->velocity.twist.linear.y = 0.0;
	sim_location_info_->velocity.twist.linear.z = 0.0;
	sim_location_info_->velocity.twist.angular.x = 0.0;
	sim_location_info_->velocity.twist.angular.y = 0.0;
	sim_location_info_->velocity.twist.angular.z = 0.0;
	
	// 初始化车辆信息
	sim_vehicle_info_ = std::make_shared<ara::adsfi::VehicleInformation>();
	sim_vehicle_info_->header.seq = 0;
	sim_vehicle_info_->header.time = avos::common::NodeTime::Now().ToSecond();
	sim_vehicle_info_->vehicle_act_state.speed = 0.0;
	sim_vehicle_info_->vehicle_act_state.steer_angle = 0.0;
	sim_vehicle_info_->vehicle_act_state.acceleration = 0.0;
	// sim_vehicle_info_->vehicle_act_state.gear = 0;
	sim_vehicle_info_->vehicle_act_state.drive_mode = 0;
	sim_vehicle_info_->vehicle_act_state.shift_position = 0;
	
	// 初始化融合目标数组（空数组，无障碍物）
	sim_fusion_objarray_ = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
	sim_fusion_objarray_->header.seq = 0;
	sim_fusion_objarray_->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	sim_fusion_objarray_->header.stamp.nsec = 0;
	sim_fusion_objarray_->header.frameId = "simulation";
	sim_fusion_objarray_->object.clear();  // 无障碍物
	
	std::cout << "Simulation data initialized:" << std::endl;
	std::cout << "  Initial location: lon=" << sim_location_info_->pose.pose.position.x 
	          << ", lat=" << sim_location_info_->pose.pose.position.y << std::endl;
	std::cout << "  Initial heading: " << sim_location_info_->pose.pose.orientation.z << std::endl;
	std::cout << "  Vehicle speed: " << sim_vehicle_info_->vehicle_act_state.speed << std::endl;
}

// 路径可视化函数
void AdsfiInterface::VisualizePaths(const PlanningTrajectory& final_trajectory,
                                    const avos::planning::Paths& casadi_path,
                                    const avos::planning::Paths& behavior_path)
{
	// 定义路径点结构（用于点云可视化）
	struct PathPoint {
		double x, y, z;
		double velocity;
	};
	
	// 使用点云方式可视化路径（支持颜色）
	static RGBPointCloud<PathPoint> viz_final_path("planning/final_trajectory");
	static RGBPointCloud<PathPoint> viz_casadi_path("planning/casadi_path");
	static RGBPointCloud<PathPoint> viz_behavior_path("planning/behavior_path");
	
	// 速度阈值
	const double UP_SPEED = 40.0 / 3.6;  // 40 km/h = 11.11 m/s
	
	// 设置颜色函数：根据速度设置颜色
	auto setColorByVelocity = [UP_SPEED](void* p) -> float {
		PathPoint* point = static_cast<PathPoint*>(p);
		double velocity = point->velocity;
		
		if (fabs(velocity) <= 0.001) {
			return 1.0;  // 白色：G=1, R=1, B=1
		} else if (velocity < 0) {
			return 0.0;  // 黑色：G=0, R=0, B=0
		} else if (velocity >= UP_SPEED) {
			return 0.0;  // 红色：G=0, R=1, B=0
		} else if (velocity < UP_SPEED / 2.0) {
			return 1.0;  // 绿色到黄色渐变：G=1
		} else if (velocity < UP_SPEED) {
			return 1.0 - (velocity - UP_SPEED / 2.0) / (UP_SPEED / 2.0);  // 黄色到红色渐变
		}
		return 0.0;
	};
	
	// 可视化最终轨迹
	viz_final_path.Reset();
	viz_final_path.SetRFunc([UP_SPEED](void* p) -> float {
		PathPoint* point = static_cast<PathPoint*>(p);
		double velocity = point->velocity;
		if (fabs(velocity) <= 0.001) return 1.0;  // 白色
		if (velocity < 0) return 0.0;  // 黑色
		if (velocity >= UP_SPEED) return 1.0;  // 红色
		if (velocity < UP_SPEED / 2.0) return velocity / (UP_SPEED / 2.0);  // 绿色到黄色
		return 1.0;  // 黄色到红色
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
		viz_final_path.publish('to_control');  // 使用自定义颜色函数
	}
	
	// 可视化 Casadi 优化路径
	// viz_casadi_path.Reset();
	// viz_casadi_path.SetRFunc([UP_SPEED](void* p) -> float {
	// 	PathPoint* point = static_cast<PathPoint*>(p);
	// 	double velocity = point->velocity;
	// 	if (fabs(velocity) <= 0.001) return 1.0;
	// 	if (velocity < 0) return 0.0;
	// 	if (velocity >= UP_SPEED) return 1.0;
	// 	if (velocity < UP_SPEED / 2.0) return velocity / (UP_SPEED / 2.0);
	// 	return 1.0;
	// });
	// viz_casadi_path.SetGFunc(setColorByVelocity);
	// viz_casadi_path.SetBFunc([](void* p) -> float { rXPidLonControl函数进行封装eturn 0.0; });
	
	// if (casadi_path.calculate_path.path.size() > 0) {
	// 	for (auto it = casadi_path.calculate_path.path.begin(); 
	// 	     it != casadi_path.calculate_path.path.end(); ++it) {
	// 		PathPoint point;
	// 		point.x = it->xg;
	// 		point.y = it->yg;
	// 		point.z = 0.0;
	// 		point.velocity = it->velocity;
	// 		viz_casadi_path.inner_points.push_back(point);
	// 	}
	// 	if (!viz_casadi_path.inner_points.empty()) {
	// 		viz_casadi_path.publish(' ');
	// 	}
	// }
	
}
