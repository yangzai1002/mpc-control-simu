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
	void SetDifopConfig(const avos::SocketConfig &socket_config);
	int Init(const DecoderConfig &config) ;
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
	void ConvertHuaweiPacktsToPoint(const ara::adsfi::CommonUdpPacket &packet, ara::adsfi::MsgLidarFrame &point_cloud, double header_ts);

	int GetSupplementaryData(ara::adsfi::CommonUdpPacket &packets_ptr);
	int SetSupplementaryData(const ara::adsfi::SensorLidarPacket &packets_ptr, int debug=0);

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

	ara::adsfi::MsgPointXYZIRT           point;
	ara::adsfi::CommonUdpPacket            msop_buf, difop_buf;
	char supplementary_buf[SUPPLE_BUFFER_SIZE];
	// LockQueue<ara::adsfi::CommonUdpPacket> udp_recv_msop_queue;
	// LockQueue<ara::adsfi::CommonUdpPacket> udp_recv_difop_queue;

	double sin_table[CULCULATE_TABLE_SIZE];
	double cos_table[CULCULATE_TABLE_SIZE];


	avos::SocketConfig current_socket_config;
public:
	DecoderConfig decoder_config;

};


}
}







#endif  // __AVOS_DRIVER_RS_BP_DECODER_H__

