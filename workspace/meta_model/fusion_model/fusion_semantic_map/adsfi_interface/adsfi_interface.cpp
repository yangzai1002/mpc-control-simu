#include "adsfi_interface/adsfi_interface.h"


void AdsfiInterface::Init() {
		SetScheduleInfo("message");
		std::cout << "init" << std::endl;
		semanticpro_ptr = SemanticMLidarFuse::GetInstance();
		// ApLogInterface::Instance()->Init("XFusion_semantic_map");
		//ApError("fusion_semantic_map")
		//ApInfo("fusion_semantic_map")
}

void MsgLidarFrame2SensorLidarPointCloud(const ara::adsfi::MsgLidarFrame& lidar_frame,SensorLidarPointCloud &point_cloud_out)
{
      double lidar_time = lidar_frame.timestamp.sec+lidar_frame.timestamp.nsec*1e-9;
      point_cloud_out.header.time = lidar_time;
      point_cloud_out.points.resize(lidar_frame.pointCloud.size());
      SensorLidarPoint new_pot;
      for(int i=0;i<lidar_frame.pointCloud.size();++i)
      {
        const auto& pot = lidar_frame.pointCloud[i];
        // new_pot.time_stamp
        point_cloud_out.points[i].x = pot.x;
        point_cloud_out.points[i].y = pot.y;
        point_cloud_out.points[i].z = pot.z;
        point_cloud_out.points[i].angle = pot.angle;
        point_cloud_out.points[i].ring = pot.ring;
        point_cloud_out.points[i].intensity = pot.intensity;
        // point_cloud_out.points.push_back(new_pot);
      }
}


void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &lidar_ptr) {
		std::cout << "sensor_lidar_name:"<<name<<"front lidar callback:" << lidar_ptr->seq << std::endl;
		//
		// std::cout << "sensor_lidar_name:"<<name<<"stamp:"<<std::fixed<<stamp<<" lidar_points_size:" << lidar_ptr->pointCloud.size() << std::endl;
		
		if(sensor_lidar_remap_.find(name)==sensor_lidar_remap_.end())
		{
			// AERROR<<"input lidar name error:"<<name<<" plese ref:"
			for(auto &it:sensor_lidar_remap_)
			{
				ApError("fusion_semantic_map")<<"input lidar name error:"<<name<<" plese ref:"<<it.first;
				std::cout<<"error:"<<"input lidar name error:"<<name<<" plese ref:"<<it.first;
			}
			return;
		}
		std::cout<<"sensor_lidar_remap:"<<name<<"idx:"<<sensor_lidar_remap_[name]<<std::endl;
		semanticpro_ptr->CheckLidarData(lidar_ptr,sensor_lidar_remap_[name]);
}


void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_ptr) {
		double stamp = location_ptr->header.timestamp.sec+location_ptr->header.timestamp.nsec*1e-9;
		std::cout << "location  callback:" << location_ptr->header.seq <<"stamp:"<<std::fixed<<stamp << std::endl;
		semanticpro_ptr->PushLocData(location_ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &business_ptr) {
		std::cout << "IdpBusinessCommand  callback:" << business_ptr->header.seq << std::endl;
		semanticpro_ptr->PushBusinessCommandData(*business_ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresult_ptr) {
		std::cout << "IdpPerceptionVPResult  callback:" << vpresult_ptr->header.seq << std::endl;
		semanticpro_ptr->PushVresultData(*vpresult_ptr);
}

int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &semanticmap_ptr)
{
	semanticpro_ptr->static_env_result_deque_.waitAndPop(semanticmap_ptr);
  std::cout<<"Process_out"<<std::endl;
	return 0;

}
