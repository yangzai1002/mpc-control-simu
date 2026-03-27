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
#ifndef __RS_RADAR_DATA_POOL_H__
#define __RS_RADAR_DATA_POOL_H__
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
#include "ara/adsfi/impl_type_sensorradarpointcloud.h"
#include "ara/adsfi/impl_type_sensorradarpacket.h"


#define UDP_BUFFER_SIZE 1500
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
	double wheel_base_length;
	double reduce_ratio;
	int    debug;
	int    filter_obj;
	// HWRadarDriverConfig driver_cfg;

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

		wheel_base_length = 0.0;
	    reduce_ratio = 0.0;

		debug = 0;
		filter_obj = 0;
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

typedef struct HWRadarDriverConfig
{
	std::string radar_name;
	std::string native_ip;
	std::string target_ip;
	int udp_buf_size;
	int data_port;
	int heart_port;
	int vehicle_port;
	std::string config_name;
	std::string vehicle_param;
	int  debug;
	int  interval_ms;

	DecoderConfig decoder_config;

	HWRadarDriverConfig() {
		radar_name = "";
		udp_buf_size = 0;
		native_ip = "";
		target_ip = "";
		data_port = 0;
		heart_port = 0;
		vehicle_port = 0;
		config_name = "";
		vehicle_param = "";
		interval_ms   	= 50;
		debug    		= 0;
	}
} HWRadarDriverConfig;


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
	ara::adsfi::SensorRadarPointCloud points;
} PacketPointData;

typedef struct SupplementaryData
{
	uint16_t vertical_angle[192]; // 0.00390625 du
    uint16_t vertical_angle_offset[480][4]; // 0.001  du
    uint16_t horizontal_angle_offset[192][4]; // 0.001  du

	int size()
	{
		return sizeof(SupplementaryData);
	}

}SupplementaryData;

struct VehicleInfo {
  int16_t veh_spd_dir = 0; 	  // 车辆运动方向  0:静止 1:向前运动  2:向后运动
	double  yawrate = 0.0;       // 横摆角速度(逆时针为正,顺时针为负) 单位:deg/s。old:rad/s, left +, right -
	double  vehSpd = 0.0;      // 车速 m/s
	double  longitudeAcc; // 纵向加速度(前正后负), 单位:m/s2
	double  lateralAcc; // 横向加速度(左正右负)
	double  steer_angle = 0;    // 方向盘转角(逆时针为正,顺时针为负),单位:deg。old:rad
	double  steeringVelocity; // 方向盘转角速度(逆时针为正,顺时针为负) 单位:deg/s
	int16_t gear  = 0; /*0x0-fail
						0x1-P
						0x2-N
						0x3-R
						0x4-Gear 1
						0x5-Gear 2
						0x6-Gear 3
						0x7-Gear 4
						0x8-Gear 5
						0x9-Gear 6
						0xA-Gear 7
						0xB-Gear 8
						0xC-Gear 9
						0xD-Gear 10
						0xF-not defined */
};

}
}


#endif
