#include "adsfi_interface.h"

#include "ap_log/ap_log_interface.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <string>

void AdsfiInterface::Init()  
{
	// 设置为定时触发模式 10ms (100Hz)
	SetScheduleInfo("timmer", 10);
	// ApLogInterface::Instance()->Init("location");
	ApInfo("location") << "init LOCATION";
}

void AdsfiInterface::DeInit()  
{
	ApInfo("location") << "deinit LOCATION";
}

// MsgHafLocation 回调: /tpimu, /tpododr, /tpliodr
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) 
{
	ApInfo("location") << "[LOCATION] MsgHafLocation callback: " << name;
	if (name == "tpimu") {
		ApInfo("location") << "[LOCATION] tpimu seq=" << ptr->header.seq;
		safebuf_tpimu_.Set(ptr);
	} 
	else if (name == "tpododr") {
		ApInfo("location") << "[LOCATION] ododr seq=" << ptr->header.seq;
		safebuf_ododr_.Set(ptr);
	}
	else if (name == "tpliodr") {
		ApInfo("location") << "[LOCATION] liodr seq=" << ptr->header.seq;
		safebuf_liodr_.Set(ptr);
	}
}

// MsgLidarFrame 回调: /tpfront_middle_lidar_points, /tpfront_left_lidar_points, /tpfront_right_lidar_points
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &ptr) 
{
	ApInfo("location") << "[LOCATION] MsgLidarFrame callback: " << name;
	if (name == "tpfront_middle_lidar_points") {
		ApInfo("location") << "[LOCATION] front_middle seq=" << ptr->seq;
		safebuf_lidar_front_middle_.Set(ptr);
	} 
	else if (name == "tpfront_left_lidar_points") {
		ApInfo("location") << "[LOCATION] front_left seq=" << ptr->seq;
		safebuf_lidar_front_left_.Set(ptr);
	}
	else if (name == "tpfront_right_lidar_points") {
		ApInfo("location") << "[LOCATION] front_right seq=" << ptr->seq;
		safebuf_lidar_front_right_.Set(ptr);
	}
}

// MsgHafIMU 回调: /tpsensorimu
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafIMU> &ptr) 
{
	ApInfo("location") << "[LOCATION] MsgHafIMU callback: " << name;
	if (name == "tpsensorimu") {
		ApInfo("location") << "[LOCATION] sensorimu seq=" << ptr->imuHeader.seq;
		safebuf_sensorimu_.Set(ptr);
	}
}

// MsgHafWheelSpeedList 回调: /tpwheelspeed
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &ptr) 
{
	ApInfo("location") << "[LOCATION] MsgHafWheelSpeedList callback: " << name;
	if (name == "tpwheelspeed") {
		ApInfo("location") << "[LOCATION] wheelspeed seq=" << ptr->header.seq;
		safebuf_wheelspeed_.Set(ptr);
	}
}

void AdsfiInterface::CreateEmptyLocation(std::shared_ptr<ara::adsfi::MsgHafLocation> &out)
{
	out = std::make_shared<ara::adsfi::MsgHafLocation>();
	out->header.timestamp.sec = static_cast<int32_t>(avos::common::NodeTime::Now().ToSecond());
	out->header.timestamp.nsec = 0;
	out->header.seq = 0;
	out->header.frameID = "location_empty";
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr) 
{
	ApInfo("location") << "PROCESS LOCATION";

	// 获取 MsgHafLocation 数据
	std::shared_ptr<ara::adsfi::MsgHafLocation> tpimu_ptr;
	std::shared_ptr<ara::adsfi::MsgHafLocation> ododr_ptr;
	std::shared_ptr<ara::adsfi::MsgHafLocation> liodr_ptr;

	bool bGet_tpimu = safebuf_tpimu_.Get(tpimu_ptr);
	bool bGet_ododr = safebuf_ododr_.Get(ododr_ptr);
	bool bGet_liodr = safebuf_liodr_.Get(liodr_ptr);

	// 获取其他传感器数据 (暂时只获取，后续可用于融合)
	std::shared_ptr<ara::adsfi::MsgLidarFrame> lidar_front_middle_ptr;
	std::shared_ptr<ara::adsfi::MsgLidarFrame> lidar_front_left_ptr;
	std::shared_ptr<ara::adsfi::MsgLidarFrame> lidar_front_right_ptr;
	std::shared_ptr<ara::adsfi::MsgHafIMU> sensorimu_ptr;
	std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> wheelspeed_ptr;

	bool bGet_lidar_middle = safebuf_lidar_front_middle_.Get(lidar_front_middle_ptr);
	bool bGet_lidar_left = safebuf_lidar_front_left_.Get(lidar_front_left_ptr);
	bool bGet_lidar_right = safebuf_lidar_front_right_.Get(lidar_front_right_ptr);
	bool bGet_sensorimu = safebuf_sensorimu_.Get(sensorimu_ptr);
	bool bGet_wheelspeed = safebuf_wheelspeed_.Get(wheelspeed_ptr);

	// 只有从未收到过 tpimu 时才会 false
	if (!bGet_tpimu) {
		ApInfo("location") << "[LOCATION] Waiting for tpimu data, sending empty location";
		CreateEmptyLocation(loc_ptr);
		return 0;
	}

	// 融合输出: tpimu 全部赋值给输出
	loc_ptr = std::make_shared<ara::adsfi::MsgHafLocation>(*tpimu_ptr);

	// 如果有 liodr 数据，用它填充 dr_pose 字段
	if (bGet_liodr) {
		loc_ptr->dr_pose = liodr_ptr->pose;
	} else if (bGet_ododr) {
		// 没有 liodr 时，尝试用 ododr
		loc_ptr->dr_pose = ododr_ptr->pose;
	} else {
		// 都没有时，dr_pose 设为与 pose 一致
		// loc_ptr->dr_pose = tpimu_ptr->pose;
	}

	ApInfo("location") << "[LOCATION] Output: pose=(" << loc_ptr->pose.pose.position.x 
	      << "," << loc_ptr->pose.pose.position.y << "), "
	      << "dr_pose=(" << loc_ptr->dr_pose.pose.position.x 
	      << "," << loc_ptr->dr_pose.pose.position.y << ")";

	return 0;
}
