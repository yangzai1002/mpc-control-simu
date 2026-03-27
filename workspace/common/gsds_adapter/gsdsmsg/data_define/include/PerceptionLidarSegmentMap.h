#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint2d.h"
/*
感知激光模型分割可通行区域，输出结果以图像栅格形式输出，一个像素表示多少米
*/

typedef struct PerceptionLidarSegmentMap{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//功能是否打开，数据是否有效
	double	trigger_stamp;	//数据触发或数据同步时间
	double	receive_stamp;	//接收数据时间
	double	send_stamp;	//数据发送时间
	std::vector<unsigned char>	grids;	//激光模型分割栅格结果
	CommonPoint2d	grid_size;	//分割结果用图像表示，图像的长宽
	CommonPoint2d	voxel_size;	//栅格大小
	CommonPoint2d	range_min;	//自车坐标系下，栅格图起始点坐标

	PerceptionLidarSegmentMap(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1265093007));
		key_hash_list.push_back(msghandle::type::fix_int32(1696360563));
		key_hash_list.push_back(msghandle::type::fix_int32(2120142958));
		key_hash_list.push_back(msghandle::type::fix_int32(712226955));
		key_hash_list.push_back(msghandle::type::fix_int32(229882144));
		key_hash_list.push_back(msghandle::type::fix_int32(1157228747));
		key_hash_list.push_back(msghandle::type::fix_int32(920372028));
		key_hash_list.push_back(msghandle::type::fix_int32(584519242));
		Clear();
	}

void Clear(){
	enable={};
	trigger_stamp={};
	receive_stamp={};
	send_stamp={};
	grids={};
	grid_size={};
	voxel_size={};
	range_min={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,trigger_stamp,receive_stamp,send_stamp,grids,grid_size,voxel_size,range_min);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","trigger_stamp","receive_stamp","send_stamp","grids","grid_size","voxel_size","range_min");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,trigger_stamp,receive_stamp,send_stamp,grids,grid_size,voxel_size,range_min);
} PerceptionLidarSegmentMap;
REGISTER(PerceptionLidarSegmentMap,PerceptionLidarSegmentMap_Func);
