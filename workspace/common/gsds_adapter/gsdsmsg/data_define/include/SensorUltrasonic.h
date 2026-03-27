#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "CommonPoint3i.h"
/*
超声波传感器数据协议定义
*/

typedef struct SensorUltrasonic{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	int	max_prob_num;	//超声波探头最大数量
	std::vector<CommonPoint3i>	probs;	//超声波探头数据(x: 探头id； y:探头检测距离,0无检测，非零检出距离，单位毫米；z:探头类型，预留

	SensorUltrasonic(){
		key_hash_list.resize(7);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1385300467));
		key_hash_list.push_back(msghandle::type::fix_int32(76769371));
		Clear();
	}

void Clear(){
	header={};
	max_prob_num={};
	probs={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,max_prob_num,probs);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","max_prob_num","probs");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,max_prob_num,probs);
} SensorUltrasonic;
REGISTER(SensorUltrasonic,SensorUltrasonic_Func);
