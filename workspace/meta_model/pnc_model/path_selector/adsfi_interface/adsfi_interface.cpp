#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include "log/avos_node.h"
#include "ap_log/ap_log_interface.h"

void AdsfiInterface::Init()  
{
	// 设置为消息触发模式
	// SetScheduleInfo("messge");
	SetScheduleInfo("timmer", 100);
	// ApLogInterface::Instance()->Init("path_selector");

	ApInfo("path_selector") << "init path_selector";
	
	current_business_mode_ = 0;
}

void AdsfiInterface::DeInit()  
{
	ApInfo("path_selector") << "deinit path_selector";
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr) 
{
	if (name == "tppathplan_adjust") {
		ApInfo("path_selector") << "MsgHafEgoTrajectory adjust callback:" << ptr->egoTrajectoryHeader.seq;
		safebuf_pathplan_adjust_.Set(ptr);
	} 
	else if (name == "tppathplan_arrive") {
		ApInfo("path_selector") << "MsgHafEgoTrajectory arrive callback:" << ptr->egoTrajectoryHeader.seq;
		safebuf_pathplan_arrive_.Set(ptr);
	}
	else if (name == "tppathplan_follow") {
		ApInfo("path_selector") << "MsgHafEgoTrajectory follow callback:" << ptr->egoTrajectoryHeader.seq;
		safebuf_pathplan_follow_.Set(ptr);
	}
	else if (name == "tppathplan_track") {
		ApInfo("path_selector") << "MsgHafEgoTrajectory track callback:" << ptr->egoTrajectoryHeader.seq;
		safebuf_pathplan_track_.Set(ptr);
	}
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) 
{
	ApInfo("path_selector") << "BusinessCommand callback:" << ptr->header.seq;
	ApInfo("path_selector") << "BusinessCommand business_mode = " << ptr->business_mode;
	safebuf_bus_command_.Set(ptr);
}

void AdsfiInterface::CreateEmptyTrajectory(std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &out)
{
	out = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>();
	out->egoTrajectoryHeader.timestamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	out->egoTrajectoryHeader.timestamp.nsec = 0;
	out->egoTrajectoryHeader.seq = 0;
	out->egoTrajectoryHeader.frameID = "path_selector_empty";
	out->trajectoryPoints.clear();
	out->wayPoints.clear();
	out->gear = 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &planning_result_ptr) 
{
	ApInfo("path_selector") << "PROCESS PATH_SELECTOR";

	std::shared_ptr<ara::adsfi::BusinessCommand> bus_command_ptr;
	std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> pathplan_adjust_ptr;
	std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> pathplan_arrive_ptr;
	std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> pathplan_follow_ptr;
	std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> pathplan_track_ptr;

	bool bGet_bus_cmd = safebuf_bus_command_.Get(bus_command_ptr);
	bool bGet_adjust = safebuf_pathplan_adjust_.Get(pathplan_adjust_ptr);
	bool bGet_arrive = safebuf_pathplan_arrive_.Get(pathplan_arrive_ptr);
	bool bGet_follow = safebuf_pathplan_follow_.Get(pathplan_follow_ptr);
	bool bGet_track = safebuf_pathplan_track_.Get(pathplan_track_ptr);

	// 更新业务模式
	if (bGet_bus_cmd) {
		current_business_mode_ = bus_command_ptr->business_mode;
		ApInfo("path_selector") << "Updated business_mode to: " << current_business_mode_;
	}

	// 根据 business_mode 选择输出路径
	switch (current_business_mode_) {
		case 0:
			// business_mode == 0: 发送空路径
			ApInfo("path_selector") << "business_mode=0, sending empty trajectory";
			CreateEmptyTrajectory(planning_result_ptr);
			break;

		case 3:
		case 31:
			// business_mode == 3 或 31: 选择 tppathplan_follow
			if (bGet_follow && pathplan_follow_ptr) {
				ApInfo("path_selector") << "business_mode=" << current_business_mode_ 
				      << ", selecting tppathplan_follow, points: " << pathplan_follow_ptr->trajectoryPoints.size();
				planning_result_ptr = pathplan_follow_ptr;
			} else {
				ApInfo("path_selector") << "business_mode=" << current_business_mode_ 
				      << ", tppathplan_follow not available, sending empty trajectory";
				CreateEmptyTrajectory(planning_result_ptr);
			}
			break;
		case 1:
		case 11:
			// business_mode == 11: 选择 tppathplan_track
			if (bGet_track && pathplan_track_ptr) {
				ApInfo("path_selector") << "business_mode=11, selecting tppathplan_track, points: " 
				      << pathplan_track_ptr->trajectoryPoints.size();
				planning_result_ptr = pathplan_track_ptr;
			} else {
				ApInfo("path_selector") << "business_mode=11, tppathplan_track not available, sending empty trajectory";
				CreateEmptyTrajectory(planning_result_ptr);
			}
			break;

		case 4:
			// business_mode == 4: 选择 tppathplan_adjust
			if (bGet_adjust && pathplan_adjust_ptr) {
				ApInfo("path_selector") << "business_mode=4, selecting tppathplan_adjust, points: " 
				      << pathplan_adjust_ptr->trajectoryPoints.size();
				planning_result_ptr = pathplan_adjust_ptr;
			} else {
				ApInfo("path_selector") << "business_mode=4, tppathplan_adjust not available, sending empty trajectory";
				CreateEmptyTrajectory(planning_result_ptr);
			}
			break;

		default:
			// 其他模式暂不处理，发送空路径
			ApInfo("path_selector") << "business_mode=" << current_business_mode_ 
			      << " not handled, sending empty trajectory";
			CreateEmptyTrajectory(planning_result_ptr);
			break;
	}

	return 0;
}
