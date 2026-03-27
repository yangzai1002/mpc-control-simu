#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "Pose.h"

/*

*/

typedef struct BusinessCommandParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	std::string	patrol_name;	//循迹文件名称
	std::string	patrol_dest;	//循迹目的地
	int	patrol_type;	//循迹类型：0-TurnAround，1-Reverse，2-Oneway，3-GlobalOneway，4-GlobalRawplan
	int	patrol_loop;	//循迹循环：0-不循环，1-循环
	std::string	patrol_roads;	//路网道路信息
	double	max_speed;	//行驶最大速度
	int	task_avoid_mode;	//避障属性：0--停障  1--避障  2--越障  3-忽略障碍物模式
	int	command;	//0:stop 1:start 2:pause
	double	follow_min_dis;	//跟随最小距离
	double	follow_thw;	//最小跟随时距
	int	follow_mode;	//跟随模式：0牵引1轨迹
	int	follow_x;	//跟随目标在2D图像上的x轴坐标
	int	follow_y;	//跟随目标在2D图像上的y轴坐标
	int	view_width;	//视图宽度
	int	view_height;	//视图高度
	int	id;	//跟随目标id
	double	return_lat;	//返航点纬度
	double	return_lon;	//返航点经度
	std::string	fence;	//电子围栏（目前传的是文件的绝对路径）
	gsds::Pose	pose;	//姿态信息

	BusinessCommandParameter(){
		key_hash_list.resize(24);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1351678489));
		key_hash_list.push_back(msghandle::type::fix_int32(1314901530));
		key_hash_list.push_back(msghandle::type::fix_int32(1485198129));
		key_hash_list.push_back(msghandle::type::fix_int32(1933756248));
		key_hash_list.push_back(msghandle::type::fix_int32(1267394020));
		key_hash_list.push_back(msghandle::type::fix_int32(473763977));
		key_hash_list.push_back(msghandle::type::fix_int32(284743270));
		key_hash_list.push_back(msghandle::type::fix_int32(702034301));
		key_hash_list.push_back(msghandle::type::fix_int32(331299692));
		key_hash_list.push_back(msghandle::type::fix_int32(1454182311));
		key_hash_list.push_back(msghandle::type::fix_int32(549461431));
		key_hash_list.push_back(msghandle::type::fix_int32(1884225875));
		key_hash_list.push_back(msghandle::type::fix_int32(263789421));
		key_hash_list.push_back(msghandle::type::fix_int32(962352327));
		key_hash_list.push_back(msghandle::type::fix_int32(1850430710));
		key_hash_list.push_back(msghandle::type::fix_int32(1961503166));
		key_hash_list.push_back(msghandle::type::fix_int32(180447030));
		key_hash_list.push_back(msghandle::type::fix_int32(1320737504));
		key_hash_list.push_back(msghandle::type::fix_int32(1939061350));
		key_hash_list.push_back(msghandle::type::fix_int32(89491876));
		Clear();
	}

void Clear(){
	patrol_name={};
	patrol_dest={};
	patrol_type={};
	patrol_loop={};
	patrol_roads={};
	max_speed={};
	task_avoid_mode={};
	command={};
	follow_min_dis={};
	follow_thw={};
	follow_mode={};
	follow_x={};
	follow_y={};
	view_width={};
	view_height={};
	id={};
	return_lat={};
	return_lon={};
	fence={};
	pose={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,patrol_name,patrol_dest,patrol_type,patrol_loop,patrol_roads,max_speed,task_avoid_mode,command,follow_min_dis,follow_thw,follow_mode,follow_x,follow_y,view_width,view_height,id,return_lat,return_lon,fence,pose);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","patrol_name","patrol_dest","patrol_type","patrol_loop","patrol_roads","max_speed","task_avoid_mode","command","follow_min_dis","follow_thw","follow_mode","follow_x","follow_y","view_width","view_height","id","return_lat","return_lon","fence","pose");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,patrol_name,patrol_dest,patrol_type,patrol_loop,patrol_roads,max_speed,task_avoid_mode,command,follow_min_dis,follow_thw,follow_mode,follow_x,follow_y,view_width,view_height,id,return_lat,return_lon,fence,pose);
} BusinessCommandParameter;
REGISTER(BusinessCommandParameter,BusinessCommandParameter_Func);
