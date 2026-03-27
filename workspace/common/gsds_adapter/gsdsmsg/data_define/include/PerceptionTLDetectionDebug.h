#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "PerceptionSingleTLDetectionDebug.h"
#include "PerceptionSingleTLBboxDebug.h"
/*

*/

typedef struct PerceptionTLDetectionDebug{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	double	receive_stamp;	//接收时间戳
	double	send_stamp;	//发送时间戳
	double	trigger_stamp;	//触发时间戳
	std::string	camera_name;	//相机名称：当前选择哪个相机进行红绿灯检测
	std::vector<PerceptionSingleTLDetectionDebug>	traffic_light;	//vector，每个元素为1个light详细信息
	double	distance_car2stopline;	//自车距离停止线距离
	double	distance_stopline2lights;	//停止线距离灯的距离
	std::vector<PerceptionSingleTLBboxDebug>	detect_objs;	//全部的检测目标
	std::string	car_in_road_id;	//自车所在道路road id

	PerceptionTLDetectionDebug(){
		key_hash_list.resize(14);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(1879357055));
		key_hash_list.push_back(msghandle::type::fix_int32(925801534));
		key_hash_list.push_back(msghandle::type::fix_int32(164829545));
		key_hash_list.push_back(msghandle::type::fix_int32(84316979));
		key_hash_list.push_back(msghandle::type::fix_int32(768164667));
		key_hash_list.push_back(msghandle::type::fix_int32(605534508));
		key_hash_list.push_back(msghandle::type::fix_int32(1843501085));
		key_hash_list.push_back(msghandle::type::fix_int32(1073690176));
		key_hash_list.push_back(msghandle::type::fix_int32(281846025));
		Clear();
	}

void Clear(){
	header={};
	receive_stamp={};
	send_stamp={};
	trigger_stamp={};
	camera_name={};
	traffic_light={};
	distance_car2stopline={};
	distance_stopline2lights={};
	detect_objs={};
	car_in_road_id={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,receive_stamp,send_stamp,trigger_stamp,camera_name,traffic_light,distance_car2stopline,distance_stopline2lights,detect_objs,car_in_road_id);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","receive_stamp","send_stamp","trigger_stamp","camera_name","traffic_light","distance_car2stopline","distance_stopline2lights","detect_objs","car_in_road_id");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,receive_stamp,send_stamp,trigger_stamp,camera_name,traffic_light,distance_car2stopline,distance_stopline2lights,detect_objs,car_in_road_id);
} PerceptionTLDetectionDebug;
REGISTER(PerceptionTLDetectionDebug,PerceptionTLDetectionDebug_Func);
