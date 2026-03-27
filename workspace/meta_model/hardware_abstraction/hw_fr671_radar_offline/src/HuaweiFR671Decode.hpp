#ifndef __HUAWEI_FR671_DECODE_H__
#define __HUAWEI_FR671_DECODE_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "UserLockQueue.h"
#include "data_pool.h"
#include "huawei_decode.h"
#include "multicast_udp.h"
#include "udp_client.h"
#include "custom_stack.h"
#include "log/log.h"
#include "params/radar_shm_param.h"
#include "HuaweiFR671Decode.hpp"

class FR671DriverOffline {
public:
    FR671DriverOffline() {
    }

    void DecodePackets(const ara::adsfi::SensorRadarPacket& packets, // 原始数据
                       ara::adsfi::SensorRadarPointCloud&   points) {

        RadarShmParam::GetRadarModuleParam(radar_name_, decoder.decoder_config.pre_rot_axis_0,
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
        points.header = packets.header;
		points.header.data_name = "radar_point";
        points.objs.clear();
        points.objs.reserve(50000);
		ara::adsfi::SensorRadarPointCloud packet_point_cloud;

        for (int i = 0; i < packets.msop_packet.size(); i++) {
			packet_point_cloud.objs.clear();
			decoder.ConvertPacktsToPoint(packets.msop_packet[i], packet_point_cloud, first_pkg_ts, pub_logic);
			// if(abs(first_pkg_ts/1000.0 - packets.header.time > 0.01))
			// {
			// 	printf("packets msg's header ts[%.3f] != header_udp's ts[%.3f]\n", packets.header.time, first_pkg_ts/1000.0);
			// }
			points.objs.insert(points.objs.end(), packet_point_cloud.objs.begin(), packet_point_cloud.objs.end());
        }
        double time_2 = Tools::Second();
        // AINFO << "DecodePacketsConvert points:" << points.objs.size() << " timecost: " << (time_2 - time_1) * 1000 << " ms";
		double header_ts = ADSFI_TO_SEC(packets.header.time.sec, packets.header.time.nsec);
		printf("frame header ts: %.3f, point cloud size: %d\n", header_ts, points.objs.size());
    }

private:
	bool 		  init =  false;
	int			  pub_logic;
	uint64_t      first_pkg_ts;
public:
    std::string   radar_name_ = "fr671";
	int 		  sensor_id_ = 0;
    avos::driver::HWDecoder decoder;
	avos::driver::DecoderConfig   radar_cali_config;
	avos::driver::HWRadarDriverConfig	  driver_cfg;
};

#endif
