#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionLinePart.h"
#include "CommonPoint3f.h"
/*
感知车道线，单个车道线
*/

typedef struct PerceptionLaneline{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	valid;	//有效标识
	int	trk_id;	//车道线跟踪编号
	unsigned char	line_type;	//车道线类型 Unknown=0;LaneLine=1;Curb=2;Center=3;Guardrail=4;ConcreteBarrier=5;Fence=6;Wall=7;Canopy=8;Virtual=9;Cone=10
	int	camera_source;	//相机来源编号
	int	line_location;	//车道线相对位置 LEFT=0;LEFTL=1;LEFTLL=2;RIGHT=3;RIGHTR=4;RIGHTRR=5;LEFTOUTSIDE=6;RIGHTOUTSIDE=7;UNKNOWN=8
	std::vector<PerceptionLinePart>	segment_lane_lines;	//分段拟合车道线
	std::vector<CommonPoint3f>	img_points;	//车道线在相机图像中的采样点x->u y->v;z->unvalid

	PerceptionLaneline(){
		key_hash_list.resize(11);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(375488606));
		key_hash_list.push_back(msghandle::type::fix_int32(725714956));
		key_hash_list.push_back(msghandle::type::fix_int32(1899676699));
		key_hash_list.push_back(msghandle::type::fix_int32(866957248));
		key_hash_list.push_back(msghandle::type::fix_int32(924109183));
		key_hash_list.push_back(msghandle::type::fix_int32(641559757));
		key_hash_list.push_back(msghandle::type::fix_int32(829360394));
		Clear();
	}

void Clear(){
	valid={};
	trk_id={};
	line_type={};
	camera_source={};
	line_location={};
	segment_lane_lines={};
	img_points={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,valid,trk_id,line_type,camera_source,line_location,segment_lane_lines,img_points);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","valid","trk_id","line_type","camera_source","line_location","segment_lane_lines","img_points");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,valid,trk_id,line_type,camera_source,line_location,segment_lane_lines,img_points);
} PerceptionLaneline;
REGISTER(PerceptionLaneline,PerceptionLaneline_Func);
