
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_sensorlidarpacket.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "HuaweiL106.hpp"

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {
		driver_ptr = nullptr;
	}
	~AdsfiInterface() {

	}

	void Init() ;

	int Process(const std::string name, std::shared_ptr<ara::adsfi::MsgLidarFrame> &msg);


	int Process(const std::string name,std::shared_ptr<ara::adsfi::SensorLidarPacket> &msg) ;


private:
	void ReadLidarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
	                            const std::string &lidar_name, avos::driver::DecoderConfig &config, L106Driver *driver) ;

private:

    std::shared_ptr<L106Driver> driver_ptr;

public:
	BASE_TEMPLATE_FUNCION
};
#endif








