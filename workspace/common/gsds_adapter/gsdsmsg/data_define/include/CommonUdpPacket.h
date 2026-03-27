#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
/*
一帧UDP数据
*/

typedef struct CommonUdpPacket{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头信息
	std::vector<unsigned char>	udp_packet;	//点云数据内容

	CommonUdpPacket(){
		key_hash_list.resize(6);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1393712646));
		key_hash_list.push_back(msghandle::type::fix_int32(156343802));
		Clear();
	}

void Clear(){
	header={};
	udp_packet={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,udp_packet);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","udp_packet");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,udp_packet);
} CommonUdpPacket;
REGISTER(CommonUdpPacket,CommonUdpPacket_Func);
