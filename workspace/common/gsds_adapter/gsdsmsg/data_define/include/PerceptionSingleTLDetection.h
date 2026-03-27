#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "svchar.h"
/*

*/

typedef struct PerceptionSingleTLDetection{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned char	color;	//红绿灯颜色： UNKNOWN_COLOR=0, RED=1, YELLOW=2, BLACK=3
	std::string	id;	//红绿灯唯一ID标识
	unsigned char	flicker;	//红绿灯闪烁状态：NORMAL=0, FLICKER_RED=1, FLICKER_YELLOW=2, FLICKER_GREEN=3, LENGTH_BLACK=4, LENGTH_FLICKER_YELLOW=5, UNORMAL=6
	bool	occlude;	//灯盘是否被遮挡（暂时未用）
	unsigned char	number;	//倒计时数字0-9
	double	confidence;	//红绿灯灯盘识别置信度
	double	tracking_time;	//跟踪持续时长
	unsigned char	sublight_type;	//灯盘类型：UNKNOWN=1, CIRCLE=2, ARROW_LEFT=3, ARROW_FORWARD=4, ARROW_RIGHT=5, ARROW_LEFT_AND_FORWARD=6, ARROW_RIGHT_AND_FORWARD=7, ARROW_U_TURN=8（有问题，暂时别用该字段）
	std::vector<svchar>	light_cycle;	//红绿灯周期【绿，黄，红】
	bool	using_obu;	//是否使用obu（定义不明确，暂时别用）

	PerceptionSingleTLDetection(){
		key_hash_list.resize(14);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(699328420));
		key_hash_list.push_back(msghandle::type::fix_int32(1459599047));
		key_hash_list.push_back(msghandle::type::fix_int32(709295138));
		key_hash_list.push_back(msghandle::type::fix_int32(1826524179));
		key_hash_list.push_back(msghandle::type::fix_int32(356504717));
		key_hash_list.push_back(msghandle::type::fix_int32(1989995293));
		key_hash_list.push_back(msghandle::type::fix_int32(964175536));
		key_hash_list.push_back(msghandle::type::fix_int32(252361317));
		key_hash_list.push_back(msghandle::type::fix_int32(1158567215));
		key_hash_list.push_back(msghandle::type::fix_int32(2087441648));
		Clear();
	}

void Clear(){
	color={};
	id={};
	flicker={};
	occlude={};
	number={};
	confidence={};
	tracking_time={};
	sublight_type={};
	light_cycle={};
	using_obu={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,color,id,flicker,occlude,number,confidence,tracking_time,sublight_type,light_cycle,using_obu);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","color","id","flicker","occlude","number","confidence","tracking_time","sublight_type","light_cycle","using_obu");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,color,id,flicker,occlude,number,confidence,tracking_time,sublight_type,light_cycle,using_obu);
} PerceptionSingleTLDetection;
REGISTER(PerceptionSingleTLDetection,PerceptionSingleTLDetection_Func);
