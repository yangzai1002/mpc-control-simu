#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonPoint2d.h"
/*

*/

typedef struct PerceptionObject2D{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned int	obj_id;	//目标类型，默认0
	unsigned int	child_id;	//目标细分类型，默认0
	float	prob;	//得分置信度
	float	prob2d;	//得分置信度
	float	prob3d;	//得分置信度
	float	x;	//2D框坐标x，默认0
	float	y;	//2D框坐标y，默认0
	float	w;	//2D框宽w，默认0
	float	h;	//2D框高h，默认0
	bool	orientation_enable;	//默认false
	unsigned int	orientation_id;	//目标航向类别，默认0
	float	orientation_score;	//目标航向置信度，默认0
	bool	vrotate_enable;	//默认false
	float	vrotate;	//图像坐标系下，目标航向角度，默认0
	float	vrotate_score;	//图像坐标系下，目标航向角度置信度，默认0
	bool	key_points_enable;	//默认false
	std::vector<CommonPoint2d>	key_points;	//图像坐标系下，目标航向角度，默认0
	float	vheight;	//图像坐标系下，目标航向角度置信度，默认0
	float	occlusion;	//遮挡比例, -1表示无效，程序并未做该功能，默认-1
	float	truncation;	//截断比例, -1表示无效，程序并未做该功能，默认-1

	PerceptionObject2D(){
		key_hash_list.resize(24);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(513022840));
		key_hash_list.push_back(msghandle::type::fix_int32(370183404));
		key_hash_list.push_back(msghandle::type::fix_int32(1580392433));
		key_hash_list.push_back(msghandle::type::fix_int32(341528867));
		key_hash_list.push_back(msghandle::type::fix_int32(1906480726));
		key_hash_list.push_back(msghandle::type::fix_int32(504871703));
		key_hash_list.push_back(msghandle::type::fix_int32(1856362957));
		key_hash_list.push_back(msghandle::type::fix_int32(1763775960));
		key_hash_list.push_back(msghandle::type::fix_int32(1090009482));
		key_hash_list.push_back(msghandle::type::fix_int32(1579324837));
		key_hash_list.push_back(msghandle::type::fix_int32(337172383));
		key_hash_list.push_back(msghandle::type::fix_int32(827918264));
		key_hash_list.push_back(msghandle::type::fix_int32(1089127552));
		key_hash_list.push_back(msghandle::type::fix_int32(1501899668));
		key_hash_list.push_back(msghandle::type::fix_int32(1067722820));
		key_hash_list.push_back(msghandle::type::fix_int32(1426342198));
		key_hash_list.push_back(msghandle::type::fix_int32(2140158423));
		key_hash_list.push_back(msghandle::type::fix_int32(1239804507));
		key_hash_list.push_back(msghandle::type::fix_int32(1396774360));
		key_hash_list.push_back(msghandle::type::fix_int32(1037873804));
		Clear();
	}

void Clear(){
	obj_id={};
	child_id={};
	prob={};
	prob2d={};
	prob3d={};
	x={};
	y={};
	w={};
	h={};
	orientation_enable={};
	orientation_id={};
	orientation_score={};
	vrotate_enable={};
	vrotate={};
	vrotate_score={};
	key_points_enable={};
	key_points={};
	vheight={};
	occlusion={};
	truncation={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,obj_id,child_id,prob,prob2d,prob3d,x,y,w,h,orientation_enable,orientation_id,orientation_score,vrotate_enable,vrotate,vrotate_score,key_points_enable,key_points,vheight,occlusion,truncation);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","obj_id","child_id","prob","prob2d","prob3d","x","y","w","h","orientation_enable","orientation_id","orientation_score","vrotate_enable","vrotate","vrotate_score","key_points_enable","key_points","vheight","occlusion","truncation");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,obj_id,child_id,prob,prob2d,prob3d,x,y,w,h,orientation_enable,orientation_id,orientation_score,vrotate_enable,vrotate,vrotate_score,key_points_enable,key_points,vheight,occlusion,truncation);
} PerceptionObject2D;
REGISTER(PerceptionObject2D,PerceptionObject2D_Func);
