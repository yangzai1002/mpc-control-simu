#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionSingleTLBboxDebug.h"
#include "CommonPoint3d.h"
/*

*/

typedef struct PerceptionSingleTLDetectionDebug{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	unsigned char	color;	//红绿灯颜色
	std::string	id;	//红绿灯唯一ID标识
	unsigned char	flicker;	//红绿灯闪烁状态
	bool	occlude;	//遮挡情况
	unsigned char	number;	//0~99
	double	confidence;	//置信度
	double	tracking_time;	//跟踪时长
	std::vector<PerceptionSingleTLBboxDebug>	light_detection;	//红绿灯检测Bbox
	std::vector<PerceptionSingleTLBboxDebug>	light_track;	//红绿灯跟踪Bbox
	std::vector<PerceptionSingleTLBboxDebug>	light_hdmap;	//红绿灯地图Bbox
	std::vector<PerceptionSingleTLBboxDebug>	number_detection;	//倒计时牌识别Bbox
	unsigned char	sub_light_type;	//sub_light_type
	double	distance_car2light;	//自车到红绿灯距离
	std::vector<CommonPoint3d>	light_position;	//红绿灯3D位置
	unsigned char	height;	//红绿灯高度
	unsigned char	sub_light_type_by_hdmap;	//地图中给的sub_light的类型

	PerceptionSingleTLDetectionDebug(){
		key_hash_list.resize(20);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(202070711));
		key_hash_list.push_back(msghandle::type::fix_int32(1459599047));
		key_hash_list.push_back(msghandle::type::fix_int32(1248658013));
		key_hash_list.push_back(msghandle::type::fix_int32(125202316));
		key_hash_list.push_back(msghandle::type::fix_int32(1395904784));
		key_hash_list.push_back(msghandle::type::fix_int32(1994731374));
		key_hash_list.push_back(msghandle::type::fix_int32(959216590));
		key_hash_list.push_back(msghandle::type::fix_int32(1158843348));
		key_hash_list.push_back(msghandle::type::fix_int32(557304349));
		key_hash_list.push_back(msghandle::type::fix_int32(1885840941));
		key_hash_list.push_back(msghandle::type::fix_int32(2118636965));
		key_hash_list.push_back(msghandle::type::fix_int32(874287035));
		key_hash_list.push_back(msghandle::type::fix_int32(822674279));
		key_hash_list.push_back(msghandle::type::fix_int32(278642926));
		key_hash_list.push_back(msghandle::type::fix_int32(299879872));
		key_hash_list.push_back(msghandle::type::fix_int32(968487423));
		Clear();
	}

void Clear(){
	color={};
	id={};
	flicker={};
	occlude={};
	number={};
	confidence={};
	tracking_time={};
	light_detection={};
	light_track={};
	light_hdmap={};
	number_detection={};
	sub_light_type={};
	distance_car2light={};
	light_position={};
	height={};
	sub_light_type_by_hdmap={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,color,id,flicker,occlude,number,confidence,tracking_time,light_detection,light_track,light_hdmap,number_detection,sub_light_type,distance_car2light,light_position,height,sub_light_type_by_hdmap);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","color","id","flicker","occlude","number","confidence","tracking_time","light_detection","light_track","light_hdmap","number_detection","sub_light_type","distance_car2light","light_position","height","sub_light_type_by_hdmap");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,color,id,flicker,occlude,number,confidence,tracking_time,light_detection,light_track,light_hdmap,number_detection,sub_light_type,distance_car2light,light_position,height,sub_light_type_by_hdmap);
} PerceptionSingleTLDetectionDebug;
REGISTER(PerceptionSingleTLDetectionDebug,PerceptionSingleTLDetectionDebug_Func);
