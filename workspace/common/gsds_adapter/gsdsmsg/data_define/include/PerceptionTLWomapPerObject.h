#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionTLWomapBbox.h"
/*
一个交通灯的颜色、倒计时、箭头等信息
*/

typedef struct PerceptionTLWomapPerObject{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	id;	//灯的id，暂时未赋值
	bool	valid;	//检出的框基本都是true
	PerceptionTLWomapBbox	detObject;	//交通灯灯板框Bbox坐标
	bool	occlude;	//是否遮挡，未启用
	unsigned short	color;	//0：unknown；1：red；2：yellow；3：green；4：black
	float	color_score;	//颜色分类得分
	PerceptionTLWomapBbox	numberObject;	//当前灯的倒计时灯牌Bbox，仅hasNumber为true时有效
	bool	hasNumber;	//为true说明当前交通灯有倒计时灯牌
	unsigned int	number;	//当前倒计时数字
	float	number_score;	//倒计时模型输出score，仅hasNumber为true时有效
	unsigned short	lightType;	//灯盘类型。HORIZONTAL：0，VERTICAL：1，QUADRATE：2，UNKNOWN：3
	unsigned short	arrow;	//灯盘箭头属性。CM_ARROW_UNKNOWN：0，CM_ARROW_FRONT：1，CM_ARROW_LEFT：2，CM_ARROW_RIGHT：3，CM_ARROW_U：4，CM_ARROW_CIRCLE：5，CM_ARROW_RIDER：6，CM_ARROW_LEFT_AND_FORWARD：7,CM_ARROW_RIGHT_AND_FORWARD:8
	float	arrow_score;	//灯盘箭头属性分类置信度
	float	distance_car2light;	//暂时未启用
	unsigned short	numberType;	//NUMBER_TYPE_UNKNOWN,NUMBER_TYPE_UNION,NUMBER_TYPE_SINGLE

	PerceptionTLWomapPerObject(){
		key_hash_list.resize(19);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(335070803));
		key_hash_list.push_back(msghandle::type::fix_int32(1291618249));
		key_hash_list.push_back(msghandle::type::fix_int32(1864650510));
		key_hash_list.push_back(msghandle::type::fix_int32(1963339082));
		key_hash_list.push_back(msghandle::type::fix_int32(1999907616));
		key_hash_list.push_back(msghandle::type::fix_int32(414565115));
		key_hash_list.push_back(msghandle::type::fix_int32(1309166820));
		key_hash_list.push_back(msghandle::type::fix_int32(121576113));
		key_hash_list.push_back(msghandle::type::fix_int32(808003268));
		key_hash_list.push_back(msghandle::type::fix_int32(1922961134));
		key_hash_list.push_back(msghandle::type::fix_int32(1795173754));
		key_hash_list.push_back(msghandle::type::fix_int32(1895024678));
		key_hash_list.push_back(msghandle::type::fix_int32(347701446));
		key_hash_list.push_back(msghandle::type::fix_int32(1111056420));
		key_hash_list.push_back(msghandle::type::fix_int32(1809920178));
		Clear();
	}

void Clear(){
	id={};
	valid={};
	detObject={};
	occlude={};
	color={};
	color_score={};
	numberObject={};
	hasNumber={};
	number={};
	number_score={};
	lightType={};
	arrow={};
	arrow_score={};
	distance_car2light={};
	numberType={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,id,valid,detObject,occlude,color,color_score,numberObject,hasNumber,number,number_score,lightType,arrow,arrow_score,distance_car2light,numberType);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","valid","detObject","occlude","color","color_score","numberObject","hasNumber","number","number_score","lightType","arrow","arrow_score","distance_car2light","numberType");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,valid,detObject,occlude,color,color_score,numberObject,hasNumber,number,number_score,lightType,arrow,arrow_score,distance_car2light,numberType);
} PerceptionTLWomapPerObject;
REGISTER(PerceptionTLWomapPerObject,PerceptionTLWomapPerObject_Func);
