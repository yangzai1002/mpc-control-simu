#include "adsfi_interface/adsfi_interface.h"


void AdsfiInterface::Init() {
		SetScheduleInfo("message");
    // ApLogInterface::Instance()->Init("XFusion_lidar_terrain_map");
		ApInfo("fusion_lidar_terrain_map") << "XFusion_lidar_terrain_map init";
		std::string error_info;
		if(terrain3_handle.Init(error_info)!=0)
		{
			ApError("fusion_lidar_terrain_map") << "Terrainmain init fail:"<<error_info;
		}
		//ApError("fusion_lidar_terrain_map")
		//ApInfo("fusion_lidar_terrain_map")
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &lidar_ptr) {
		if(name!="tpfront_middle_lidar_points")return;
		double lidar_time = lidar_ptr->timestamp.sec+lidar_ptr->timestamp.nsec*1e-9;
		ApInfo("fusion_lidar_terrain_map") << "sensor_lidar_name:"<<name<<"front lidar callback:" << lidar_ptr->seq <<"lidar_time:"<<std::to_string(lidar_time);
		// SensorLidarPointCloud lidar_in;
		// lidar_in.header.time = lidar_time;
		// lidar_in.points.resize(lidar_ptr->pointCloud.size());
		// SensorLidarPoint new_pot;
		// for(int i=0;i<lidar_ptr->pointCloud.size();++i)
		// {
		// 	const auto& pot = lidar_ptr->pointCloud[i];
		// 	// new_pot.time_stamp
		// 	lidar_in.points[i].x = pot.x;
		// 	lidar_in.points[i].y = pot.y;
		// 	lidar_in.points[i].z = pot.z;
		// 	lidar_in.points[i].angle = pot.angle;
		// 	lidar_in.points[i].ring = pot.ring;
		// 	lidar_in.points[i].intensity = pot.intensity;
		// 	// lidar_in.points.push_back(new_pot);
		// }
		terrain3_handle.PushFrontPointcloud(lidar_ptr);
}

// void AdsfiInterface::Callback(const std::string &name, const std::string &topic,const std::shared_ptr<SensorLidarPointCloud> &lidar_ptr) {
//     std::cout << "left_front lidar callback:" << lidar_ptr->header.seq << std::endl;
// 	terrain3_handle.CheckLidarData(*lidar_ptr,1);
// }
// void AdsfiInterface::Callback(const std::string &name, const std::string &topic,const std::shared_ptr<SensorLidarPointCloud> &lidar_ptr) {
//     std::cout << "right lidar callback:" << lidar_ptr->header.seq << std::endl;
// 	terrain3_handle.CheckLidarData(*lidar_ptr,2);adsfi::LocationInfo
// }

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_ptr) {
		std::cout << "location  callback:" << location_ptr->header.seq << std::endl;
		terrain3_handle.PushLocData(*location_ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &business_ptr) {
		std::cout << "IdpBusinessCommand  callback:" << business_ptr->header.seq << std::endl;
		terrain3_handle.PushBusinessCommandData(*business_ptr);
}


void MsgCameraVPResult2PerceptionVPResult(const ara::adsfi::MsgCameraVPResult& msg_vpres,PerceptionVPResult& vpresult)
{
	
      double cur_stamp = msg_vpres.header.timestamp.sec+msg_vpres.header.timestamp.nsec*1e-9;
      vpresult.header.time = cur_stamp;
      vpresult.header.data_name = msg_vpres.header.frameID;
      if(msg_vpres.header.time_chain_vec.size()>0)
      {

      vpresult.receive_stamp = msg_vpres.header.time_chain_vec[0].t1;
      vpresult.send_stamp = msg_vpres.header.time_chain_vec[0].t2;
      }
      for(const auto &cam_res:msg_vpres.vp_result_vec)
      {
        PerceptionCameraResult vp;
        PerceptionVisionObject obj;
        vp.valid = true;
        vp.camera_name = cam_res.header.frameID;
        vp.stamp = cam_res.header.timestamp.sec+cam_res.header.timestamp.nsec*1e-9;
        int num_2d = cam_res.image_objects.detectionOut2d.size();
        vp.vision_objects.reserve(num_2d);
        for(int i=0;i<num_2d;++i)
        {
          const auto &object2d = cam_res.image_objects.detectionOut2d[i];
          obj.object_2d.prob = object2d.confidence;
          obj.object_2d.obj_id = object2d.cls;
          obj.object_2d.w = object2d.rect.size.x;
          obj.object_2d.h = object2d.rect.size.y;
          obj.object_2d.x = object2d.rect.center.x - obj.object_2d.w*0.5;
          obj.object_2d.y = object2d.rect.center.y - obj.object_2d.h*0.5;
          vp.vision_objects.push_back(obj);
        }
        //obj 3d
        int num_3d = cam_res.image_objects.detectionOut3d.size();
        for(int i=0;i<num_3d;++i)
        {
          const auto &object3d = cam_res.image_objects.detectionOut3d[i];
          unsigned int idx = object3d.types;
          auto &select_obj = vp.vision_objects[idx];
          select_obj.object_3d.enable=true;
          select_obj.object_3d.x = object3d.rect.center.x;
          select_obj.object_3d.y = object3d.rect.center.y;
          select_obj.object_3d.z = object3d.rect.center.z;
          select_obj.object_3d.length = object3d.rect.size.x;          // 沿X轴的尺寸（前后方向）
                select_obj.object_3d.width = object3d.rect.size.y;            // 沿Y轴的尺寸（左右方向）
                select_obj.object_3d.height = object3d.rect.size.z;          // 沿Z轴的尺寸（上下方向）
                select_obj.object_3d.protate = object3d.rect.orientation;
        }
        
        for(const auto &image_data:cam_res.image_segmentation_vec)
        {
          vp.vision_segment.camera_width = image_data.timestamp.sec;
          vp.vision_segment.camera_height = image_data.timestamp.nsec;
          if(image_data.frameID=="seg")//seg
          {
            // vp.vision_segment.seg.cols = image_data.width;
            // vp.vision_segment.seg.rows = image_data.height;
            vp.vision_segment.seg.height = image_data.height;
            vp.vision_segment.seg.width = image_data.width;
            vp.vision_segment.seg.data.resize(image_data.dataSize);
            vp.vision_segment.seg_valid=true;
            memcpy(vp.vision_segment.seg.data.data(), image_data.rawData.data(), image_data.dataSize);
          }else if(image_data.frameID=="resa")//resa
          {
            // vp.vision_segment.resa.cols = image_data.width;
            // vp.vision_segment.resa.rows = image_data.height;
            vp.vision_segment.resa.height = image_data.height;
            vp.vision_segment.resa.width = image_data.width;
            vp.vision_segment.resa.data.resize(image_data.dataSize);
            vp.vision_segment.resa_valid=true;
            memcpy(vp.vision_segment.resa.data.data(), image_data.rawData.data(), image_data.dataSize);
          }
        }
        
        vpresult.camera_vpresults.push_back(vp);
      }
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresult_ptr) {
		std::cout << "IdpPerceptionVPResult  callback:" << vpresult_ptr->header.seq << std::endl;
		static double last_stamp=0;
		double cur_stamp = vpresult_ptr->header.timestamp.sec+vpresult_ptr->header.timestamp.nsec*1e-9;
		if(cur_stamp==last_stamp){
			return;
		}
		last_stamp = cur_stamp;
		//转换
		PerceptionVPResult vpresult;
		MsgCameraVPResult2PerceptionVPResult(*vpresult_ptr,vpresult);
		terrain3_handle.PushVresultData(vpresult);
}

int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &terrmain_map_ptr)
{
	terrain3_handle.terrain_map_result_deque_.waitAndPop(terrmain_map_ptr);
	return 0;

}
