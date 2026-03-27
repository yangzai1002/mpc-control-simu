#include "adsfi_interface/adsfi_interface.h"


void AdsfiInterface::Init() {
		SetScheduleInfo("message");
		// std::cout << "init" << std::endl;
		// ApLogInterface::Instance()->Init("XFusion_follow");
		std::string error_info;
		error_handle_ptr = std::make_shared<pertarget::FaultDiagnosis>();
		handle.Init(error_info,error_handle_ptr);
		if(handle.InitedFaild()){
			error_handle_ptr->SetInitError(error_info);
			ApError("fusion_follow")<<"handle.InitedFaild:"<<error_info;
		}
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &lidar_ptr) {
		double stamp = lidar_ptr->timestamp.sec + lidar_ptr->timestamp.nsec * 1e-9;
		std::cout << "sensor_lidar_name:"<<name<<"stamp:"<<std::fixed<<stamp<<" lidar_points_size:" << lidar_ptr->pointCloud.size() << std::endl;
		
		if(sensor_lidar_remap_.find(name)==sensor_lidar_remap_.end())
		{
			// ApError("fusion_follow")<<"input lidar name error:"<<name<<" plese ref:"
			for(auto &it:sensor_lidar_remap_)
			{
				ApError("fusion_follow")<<"input lidar name error:"<<name<<" plese ref:"<<it.first;
			}
			return;
		}
		handle.PushPointcloud(lidar_ptr,sensor_lidar_remap_.at(name));
}
// void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<RadarPointCloud> &radar_ptr) {
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::SensorRadarPointCloud> &radar_ptr) {
		std::cout << "sensor_radar_name:"<<name<<"radar callback:" << radar_ptr->header.seq << std::endl;
		if(sensor_radar_remap_.find(name)==sensor_radar_remap_.end())
		{
			// ApError("fusion_follow")<<"input lidar name error:"<<name<<" plese ref:"
			for(auto &it:sensor_radar_remap_)
			{
				ApError("fusion_follow")<<"input radar name error:"<<name<<" plese ref:"<<it.first;
			}
			return;
		}
		handle.PushRadarcloud(radar_ptr,sensor_radar_remap_.at(name));
}


// void AdsfiInterface::Callback(const std::string &name, const std::string &topic,const std::shared_ptr<SensorLidarPointCloud> &lidar_ptr) {
//     std::cout << "left_front lidar callback:" << lidar_ptr->header.seq << std::endl;
// 	handle.CheckLidarData(*lidar_ptr,1);
// }
// void AdsfiInterface::Callback(const std::string &name, const std::string &topic,const std::shared_ptr<SensorLidarPointCloud> &lidar_ptr) {
//     std::cout << "right lidar callback:" << lidar_ptr->header.seq << std::endl;
// 	handle.CheckLidarData(*lidar_ptr,2);
// }
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHaf3dDetectionOutArray> &lidar_objs_ptr) {
		std::cout << "IdpPerceptionLidarObjectsData  callback:" << lidar_objs_ptr->seq << std::endl;
		handle.PushLidarObjsData(*lidar_objs_ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_ptr) {
		std::cout << "LocationInfo  callback:" << location_ptr->header.seq << std::endl;
		handle.PushLocData(*location_ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &business_ptr) {
		std::cout << "BusinessCommand  callback:" << business_ptr->header.seq << std::endl;
		handle.PushBusinessCommandData(*business_ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresult_ptr) {
		std::cout << "MsgCameraVPResult  callback:" << vpresult_ptr->header.seq << std::endl;
		handle.PushVresultData(*vpresult_ptr);
}

int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &fusion_objs)
{
	// name = "/tpperceptionobjects";
	if(name == "tpperceptionobjects"){
		handle.objects_result_deque_.waitAndPop(fusion_objs);
		return 0;
	}
	if(name == "tptarget_obj"){//name = "/tptarget_obj";
		handle.target_object_result_deque_.waitAndPop(fusion_objs);
		return 0;
	}
	
	return -1;

}

// int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &target_obj)
// {
// 	// name = "/tptarget_obj";
// 	
// 	return 0;

// }
