
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
// #include "ara/adsfi/impl_type_idpsensorlidarpointcloud.h"
// #include "ara/adsfi/impl_type_idpsensorimagelist.h"
// #include "adsfi/impl_type_locationinfo.h"//adsfi::LocationInfo
// #include "ara/adsfi/impl_type_idpbusinesscommand.h"//ara::adsfi::BusinessCommand
// #include "adsfi/impl_type_fusionobjectarray.h"//adsfi::FusionObjectArray
// #include "ara/adsfi/impl_type_idpperceptionvpresult.h"//视觉 PerceptionVPResult
// #include "ara/adsfi/impl_type_idplidarpointcloud2datatype.h"//SensorLidarPointCloud;
#include <string>
#include <memory>
// #include "adsfi_manager/base_adsfi_interface.h"
// #include "adsfi_manager/lock_queue.h"
// #include "adsfi_interface/src/data_pool.hpp"
#include "SemanticMapM.hpp"
#include "perception_fusion_common/common/utils/sem.h"
#include "ap_log/ap_log_interface.h"
void MsgLidarFrame2SensorLidarPointCloud(const ara::adsfi::MsgLidarFrame& lidar_frame,SensorLidarPointCloud &point_cloud_out);
class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init() override;

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &lidar_ptr);
	// void Callback(const std::string &name, const std::string &topic, const std::shared_ptr<SensorLidarPointCloud> &lidar_ptr);
	// void Callback(const std::string &name, const std::string &topic, const std::shared_ptr<SensorLidarPointCloud> &lidar_ptr);

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &location_ptr);

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &business_ptr);

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgCameraVPResult> &vpresult_ptr);


	int Process(const std::string &name,std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &semanticmap_ptr);

// private:
	avos::perception::Sem process_semantic_sem_;//语义地图信号量
	SemanticMLidarFuse* semanticpro_ptr = nullptr;
	#if defined(SCFI_SDK_X86)
															//左前						前							右前
	std::map<std::string,int> sensor_lidar_remap_= {{"tpfront_left_lidar_points_offline",1},{"tpfront_middle_lidar_points_offline",0},{"tpfront_right_lidar_points_offline",2}};
	#else
															//左前						前							右前
	std::map<std::string,int> sensor_lidar_remap_= {{"tpfront_left_lidar_points",1},{"tpfront_middle_lidar_points",0},{"tpfront_right_lidar_points",2}};
	#endif

	BASE_TEMPLATE_FUNCION
};
#endif








