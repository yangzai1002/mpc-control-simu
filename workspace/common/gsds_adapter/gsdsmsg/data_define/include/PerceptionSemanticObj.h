#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionLWH.h"
#include "PerceptionSemanticCell.h"
/*
语义地图小目标object
*/

typedef struct PerceptionSemanticObj{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	float	x;	//目标x形心坐标；弹坑等特殊目标为形心或其他商定位置
	float	y;	//目标y形心坐标；弹坑等特殊目标为形心或其他商定位置
	float	z;	//目标z形心坐标；弹坑等特殊目标为形心或其他商定位置
	unsigned char	type;	//栅格种类 （ >=140可通行 < 140不可通行）
	PerceptionLWH	obj_size;	//目标三维尺寸，长宽高
	float	heading;	//目标朝向，普通目标默认为0，特殊目标才负值，如壕沟等有朝向的目标
	float	speed;	//速度
	std::vector<PerceptionSemanticCell>	cells;	//包含的实际栅格
	int	plan_risk_level;	//规控输出，0: 无风险，1：低，2：中，3：高（AEB）

	PerceptionSemanticObj(){
		key_hash_list.resize(13);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1127113726));
		key_hash_list.push_back(msghandle::type::fix_int32(1071085074));
		key_hash_list.push_back(msghandle::type::fix_int32(1015056422));
		key_hash_list.push_back(msghandle::type::fix_int32(110152711));
		key_hash_list.push_back(msghandle::type::fix_int32(1766504806));
		key_hash_list.push_back(msghandle::type::fix_int32(2063039426));
		key_hash_list.push_back(msghandle::type::fix_int32(515640322));
		key_hash_list.push_back(msghandle::type::fix_int32(1091690360));
		key_hash_list.push_back(msghandle::type::fix_int32(1586299318));
		Clear();
	}

void Clear(){
	x={};
	y={};
	z={};
	type={};
	obj_size={};
	heading={};
	speed={};
	cells={};
	plan_risk_level={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,x,y,z,type,obj_size,heading,speed,cells,plan_risk_level);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","x","y","z","type","obj_size","heading","speed","cells","plan_risk_level");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,x,y,z,type,obj_size,heading,speed,cells,plan_risk_level);
} PerceptionSemanticObj;
REGISTER(PerceptionSemanticObj,PerceptionSemanticObj_Func);
