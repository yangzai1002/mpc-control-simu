#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
定位状态信息组合
*/

typedef struct LocNavStatus{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	utc_time;	//utc 时间
	char	gps_status;	//gps 时间
	int	pos_status;	//位置状态
	int	att_status;	//default_description
	int	init_status;	//初始化状态
	int	sate_num;	//卫星数
	float	hdop;	//default_description
	float	pdop;	//default_description
	float	diff_age;	//差分期龄

	LocNavStatus(){
		key_hash_list.resize(13);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1803306908));
		key_hash_list.push_back(msghandle::type::fix_int32(1637947088));
		key_hash_list.push_back(msghandle::type::fix_int32(1550986220));
		key_hash_list.push_back(msghandle::type::fix_int32(472938781));
		key_hash_list.push_back(msghandle::type::fix_int32(1868031386));
		key_hash_list.push_back(msghandle::type::fix_int32(266155426));
		key_hash_list.push_back(msghandle::type::fix_int32(1770084505));
		key_hash_list.push_back(msghandle::type::fix_int32(2007868017));
		key_hash_list.push_back(msghandle::type::fix_int32(285248072));
		Clear();
	}

void Clear(){
	utc_time={};
	gps_status={};
	pos_status={};
	att_status={};
	init_status={};
	sate_num={};
	hdop={};
	pdop={};
	diff_age={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,utc_time,gps_status,pos_status,att_status,init_status,sate_num,hdop,pdop,diff_age);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","utc_time","gps_status","pos_status","att_status","init_status","sate_num","hdop","pdop","diff_age");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,utc_time,gps_status,pos_status,att_status,init_status,sate_num,hdop,pdop,diff_age);
} LocNavStatus;
REGISTER(LocNavStatus,LocNavStatus_Func);
