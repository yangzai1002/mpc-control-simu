#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionFinalObjects.h"
#include "PlanningTrajectory.h"
#include "CommonPoint3f.h"
#include "AppVideoSplitParam.h"
/*
视频叠加参数配置
*/

typedef struct AppVideoEditorParameter{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	output_mode;	//1-全彩；2-灰度；3-二值；4-特征图
	int	remote_control_alarm;	//是否打开遥控预警（0-关闭；1-打开）
	int	remote_control_assist;	//是否打开遥控辅助（0-关闭；1-打开）
	int	front_source;	//前相机数据源（0-前视相机；4-前视微光）
	int	split_adaptive;	//是否自适应切换视频内容（0-关闭；1-打开）
	int	video_left_;	//0-前视；1-为左前；2-右前；3-后视； 4-前视微光；-1-无
	int	video_main_;	//0-前视；1-为左前；2-右前；3-后视；  4-前视微光；-1-无
	int	video_right_;	//0-前视；1-为左前；2-右前；3-后视； 4-前视微光； -1-无
	PerceptionFinalObjects	objects;	//目标识别结果
	PlanningTrajectory	remote_control;	//遥控时的路径规划
	PlanningTrajectory	path;	//路径规划（循迹、跟随等 ）
	std::vector<CommonPoint3f>	target_points;	//返航点
	int	video_third_person;	//是否开启第三人称（上帝）视角（0-关闭；1-打开）
	CommonPoint3f	target_point;	//定速巡航目标点
	std::vector<AppVideoSplitParam>	split_parames_;	//相机分割参数列表

	AppVideoEditorParameter(){
		key_hash_list.resize(19);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1230966866));
		key_hash_list.push_back(msghandle::type::fix_int32(1748769399));
		key_hash_list.push_back(msghandle::type::fix_int32(580333350));
		key_hash_list.push_back(msghandle::type::fix_int32(420046504));
		key_hash_list.push_back(msghandle::type::fix_int32(1982190066));
		key_hash_list.push_back(msghandle::type::fix_int32(45441525));
		key_hash_list.push_back(msghandle::type::fix_int32(1288008535));
		key_hash_list.push_back(msghandle::type::fix_int32(1710610222));
		key_hash_list.push_back(msghandle::type::fix_int32(516893804));
		key_hash_list.push_back(msghandle::type::fix_int32(1316682833));
		key_hash_list.push_back(msghandle::type::fix_int32(1991065149));
		key_hash_list.push_back(msghandle::type::fix_int32(2029503444));
		key_hash_list.push_back(msghandle::type::fix_int32(1728672335));
		key_hash_list.push_back(msghandle::type::fix_int32(1429321159));
		key_hash_list.push_back(msghandle::type::fix_int32(2112118773));
		Clear();
	}

void Clear(){
	output_mode={};
	remote_control_alarm={};
	remote_control_assist={};
	front_source={};
	split_adaptive={};
	video_left_={};
	video_main_={};
	video_right_={};
	objects={};
	remote_control={};
	path={};
	target_points={};
	video_third_person={};
	target_point={};
	split_parames_={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,output_mode,remote_control_alarm,remote_control_assist,front_source,split_adaptive,video_left_,video_main_,video_right_,objects,remote_control,path,target_points,video_third_person,target_point,split_parames_);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","output_mode","remote_control_alarm","remote_control_assist","front_source","split_adaptive","video_left_","video_main_","video_right_","objects","remote_control","path","target_points","video_third_person","target_point","split_parames_");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,output_mode,remote_control_alarm,remote_control_assist,front_source,split_adaptive,video_left_,video_main_,video_right_,objects,remote_control,path,target_points,video_third_person,target_point,split_parames_);
} AppVideoEditorParameter;
REGISTER(AppVideoEditorParameter,AppVideoEditorParameter_Func);
