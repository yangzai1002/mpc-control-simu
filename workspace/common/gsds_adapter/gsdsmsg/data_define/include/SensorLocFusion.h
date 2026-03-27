#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "LocLLH.h"
#include "LocPosition.h"
#include "LocTwist.h"
#include "Vector3.h"
#include "LocNavStatus.h"
#include "LocPoseDR.h"
/*
融合定位输出
*/

typedef struct SensorLocFusion{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//消息头
	LocLLH	llh;	//经纬高信息
	LocPosition	pose;	//位置信息
	LocTwist	twist;	//速度信息
	Vector3	acceleration;	//加速度信息
	float	v;	//合速度信息
	LocNavStatus	nav_status;	//定位状态信息
	LocPoseDR	dr;	//DR 相关信息

	SensorLocFusion(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1462902908));
		key_hash_list.push_back(msghandle::type::fix_int32(268951214));
		key_hash_list.push_back(msghandle::type::fix_int32(1949177716));
		key_hash_list.push_back(msghandle::type::fix_int32(953934499));
		key_hash_list.push_back(msghandle::type::fix_int32(665617608));
		key_hash_list.push_back(msghandle::type::fix_int32(411227680));
		key_hash_list.push_back(msghandle::type::fix_int32(563616066));
		key_hash_list.push_back(msghandle::type::fix_int32(254567530));
		Clear();
	}

void Clear(){
	header={};
	llh={};
	pose={};
	twist={};
	acceleration={};
	v={};
	nav_status={};
	dr={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,llh,pose,twist,acceleration,v,nav_status,dr);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","llh","pose","twist","acceleration","v","nav_status","dr");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,llh,pose,twist,acceleration,v,nav_status,dr);
} SensorLocFusion;
REGISTER(SensorLocFusion,SensorLocFusion_Func);
