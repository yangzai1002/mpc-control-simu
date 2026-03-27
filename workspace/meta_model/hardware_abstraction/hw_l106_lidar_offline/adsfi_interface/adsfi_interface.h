
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_sensorlidarpacket.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "src/HuaweiL106Decode.hpp"

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


	void Callback(const std::string name,const std::shared_ptr<ara::adsfi::SensorLidarPacket> &msg);

	void SetLidarPackets(const std::shared_ptr<ara::adsfi::SensorLidarPacket> &ptr){

            std::lock_guard<std::mutex> lock(lidar_packets_mtx_);
            lidar_packets_ptr = ptr;
            m_cond.notify_one();

	}
	void GetLidarPackets(ara::adsfi::SensorLidarPacket &msg){
		 std::unique_lock<std::mutex> lock(lidar_packets_mtx_);
        m_cond.wait(lock);
        msg = *lidar_packets_ptr;
	}


private:
	void ReadLidarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
	                            const std::string &lidar_name, avos::driver::DecoderConfig &config, L106DriverOffline *driver) ;

private:

    std::shared_ptr<L106DriverOffline> driver_ptr;

    std::mutex                                    lidar_packets_mtx_;
    std::condition_variable                       m_cond;
    std::shared_ptr<ara::adsfi::SensorLidarPacket> lidar_packets_ptr;

public:
	BASE_TEMPLATE_FUNCION
};
#endif








