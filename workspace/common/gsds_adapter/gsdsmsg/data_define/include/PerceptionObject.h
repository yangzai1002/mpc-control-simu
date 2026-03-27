#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionRelInfo.h"
#include "PerceptionLWH.h"
#include "PerceptionAbsInfo.h"
#include "PerceptionObjImgInfo.h"
/*
感知输出的障碍物目标信息，对外接口
*/

typedef struct PerceptionObject{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//目标id,唯一识别号
	int	type;	//目标种类
	int	age;	//目标存在周期数（frame count）
	unsigned char	fusion_source;	//融合状态 0:未知 1:视觉目标检测 2:激光目标检测 3:毫米波 4:视觉分割 5:视觉+毫米波 6:视觉+激光 7:毫米波+激光 8:视觉+毫米波+激光
	unsigned char	motion_state;	//目标运动状态, 0:未知 1:静止 2:慢速移动 3:运动
	PerceptionRelInfo	vcs_info;	//目标相对坐标 x y z heading vx vy cells ploygen等
        PerceptionObjImgInfo    img_info;
	PerceptionLWH	obj_size;	//目标尺寸 L W H
	PerceptionAbsInfo	dr_info;	//目标dr坐标信息 xg yg heading vxg vyg speed
	PerceptionAbsInfo	utm_info;	//目标utm坐标信息 xg yg heading vxg vyg speed
	unsigned char	carlight;	//目标车灯状态 最低位(&0x01)：0，无效，1，有效; 其他高位代表对应灯状态，1为亮: 左转向灯(&0x02) 右转向灯(&0x04) 刹车灯(&0x08) 示廓灯(&0x10)
	std::vector<unsigned char>	reserved;	//预留，供客户使用
	int	plan_risk_level;	//规控输出，0：无风险，1：低，2：中，3：高

	PerceptionObject(){
		Clear();
	}

void Clear(){
	key_hash_list.clear();
	id={};
	type={};
	age={};
	fusion_source={};
	motion_state={};
	vcs_info={};
        img_info = {};	
        obj_size={};
	dr_info={};
	utm_info={};
	carlight={};
	reserved={};
	plan_risk_level={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,type,age,fusion_source,motion_state,vcs_info,obj_size,dr_info,utm_info,carlight,reserved,plan_risk_level);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","type","age","fusion_source","motion_state","vcs_info","img_info","obj_size","dr_info","utm_info","carlight","reserved","plan_risk_level");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,type,age,fusion_source,motion_state,vcs_info,img_info,obj_size,dr_info,utm_info,carlight,reserved,plan_risk_level);
} PerceptionObject;
REGISTER(PerceptionObject,PerceptionObject_Func);
