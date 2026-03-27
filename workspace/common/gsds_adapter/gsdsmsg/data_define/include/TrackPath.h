#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "TrackPoint.h"
/*

*/

typedef struct TrackPath{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//default_description
	int	status;	//业务路径状态：0--空闲/结束  1--执行  2--暂停
	int	avoid;	//避障属性：0--停障  1--避障  2--越障
	double	speed;	//速度属性（km/h）
	std::vector<TrackPoint>	points;	//路点信息

	TrackPath(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(615898035));
		key_hash_list.push_back(msghandle::type::fix_int32(1521954213));
		key_hash_list.push_back(msghandle::type::fix_int32(1823992829));
		key_hash_list.push_back(msghandle::type::fix_int32(2104120274));
		key_hash_list.push_back(msghandle::type::fix_int32(1043489130));
		Clear();
	}

void Clear(){
	header={};
	status={};
	avoid={};
	speed={};
	points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,status,avoid,speed,points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","status","avoid","speed","points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,status,avoid,speed,points);
} TrackPath;
REGISTER(TrackPath,TrackPath_Func);
