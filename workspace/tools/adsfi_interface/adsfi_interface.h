
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init() override {
		SetScheduleInfo("timmer", 100);
		std::cout << "init" << std::endl;
	}

	void DeInit() override {
		std::cout << "deinit" << std::endl;
	}


	void Callback(const std::shared_ptr<ara::adsfi::MsgLidarFrame> &lidar_ptr) {
		// std::cout << "lidar1111 callback:" << lidar_ptr->header.seq << std::endl;
	}


	void Callback(const std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr) {
		// std::cout << "imagelist2222  callback:" << image_list_ptr->header.seq << std::endl;
	}



	int Process(std::shared_ptr<ara::adsfi::MsgImageDataList> &image_list_ptr) {
		static int i = 0;
		// image_list_ptr->header.seq = i++;
		return 0;
	}


	int Process(std::shared_ptr<ara::adsfi::MsgLidarFrame> &ptr) {
		static int i = 0;
		// ptr->header.seq = i++;
		return 0;
	}



	BASE_TEMPLATE_FUNCION
};
#endif








