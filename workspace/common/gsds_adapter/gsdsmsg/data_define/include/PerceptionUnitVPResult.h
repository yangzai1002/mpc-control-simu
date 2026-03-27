#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionDetectionResult.h"
#include "PerceptionSegmentResult.h"
#include "PerceptionLaneInstResult.h"
#include "PerceptionInstSegmentResult.h"
#include "PerceptionWheelResult.h"
#include "PerceptionWheelPointsResult.h"
#include "PerceptionVehicleHTResult.h"
/*
感知融合最终输出的目标topic内容
*/

typedef struct PerceptionUnitVPResult{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	model_version;	//default_description
	PerceptionDetectionResult	detectstrut;	//目标检测结果
	PerceptionSegmentResult	segmentstrut;	//语义分割输出结果
	PerceptionSegmentResult	lanesegstrut;	//车道线语义分割输出结果
	PerceptionLaneInstResult	laneInst;	//车道线实例输出结果
	PerceptionInstSegmentResult	instSegStrut;	//实例分割输出结果
	PerceptionWheelResult	wheelsStrut;	//车轮2D检测输出结果
	PerceptionWheelPointsResult	wheelPointStrut;	//车轮接地点2D检测输出结果
	PerceptionVehicleHTResult	vehicleHeadTailsStrut;	//实例分割输出结果

	PerceptionUnitVPResult(){
		key_hash_list.resize(13);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(59556486));
		key_hash_list.push_back(msghandle::type::fix_int32(396068663));
		key_hash_list.push_back(msghandle::type::fix_int32(900983999));
		key_hash_list.push_back(msghandle::type::fix_int32(1024249763));
		key_hash_list.push_back(msghandle::type::fix_int32(995214776));
		key_hash_list.push_back(msghandle::type::fix_int32(971903103));
		key_hash_list.push_back(msghandle::type::fix_int32(1803143056));
		key_hash_list.push_back(msghandle::type::fix_int32(1034423142));
		key_hash_list.push_back(msghandle::type::fix_int32(391521490));
		Clear();
	}

void Clear(){
	model_version={};
	detectstrut={};
	segmentstrut={};
	lanesegstrut={};
	laneInst={};
	instSegStrut={};
	wheelsStrut={};
	wheelPointStrut={};
	vehicleHeadTailsStrut={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,model_version,detectstrut,segmentstrut,lanesegstrut,laneInst,instSegStrut,wheelsStrut,wheelPointStrut,vehicleHeadTailsStrut);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","model_version","detectstrut","segmentstrut","lanesegstrut","laneInst","instSegStrut","wheelsStrut","wheelPointStrut","vehicleHeadTailsStrut");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,model_version,detectstrut,segmentstrut,lanesegstrut,laneInst,instSegStrut,wheelsStrut,wheelPointStrut,vehicleHeadTailsStrut);
} PerceptionUnitVPResult;
REGISTER(PerceptionUnitVPResult,PerceptionUnitVPResult_Func);
