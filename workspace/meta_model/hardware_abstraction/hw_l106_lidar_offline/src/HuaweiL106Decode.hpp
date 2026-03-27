#ifndef __AVOS_DRIVER_HUAWEI_L106_LIDAR_DECODER_HPP__
#define __AVOS_DRIVER_HUAWEI_L106_LIDAR_DECODER_HPP__


#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "custom_stack.h"
#include "log/log.h"

#include "UserLockQueue.h"
#include "data_pool.h"
#include "huawei_decode.h"
#include "multicast_udp.h"
#include "params/lidar_shm_param.h"

class L106DriverOffline {
public:
    L106DriverOffline() {
    }

    void DecodePackets(const ara::adsfi::SensorLidarPacket& packets, // 原始数据
                       ara::adsfi::MsgLidarFrame&   points) {

        LidarShmParam::GetLidarModuleParam(lidar_name_, decoder.decoder_config.pre_rot_axis_0,
	                                   decoder.decoder_config.pre_rot_axis_1,
	                                   decoder.decoder_config.pre_rot_axis_2,
	                                   decoder.decoder_config.pre_rot_degree_0,
	                                   decoder.decoder_config.pre_rot_degree_1,
	                                   decoder.decoder_config.pre_rot_degree_2,
	                                   decoder.decoder_config.tf_x,
	                                   decoder.decoder_config.tf_y,
	                                   decoder.decoder_config.tf_z,
	                                   decoder.decoder_config.tf_roll,
	                                   decoder.decoder_config.tf_pitch,
	                                   decoder.decoder_config.tf_yaw
	                                  );
        decoder.decoder_config.Init();

        double time_1 = Tools::Second();
		static long long lidar_seq = 0;
        points.pointCloud.clear();
        points.pointCloud.reserve(50000);
		ara::adsfi::MsgLidarFrame packet_point_cloud;
		if(false == init)
		{
			if(0 == decoder.SetSupplementaryData(packets, driver_cfg.debug))
			{
				init = true;
			}
		}
		double header_ts = ADSFI_TO_SEC(packets.header.time.sec, packets.header.time.nsec);
		points.timestamp.sec = ADSFI_GET_SEC(header_ts);
        points.timestamp.nsec = ADSFI_GET_NSEC(header_ts);
		points.seq = lidar_seq++;
        points.frameID = "/base_link";
        for (int i = 0; i < packets.msop_packet.size(); i++) {
			packet_point_cloud.pointCloud.clear();
			decoder.ConvertHuaweiPacktsToPoint(packets.msop_packet[i], packet_point_cloud, header_ts);
			points.pointCloud.insert(points.pointCloud.end(), packet_point_cloud.pointCloud.begin(), packet_point_cloud.pointCloud.end());
        }
        double time_2 = Tools::Second();
        // AINFO << "DecodePacketsConvert points:" << points.pointCloud.size() << " timecost: " << (time_2 - time_1) * 1000 << " ms";
		printf("frame header ts: %.3f, point cloud size: %d\n", header_ts, points.pointCloud.size());
    }

private:
	bool 		  init =  false;
	int			  pub_logic;
	uint64_t      first_pkg_ts;
public:
    std::string   lidar_name_;
    avos::driver::HWDecoder decoder;
	avos::driver::DecoderConfig   lidar_cali_config;
	avos::driver::HWLidarDriverConfig	  driver_cfg;
};

#endif