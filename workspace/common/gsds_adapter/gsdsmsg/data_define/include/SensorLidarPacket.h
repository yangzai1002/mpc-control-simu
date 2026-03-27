#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "CommonSensorCalibration.h"
#include "CommonUdpPacket.h"
/*
激光雷达原始数据
*/

typedef struct SensorLidarPacket{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头信息
	CommonSensorCalibration	calibration;	//标定信息
	std::vector<CommonUdpPacket>	msop_packet;	//点云数据内容
	CommonUdpPacket	difop_packet;	//激光头标定信息内容

	SensorLidarPacket(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1393712646));
		key_hash_list.push_back(msghandle::type::fix_int32(1622521741));
		key_hash_list.push_back(msghandle::type::fix_int32(76632703));
		key_hash_list.push_back(msghandle::type::fix_int32(2080078428));
		Clear();
	}

void Clear(){
	header={};
	calibration={};
	msop_packet={};
	difop_packet={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,calibration,msop_packet,difop_packet);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","calibration","msop_packet","difop_packet");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,calibration,msop_packet,difop_packet);
} SensorLidarPacket;
REGISTER(SensorLidarPacket,SensorLidarPacket_Func);
