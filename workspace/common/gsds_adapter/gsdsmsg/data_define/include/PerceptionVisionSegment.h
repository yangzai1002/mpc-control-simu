#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "SensorImage.h"
/*
单个相机感知视觉输出
*/

typedef struct PerceptionVisionSegment{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	camera_width;	//相机原图的分辨率
	int	camera_height;	//相机原图的分辨率
	bool	resa_valid;	//车道线分割和杆有效性，默认给false
	std::vector<unsigned int>	resa_roi;	//车道线ROI区域
	SensorImage	resa;	//车道线和杆分割mask
	bool	seg_valid;	//语义分割有效性，默认给false
	std::vector<unsigned int>	seg_roi;	//语义分割ROI区域
	SensorImage	seg;	//语义分割mask
	SensorImage	resacrop;	//车道线和杆分割mask crop
	SensorImage	segcrop;	//语义分割mask crop

	PerceptionVisionSegment(){
		key_hash_list.resize(14);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(62241135));
		key_hash_list.push_back(msghandle::type::fix_int32(1591810720));
		key_hash_list.push_back(msghandle::type::fix_int32(925449649));
		key_hash_list.push_back(msghandle::type::fix_int32(684433224));
		key_hash_list.push_back(msghandle::type::fix_int32(885351833));
		key_hash_list.push_back(msghandle::type::fix_int32(751865661));
		key_hash_list.push_back(msghandle::type::fix_int32(918584063));
		key_hash_list.push_back(msghandle::type::fix_int32(18873643));
		key_hash_list.push_back(msghandle::type::fix_int32(873531687));
		key_hash_list.push_back(msghandle::type::fix_int32(2143986173));
		Clear();
	}

void Clear(){
	camera_width={};
	camera_height={};
	resa_valid={};
	resa_roi={};
	resa={};
	seg_valid={};
	seg_roi={};
	seg={};
	resacrop={};
	segcrop={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,camera_width,camera_height,resa_valid,resa_roi,resa,seg_valid,seg_roi,seg,resacrop,segcrop);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","camera_width","camera_height","resa_valid","resa_roi","resa","seg_valid","seg_roi","seg","resacrop","segcrop");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,camera_width,camera_height,resa_valid,resa_roi,resa,seg_valid,seg_roi,seg,resacrop,segcrop);
} PerceptionVisionSegment;
REGISTER(PerceptionVisionSegment,PerceptionVisionSegment_Func);
