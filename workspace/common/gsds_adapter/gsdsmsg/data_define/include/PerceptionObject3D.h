#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionObject3D{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//默认值false
	float	x;	//目标在3d坐标系下的中心点 (x,y,z)坐标x，默认值0
	float	y;	//目标在3d坐标系下的中心点 (x,y,z)坐标y，默认值0
	float	z;	//目标在3d坐标系下的中心点 (x,y,z)坐标z，默认值0
	float	width;	//目标的物理尺寸，默认值0
	float	height;	//目标的物理尺寸，默认值0
	float	length;	//目标的物理尺寸，默认值0
	float	protate;	//目标的在3D坐标下的航向角度，默认值0
	float	cx_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1
	float	cy_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1
	float	cz_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1
	float	width_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1
	float	height_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1
	float	length_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1
	float	protate_score;	//以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能，默认值-1

	PerceptionObject3D(){
		key_hash_list.resize(19);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(7539101));
		key_hash_list.push_back(msghandle::type::fix_int32(12194192));
		key_hash_list.push_back(msghandle::type::fix_int32(1532589402));
		key_hash_list.push_back(msghandle::type::fix_int32(905500964));
		key_hash_list.push_back(msghandle::type::fix_int32(2104501929));
		key_hash_list.push_back(msghandle::type::fix_int32(971768464));
		key_hash_list.push_back(msghandle::type::fix_int32(362279977));
		key_hash_list.push_back(msghandle::type::fix_int32(1330384812));
		key_hash_list.push_back(msghandle::type::fix_int32(1990409665));
		key_hash_list.push_back(msghandle::type::fix_int32(498017292));
		key_hash_list.push_back(msghandle::type::fix_int32(1153108567));
		key_hash_list.push_back(msghandle::type::fix_int32(330947336));
		key_hash_list.push_back(msghandle::type::fix_int32(1268188335));
		key_hash_list.push_back(msghandle::type::fix_int32(2117535880));
		key_hash_list.push_back(msghandle::type::fix_int32(916445467));
		Clear();
	}

void Clear(){
	enable={};
	x={};
	y={};
	z={};
	width={};
	height={};
	length={};
	protate={};
	cx_score={};
	cy_score={};
	cz_score={};
	width_score={};
	height_score={};
	length_score={};
	protate_score={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,x,y,z,width,height,length,protate,cx_score,cy_score,cz_score,width_score,height_score,length_score,protate_score);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","x","y","z","width","height","length","protate","cx_score","cy_score","cz_score","width_score","height_score","length_score","protate_score");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,x,y,z,width,height,length,protate,cx_score,cy_score,cz_score,width_score,height_score,length_score,protate_score);
} PerceptionObject3D;
REGISTER(PerceptionObject3D,PerceptionObject3D_Func);
