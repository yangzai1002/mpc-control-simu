
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "BD21ByCanWS.hpp"
#include "SocketCan.hpp"
#include "UserLockQueue.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"
#include "can_recv_protocol.h"
#include "config_loader/custom_stack.h"
#include "geometry_tools.hpp"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"
#include "ymdhms2GpsTs.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
	AdsfiInterface() {
		driver_ptr = nullptr;
	    loc_fusion_data_ptr = nullptr;
	    sensor_imu_data_ptr = nullptr;
		sensor_imu_sync_data_ptr = nullptr;
	    gnss_data_ptr = nullptr;
	    wheel_speed_data_ptr = nullptr;

        imu_seq = 0;
        gps_seq = 0;
        loc_fusion_seq = 0;
        wheel_speed_seq = 0;
        imu_valid = false;
        gps_valid = false;
        loc_fusion_valid = false;
        wheel_speed_valid = false;
    }
    ~AdsfiInterface() {
    }

    void Init() override;

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafIMU> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg);

private:


    std::shared_ptr<BD21Driver> driver_ptr;
    std::shared_ptr<avos::BD21::LocFusionData> loc_fusion_data_ptr;
    std::shared_ptr<avos::BD21::SensorImuData> sensor_imu_data_ptr;
	std::shared_ptr<avos::BD21::SensorImuData> sensor_imu_sync_data_ptr;
    std::shared_ptr<avos::BD21::GnssData> gnss_data_ptr;
    std::shared_ptr<avos::BD21::WheelSpeedData> wheel_speed_data_ptr;

    long long imu_seq;
    long long gps_seq;
    long long loc_fusion_seq;
    long long wheel_speed_seq;
    bool      imu_valid;
    bool      gps_valid;
    bool      loc_fusion_valid;
    bool      wheel_speed_valid;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
