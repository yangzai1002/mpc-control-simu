#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "VehicleLatControl.h"
#include "VehicleLonControl.h"
#include "LoncontrolDebugStruct.h"
#include "LatcontrolDebugStruct.h"
/*
车辆控制信息
*/

typedef struct VehicleActControl{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	VehicleLatControl	lat_control;	//横向控制
	VehicleLonControl	lon_control;	//纵向控制
	LoncontrolDebugStruct	lon_control_debug;	//车辆纵向控制Debug信息
	LatcontrolDebugStruct	lat_control_debug;	//车辆横向控制Debug信息

	VehicleActControl(){
		key_hash_list.resize(9);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1248715189));
		key_hash_list.push_back(msghandle::type::fix_int32(1531890399));
		key_hash_list.push_back(msghandle::type::fix_int32(1920004563));
		key_hash_list.push_back(msghandle::type::fix_int32(1385040237));
		Clear();
	}

void Clear(){
	header={};
	lat_control={};
	lon_control={};
	lon_control_debug={};
	lat_control_debug={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,lat_control,lon_control,lon_control_debug,lat_control_debug);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","lat_control","lon_control","lon_control_debug","lat_control_debug");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,lat_control,lon_control,lon_control_debug,lat_control_debug);
} VehicleActControl;
REGISTER(VehicleActControl,VehicleActControl_Func);
