#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "SizeWH.h"
#include "LocThreeDimPose.h"
#include "LocThreeDimRotation.h"
/*

*/

typedef struct PerCameraInfo{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	cam_name;	//相机名称
	SizeWH	size;	//相机原分辨率
	SizeWH	input_size;	//视觉模型需要的分辨率
	SizeWH	scaled_size;	//相机去畸变后的分辨率（非GDC方案，数据平台中用）
	std::vector<double>	intriK;	//相机内参矩阵
	std::vector<double>	intriD;	//相机畸变系数
	double	epsilon;	//全相机模型校正系数，采用针孔相机时本系数为0
	LocThreeDimPose	extriT;	//相机位于车体坐标系(前左上)坐标为(x,y,z) 则本处填写[y, z, -x]
	LocThreeDimRotation	extriR_euler;	//pitch, yaw, roll  单位:度 pitch>0,点云上升；yaw>0 点云右移； roll>0 点云左升右降
	std::vector<double>	newK;	//标准相机K
	double	base_euler;	//理想坐标系相对前向基础相机的理想yaw角的偏差

	PerCameraInfo(){
		key_hash_list.resize(15);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(18345425));
		key_hash_list.push_back(msghandle::type::fix_int32(1191488462));
		key_hash_list.push_back(msghandle::type::fix_int32(541052448));
		key_hash_list.push_back(msghandle::type::fix_int32(1321971932));
		key_hash_list.push_back(msghandle::type::fix_int32(457927387));
		key_hash_list.push_back(msghandle::type::fix_int32(84018741));
		key_hash_list.push_back(msghandle::type::fix_int32(17945704));
		key_hash_list.push_back(msghandle::type::fix_int32(2078273487));
		key_hash_list.push_back(msghandle::type::fix_int32(1708696329));
		key_hash_list.push_back(msghandle::type::fix_int32(1266537426));
		key_hash_list.push_back(msghandle::type::fix_int32(2076727283));
		Clear();
	}

void Clear(){
	cam_name={};
	size={};
	input_size={};
	scaled_size={};
	intriK={};
	intriD={};
	epsilon={};
	extriT={};
	extriR_euler={};
	newK={};
	base_euler={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,cam_name,size,input_size,scaled_size,intriK,intriD,epsilon,extriT,extriR_euler,newK,base_euler);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","cam_name","size","input_size","scaled_size","intriK","intriD","epsilon","extriT","extriR_euler","newK","base_euler");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,cam_name,size,input_size,scaled_size,intriK,intriD,epsilon,extriT,extriR_euler,newK,base_euler);
} PerCameraInfo;
REGISTER(PerCameraInfo,PerCameraInfo_Func);
