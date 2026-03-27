#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "CommonSensorCalibration.h"
#include "CommonUdpPacket.h"
/*
毫米波原始Udp数据
*/

typedef struct SensorRadarPacket{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头
	CommonSensorCalibration	calibration;	//标定信息
	std::vector<CommonUdpPacket>	msop_packet;	//点云信息
	CommonUdpPacket	difop_packet;	//设备信息

	SensorRadarPacket(){
		key_hash_list.resize(8);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1462902908));
		key_hash_list.push_back(msghandle::type::fix_int32(1622521741));
		key_hash_list.push_back(msghandle::type::fix_int32(1695467671));
		key_hash_list.push_back(msghandle::type::fix_int32(1791436529));
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
} SensorRadarPacket;
REGISTER(SensorRadarPacket,SensorRadarPacket_Func);
