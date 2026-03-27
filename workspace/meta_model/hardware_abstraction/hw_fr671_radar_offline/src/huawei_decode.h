#ifndef __AVOS_DRIVER_HUAWEI_RADAR_DECODER_H__
#define __AVOS_DRIVER_HUAWEI_RADAR_DECODER_H__

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <string.h>
#include <atomic>

#include "huawei_4d_radar.h"
#include "data_pool.h"
#include "udp_client.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"
#include "adsfi_manager/base_adsfi_interface.h"

#define DEG2ARC 3.14159265 / 180.0

namespace avos {
namespace driver {

class HWDecoder
{
public:
	HWDecoder();
	~HWDecoder();

public:
	int Init(const DecoderConfig &config, int sensor_id, int frame_interval_ms=0) ;
	bool IsPktForOneFrame() ;
	void ConvertPacktsToPoint(const ara::adsfi::CommonUdpPacket &msop_packet, ara::adsfi::SensorRadarPointCloud &point_cloud, uint64_t &first_pkg_ts, int &pub_logic);

	double GetActCosValue(int act_angle) {
		while (act_angle < 0) {
			act_angle += CULCULATE_TABLE_SIZE;
		}
		while (act_angle > CULCULATE_TABLE_SIZE) {
			act_angle -= CULCULATE_TABLE_SIZE;
		}
		if (act_angle >= CULCULATE_TABLE_SIZE || act_angle < 0) {
			return 0;
		}
		return cos_table[act_angle];
	}

	double GetActSinValue(int act_angle) {
		while (act_angle < 0) {
			act_angle += CULCULATE_TABLE_SIZE;
		}
		while (act_angle > CULCULATE_TABLE_SIZE) {
			act_angle -= CULCULATE_TABLE_SIZE;
		}
		if (act_angle >= CULCULATE_TABLE_SIZE || act_angle < 0) {
			return 0;
		}
		return sin_table[act_angle];
	}

	void ConvertPoint(ara::adsfi::SensorRadarPoint4D &point, const DecoderConfig &config) {

		float x_coord = point.x;
		float y_coord = point.y;
		float z_coord = point.z;
		point.x = config.transform[0] * x_coord + config.transform[1] * y_coord + config.transform[2] * z_coord + config.transform[3];
		point.y = config.transform[4] * x_coord + config.transform[5] * y_coord + config.transform[6] * z_coord + config.transform[7];
		point.z = config.transform[8] * x_coord + config.transform[9] * y_coord + config.transform[10] * z_coord + config.transform[11];
		float vx=point.vx,vy=point.vy;
		point.vx = config.transform[0] * vx + config.transform[1] * vy;
		point.vy = config.transform[4] * vx + config.transform[5] * vy;
		// point.angle = point.angle + config.tf_yaw;
		// point.angle = point.angle >= 0.0 ? point.angle : point.angle + 360.0;
		// point.angle = point.angle <= 360.0 ? point.angle : point.angle - 360.0;
	}

	void ec_add(const std::string &ec, const std::string &extar_info, uint8_t threshold=1) {
        std::lock_guard<std::mutex> lock(_mtx);
        static timeval tv = {};
        gettimeofday(&tv, nullptr);

        auto ptr = ec_map.find(ec);
        if (ptr == ec_map.end()) {
            ec_map[ec] = ErrCodeState();
        }
        ec_map[ec].normal_cnt = 0;
        ec_map[ec].abnormal_cnt++;
        ec_map[ec].abnormal_ts = tv.tv_sec*1000 + tv.tv_usec / 1000.0;
        if((ec_map[ec].abnormal_cnt >= threshold)&&(!ec_map[ec].last_state_is_abnormal))
        {
            ec_map[ec].last_state_is_normal =  false;
            ec_map[ec].last_state_is_abnormal = true;

            std::string info = std::to_string(ec_map[ec].abnormal_ts)+" ms: "+extar_info;
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, info);
        }
    }

    void ec_remove(const std::string &ec, uint8_t threshold=1) {
        std::lock_guard<std::mutex> lock(_mtx);
        static timeval tv = {};
        gettimeofday(&tv, nullptr);

        auto ptr = ec_map.find(ec);
        if (ptr == ec_map.end()) {
            ec_map[ec] = ErrCodeState();
        }
        ec_map[ec].abnormal_cnt = 0;
        ec_map[ec].normal_cnt++;
        ec_map[ec].normal_ts = tv.tv_sec*1000 + tv.tv_usec / 1000.0;
        if((ec_map[ec].normal_cnt >= threshold)&&(!ec_map[ec].last_state_is_normal))
        {
            ec_map[ec].last_state_is_normal =  true;
            ec_map[ec].last_state_is_abnormal = false;

		FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lock(_mtx);
        for (const auto &ec : ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        ec_map.clear();
    }

	int sensor_id_ = 0;
private:
	std::mutex                           _mtx;
    typedef struct ErrCodeState
    {
        uint8_t normal_cnt;
        uint8_t abnormal_cnt;
        uint64_t normal_ts;//ms
        uint64_t abnormal_ts;//ms
        bool     last_state_is_normal;
        bool     last_state_is_abnormal;
        ErrCodeState()
        {
            normal_cnt = 0;
            abnormal_cnt = 0;
            normal_ts = 0;
            abnormal_ts = 0;
            last_state_is_normal = false;
            last_state_is_abnormal = false;
        }
    }ErrCodeState;
    std::map<std::string, ErrCodeState> ec_map; //<ecc_code, state>
private:
	int32_t frame_id_ = -1;
	int32_t udp_pkg_id_ = -1;
	int32_t udp_pkg_num_ = -1;
	int32_t udp_pkg_count_ = 0;
	int32_t obj_num_ = -1;
	int32_t obj_recv_sum_ = 0;
	int32_t obj_num_in_pkg_ = 0;
	bool	is_new = false;
	int32_t interval_ms_ = 50;
	long long frame_ts_ = 0;
	std::atomic<bool> one_frame_;

	avos::UdpClient check_alive_client_;
	avos::UdpClient vehicle_info_client_;

	ara::adsfi::SensorRadarPoint4D           point;
	ara::adsfi::CommonUdpPacket            msop_buf;

	double sin_table[CULCULATE_TABLE_SIZE];
	double cos_table[CULCULATE_TABLE_SIZE];
public:
	DecoderConfig decoder_config;

	//对应的角度标准方差, -1代表无效值
	const std::array<float, 32>  angle_std = {0.005, 0.007, 0.010, 0.014, 0.020, 0.029, 0.041, 0.058, 0.082, 0.116, 0.165, 0.234,
							0.332, 0.471, 0.669, 0.949, 1.346, 1.909, 2.709, 3.843, 5.451, 7.734, 10.971, 15.565,
							22.081, 31.325, 44.439, 63.044, 89.437, 126.881, 180.00, -1};
	//对应的距离/速率/加速率标准方差, -1代表无效值
	const std::array<float, 32>  other_std = {0.005, 0.006, 0.008, 0.011, 0.014, 0.018, 0.023, 0.029, 0.038, 0.049, 0.063, 0.081,
							0.105, 0.135, 0.174, 0.224, 0.288, 0.371, 0.478, 0.616, 0.794, 1.023, 1.317, 1.697, 2.187, 2.817, 3.630,
							4.676, 6.025, 7.762, 10.00, -1};
};


}
}

#endif  // __AVOS_DRIVER_RS_BP_DECODER_H__

