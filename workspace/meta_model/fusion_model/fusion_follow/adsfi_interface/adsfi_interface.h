
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
// #include "ara/adsfi/impl_type_idpsensorlidarpointcloud.h"
// #include "ara/adsfi/impl_type_idpsensorimagelist.h"
// #include "adsfi/impl_type_locationinfo.h"//adsfi::LocationInfo
// #include "ara/adsfi/impl_type_idpbusinesscommand.h"//ara::adsfi::BusinessCommand
// #include "adsfi/impl_type_fusionobjectarray.h"//adsfi::FusionObjectArray
// #include "ara/adsfi/impl_type_idpperceptionvpresult.h"//视觉 ara::adsfi::IdpPerceptionVPResult
// #include "ara/adsfi/impl_type_idplidarpointcloud2datatype.h"//SensorLidarPointCloud;
#include <string>
#include <memory>
// #include "adsfi_manager/base_adsfi_interface.h"
// #include "adsfi_manager/lock_queue.h"
// #include "adsfi_interface/src/data_pool.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "target_finder_entry.h"
#include "ap_log/ap_log_interface.h"

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init() override;

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &lidar_ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::SensorRadarPointCloud> &radar_ptr);
	//adsfi::FusionObjectArray
	// void Callback(const std::string &name, const std::shared_ptr<adsfi::FusionObjectArray> &lidar_objs_ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHaf3dDetectionOutArray> &lidar_objs_ptr);
	
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_ptr);

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &business_ptr);

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresult_ptr);


	int Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &fusion_objs);
	// int Process(const std::string &name,std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &target_obj);

	std::shared_ptr<pertarget::FaultDiagnosis> error_handle_ptr;/// = std::make_shared<pertarget::FaultDiagnosis>();
// private:
	// avos::perception::Sem process_semantic_sem_;//语义地图信号量
	pertarget::target_finder_entry handle;
	#if defined(SCFI_SDK_X86)
	//x86
		std::map<std::string,int> sensor_lidar_remap_= {{"tpfront_left_lidar_points_offline",LEFT_FRONT_LIDAR_POINTS_CLOUD},{"tpfront_middle_lidar_points_offline",FRONT_LIDAR_POINTS_CLOUD},{"tpfront_right_lidar_points_offline",RIGHT_FRONT_LIDAR_POINTS_CLOUD}};
		std::map<std::string,int> sensor_radar_remap_= {{"tpfront_left_radar_points_offline",LEFT_FRONT_RADAR_POINTS},{"tpfront_middle_radar_points_offline",FRONT_RADAR_POINTS},{"tpfront_right_radar_points_offline",RIGHT_FRONT_RADAR_POINTS}};
	#else
	//arm												//左前						前							右前
		std::map<std::string,int> sensor_lidar_remap_= {{"tpfront_left_lidar_points",LEFT_FRONT_LIDAR_POINTS_CLOUD},{"tpfront_middle_lidar_points",FRONT_LIDAR_POINTS_CLOUD},{"tpfront_right_lidar_points",RIGHT_FRONT_LIDAR_POINTS_CLOUD}};
		std::map<std::string,int> sensor_radar_remap_= {{"tpfront_left_radar_points",LEFT_FRONT_RADAR_POINTS},{"tpfront_middle_radar_points",FRONT_RADAR_POINTS},{"tpfront_right_radar_points",RIGHT_FRONT_RADAR_POINTS}};
	#endif
	BASE_TEMPLATE_FUNCION
};
#endif








