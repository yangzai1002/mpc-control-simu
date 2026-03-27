#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "custom_stack.h"
#include "log/avos_node.h"

#include "common_header/geotool.h"
#include "XPidLonControl.h"
#include  "XPurepursuitLateralControl.h"
#include "XSelectLocForControl.h"
#include "XTrajectoryProcess.h"
#include "base_control/lat/latcontrol_header.h"
#include "base_control/adsfi_struct_adapter.h"

void AdsfiInterface::Init()  
{
	SetScheduleInfo("timmer", 50);
	// ApLogInterface::Instance()->Init("Control");
	std::cout << "init" << std::endl;
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

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) 
{
	std::cout << "LocationInfo  callback:" << ptr->header.seq << std::endl;
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
	ret = safebuf_buscommand.Get(bus_command_ptr);
	if(ret == false)
	{
		std::cout << "Process error BusinessCommand have not new data\n";
		if(last_bus_command_ptr != nullptr)
			bus_command_ptr = last_bus_command_ptr;
		else
			return -1;
	}
	last_bus_command_ptr = bus_command_ptr;
	// ret = planningresult_deque.getLatestAndClear(planning_result_ptr);
	ret = safebuf_planningresult.Get(planning_result_ptr);
	if(ret == false)
	{
		std::cout << "Process error PlanningResult have not new data\n";
		return -1;
	}
	// ret = vehicleinfo_deque.getLatestAndClear(vehicle_info_ptr);
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
	ret = safebuf_faultdata.Get(fault_data_ptr);
    if(ret == false)
    {
        std::cout << "Process warning: FaultData have not new data\n";
        return -1;
    }

    ara::adsfi::BusinessCommand m_13_0 = *bus_command_ptr;
    ara::adsfi::MsgHafLocation m_4_0 = *loc_ptr;
    ara::adsfi::MsgHafLocation m_14_0;
    PlanningTrajectory m_3_0;
    svsPointMsg m_7_0;
    ara::adsfi::VehicleInformation m_5_0;
	m_5_0 = *vehicle_info_ptr;

    LoncontrolDebugStruct m_11_1;
    LatcontrolDebugStruct m_9_0;
    ara::adsfi::VehicleLatControl m_9_1;
    ara::adsfi::FaultData m_12_0;
    if (fault_data_ptr != nullptr) {
        m_12_0 = *fault_data_ptr;
    }
    ara::adsfi::VehicleActControl m_11_0;

    // ==================== 非自主模式拦截逻辑 ====================
	if (m_13_0.business_mode == 0 || m_13_0.business_mode == 5) {
        
        // 1. 按照下游期望的标准控制头格式填充 Header 信息
        static long long remote_seq_counter = 0; // 局部静态变量，保证遥控模式下序列号依然递增
        m_11_0.header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond()); 
        m_11_0.header.seq = remote_seq_counter++; 
        m_11_0.header.module_name = "Control";
        m_11_0.header.data_name = "Control_20260305"; // 与 XPidLonControl 保持一致

        // 2. 主动清除自驾模式下的所有控制专属故障码
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3091001");
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3092002");
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3092003");
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3092004");

        FaultHandle::FaultApi::Instance()->ResetFaultCode("3092006"); // 规划超时
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3092007"); // 定位不自洽
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3092008"); // 车辆受阻

        // 3. 赋值空控制量(仅带标准Header)给输出指针，然后返回跳过自驾逻辑
        *control_ptr = m_11_0;
        ApInfo("control") << "当前处于遥控模式(business_mode=" << m_13_0.business_mode 
                          << ")，已输出标准 Header，跳过自动驾驶控制计算。";
        return 0;
    }
	if (planning_result_ptr) {
        adsfi::utils::ConvertMsgHafToPlanningTraj(*planning_result_ptr, m_3_0);
    }
    XSelectLocForControl (m_13_0,m_4_0,m_14_0);
    XTrajectoryProcess (m_3_0,m_14_0,m_7_0);
    XPurepursuitLateralControl (m_7_0,m_5_0,m_14_0,m_11_1,m_9_0,m_9_1);
    XPidLonControl (m_7_0,m_9_0,m_9_1,m_5_0,m_14_0,m_12_0,m_11_0,m_11_1);

    *control_ptr = m_11_0;
    return 0;
}
