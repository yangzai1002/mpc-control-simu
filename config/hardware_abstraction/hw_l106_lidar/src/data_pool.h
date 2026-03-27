/*****************************************************************************
*  business platform data pool header                                                 *
*  Copyright (C) 2020 xia yang  xiayang@idriverplus.com.                         *
*                                                                            *
*  This file is part of rs lidar.                                                 *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License version 3 as         *
*  published by the Free Software Foundation.                                *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with OST. If not, see <http://www.gnu.org/licenses/>.               *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*  @file     data_pool.h                                               *
*  @brief    define data pool data                                          *
*  Details.                                                                  *
*                                                                            *
*  @author   Xia. Yang                                                       *
*  @email    xiayang@idriverplus.com                                              *
*  @version  0.0.0.1                                              *
*  @date     2020-12-06                                              *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2020/07/09 | 0.0.0.1   | Xia. Yang      | Create file                     *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#ifndef __RS_LIDAR_DATA_POOL_H__
#define __RS_LIDAR_DATA_POOL_H__
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <atomic>

#include <Eigen/Dense>
#include "log/log.h"

#include "config_loader/custom_stack.h"
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_sensorlidarpacket.h"


#define SUPPLE_BUFFER_SIZE	3024
// #define UDP_BUFFER_SIZE 1021
#define UDP_BUFFER_SIZE 1025
#define CULCULATE_TABLE_SIZE 360*100
#define DRIVER_MAX_NUM_THREAD 7

namespace avos {
namespace driver {

typedef struct DecoderConfig
{
	int pre_rot_axis_0 = 0;
	int pre_rot_axis_1 = 1;
	int pre_rot_axis_2 = 2;
	double pre_rot_degree_0 = 0.0;
	double pre_rot_degree_1 = 0.0;
	double pre_rot_degree_2 = 0.0;
	double tf_x;
	double tf_y;
	double tf_z;
	double tf_roll;
	double tf_pitch;
	double tf_yaw;
	float transform[16];

	double car_modle_front_x;
	double car_modle_back_x;
	double car_modle_left_y;
	double car_modle_right_y;
	double car_modle_top_z;
	double car_modle_button_z;
	int    print_data; //print udp data
	int    decoder_debug;
	int    error_debug;
	// HWLidarDriverConfig driver_cfg;

	DecoderConfig() {
		tf_x = 0.0;
		tf_y = 0.0;
		tf_z = 0.0;
		tf_roll = 0.0;
		tf_pitch = 0.0;
		tf_yaw = 0.0;

		car_modle_front_x = 0.0;
		car_modle_back_x = 0.0;
		car_modle_left_y = 0.0;
		car_modle_right_y = 0.0;
		car_modle_top_z = 0.0;
		car_modle_button_z = 0.0;
		print_data = 0;
		decoder_debug = 0;
		error_debug = 0;
	}

	void Init() {
		Eigen::Matrix4f pre_rotation_matrix = Eigen::Matrix4f::Identity();

		Eigen::Quaternionf pre_rotation_q = Eigen::Quaternionf::Identity();

		// 1st-axis
		Eigen::Vector3f rot_axis = Eigen::Vector3f::Zero();
		rot_axis[pre_rot_axis_0] = 1;
		Eigen::AngleAxis<float> axis_rot = Eigen::AngleAxis<float>(pre_rot_degree_0 / 180.0 * M_PI,
		                                   rot_axis);
		pre_rotation_q = axis_rot * pre_rotation_q;
		// 2rd-axis
		rot_axis = Eigen::Vector3f::Zero();
		rot_axis[pre_rot_axis_1] = 1;
		axis_rot = Eigen::AngleAxis<float>(pre_rot_degree_1 / 180.0 * M_PI,
		                                   rot_axis);
		pre_rotation_q = axis_rot * pre_rotation_q;
		// 3rd-axis
		rot_axis = Eigen::Vector3f::Zero();
		rot_axis[pre_rot_axis_2] = 1;
		axis_rot = Eigen::AngleAxis<float>(pre_rot_degree_2 / 180.0 * M_PI,
		                                   rot_axis);
		pre_rotation_q = axis_rot * pre_rotation_q;

		pre_rotation_matrix.block<3, 3>(0, 0) =
		    pre_rotation_q.normalized().toRotationMatrix();

		// translate
		Eigen::Matrix4f transform_matrix = Eigen::Matrix4f::Identity();
		transform_matrix.block<3, 1>(0, 3) = Eigen::Vector3f(
		        tf_x, tf_y, tf_z);
		// euler angle
		Eigen::Matrix3f rotation_m = Eigen::Matrix3f::Identity();
		rotation_m = Eigen::AngleAxis<float>(tf_yaw / 180.0 * M_PI, Eigen::Vector3f::UnitZ())
		             * Eigen::AngleAxis<float>(tf_pitch / 180.0 * M_PI, Eigen::Vector3f::UnitY())
		             * Eigen::AngleAxis<float>(tf_roll / 180.0 * M_PI, Eigen::Vector3f::UnitX());

		transform_matrix.block<3, 3>(0, 0) = rotation_m;
		transform_matrix = transform_matrix * pre_rotation_matrix;
		//
		for (int r = 0; r < 4; ++r) {
			for (int c = 0; c < 4; ++c) {
				transform[c + r * 4] = transform_matrix(r, c);
			}
		}
	}
} DecoderConfig;

typedef struct HWLidarDriverConfig
{
	// std::string lidar_type;
	std::string native_ip;
	int udp_buf_size;
	int udp_wait_us;
	int pointcloud_port;
	int checkalive_port;
	int supplemental_port;
	int work_mode;   // 0:online_mode  1:offline_mode
	int phase_lock;
	std::string packets_topic;
	std::string pointcloud_topic;
	std::string dynamic_phase_topic;
	std::string config_name;
	std::string vehicle_param;
	bool pub_pointcloud;
	int  dynamic_phase;
	int  debug_ts;	 //bit 0 of debug_mask
	int  debug; 	 //bit 1 of debug_mask
	int  ten_ms_filter_l;
	int  ten_ms_filter_r;
	int  interval_ms;

	DecoderConfig decoder_config;

	HWLidarDriverConfig() {
		work_mode = 0;
		udp_buf_size = 0;
		// lidar_type = "";
		native_ip = "";
		udp_wait_us		= 0;
		pointcloud_port = 0;
		checkalive_port = 0;
		phase_lock = 60;
		packets_topic = "";
		pointcloud_topic = "";
		dynamic_phase_topic = "";
		config_name = "";
		vehicle_param = "";
		pub_pointcloud = true;
		dynamic_phase  = 0;
		ten_ms_filter_l = 10;
		ten_ms_filter_r = 90;
		interval_ms   	= 50;
		debug_ts		= 0;
		debug    		= 0;
	}
} HWLidarDriverConfig;


typedef struct PacketData
{
	unsigned char data[UDP_BUFFER_SIZE];
	uint16_t size;
	PacketData() {
		size = 0;
		memset(data, 0, UDP_BUFFER_SIZE);
	}

	PacketData(const PacketData & c)
	{
		memcpy(data, c.data, UDP_BUFFER_SIZE);
		size = c.size;
	}

	PacketData& operator=(const PacketData& c)
	{
		// 避免自赋值
		if (this != &c)
		{
			memcpy(data, c.data, UDP_BUFFER_SIZE);
			size = c.size;
		}
		return *this;
	}
} PacketData;

typedef struct PacketPointData
{
	ara::adsfi::MsgLidarFrame points;
} PacketPointData;

typedef struct SupplementaryData
{
	int vertical_angle[192]; // 0.00390625 du
	int vertical_angle_offset[480][4]; // 0.001  du
	int horizontal_angle_offset[192][4]; // 0.001  du

	int size()
	{
		return sizeof(SupplementaryData);
	}

} SupplementaryData;

}
}


#endif
