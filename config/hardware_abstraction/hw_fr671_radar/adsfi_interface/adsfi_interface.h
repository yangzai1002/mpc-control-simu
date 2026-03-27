
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_sensorradarpointcloud.h"
#include "ara/adsfi/impl_type_sensorradarpacket.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "HuaweiFR671V2.hpp"
class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {
		driver_ptr = nullptr;
	}
	~AdsfiInterface() {

	}

	void Init() ;

	int Process(const std::string &name,std::shared_ptr<ara::adsfi::SensorRadarPointCloud> &msg);


	int Process(const std::string &name,std::shared_ptr<ara::adsfi::SensorRadarPacket> &msg) ;

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg_ptr);


	void Callback(const std::string &name,  const std::shared_ptr<ara::adsfi::VehicleInformation> &msg_ptr);

private:

	void ReadLidarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
                            const std::string &radar_name, avos::driver::DecoderConfig &config, FR671 *driver) ;

private:

    std::shared_ptr<FR671> driver_ptr;

public:
	BASE_TEMPLATE_FUNCION
};
#endif








