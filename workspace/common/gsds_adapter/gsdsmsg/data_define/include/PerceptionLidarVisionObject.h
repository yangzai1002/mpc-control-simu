#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionRect.h"
#include "PerceptionOccupyCell.h"
#include "PerceptionObjPointLabels.h"
/*
匹配语义分割信息的点云聚类目标输出结构体
*/

typedef struct PerceptionLidarVisionObject{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//目标ID
	int	age;	//跟踪帧数
	int	guess_age;	//丢失帧数
	float	x;	//vcs坐标，kal滤波后
	float	y;	//vcs坐标，kal滤波后
	float	z;	//vcs坐标，kal滤波后
	float	x_det;	//vcs坐标，原始坐标
	float	y_det;	//vcs坐标，原始坐标
	float	z_det;	//vcs坐标，原始坐标
	float	vxabs;	//绝对速度
	float	vyabs;	//绝对速度
	float	vzabs;	//绝对速度
	float	width;	//目标宽度
	float	length;	//目标长度
	float	height;	//目标高度
	float	heading_vcs;	//vcs坐标朝向
	float	heading_dr;	//dr坐标朝向
	float	confi;	//目标置信度
	int	type;	//目标类别
	PerceptionRect	box2d;	//图像上坐标
	int	box2d_in_cam_index;	//相机序号
	std::vector<PerceptionOccupyCell>	cells;	//轮廓点
	PerceptionObjPointLabels	plabel;	//目标上点的类别统计
	int	filter_method;	//目标过滤规则
	bool	is_noise;	//是否噪声目标
	int	cam_trk_id;	//匹配上的视觉id
	int	cam_trk_age;	//匹配上的视觉age
	PerceptionRect	cam_trk_box2d;	//匹配上的视觉2d框
	float	vx_ground;	//相对自车方向的绝对速度
	float	vy_ground;	//相对自车方向的绝对速度

	PerceptionLidarVisionObject(){
		key_hash_list.resize(34);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(573956168));
		key_hash_list.push_back(msghandle::type::fix_int32(519961177));
		key_hash_list.push_back(msghandle::type::fix_int32(1278656951));
		key_hash_list.push_back(msghandle::type::fix_int32(502979683));
		key_hash_list.push_back(msghandle::type::fix_int32(282534006));
		key_hash_list.push_back(msghandle::type::fix_int32(62088329));
		key_hash_list.push_back(msghandle::type::fix_int32(514187018));
		key_hash_list.push_back(msghandle::type::fix_int32(1345579789));
		key_hash_list.push_back(msghandle::type::fix_int32(29488912));
		key_hash_list.push_back(msghandle::type::fix_int32(1950516103));
		key_hash_list.push_back(msghandle::type::fix_int32(1470672152));
		key_hash_list.push_back(msghandle::type::fix_int32(990828201));
		key_hash_list.push_back(msghandle::type::fix_int32(1526119928));
		key_hash_list.push_back(msghandle::type::fix_int32(596223299));
		key_hash_list.push_back(msghandle::type::fix_int32(1325858381));
		key_hash_list.push_back(msghandle::type::fix_int32(980872861));
		key_hash_list.push_back(msghandle::type::fix_int32(1038899053));
		key_hash_list.push_back(msghandle::type::fix_int32(459780247));
		key_hash_list.push_back(msghandle::type::fix_int32(1166442313));
		key_hash_list.push_back(msghandle::type::fix_int32(1322586512));
		key_hash_list.push_back(msghandle::type::fix_int32(209232180));
		key_hash_list.push_back(msghandle::type::fix_int32(2023185611));
		key_hash_list.push_back(msghandle::type::fix_int32(1596638254));
		key_hash_list.push_back(msghandle::type::fix_int32(641686620));
		key_hash_list.push_back(msghandle::type::fix_int32(2028894860));
		key_hash_list.push_back(msghandle::type::fix_int32(2025133780));
		key_hash_list.push_back(msghandle::type::fix_int32(260076184));
		key_hash_list.push_back(msghandle::type::fix_int32(81281320));
		key_hash_list.push_back(msghandle::type::fix_int32(554782772));
		key_hash_list.push_back(msghandle::type::fix_int32(362026503));
		Clear();
	}

void Clear(){
	id={};
	age={};
	guess_age={};
	x={};
	y={};
	z={};
	x_det={};
	y_det={};
	z_det={};
	vxabs={};
	vyabs={};
	vzabs={};
	width={};
	length={};
	height={};
	heading_vcs={};
	heading_dr={};
	confi={};
	type={};
	box2d={};
	box2d_in_cam_index={};
	cells={};
	plabel={};
	filter_method={};
	is_noise={};
	cam_trk_id={};
	cam_trk_age={};
	cam_trk_box2d={};
	vx_ground={};
	vy_ground={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,age,guess_age,x,y,z,x_det,y_det,z_det,vxabs,vyabs,vzabs,width,length,height,heading_vcs,heading_dr,confi,type,box2d,box2d_in_cam_index,cells,plabel,filter_method,is_noise,cam_trk_id,cam_trk_age,cam_trk_box2d,vx_ground,vy_ground);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","age","guess_age","x","y","z","x_det","y_det","z_det","vxabs","vyabs","vzabs","width","length","height","heading_vcs","heading_dr","confi","type","box2d","box2d_in_cam_index","cells","plabel","filter_method","is_noise","cam_trk_id","cam_trk_age","cam_trk_box2d","vx_ground","vy_ground");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,age,guess_age,x,y,z,x_det,y_det,z_det,vxabs,vyabs,vzabs,width,length,height,heading_vcs,heading_dr,confi,type,box2d,box2d_in_cam_index,cells,plabel,filter_method,is_noise,cam_trk_id,cam_trk_age,cam_trk_box2d,vx_ground,vy_ground);
} PerceptionLidarVisionObject;
REGISTER(PerceptionLidarVisionObject,PerceptionLidarVisionObject_Func);
