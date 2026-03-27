#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct PerceptionTrafficSign{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	bool	enable;	//默认值false
	int	class_id;	//模型给出的标识牌详细类型，默认值MTS_BACKGROUND
	int	sign_meaning;	//标识牌，同协议sign_meaning含义，默认值MsgSignMeaning::UNSUPPORT
	int	sign_type;	//标识牌形状，默认值MsgSignType::OTHER
	int	track_id;	//默认值0
	float	score;	//默认值0.0
	int	xmin;	//默认值0
	int	ymin;	//默认值0
	int	xmax;	//默认值0
	int	ymax;	//默认值0
	int	position;	//交通标识牌位置，默认值-1
	int	lat_distance;	//交通标识的横向距离，-1
	int	long_distance;	//交通标识的纵向距离，-1
	int	width;	//交通标识宽度 : mm，-1
	int	height;	//交通标识高度 : mm，-1
	int	height_distance;	//交通标识牌距离地面高度 : mm，-1
	int	speed_limit_type;	//交通标识牌限速类型：0-最高限速；1-最低限速，默认值MsgSignSpeedLimitType::SLT_OTHER
	int	speed_limit_value;	//交通限速牌数值 : km/h，默认值MsgSignMeaning::UNSUPPORT
	int	speed_limit_data_age;	//交通标识持续检测的时间 : s，默认值MsgSignMeaning::UNSUPPORT

	PerceptionTrafficSign(){
		key_hash_list.resize(23);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(7539101));
		key_hash_list.push_back(msghandle::type::fix_int32(826861067));
		key_hash_list.push_back(msghandle::type::fix_int32(1119449903));
		key_hash_list.push_back(msghandle::type::fix_int32(963075462));
		key_hash_list.push_back(msghandle::type::fix_int32(1235300719));
		key_hash_list.push_back(msghandle::type::fix_int32(797332227));
		key_hash_list.push_back(msghandle::type::fix_int32(1175635776));
		key_hash_list.push_back(msghandle::type::fix_int32(794063753));
		key_hash_list.push_back(msghandle::type::fix_int32(240564326));
		key_hash_list.push_back(msghandle::type::fix_int32(2006475951));
		key_hash_list.push_back(msghandle::type::fix_int32(1445684187));
		key_hash_list.push_back(msghandle::type::fix_int32(346396783));
		key_hash_list.push_back(msghandle::type::fix_int32(439823560));
		key_hash_list.push_back(msghandle::type::fix_int32(1275060076));
		key_hash_list.push_back(msghandle::type::fix_int32(21514875));
		key_hash_list.push_back(msghandle::type::fix_int32(761669072));
		key_hash_list.push_back(msghandle::type::fix_int32(1284159164));
		key_hash_list.push_back(msghandle::type::fix_int32(252628569));
		key_hash_list.push_back(msghandle::type::fix_int32(2000570831));
		Clear();
	}

void Clear(){
	enable={};
	class_id={};
	sign_meaning={};
	sign_type={};
	track_id={};
	score={};
	xmin={};
	ymin={};
	xmax={};
	ymax={};
	position={};
	lat_distance={};
	long_distance={};
	width={};
	height={};
	height_distance={};
	speed_limit_type={};
	speed_limit_value={};
	speed_limit_data_age={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,enable,class_id,sign_meaning,sign_type,track_id,score,xmin,ymin,xmax,ymax,position,lat_distance,long_distance,width,height,height_distance,speed_limit_type,speed_limit_value,speed_limit_data_age);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","enable","class_id","sign_meaning","sign_type","track_id","score","xmin","ymin","xmax","ymax","position","lat_distance","long_distance","width","height","height_distance","speed_limit_type","speed_limit_value","speed_limit_data_age");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,enable,class_id,sign_meaning,sign_type,track_id,score,xmin,ymin,xmax,ymax,position,lat_distance,long_distance,width,height,height_distance,speed_limit_type,speed_limit_value,speed_limit_data_age);
} PerceptionTrafficSign;
REGISTER(PerceptionTrafficSign,PerceptionTrafficSign_Func);
