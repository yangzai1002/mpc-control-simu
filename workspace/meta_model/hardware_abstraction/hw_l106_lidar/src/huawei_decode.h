#ifndef __AVOS_DRIVER_HUAWEI_LIDAR_DECODER_H__
#define __AVOS_DRIVER_HUAWEI_LIDAR_DECODER_H__

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <string.h>
#include <atomic>

#include "pointcloud_ctrl.h"
#include "pointcloud_data.h"

#include "data_pool.h"
#include "udp_client.h"
#include "custom_stack.h"
// #include "log/log.h"
#include "ap_log/ap_log_interface.h"
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
	int SetDifopConfig(const avos::SocketConfig &socket_config);
	int Init(const DecoderConfig &config, int sensor_id) ;
	bool IsPktForOneFrame() ;
	bool IsSupplementaryInformatFull() ;
	int SendAllSupplementaryInformation() ;
	void SendCheckAliveMsg() ;
	void SendLidarConfigMsg(int phase_lock) ;
	void SendSupplementaryInformation(unsigned short data_id, unsigned int check_sum,
	                                  unsigned short index);
	int ReceiveSuppInfo(char* data, int max_len) ;
	int GetFOVVerticalAngle(VerticalAngle* pkt) ;
	int  GetVerticalAngleOffset(VerticalAngleOffset* pkt, int index) ;
	int GetHorizontalAngleOffset(HorizontalAngleOffset* pkt, int index) ;
	int GetPacketAngle(const char *packet) ;
	void ConvertPacktsToPoint(const ara::adsfi::CommonUdpPacket &msop_packet, ara::adsfi::MsgLidarFrame &point_cloud, double &first_pkg_ts, int &pub_logic, bool udp_test = false);
	void ConvertHuaweiPacktsToPoint(const ara::adsfi::CommonUdpPacket &packet, ara::adsfi::MsgLidarFrame &point_cloud);

	int GetSupplementaryData(ara::adsfi::CommonUdpPacket &packets_ptr);
	int SetSupplementaryData(const ara::adsfi::SensorLidarPacket &packets_ptr);

	std::string getInterfaceNameByIP(const std::string& targetIP);
	bool isIpReachable(const std::string& ip, int debug=0);

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

	void ConvertPoint(ara::adsfi::MsgPointXYZIRT &point, const DecoderConfig &config) {

		float x_coord = point.x;
		float y_coord = point.y;
		float z_coord = point.z;
		point.x = config.transform[0] * x_coord + config.transform[1] * y_coord + config.transform[2] * z_coord + config.transform[3];
		point.y = config.transform[4] * x_coord + config.transform[5] * y_coord + config.transform[6] * z_coord + config.transform[7];
		point.z = config.transform[8] * x_coord + config.transform[9] * y_coord + config.transform[10] * z_coord + config.transform[11];
		point.angle = point.angle + config.tf_yaw;
		point.angle = point.angle >= 0.0 ? point.angle : point.angle + 360.0;
		point.angle = point.angle <= 360.0 ? point.angle : point.angle - 360.0;
	}

	void ec_add(const std::string &ec, const std::string &extar_info, const std::string &step="init", uint8_t threshold=1) {
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
			ApError(step) << "ec_add: " << ec << ", extra_info: " << info;
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
	avos::UdpClient errcode_info_client_;
	LidarErrCode lidar_err_code;
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
	int vertical_angle_[MAX_LIDAR_RING];
	int vertical_angle_offset_[480][4];
	int horizontal_angle_offset_[MAX_LIDAR_RING][4];

	std::vector<std::pair< unsigned int, unsigned int>> vec_data_id_;
	std::vector<bool> data_id_flag_;

	int32_t frame_id_ = -1;
	int32_t udp_pkg_id_ = -1;
	int32_t udp_pkg_count_ = -1;
	bool	is_new = false;
	std::atomic<bool> one_frame_;

	avos::UdpClient check_alive_client_;
	avos::UdpClient supple_info_client_;
	DecoderConfig decoder_config;

	ara::adsfi::MsgPointXYZIRT           point;
	ara::adsfi::CommonUdpPacket            msop_buf, difop_buf;
	char supplementary_buf[SUPPLE_BUFFER_SIZE];

	double sin_table[CULCULATE_TABLE_SIZE];
	double cos_table[CULCULATE_TABLE_SIZE];


	avos::SocketConfig current_socket_config;

};


}
}







#endif  // __AVOS_DRIVER_RS_BP_DECODER_H__

