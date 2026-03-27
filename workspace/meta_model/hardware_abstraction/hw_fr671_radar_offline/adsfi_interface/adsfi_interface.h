
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
#include "HuaweiFR671Decode.hpp"
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


	void Callback(const std::string &name,const std::shared_ptr<ara::adsfi::SensorRadarPacket> &msg) ;

private:

	void ReadRadarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
                            const std::string &radar_name, avos::driver::DecoderConfig &config) ;

	void SetRadarPackets(const std::shared_ptr<ara::adsfi::SensorRadarPacket> &ptr){

        std::lock_guard<std::mutex> lock(radar_packets_mtx_);
        radar_packets_ptr = ptr;
        m_cond.notify_one();
        
	}
	void GetRadarPackets(ara::adsfi::SensorRadarPacket &msg){
		 std::unique_lock<std::mutex> lock(radar_packets_mtx_);
        m_cond.wait(lock);
        msg = *radar_packets_ptr;
	}

private:

    std::shared_ptr<FR671DriverOffline> driver_ptr;

    std::mutex                                    radar_packets_mtx_;
    std::condition_variable                       m_cond;
    std::shared_ptr<ara::adsfi::SensorRadarPacket> radar_packets_ptr;

public:
	BASE_TEMPLATE_FUNCION
};
#endif








