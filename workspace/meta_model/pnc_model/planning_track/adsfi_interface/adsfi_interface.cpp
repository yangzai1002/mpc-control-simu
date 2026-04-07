#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include "custom_stack.h"
#include "log/avos_node.h"

#include "common_header/geotool.h"
//#include "BlockPerceptionData.h"
// #include  "XBehaviorComplexTrack.h"
// #include "XBridge.h"
// #include "XBridgeTrack.h"
// #include "XBusinessTrack.h"
// #include "XCasadiLocalOptimize.h"
// #include "XConfigParamReader.h"
// #include "XDynamicProgramming.h"
// #include "XEnvironmentalCognition.h"
// #include "XHybridAstar.h"
// #include "XSpeedPlanTrack.h"
// #include "XTurnAround.h"
#include "mviz_interface/mviz_interface.h"
//#define GazeboSim

void SimUTMToLatLon(double x, double y, double& lat, double& lon) {
    double a = 6378137.0;
    double f = 1.0 / 298.257223563;
    double b = a * (1 - f);
    double e = std::sqrt(1 - (b * b) / (a * a));
    double e2 = e * e;
    double e_prime_2 = e2 / (1 - e2);

    int zone = 51; // 假设在 51带
    double x_val = x - 500000.0;
    double y_val = y;

    double M = y_val / 0.9996;
    double mu = M / (a * (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256));

    double e1 = (1 - std::sqrt(1 - e2)) / (1 + std::sqrt(1 - e2));
    double phi1 = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * std::sin(2 * mu)
                  + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * std::sin(4 * mu)
                  + (151 * e1 * e1 * e1 / 96) * std::sin(6 * mu);

    double N1 = a / std::sqrt(1 - e2 * std::sin(phi1) * std::sin(phi1));
    double T1 = std::tan(phi1) * std::tan(phi1);
    double C1 = e_prime_2 * std::cos(phi1) * std::cos(phi1);
    double R1 = a * (1 - e2) / std::pow(1 - e2 * std::sin(phi1) * std::sin(phi1), 1.5);
    double D = x_val / (N1 * 0.9996);

    double lat_rad = phi1 - (N1 * std::tan(phi1) / R1) * (D * D / 2
                     - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e_prime_2) * D * D * D * D / 24
                     + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e_prime_2 - 3 * C1 * C1) * D * D * D * D * D * D / 720);

    double lon_0 = (zone - 1) * 6 - 180 + 3;
    double lon_rad = lon_0 * M_PI / 180.0 + (D - (1 + 2 * T1 + C1) * D * D * D / 6
                     + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e_prime_2 + 24 * T1 * T1) * D * D * D * D * D / 120) / std::cos(phi1);

    lat = lat_rad * 180.0 / M_PI;
    lon = lon_rad * 180.0 / M_PI;
}

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
		//InitSimulationData();
	} else {
		std::cout << "Normal mode - waiting for external inputs" << std::endl;
	}
	flag_ptr = std::make_shared<int>();

	local_planner_ptr = std::make_shared<localplanner::interface::PlannerInterface>();

	std::ifstream file("/opt/usr/zjy/path.log");  // 路径自己改,这里存的是utm坐标
   	if (!file.is_open()) {
   	        printf("open path.log failed");
   	        return;
   	}
   	std::string line;
   	bool first_point = false;
   	double first_x = 0;
   	double first_y = 0;
   	while (std::getline(file, line)) {
   	        if (line.empty()) continue;
   	        double x, y;
   	        // 按逗号分割
   	        if (sscanf(line.c_str(), "%lf,%lf", &x, &y) == 2)
   	        {
   	            // 创建轨迹点
   	        	if(!first_point)
   	        	{
   	        		first_x = x;
   	        		first_y = y;
   	        		first_point = true;
   	        	}

   	        	GlobalPathPoint point;
				double transfer_lon =0.0;
				double transfer_lat =0.0;
				
				//double transfer_x = x - first_x;
				//double transfer_y = y - first_y;
				//SimUTMToLatLon(transfer_x,transfer_y,transfer_lat,transfer_lon);
   	            point.longitude = x - first_x;
   	            point.latitude = y - first_y;

//   	            point.x = x;
//   	            point.y = y;
   	            // 添加到结果
   	            interface_global_path.push_back(point);
   	        }
   	 }
	 std::cout <<"interface_global_path.szie2222"<<interface_global_path.size()<<std::endl;
   	  /*最后一个点是第一个点绝对utm坐标，平移使用*/
   	origin_utm_x = first_x;
   	origin_utm_y = first_y;
   	//interface_global_path->push_back(point);
}

void AdsfiInterface::DeInit()  
{
	std::cout << "deinit" << std::endl;
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) 
{
	std::string tagstr = "callback_point_size=" + std::to_string(ptr->business_path.points.size());
	//AVOS_CODETAG_ADV1(tagstr.c_str(),0,0);
	std::cout << "BusinessCommand  callback:" << ptr->header.seq << std::endl;
	AINFO<< "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSsss = " << ptr->business_path.points.size() << std::endl;
	// buscommand_deque.push(ptr);
	safebuf_bus_command_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr) 
{
	std::cout << "Perception_grid_map callback" << std::endl;
	static double last_ts = 0;
	timeval  tv_data{};
    gettimeofday(&tv_data, nullptr);
    double start_ts = tv_data.tv_sec + tv_data.tv_usec / 1000000.0;
	double recv_time_cost = (start_ts - last_ts) *1000;
    std::cout <<"PerceptionStaticEnv recv_time_cost:"<<recv_time_cost<<std::endl;
	last_ts = start_ts;
	safebuf_Perception_grid_map.Set(ptr);
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

// void coverter(std::shared_ptr<adsfi::PlanningResult> planning_result_ptr,PlanningTrajectory & out)
// {
//     //need add
//     for (size_t i = 0; i < planning_result_ptr->trajectoryPoints.size(); i++)
//     {

//         out.trajectory[i].velocity = planning_result_ptr->trajectoryPoints[i].speed;
//         out.trajectory[i].a = planning_result_ptr->trajectoryPoints[i].acc;
//         out.trajectory[i].t = occupancy_gridplanning_result_ptr->trajectoryPoints[i].timeRelative;
//         // out.trajectory[i].velocity = planning_result_ptr->trajectoryPoints[i].deltaAcc;
//         out.trajectory[i].angle = planning_result_ptr->trajectoryPoints[i].steerAngle;

//         out.trajectory[i].x = planning_result_ptr->trajectoryPoints[i].wayPoint.x;
//         out.trajectory[i].y = planning_result_ptr->trajectoryPoints[i].wayPoint.y;

//     }
// }

//TODO
// void coverter(std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr,ara::adsfi::IdpSensorLocFusion & out)
// {
//     //need add
//     double loc_last_time = static_cast<double>(loc_ptr->header.stamp.sec) 
//                                                     + static_cast<double>(loc_ptr->header.stamp.nsec) * 1e-9;
//     out.header.time = loc_last_time;
// 	out.llh.lat = loc_ptr->pose.pose.position.x;
// 	out.llh.lon = loc_ptr->pose.pose.position.y;
// 	double heading = loc_ptr->pose.pose.orientation.z;
// 	avos::vectormap::GeoTool geotool;
// 	avos::vectormap::PointGPS gps_point(out.llh.lon, out.llh.lat, heading);
// 	avos::vectormap::PointGCCS loc;occupancy_source
// 	geotool.GPS2GCCS(gps_point, loc);
//     out.pose.pose_euler.position.x = loc.xg;
//     out.pose.pose_euler.position.y = loc.yg;
//     // out.pose.pose_euler.rotation.yaw = loc.angle;
//     out.pose.pose_euler.rotation.yaw = heading;
// 	AINFO << "loc.xg = " << loc.xg << ",loc.yg = " << loc.yg << ", out.pose.pose_euler.rotation.yaw = " << out.pose.pose_euler.rotation.yaw;
// 	out.pose.pose_euler.rotation.pitch = loc_ptr->pose.pose.orientation.y;
// 	out.pose.pose_euler.rotation.roll = loc_ptr->pose.pose.orientation.x;
// 	double speed = std::hypot(loc_ptr->velocity.twist.linear.x, loc_ptr->velocity.twist.linear.y);
// 	speed = std::hypot(speed, loc_ptr->velocity.twist.linear.z);
// 	out.v = speed;
// 	out.twist.angular_velocity.z = loc_ptr->velocity.twist.angular.z;
// }

Adsfi::HafQuaternion eulerToQuaternion(double roll, double pitch, double yaw) {
    // 角度取模，确保在 [-2π, 2π] 范围内，提升稳定性
    roll = fmod(roll, 2 * M_PI);
    pitch = fmod(pitch, 2 * M_PI);
    yaw = fmod(yaw, 2 * M_PI);

    // 计算半角的正弦和余弦
    double cr = cos(roll / 2.0);
    double sr = sin(roll / 2.0);
    double cp = cos(pitch / 2.0);
    double sp = sin(pitch / 2.0);
    double cy = cos(yaw / 2.0);
    double sy = sin(yaw / 2.0);

    // 代入公式计算四元数分量
    Adsfi::HafQuaternion  q;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    q.w = cr * cp * cy + sr * sp * sy;

    return q;
}

void AdsfiInterface::coverterLoc(std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr,localplanner::interface::Location & out)
{
    //need add
    double loc_last_time = static_cast<double>(loc_ptr->header.timestamp.sec) 
                                                    + static_cast<double>(loc_ptr->header.timestamp.nsec) * 1e-9;
    //out.header.time = loc_last_time;
	
	// out.longitude = loc_ptr->llh.x;
	// out.latitude = loc_ptr->llh.y;
	// //double heading = loc_ptr->pose.pose.orientation.z;
	// //out.heading = M_PI/2 - (heading *M_PI / 180);

	// double heading_deg = 90.0 - loc_ptr->pose.pose.orientation.z;
    // while (heading_deg < 0) heading_deg += 360.0;
	// out.heading = heading_deg;

	///if(simulation_mode_) /*仿真模式下gazebo*/
	//{
		// auto normalize = [PI, TWO_PI](double rad) {
		// 	// 手工取模 2π，处理正负
		// 	while (rad < 0)    rad += TWO_PI;
		// 	while (rad > PI)   rad -= TWO_PI;
		// 	return rad;
		// };
    	//SimUTMToLatLon(loc_ptr->pose.pose.position.x, loc_ptr->pose.pose.position.y, sensor_loc.latitude, sensor_loc.longitude);
		
    	
    	//double heading_deg = 90.0 - (loc_ptr->pose.pose.orientation.z * 180.0 / M_PI);
    	//while (heading_deg < 0) heading_deg += 360.0;
		#ifdef GazeboSim
		out.longitude = loc_ptr->pose.pose.position.x;
		out.latitude = loc_ptr->pose.pose.position.y;
		out.heading = (90.0 - loc_ptr->pose.pose.orientation.z * 180.0 / M_PI);
		out.veloity = loc_ptr->velocity.twist.linear.x;
		out.height = 0.0;
    	out.acc = 0.0;
		double heading_viz = loc_ptr->pose.pose.orientation.z *180 / M_PI;
		#else
		GeoPointGPS  temp_point_gps(loc_ptr->llh.x, loc_ptr->llh.y, loc_ptr->llh.z);
    	GeoPointGCCS temp_point_gccs;
  
		if (std::isnan(loc_ptr->llh.x) == false && std::isnan(loc_ptr->llh.y) == false &&
			std::isnan(loc_ptr->llh.z) == false) {
			GeometryTool::GPS2GCCS(temp_point_gps, temp_point_gccs);
		}
		out.longitude = temp_point_gccs.xg - origin_utm_x ;
		out.latitude = temp_point_gccs.yg - origin_utm_y;
        out.heading = loc_ptr->pose.pose.orientation.z;

		double heading_viz = 90.0 - loc_ptr->pose.pose.orientation.z;
        while (heading_viz < 0)    heading_viz += 360;
        while (heading_viz > 180)   heading_viz -= 360;

		out.height = loc_ptr->llh.z;
    	out.acc = loc_ptr->acceleration.accel.linear.x;
		#endif
		
		mdc::visual::Marker marker;
		marker.points.clear();
		//marker.header.frame_id = current_waypoints_.header.frame_id;
		marker.header.frameId = "base_link";
		const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
		uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
		uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;;
		marker.header.stamp.sec = sec;
		marker.header.stamp.nsec = nsec;
		marker.ns = "plan";
		marker.id = 0;
		marker.type = mdc::visual::MarkerType::ARROW;
		marker.action = mdc::visual::MarkerAction::ADD;
		marker.scale.x = 1.0;
		marker.scale.y = 1.0;
		marker.scale.z = 1.0;
		marker.color.a = 1.0;
		marker.color.r = 0;
		marker.color.g = 0;
		marker.color.b = 1;

		//geometry_msgs::Point p;
		marker.pose.position.x = out.longitude;
		marker.pose.position.y = out.latitude;
		marker.pose.position.z = 0;
		// p.z = traj.z.at(i) + z;
		//printf("p.x:%f,p.y:%f\n",p.x, p.y);
		//p.z = 0;
		Adsfi::HafQuaternion q = eulerToQuaternion(0, 0, heading_viz * M_PI / 180.0);
		//marker.points.push_back(p);
		marker.pose.orientation.x = q.x;
		marker.pose.orientation.y = q.y;
		marker.pose.orientation.z = q.z;
		marker.pose.orientation.w = q.w;
		current_pos_Pub.Publish(marker);

	//}
	//avos::vectormap::GeoTool geotool;
	//avos::vectormap::PointGPS gps_point(out.llh.lon, out.llh.lat, heading);
	//avos::vectormap::PointGCCS loc;
	//geotool.GPS2GCCS(gps_point, loc);
   // out.pose.pose_euler.position.x = loc.xg;
    //out.pose.pose_euler.position.y = loc.yg;
    // out.pose.pose_euler.rotation.yaw = loc.angle;
    //out.pose.pose_euler.rotation.yaw = heading;
	//AINFO << "loc.xg = " << loc.xg << ",loc.yg = " << loc.yg << ", out.pose.pose_euler.rotation.yaw = " << out.pose.pose_euler.rotation.yaw;
	////out.pose.pose_euler.rotation.pitch = loc_ptr->pose.pose.orientation.y;
	//out.pose.pose_euler.rotation.roll = loc_ptr->pose.pose.orientation.x;
	//double speed = std::hypot(loc_ptr->velocity.twist.linear.x, loc_ptr->velocity.twist.linear.y);
	//speed = std::hypot(speed, loc_ptr->velocity.twist.linear.z);
	//out.v = speed;
	//out.twist.angular_velocity.z = loc_ptr->velocity.twist.angular.z;
}
void AdsfiInterface::coverterGridMap(std::shared_ptr<ara::adsfi::PerceptionStaticEnv>& map_ptr,std::vector<double>& occupancy_grid)
{
    //occupancy_grid.assign(1000 * 1000, 0.0);
 

    const int DST_SIZE = 500;
    // 安全检查：输入数据为空直接返回
    if (map_ptr->reserved.empty() || map_ptr->map_width < DST_SIZE || map_ptr->map_height < DST_SIZE || 
	   map_ptr->reserved.size() <  DST_SIZE*DST_SIZE) {
		std::cout <<"PerceptionStaticEnv map size error"<<std::endl;
        return;
    }

    // 2. 遍历所有输入栅格单元
	int index = 0;
    for (const auto& grid_value : map_ptr->reserved) {
        // 获取栅格类型
       // uint8_t type = patch.type;

        // ===================== 转换规则 =====================
        // 原 type = 0（未知）→ 目标 1（占用）
        // 原 type = 2（占用）→ 目标 1（占用）
        // 原 type = 1（空闲）→ 目标 0（空闲）
        double value = 0.0;
        if (grid_value == 2) {
            value = 1.0;
        }

        // 边界检查：必须在 1000x1000 范围内
        if (index < DST_SIZE * DST_SIZE) {
            occupancy_grid[index] = value;
        }
		index++;
    }

}
// void coverter(std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_objarray_ptr,ara::adsfi::PerceptionStaticEnv & out)
// {
//     //need add
//     double fusion_objarray_last_time = static_cast<double>(fusion_objarray_ptr->header.stamp.sec) 
//                                                     + static_cast<double>(fusion_objarray_ptr->header.stamp.nsec) * 1e-9;
//     out.header.time = fusion_objarray_last_time;
// 	out.header.data_name = fusion_objarray_ptr->header.frameId;
// 	out.semantic_objs.clear();
// 	for (size_t i = 0; i < fusion_objarray_ptr->object.size(); i++)
// 	{
// 		ara::adsfi::PerceptionSemanticObj perception_semantic_obj;
// 		perception_semantic_obj.x = fusion_objarray_ptr->object[i].rect3d.center.x;
// 		perception_semantic_obj.y = fusion_objarray_ptr->object[i].rect3d.center.y;
// 		perception_semantic_obj.z = fusion_objarray_ptr->object[i].rect3d.center.z;
// 		perception_semantic_obj.type = fusion_objarray_ptr->object[i].cls;
// 		//TODO
// 		perception_semantic_obj.obj_size.width = fusion_objarray_ptr->object[i].rect3d.size.x;
// 		perception_semantic_obj.obj_size.length = fusion_objarray_ptr->object[i].rect3d.size.y;
// 		perception_semantic_obj.obj_size.height = fusion_objarray_ptr->object[i].rect3d.size.z;
// 		// perception_semantic_obj.speed = fusion_objarray_ptr->object[i].velocity;
// 		ara::adsfi::PerceptionSemanticCell perception_semantic_cell;
// 		for (size_t j = 0; j < fusion_objarray_ptr->object[i].pointsInObject.size(); j++)
// 		{
// 			perception_semantic_cell.x = fusion_objarray_ptr->object[i].pointsInObject[j].x;
// 			perception_semantic_cell.y = fusion_objarray_ptr->object[i].pointsInObject[j].y;
// 			// perception_semantic_cell.Z = fusion_objarray_ptr->object[i].pointsInObject[j].z;
// 			perception_semantic_obj.cells.push_back(perception_semantic_cell);
// 		}
// 		out.semantic_objs.push_back(perception_semantic_obj);
// 	}0.1
    
// }

// void coverter(const PlanningTrajectory & in, std::shared_ptr<adsfi::PlanningResult> &out)
// {
//     //need add
// 	out = std::make_shared<adsfi::PlanningResult>();
//     out->header.stamp.sec = static_cast<int32_t>(in.header.time);
//     out->header.stamp.nsec = static_cast<int32_t>((in.header.time - static_cast<int32_t>(in.header.time)) * 1e9);
// 	out->gear = 0;
// 	if(!in.trajectory.empty()) {orientation
// 		out->gear = in.trajectory.back().direction;
// 	}
	
// 	// 先清空并调整大小，避免越界访问
// 	out->trajectoryPoints.clear();
// 	out->trajectoryPoints.resize(in.trajectory.size());
	
// 	for (size_t i = 0; i < in.trajectory.size(); i++)
//     {
//         out->trajectoryPoints[i].speed = in.trajectory[i].velocity;
// 		out->trajectoryPoints[i].acc = in.trajectory[i].a;
// 		out->trajectoryPoints[i].timeRelative = in.trajectory[i].t;
// 		// out->trajectoryPoints[i].steerAngle = in.trajectory[i].angle;
// 		out->trajectoryPoints[i].wayPoint.x = in.trajectory[i].xg;
// 		out->trajectoryPoints[i].wayPoint.y = in.trajectory[i].yg;
// 		// out->trajectoryPoints[i].wayPoint.z = in.trajectory[i].z;
// 		out->trajectoryPoints[i].wayPoint.theta = in.trajectory[i].angleglobal;
// 		out->trajectoryPoints[i].wayPoint.curvature = in.trajectory[i].curvature;
// 		out->trajectoryPoints[i].wayPoint.s = in.trajectory[i].length;
// 		// out->trajectoryPoints[i].wayPoint.source = in.trajectory[i].source;
//     // out->trajectoryPoints[i].wayPoint.type = in.trajectory[i].type;
//     // out->trajectoryPoints[i].wayPoint.type_description = in.trajectory[i].type_description;
//     // out->trajectoryPoints[i].wayPoint.type_confidence = in.trajectory[i].type_confidence;
// 	}
// }



int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory>  &plan_result_ptr) 
{
	AINFO << "PROCESS PLANNINGTRACK";

	std::shared_ptr<ara::adsfi::BusinessCommand> bus_command_ptr;
	std::shared_ptr<ara::adsfi::PerceptionStaticEnv> gridMap_ptr;
	std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr;
	std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr;
	//bool bGet_bus_cmd = safebuf_bus_command_.Get(bus_command_ptr);
	//bool bGet_fus_obj = safebuf_fusion_objarray_.Get(fusion_objarray_ptr);
	//bool bGet_veh_inf = safebuf_vehicle_info_.Get(vehicle_info_ptr);
	bool bGet_loc_inf = safebuf_location_info_.Get(loc_ptr);

	// 先获取定位信息，因为生成轨迹需要当前位置和航向
	// ret = locationinfo_deque.getLatestAndClear(loc_ptr);
	// if(bGet_loc_inf == false)
	// {orientation
	// 	if(simulation_mode_)
	// 	{
	// 		// 仿真模式下，使用预设的定位信息
	// 		loc_ptr = sim_location_info_;
	// 		if(!loc_ptr)
	// 		{
	// 			std::cout << "Simulation mode: LocationInfo not initialized\n";
	// 			return -1;
	// 		}loc0.0
	// 		std::cout << "Simulation mode: using preset LocationInfo\n";
	// 	}
	// 	else
	// 	{
	// 		std::cout << "Process error LocationInfo have not new data\n";
	// 		return -1;
	// 	}
	// }

	if(bGet_loc_inf == false)
	{
		
		std::cout << "Process error LocationInfo have not new data\n";
		return -1;
		
	}
#ifndef GazeboSim

	bool bGet_map_info = safebuf_Perception_grid_map.Get(gridMap_ptr);

	if(bGet_map_info == false)
	{
		
		std::cout << "Process error Perception_grid_map have not new data\n";
		return -1;
		
	}else if(gridMap_ptr->map_width != 500 || gridMap_ptr->map_height != 500)
	{
		std::cout << "Process error Perception_grid_map size error map_width: "<<gridMap_ptr->map_width <<
		"map_height: " <<gridMap_ptr->map_height <<std::endl;
		return -1;
	}else
	{
		;
	}
#endif
    timeval        tv_data{};
    gettimeofday(&tv_data, nullptr);
    double start_ts = tv_data.tv_sec + tv_data.tv_usec / 1000000.0;
	//static ara::adsfi::IdpSensorLocFusion m_9_0;

	static Location sensor_loc;

	coverterLoc(loc_ptr, sensor_loc);
	std::vector<double> occupancy_grid(500 * 500, 0.0);
#ifndef GazeboSim
    coverterGridMap(gridMap_ptr, occupancy_grid);
#endif

#ifdef GazeboSim
	const double START_UTM_X = 0.0;
    const double START_UTM_Y = 0.0;
    std::vector<SimObs> sim_obstacles = {
        {START_UTM_X - 10.0, START_UTM_Y - 3.0, 2.5, 2.5},
        {START_UTM_X - 30, START_UTM_Y - 4.0, 2.5, 2.5},
		{START_UTM_X - 20.0, START_UTM_Y - 3.0, 2.5, 2.5},
        {START_UTM_X - 50.0, START_UTM_Y - 9.0, 2.5, 2.5}
    };
	

    for (const auto& obs : sim_obstacles) {
        double rel_x = obs.x - sensor_loc.longitude;
        double rel_y = obs.y - sensor_loc.latitude;
        int center_c = 250 + static_cast<int>(rel_x / 0.2);
        int center_r = 250 + static_cast<int>(rel_y / 0.2);
        int half_w = static_cast<int>((obs.w / 2.0) / 0.2);
        int half_h = static_cast<int>((obs.h / 2.0) / 0.2);

        for (int r = center_r - half_h; r < center_r + half_h; ++r) {
            for (int c = center_c - half_w; c < center_c + half_w; ++c) {
                if (r >= 0 && r < 500 && c >= 0 && c < 500) occupancy_grid[r * 500 + c] = 1.0;
            }
        }
    }
	VisualizeObstacles(sim_obstacles);
#else
    bool  bGet_veh_inf = safebuf_vehicle_info_.Get(vehicle_info_ptr);
	if(bGet_veh_inf == false)
	{
		std::cout << "Process error vehicleinfo have not new data\n";
		return -1;
		
	}else
	{
		sensor_loc.veloity = vehicle_info_ptr->vehicle_act_state.speed;
		std::cout <<"sensor_loc.veloity : "<<sensor_loc.veloity <<std::endl;

	}
#endif
	VisualGridMap(occupancy_grid,sensor_loc);
    // 仅执行一次规划
    std::vector<PathPoint> output_trajectory;
    bool success = local_planner_ptr->RunPlanner(sensor_loc, interface_global_path, occupancy_grid, 1.5, output_trajectory);

    //auto out = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>();
    // out->header.stamp.sec = static_cast<int32_t>(in.header.time);
    // out->header.stamp.nsec = static_cast<int32_t>((in.header.time - static_cast<int32_t>(in.header.time)) * 1e9);
	// out->gear = 0;
	
	// 先清空并调整大小，避免越界访问
	plan_result_ptr->trajectoryPoints.clear();
	plan_result_ptr->trajectoryPoints.resize(output_trajectory.size());
	for (size_t i = 0; i < output_trajectory.size(); i++)
    {
        plan_result_ptr->trajectoryPoints[i].speed = output_trajectory[i].v;
		plan_result_ptr->trajectoryPoints[i].acc = output_trajectory[i].a;
		//out->trajectoryPoints[i].timeRelative = in.trajectory[i].t;
		// out->trajectoryPoints[i].steerAngle = in.trajectory[i].angle;
		plan_result_ptr->trajectoryPoints[i].wayPoint.x = output_trajectory[i].x;
		plan_result_ptr->trajectoryPoints[i].wayPoint.y = output_trajectory[i].y;
		// out->trajectoryPoints[i].wayPoint.z = in.trajectory[i].z;
		plan_result_ptr->trajectoryPoints[i].wayPoint.theta = output_trajectory[i].theta;
		plan_result_ptr->trajectoryPoints[i].wayPoint.curvature = output_trajectory[i].kappa;
		//out->trajectoryPoints[i].wayPoint.s = in.trajectory[i].length;
		// out->trajectoryPoints[i].wayPoint.source = in.trajectory[i].source;
    // out->trajectoryPoints[i].wayPoint.type = in.trajectory[i].type;
    // out->trajectoryPoints[i].wayPoint.type_description = in.trajectory[i].type_description;
    // out->trajectoryPoints[i].wayPoint.type_confidence = in.trajectory[i].type_confidence;
	}
    //*将当前时刻使用的本车使用的utm坐标作为最后一个点
    VisualizeFinalPaths(output_trajectory);
    ara::adsfi::MsgHafTrajectoryPoint last_point;

	last_point.wayPoint.x = origin_utm_x;
	last_point.wayPoint.y = origin_utm_y;
	plan_result_ptr->trajectoryPoints.push_back(last_point);

	//plan_result_ptr = out;
	
	// ret = buscommand_deque.getLatestAndClear(bus_command_ptr);
	// if(bGet_bus_cmd == false)
	// {
	// 	if(!simulation_mode_)
	// 	{
	// 		std::cout << "Process error BusinessCommand have not new data, simulation mode is not enabled\n";
	// 		return -1;
	// 	}
	// 	std::cout << "Simulation mode: BusinessCommand have not new data, creating default 100m trajectory\n";
	// 	// 外部没有输入时，根据当前定位信息创建一条100米长的直线轨迹
	// 	bus_command_ptr = std::make_shared<ara::adsfi::BusinessCommand>();
	// 	bus_command_ptr->header.seq = 0;
	// 	bus_command_ptr->header.time = avos::common::NodeTime::Now().ToSecond();
	// 	bus_command_ptr->param.command = 1;  // 启动命令
	// 	bus_command_ptr->param.max_speed = 10.0;  // 最大速度
	// 	bus_command_ptr->business_mode = 1;
	// 	bus_command_ptr->param.task_avoid_mode = 0; // 1 AVOID
		
	// 	// 获取当前位置和航向（需要转换为GCCS坐标系）
	// 	avos::vectormap::GeoTool geotool;-, 2.5
	// 	avos::vectormap::PointGPS gps_point(loc_ptr->pose.pose.position.x, loc_ptr->pose.pose.position.y, 
	// 	                                     loc_ptr->pose.pose.orientation.z);
	// 	avos::vectormap::PointGCCS current_pos;
	// 	geotool.GPS2GCCS(gps_point, current_pos);
		
	// 	// 生成100米长的直线轨迹，每隔1米一个点，共101个点（起点+100个中间点）
	// 	// 注意：XBridgeTrack检查要求相邻点之间的长度差不能超过2米，所以点间距设为1米
	// 	const double trajectory_length = 100.0;  // 100米
	// 	const double point_interval = 1.0;       // 每1米一个点，确保相邻点长度差在允许范围内
	// 	const int num_points = static_cast<int>(trajectory_length / point_interval) + 1;  // 101个点
		
	// 	bus_command_ptr->business_path.points.clear();
	// 	bus_command_ptr->business_path.length = trajectory_length;
		
	// 	double yaw_rad = current_pos.angle * M_PI / 180.0;  // 转换为弧度
		
	// 	for (int i = 0; i < num_points; ++i) {
	// 		ara::adsfi::IdpBusinessPoint point;
	// 		double distance = i * point_interval;  // 距离起点的距离
	// 		// 根据航向计算下一个点的GCCS坐标
	// 		point.xg = current_pos.xg + distance * cos(yaw_rad);
	// 		point.yg = current_pos.yg + distance * sin(yaw_rad);
	// 		point.x = 0.0;  // VCS坐标，后续会转换
	// 		point.y = 0.0;
	// 		point.heading = current_pos.angle;  // 航向角（度）
	// 		point.angle = 0.0;  // 相对角度
	// 		point.direction = 0;  // 前进方向：0-前进
	// 		point.index = i;
	// 		// point.length = distance;
			
	// 		bus_command_ptr->business_path.points.push_back(point);
	// 	}
		
	// 	std::cout << "Created default trajectory with " << bus_command_ptr->business_path.points.size() 
	// 	          << " points, length: " << trajectory_length << "m" << std::endl;
	// }
	
	// // ret = fusionobjarray_deque.getLatestAndClear(fusion_objarray_ptr);
	// if(bGet_fus_obj == false)
	// {
	// 	if(simulation_mode_)
	// 	{
	// 		// 仿真模式下，创建空的融合目标数组
	// 		fusion_objarray_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
	// 		fusion_objarray_ptr->header.seq = 0;
	// 		fusion_objarray_ptr->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	// 		fusion_objarray_ptr->header.stamp.nsec = 0;
	// 		fusion_objarray_ptr->header.frameId = "simulation";
	// 		fusion_objarray_ptr->object.clear();  // 空数组，无障碍物
	// 		std::cout << "Simulation mode: using empty FusionObjectArray\n";
	// 	}
	// 	else
	// 	{
	// 		fupath_record.logsion_objarray_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
	// 		fusion_objarray_ptr->header.seq = 0;
	// 		fusion_objarray_ptr->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	// 		fusion_objarray_ptr->header.stamp.nsec = 0;heading_deg
	// 		fusion_objarray_ptr->header.frameId = "simulation";
	// 		fusion_objarray_ptr->object.clear();  // 空数组，无障碍物final_trajectory.trajectoryPoints
	// 		std::cout << "Process error FusionObjectArray have not new data\n";
	// 		// return -1;
	// 	}0.1
	// }
	// // ret = vehicleinfo_deque.getLatestAndClear(vehicle_info_ptr);
	// if(bGet_veh_inf == false)
	// {
	// 	if(simulation_mode_)
	// 	{
	// 		// 仿真模式下，使用预设的车辆信息
	// 		vehicle_info_ptr = sim_vehicle_info_;
	// 		if(!vehicle_info_ptr)
	// 		{
	// 			// 如果预设数据不存在，创建默认车辆信息
	// 			vehicle_info_ptr = std::make_shared<ara::adsfi::VehicleInformation>();
	// 			path_record.logvehicle_info_ptr->header.seq = 0;
	// 			vehicle_info_ptr->header.time = avos::common::NodeTime::Now().ToSecond();
	// 			vehicle_info_ptr->vehicle_act_state.speed = 0.0;
	// 			vehicle_info_ptr->vehicle_act_state.steer_angle = 0.0;
	// 			vehicle_info_ptr->vehicle_act_state.acceleration = 0.0;
	// 			// vehicle_info_ptr->vehicle_act_state.gear = 0;
	// 			vehicle_info_ptr->vehicle_act_state.drive_mode = 0;
	// 			vehicle_info_ptr->vehicle_act_state.shift_position = 0;
	// 		}
	// 		std::cout << "Simulation mode: using preset VehicleInformation\n";
	// 	}
	// 	else
	// 	{
	// 		std::cout << "Process error IdpVehicleInformation have not new data\n";
	// 		return -1;
	// 	}
	// }

	// static ara::adsfi::BusinessCommand m_16_0;
	// m_16_0 = *bus_command_ptr;
	// static ara::adsfi::IdpBusinessTaskPoints m_19_0;
	// static ara::adsfi::IdpCurves m_13_1;
	// static path_record.logara::adsfi::IdpPerceptionFinalObjects m_22_1;
	// static ara::adsfi::IdpPerceptionFinalObjects m_26_0;
	// static ara::adsfi::PerceptionStaticEnv m_22_0;
	// static ara::adsfi::PerceptionStaticEnv m_27_0;
	// static ara::adsfi::PerceptionStaticEnv m_6_0; //input 
	// //convert fusion_objarray_ptr to m_6_0final_trajectory.trajectoryPoints
	// coverter(fusion_objarray_ptr, m_6_0);
	// static ara::adsfi::PlanningStatus m_25_2;
	// static PlanningTarget m_23_0;
	// static PlanningTrajectory m_3_1;
	// static ara::adsfi::VehicleInformation m_4_0;
	// //convert vehicle_info_ptr to m_4_0
	// m_4_0 = *vehicle_info_ptr;
	// static avos::planning::ConfigInfo m_0_0;
	// static avos::planning::InnerPath m_10_0;
	// static avos::planning::LocalizationData m_5_1;
	// static avos::planning::Paths m_12_2;
	// static avos::planning::Paths m_13_0;
	// static avos::planning::Paths m_17_0;
	// static avos::planning::Paths m_24_2;
	// static avos::planning::Paths m_25_1;
	// static avos::planning::Paths m_2_0;
	// static avos::planplan_cost_timening::Paths m_3_0;
	// static avos::planning::Paths m_7_0;
	// static avos::planning::Perception m_5_0;
	// static avos::planning::PlanningInfo m_12_1;
	// static avos::planning::PlanningInfo m_24_1;
	// static avos::planning::PlanningInfo m_25_0;
	// static avos::planning::TrackTaskPoint m_10_1;
	// static avos::planning::UnstructEnv m_12_0;
	// static avos::planning::UnstructEnv m_24_0;
	// static avos::planning::VehicleStatus m_5_2;
	// static ara::adsfi::MsgPoint3DVec m_24_3;
	// static ara::adsfi::MsgPoint3DVec m_7_1;

	// XConfigParamReader (m_0_0);

	// BlockPerceptionData (m_6_0,m_26_0,m_16_0,m_22_0,m_22_1);
	// XBridge (m_16_0,m_9_0,m_22_0,m_22_1,m_4_0,m_5_0,m_5_1,m_5_2);
	// XBridgeTrack (m_5_1,m_16_0,m_10_0,m_19_0,m_10_1);
	// XBusinessTrack (m_5_1,m_10_0,m_5_2,m_10_1,m_0_0,m_3_0,m_25_0,m_25_1,m_25_2);
	// XEnvironmentalCognition (m_5_1,m_5_0,m_5_2,m_0_0,m_25_0,m_25_1,m_12_0,m_23_0,m_24_0,m_24_1,m_24_2,m_24_3);
	// XBehaviorComplexTrack (m_5_1,m_5_2,m_0_0,m_24_0,m_24_1,m_24_2,m_12_0,m_12_1,m_12_2);
	// XHybridAstar (m_5_1,m_12_0,m_12_1,m_0_0,m_12_2,m_2_0);
	// XDynamicProgramming (m_5_1,m_12_0,m_0_0,m_12_2,m_7_0,m_7_1);
	// XCasadiLocalOptimize (m_5_1,m_5_0,m_5_2,m_0_0,m_12_0,m_12_2,m_13_0,m_13_1final_trajectory.trajectoryPoints);
	// XTurnAround (m_5_1,m_12_0,m_0_0,m_12_2,m_17_0);
	// XSpeedPlanTrack (m_5_1,m_5_0,m_5_2,m_0_0,m_12_1,m_13_0,m_7_0,m_2_0,m_17_0,m_27_0,m_3_0,m_3_1);
	//coverter(m_3_1, control_ptr);

	// 将更新后的 m_25_2 写入共享内存，供其他进程读取
	//planning_status_shm_.SetData(m_25_2);

	// 路径可视化
	//VisualizePaths(m_3_1, m_13_0, m_12_2);
	// ThreadSafeBuffer<int> safebuf_flag_info_;
	
	VisualizeOriginPaths();

	gettimeofday(&tv_data, nullptr);
    double end_ts = tv_data.tv_sec + tv_data.tv_usec / 1000000.0;
	double plan_cost_time =  (end_ts - start_ts)*1000;	
	std::cout <<"plan_cost_time:"<<plan_cost_time<<std::endl;
	//safebuf_flag_info_.SetNotify(flag_ptr);
	return 0;
}

// int AdsfiInterface::Process(std::shared_ptr<ara::adsfi::PlanningStatus> &ptr)
// {
// 	// 从共享内存读取 PlanningStatus 数据
// 	ara::adsfi::PlanningStatus status;
	
// 	// 确保 ptr 不为空
// 	if (!ptr) {
// 		ptr = std::make_shared<ara::adsfi::PlanningStatus>();
// 	}
	
// 	// 如果这是第一次调用，尝试初始化 reader（如果还没有初始化）
// 	// 注意：如果已经在 Init() 中作为 writer 初始化，这里需要作为 reader 初始化
// 	// GetNewestData 会自动处理初始化，但第一次调用可能返回 false
// 	// 所以先尝试获取数据，如果失败且是首次调用，可以重试
	
// 	std::shared_ptr<int>  temp;
// 	safebuf_flag_info_.WaitForGet(temp);
// 	// 使用 GetNewestData 获取最新的数据
// 	// 如果共享内存还没有初始化（作为 reader），GetNewestData 会自动调用 InitReader
// 	if (planning_status_shm_.GetNewestData(status)) {
// 		// 将共享内存中的数据复制到 ptr
// 		*ptr = status;
// 		return 0;  // 成功获取数据
// 	} else {
// 		// 如果没有新数据可用，可能是：
// 		// 1. 数据还没有更新（这是正常的，返回 0）
// 		// 2. 共享内存未初始化（GetNewestData 会自动初始化，但第一次可能返回 false）
// 		// 3. 写入进程还没有运行
// 		// 返回 0 表示函数执行成功，但可能没有新数据
// 		AINFO << "error publish PlanningStatus";
// 		return -1;
// 	}
// }

// 初始化仿真数据：给定初始定位和业务轨迹
// void AdsfiInterface::InitSimulationData()
// {
// 	// 初始化定位信息（给定初始定位）
// 	sim_location_info_ = std::make_shared<ara::adsfi::MsgHafLocation>();
// 	sim_location_info_->header.seq = 0;
// 	sim_location_info_->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
// 	sim_location_info_->header.stamp.nsec = 0;
// 	sim_location_info_->header.frameId = "simulation";
	
// 	// 设置初始位置（示例：可根据项目需求修改）
// 	// 默认位置：经度116.3974，纬度39.9093（北京天安门附近）
// 	sim_location_info_->pose.pose.position.x = 116.3974;  // 经度
// 	sim_location_info_->pose.pose.position.y = 39.9093;   // 纬度
// 	sim_location_info_->pose.pose.position.z = 0.0;
	
// 	// 设置初始姿态（航向角0度，即正北方向）
// 	sim_location_info_->pose.pose.orientation.x = 0.0;  // roll
// 	sim_location_info_->pose.pose.orientation.y = 0.0;  // pitch
// 	sim_location_info_->pose.pose.orientation.z = 0.0;  // yaw (heading)
// 	sim_location_info_->pose.pose.orientation.w = 1.0;
	
// 	// 设置初始速度
// 	sim_location_info_->velocity.twist.linear.x = 0.0;
// 	sim_location_info_->velocity.twist.linear.y = 0.0;
// 	sim_location_info_->velocity.twist.linear.z = 0.0;
// 	sim_location_info_->velocity.twist.angular.x = 0.0;
// 	sim_location_info_->velocity.twist.angular.y = 0.0;
// 	sim_location_info_->velocity.twist.angular.z VisualizeOriginP= 0.0;
	
// 	// 初始化车辆信息
// 	sim_vehicle_info_ = std::make_shared<ara::adsfi::VehicleInformation>();
// 	sim_vehicle_info_->header.seq = 0;
// 	sim_vehicle_info_->header.time = avos::common::NodeTime::Now().ToSecond();
// 	sim_vehicle_info_->vehicle_act_state.speed = 0.0;
// 	sim_vehicle_info_->vehicle_act_state.steer_angle = 0.0;
// 	sim_vehicle_info_->vehicle_act_state.acceleration = 0.0;final_trajectory.trajectoryPoints
// 	// sim_vehicle_info_->vehicle_act_state.gear = 0;
// 	sim_vehicle_info_->vehicle_act_state.drive_mode = 0;
// 	sim_vehicle_info_->vehicle_act_state.shift_position = 0;
	
// 	// 初始化融合目标数组（空数组，无障碍物）
// 	sim_fusion_objarray_ = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
// 	sim_fusion_objarray_->header.seq = 0;
// 	sim_fusion_objarray_->header.stamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
// 	sim_fusion_objarray_->header.stamp.nsec = 0;
// 	sim_fusion_objarray_->header.frameId = "simulation";
// 	sim_fusion_objarray_->object.clear();  // 无障碍物
	
// 	std::cout << "Simulation data initialized:" << std::endl;
// 	std::cout << "  Initial location: lon=" << sim_location_info_->pose.pose.position.x 
// 	          << ", lat=" << sim_location_info_->pose.pose.position.y << std::endl;
// 	std::cout << "  Initial heading: " << sim_location_info_->pose.pose.orientation.z << std::endl;
// 	std::cout << "  Vehicle speed: " << sim_vehicle_info_->vehicle_act_state.speed << std::endl;
// }
void AdsfiInterface::VisualGridMap(const std::vector<double> &occupancy_grid,Location sensor_loc)
{

		// ===================== 固定配置 =====================
	const int ORIGIN_MAP_SIZE = 500;  // 原始地图大小不变
	const int CROP_SIZE = 500;          // 要截取的中心大小 500x500
	const double RESOLUTION = 0.2;     // 分辨率不变

	// 计算截取区域的起始索引（从原图中心扣 50x50）
	const int START_IDX = (ORIGIN_MAP_SIZE - CROP_SIZE) / 2; // (1000-50)/2 = 475

	// 新地图的原点（以传感器为中心，50x50 左下角坐标）
	const double MAP_ORIGIN_X = sensor_loc.longitude - (CROP_SIZE * RESOLUTION) / 2.0;
	const double MAP_ORIGIN_Y = sensor_loc.latitude - (CROP_SIZE * RESOLUTION) / 2.0;

	// ===================== 发布 50x50 栅格地图 =====================
	mdc::visual::OccupancyGrid data;
	data.header = mdc::visual::Header();
	data.header.frameId = "base_link";

	// 关键：发布的是 50x50
	data.info.resolution = RESOLUTION;
	data.info.width = CROP_SIZE;
	data.info.height = CROP_SIZE;

	// 正确的原点（50x50 小地图的左下角）
	data.info.origin.position.x = MAP_ORIGIN_X;
	data.info.origin.position.y = MAP_ORIGIN_Y;
	data.info.origin.position.z = 0;

	// 无旋转，必须这样
	data.info.origin.orientation.x = 0;
	data.info.origin.orientation.y = 0;
	data.info.origin.orientation.z = 0;
	data.info.origin.orientation.w = 1;

	auto doubleToInt8 = [](const std::vector<double>& double_vec) -> std::vector<int8_t> {
		std::vector<int8_t> int8_vec;
		int8_vec.reserve(double_vec.size());
		for (double d : double_vec) {
			double clamped = std::max(-128.0, std::min(127.0, d));
			int8_vec.push_back(static_cast<int8_t>(clamped));
		}
		return int8_vec;
	};

	// ===================== 核心：从 1000x1000 中心截取 50x50 =====================
	std::vector<int8_t> grid_data_full = doubleToInt8(occupancy_grid);
	std::vector<int8_t> grid_data_crop(CROP_SIZE * CROP_SIZE);

	// 从原图中心区域复制数据（475~525 行，475~525 列）
	for (int i = 0; i < CROP_SIZE; ++i) {
	for (int j = 0; j < CROP_SIZE; ++j) {
		// 原图索引：(START_IDX+i) * 1000 + (START_IDX+j)
		int orig_idx = (START_IDX + i) * ORIGIN_MAP_SIZE + (START_IDX + j);
		// 新图索引
		int crop_idx = i * CROP_SIZE + j;
		if(grid_data_full[orig_idx] != 0)
		{
			grid_data_crop[crop_idx] = 125;
		}else
		{
			grid_data_crop[crop_idx] = 0;
		}
		
	}
	}

	data.data = std::move(grid_data_crop);
	auto ret = gridPub.Publish(data);
	std::cout <<"grid mpa publish:"<<ret<<std::endl;
}
// 路径可视化函数
void AdsfiInterface::VisualizeOriginPaths()
                                    //const avos::planning::Paths& casadi_path,
                                    //const avos::planning::Paths& behavior_path)
{
	// 定义路径点结构（用于点云可视化）
	struct PathPoint {
		double x, y, z;
		double velocity;
	};
	
	// 使用点云方式可视化路径（支持颜色）
	static RGBPointCloud<PathPoint> viz_final_path2("planning/origin_trajectory");
	//static RGBPointCloud<PathPoint> viz_casadi_path("planning/casadi_path");
	//static RGBPointCloud<PathPoint> viz_behavior_path("planning/behavior_path");
	
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
	viz_final_path2.Reset();
	viz_final_path2.SetRFunc([UP_SPEED](void* p) -> float {
		PathPoint* point = static_cast<PathPoint*>(p);
		double velocity = point->velocity;
		if (fabs(velocity) <= 0.001) return 1.0;  // 白色
		if (velocity < 0) return 0.0;  // 黑色
		if (velocity >= UP_SPEED) return 1.0;  // 红色
		if (velocity < UP_SPEED / 2.0) return velocity / (UP_SPEED / 2.0);  // 绿色到黄色
		return 1.0;  // 黄色到红色
	});
	viz_final_path2.SetGFunc(setColorByVelocity);
	viz_final_path2.SetBFunc([](void* p) -> float { return 0.0; });
	
	for (size_t i = 0; i < interface_global_path.size(); i++) {
		double x,y;
		PathPoint point;

		//local_planner_ptr->LatLon2XY(interface_global_path[i].latitude,interface_global_path[i].longitude,x,y);
		point.x = interface_global_path[i].longitude;
		point.y = interface_global_path[i].latitude;
		//std::cout <<"x:"<<point.x<<"y:" << point.y <<std::endl;
		point.z = 0.0;
		point.velocity = 2.0;
		viz_final_path2.inner_points.push_back(point);
	}
	if (!viz_final_path2.inner_points.empty()) {
		 viz_final_path2.publish('origin_path');  // 使用自定义颜色函数
		std::cout <<" publish origin_path::: "<< viz_final_path2.inner_points.size() << std::endl;
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
	// 	return 1.0;viz_final_path
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
	// 		viz_casadi_pathVisualizeFinalPaths.publish(' ');
	// 	}
	// }Os(c
	
}


// void AdsfiInterface::VisualizePaths(const ara::adsfi::MsgHafEgoTrajectory& final_trajectory)
//                                     //const avos::planning::Paths& casadi_path,
//                                     //const avos::planning::Paths& behavior_path)
// {
// 	mdc::visual::Marker plan_trajectory_marker;
//     plan_trajectory_marker.header.frameId = "base_link";
//     const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
//     uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
//     uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;;
//     plan_trajectory_marker.header.stamp.sec = sec;
//     plan_trajectory_marker.header.stamp.nsec = nsec;
//     plan_trajectory_marker.ns = "local_plan_array_marker_colored";
//     plan_trajectory_marker.type = mdc::visual::MarkerType::LINE_STRIP;
//     plan_trajectory_marker.action = mdc::visual::MarkerAction::ADD;
//     plan_trajectory_marker.scale.x = 0.2;
//     plan_trajectory_marker.scale.y = 0.2;
//     //lane_waypoint_marker.scale.z = 0.1;
//     plan_trajectory_marker.frameLocked = false;
//     plan_trajectory_marker.color.r = 1;
//     plan_trajectory_marker.color.g = 0;
//     plan_trajectory_marker.color.b = 0;
//     plan_trajectory_marker.color.a = 1.0;
//     for(unsigned int i = 0; i < final_trajectory.trajectoryPoints.size(); i++)
//     {
//       mdc::visual::Point p;
//       p.x = final_trajectory.trajectoryPoints[i].wayPoint.x;
//       p.y = final_trajectory.trajectoryPoints[i].wayPoint.y;
//       //p.z = safety_rect.at(i).z;
//       p.z = 0;

//       plan_trajectory_marker.points.push_back(p);
//     }
//     local_plan_trajPub.Publish(plan_trajectory_marker);
// }
// 功能：把规划轨迹的【中心线 + 左边界 + 右边界】通过 MDC Marker 发布出去
void AdsfiInterface::VisualizeFinalPaths(const std::vector<PathPoint>& output_trajectory)
{
    // ==================== 1. 获取时间戳（和你原来一样） ====================
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;

    // ==================== 发布：轨迹中心线（红色，你原来的） ====================
    mdc::visual::Marker center_marker;
    center_marker.header.frameId = "base_link";
    center_marker.header.stamp.sec = sec;
    center_marker.header.stamp.nsec = nsec;
    center_marker.ns = "local_trajectory_center";
    center_marker.type = mdc::visual::MarkerType::LINE_STRIP;
    center_marker.action = mdc::visual::MarkerAction::ADD;
    center_marker.scale.x = 0.2;
    center_marker.color.r = 1.0;
    center_marker.color.g = 0.0;
    center_marker.color.b = 0.0;
    center_marker.color.a = 1.0;
    center_marker.frameLocked = false;

    // ==================== 发布：左边界（青色/蓝色） ====================
    mdc::visual::Marker left_bound_marker;
    left_bound_marker.header.frameId = "base_link";
    left_bound_marker.header.stamp.sec = sec;
    left_bound_marker.header.stamp.nsec = nsec;
    left_bound_marker.ns = "local_trajectory_left_bound";
    left_bound_marker.type = mdc::visual::MarkerType::LINE_STRIP;
    left_bound_marker.action = mdc::visual::MarkerAction::ADD;
    left_bound_marker.scale.x = 0.15;
    left_bound_marker.color.r = 0.0;
    left_bound_marker.color.g = 1.0;
    left_bound_marker.color.b = 1.0;
    left_bound_marker.color.a = 1.0;
    left_bound_marker.frameLocked = false;

    // ==================== 发布：右边界（黄色） ====================
    mdc::visual::Marker right_bound_marker;
    right_bound_marker.header.frameId = "base_link";
    right_bound_marker.header.stamp.sec = sec;
    right_bound_marker.header.stamp.nsec = nsec;
    right_bound_marker.ns = "local_trajectory_right_bound";
    right_bound_marker.type = mdc::visual::MarkerType::LINE_STRIP;
    right_bound_marker.action = mdc::visual::MarkerAction::ADD;
    right_bound_marker.scale.x = 0.15;
    right_bound_marker.color.r = 1.0;
    right_bound_marker.color.g = 1.0;
    right_bound_marker.color.b = 0.0;
   right_bound_marker.color.a = 1.0;
    right_bound_marker.frameLocked = false;

    // ==================== 填充点：中心 + 左边界 + 右边界 ====================
    for (unsigned int i = 0; i < output_trajectory.size(); ++i)
    {
        const auto& tp = output_trajectory[i];
        double x = tp.x;
        double y = tp.y;
        double theta = tp.theta;               // 轨迹点朝向
        double l = tp.l;                      // 轨迹点偏移
        double left_bound = tp.left_bound;    // 左边界
        double right_bound = tp.right_bound;  // 右边界

        // 1. 轨迹中心线
        mdc::visual::Point p_center;
        p_center.x = x;
        p_center.y = y;
        p_center.z = 0.0;
        center_marker.points.push_back(p_center);

        // 2. 计算左边界点
        double shift_left = left_bound - l;
        double nx = -std::sin(theta);
        double ny = std::cos(theta);
        double l_x = x + shift_left * nx;
        double l_y = y + shift_left * ny;

        mdc::visual::Point p_left;
        p_left.x = l_x;
        p_left.y = l_y;
        p_left.z = 0.0;
        left_bound_marker.points.push_back(p_left);

        // 3. 计算右边界点
        double shift_right = right_bound - l;
        double r_x = x + shift_right * nx;
        double r_y = y + shift_right * ny;

        mdc::visual::Point p_right;
        p_right.x = r_x;
        p_right.y = r_y;
        p_right.z = 0.0;
        right_bound_marker.points.push_back(p_right);
    }

    // ==================== 发布 ====================
    local_plan_trajPub.Publish(center_marker);
    local_plan_trajPub.Publish(left_bound_marker);
    local_plan_trajPub.Publish(right_bound_marker);
} 


void AdsfiInterface::VisualizeObstacles(const std::vector<SimObs>& sim_obstalces)
                                    //const avos::planning::Paths& casadi_path,
                                    //const avos::planning::Paths& behavior_path)
{
	mdc::visual::PointCloud<mdc::visual::PointXYZRGB> publish_points;
    publish_points.header.frameId = "base_link";
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;;
    publish_points.header.stamp.sec = sec;
    publish_points.header.stamp.nsec = nsec;
    

    for(unsigned int i = 0; i < sim_obstalces.size(); i++)
    {
	  mdc::visual::PointXYZRGB temp;
      temp.x = sim_obstalces[i].x;
      temp.y = sim_obstalces[i].y;
	  temp.color.channel.b = 0;
		temp.color.channel.g = 0;
		temp.color.channel.r = 255;
		temp.color.channel.a = 128;
      //p.z = safety_rect.at(i).z;
      temp.z = 0;

      publish_points.points.push_back(temp);
    }
    local_plan_obsPub.Publish(publish_points);
}