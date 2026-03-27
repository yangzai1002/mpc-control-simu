#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "custom_stack.h"
#include "log/avos_node.h"

// ===================== UDP 发送头文件 =====================
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <chrono>
#include <thread>


//#include <fstream>
//#include <iomanip>
//#include "geometry_tools.hpp"

// #include "common_header/geotool.h"
// #include "XPidLonControl.h"
// #include  "XPurepursuitLateralControl.h"
// #include "XSelectLocForControl.h"
// #include "XTrajectoryProcess.h"
// #include "base_control/lat/latcontrol_header.h"
// #include "base_control/adsfi_struct_adapter.h"


int udp_send_fd = -1;
struct sockaddr_in udp_target_addr;
const char* UDP_TARGET_IP = "26.28.1.122";   // 发送到本机 ROS
const int UDP_TARGET_PORT = 9000;          // 与 ROS 监听端口一致

// ===================== UDP 初始化函数 =====================
bool InitUDPSender() {
    udp_send_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_send_fd < 0) {
        std::cerr << "UDP socket create failed" << std::endl;
        return false;
    }

    udp_target_addr.sin_family = AF_INET;
    udp_target_addr.sin_port = htons(UDP_TARGET_PORT);
    udp_target_addr.sin_addr.s_addr = inet_addr(UDP_TARGET_IP);

    std::cout << "UDP Sender initialized: " << UDP_TARGET_IP << ":" << UDP_TARGET_PORT << std::endl;
    return true;
}

void SendControlToUDP(float linear_x, float angular_z) {
    if (udp_send_fd < 0) return;

    unsigned char buffer[8] = {0};
    uint32_t net_val;

    // 线性速度（大端 float）
    memcpy(&net_val, &linear_x, 4);
    net_val = htobe32(net_val);
    memcpy(buffer, &net_val, 4);

    // 角速度（大端 float）
    memcpy(&net_val, &angular_z, 4);
    net_val = htobe32(net_val);
    memcpy(buffer + 4, &net_val, 4);

    // 发送
    sendto(udp_send_fd, buffer, 8, 0,
           (struct sockaddr*)&udp_target_addr, sizeof(udp_target_addr));
}

void AdsfiInterface::Init()  
{
	SetScheduleInfo("timmer", 50);
	avos::common::AvosNode::Init("control", true, "/opt/usr/zxz/log/", "/opt/usr/zxz/log/", "/opt/usr/zxz/log/");
	std::cout << "init" << std::endl;

	Control_ptr = std::make_shared<control::Controller>();
	Control_ptr->init();
	std::cout << "init end" << std::endl;
	InitUDPSender();

	bool result = mdc::visual::Connect();
	if (result) {
		std::cout << "The MViz server address is parsed and the visualization client is created successfully." << std::endl;
	} else {
		std::cout << "error Failed to parse the MViz server address, or a visualization client has been created." << std::endl;
	}

}

void AdsfiInterface::DeInit()  
{
	std::cout << "deinit" << std::endl;
}


void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) 
{
	std::cout << "BusinessCommand  callback:" << ptr->header.seq << std::endl;
	// buscommand_deque.push(ptr);
	safebuf_buscommand.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr) 
{
	std::cout << "PlanningResult  callback:" << ptr->egoTrajectoryHeader.seq << std::endl;
	// planningresult_deque.push(ptr);
	safebuf_planningresult.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr) 
{
	std::cout << "IdpVehicleInformation  callback:" << ptr->header.seq << std::endl;
	// vehicleinfo_deque.push(ptr);
	safebuf_vehicleinfo.Set(ptr);
}

//#define RECORD_PATH
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) 
{
	std::cout << "LocationInfo  callback:" << ptr->header.seq << std::endl;

	#ifdef RECORD_PATH

    // 文件路径
    GeoPointGPS  temp_point_gps(ptr->llh.x, ptr->llh.y, ptr->llh.z);
    GeoPointGCCS temp_point_gccs;
  
    if (std::isnan(ptr->llh.x) == false && std::isnan(ptr->llh.y) == false &&
        std::isnan(ptr->llh.z) == false) {
        GeometryTool::GPS2GCCS(temp_point_gps, temp_point_gccs);
    }

    const static char* log_path = "/opt/usr/zjy/path_record.log";

    // 打开文件（ios::app = 追加写入，不覆盖原有内容）
    static  std::ofstream ofs(log_path, std::ios::out);

    static double last_x = 0.0;
    static double last_y = 0.0;

    if (ofs.is_open()) {
        double distance = std::sqrt(std::pow((temp_point_gccs.xg - last_x),2) + 
        std::pow((temp_point_gccs.yg - last_y),2));
        if(distance > 0.4 ){
            // 保留 6 位小数（和你示例格式一致）
            ofs << std::fixed << std::setprecision(6);
            
            // 按 x,y 格式写入
            ofs << temp_point_gccs.xg 
                << "," 
                << temp_point_gccs.yg 
                << std::endl;
            last_x =  temp_point_gccs.xg;
            last_y =  temp_point_gccs.yg;
        }  
    }  
    #endif  
	// locationinfo_deque.push(ptr);
	safebuf_locationinfo.Set(ptr);
	
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &ptr) 
{
    std::cout << "FaultData callback" << std::endl;
    safebuf_faultdata.Set(ptr);
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleActControl> &control_ptr) 
{
	std::shared_ptr<ara::adsfi::BusinessCommand> bus_command_ptr;
	static std::shared_ptr<ara::adsfi::BusinessCommand> last_bus_command_ptr;
	std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> planning_result_ptr;
	std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr;
	std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr;
	std::shared_ptr<ara::adsfi::FaultData> fault_data_ptr;

	bool ret = true;

	// ret = buscommand_deque.getLatestAndClear(bus_command_ptr);
	// ret = safebuf_buscommand.Get(bus_command_ptr);
	// if(ret == false)
	// {
	// 	std::cout << "Process error BusinessCommand have not new data\n";
	// 	if(last_bus_command_ptr != nullptr)
	// 		bus_command_ptr = last_bus_command_ptr;
	// 	else
	// 		return -1;
	// }
	// last_bus_command_ptr = bus_command_ptr;
	// ret = planningresult_deque.getLatestAndClear(planning_result_ptr);
	ret = safebuf_planningresult.Get(planning_result_ptr);
	if(ret == false)
	{
		std::cout << "Process error PlanningResult have not new data\n";
		return -1;
	}
	//ret = vehicleinfo_deque.getLatestAndClear(vehicle_info_ptr);
	ret = safebuf_vehicleinfo.Get(vehicle_info_ptr);
	if(ret == false)
	{
		std::cout << "Process error IdpVehicleInformation have not new data\n";
		return -1;
	}
	// ret = locationinfo_deque.getLatestAndClear(loc_ptr);
	ret = safebuf_locationinfo.Get(loc_ptr);
	if(ret == false)
	{
		std::cout << "Process error LocationInfo have not new data\n";
		return -1;
	}
	// ret = safebuf_faultdata.Get(fault_data_ptr);
    // if(ret == false)
    // {
    //     std::cout << "Process warning: FaultData have not new data\n";
    //     return -1;
    // }

    // ara::adsfi::BusinessCommand m_13_0 = *bus_command_ptr;
    // ara::adsfi::MsgHafLocation m_4_0 = *loc_ptr;
    // ara::adsfi::MsgHafLocation m_14_0;
    // PlanningTrajectory m_3_0;
    // svsPointMsg m_7_0;
    // ara::adsfi::VehicleInformation m_5_0;
	// m_5_0 = *vehicle_info_ptr;

    // LoncontrolDebugStruct m_11_1;
    // LatcontrolDebugStruct m_9_0;
    // ara::adsfi::VehicleLatControl m_9_1;
    // ara::adsfi::FaultData m_12_0;
    // if (fault_data_ptr != nullptr) {
    //     m_12_0 = *fault_data_ptr;
    // }
    //ara::adsfi::VehicleActControl m_11_0;
   // std::shared_ptr<ara::adsfi::VehicleActControl> out_vehicle_control;
	// if (planning_result_ptr) {
    //     adsfi::utils::ConvertMsgHafToPlanningTraj(*planning_result_ptr, m_3_0);
    // }
	Control_ptr->updateTrajectory(planning_result_ptr);
    Control_ptr->updateLocation(loc_ptr);
	Control_ptr->updateVehicleInfo(vehicle_info_ptr);
    Control_ptr->runControl(control_ptr);

	// ===================== 关键：把控制指令发给 UDP =====================
    float vx = control_ptr->lon_control.target_speed;
    float wz = control_ptr->lat_control.target_angle; // 或 angular_z，按你实际字段改
    SendControlToUDP(vx, wz);

    // XSelectLocForControl (m_13_0,m_4_0,m_14_0);
    // XTrajectoryProcess (m_3_0,m_14_0,m_7_0);
    // XPurepursuitLateralControl (m_7_0,m_5_0,m_14_0,m_11_1,m_9_0,m_9_1);
    // XPidLonControl (m_7_0,m_9_0,m_9_1,m_5_0,m_14_0,m_12_0,m_11_0,m_11_1);

   // control_ptr = out_vehicle_control;
    return 0;
}
