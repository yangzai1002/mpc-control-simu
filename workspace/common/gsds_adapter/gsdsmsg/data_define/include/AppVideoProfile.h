#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "CommonPoint3f.h"
#include "AppVideoSplitParam.h"
/*
视频编码参数
*/

typedef struct AppVideoProfile{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头
	int	front_source;	//前相机数据源（0-前视相机；4-前视微光）
	int	output_mode;	//1-全彩；2-灰度；3-二值；4-特征图
	int	adaptive_mode;	//自动调整码率和分辨率（0-关闭；1-打开）
	int	manual_bps;	//输出码率（kbps）
	double	manual_width;	//输出宽度
	double	manual_height;	//输出高度
	int	remote_control_assist;	//是否打开遥控辅助（0-关闭；1-打开）
	int	remote_control_alarm;	//是否打开遥控预警（0-关闭；1-打开）
	int	split_adaptive;	//是否开启自动切换视图（0-关闭；1-打开）
	int	manual_split_main;	//手动切换视图时的主图（0-前视；1-为左前；2-右前；3-后视； 4-前视微光；-1-无）
	int	manual_split_left;	//手动切换视图时的左图（0-前视；1-为左前；2-右前；3-后视； 4-前视微光；-1-无）
	int	manual_split_right;	//手动切换视图时的右图（0-前视；1-为左前；2-右前；3-后视； 4-前视微光；-1-无）
	double	receiver_reported_fps;	//接收端报告的当前帧率
	double	receiver_reported_bps;	//接收端报告的当前码率（kbps）
	long long	receiver_reported_ts;	//接收端报告时间（单位：毫秒）
	std::vector<CommonPoint3f>	points;	//目标位置
	int	video_third_person;	//是否开启第三人称（上帝）视角（0-关闭；1-打开）
	std::vector<AppVideoSplitParam>	split_area_parames;	//相机分割参数信息
	int	auto_control_assist;	//自主辅助线显示(0-关闭 1-打开)

	AppVideoProfile(){
		key_hash_list.resize(24);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(2089024390));
		key_hash_list.push_back(msghandle::type::fix_int32(420046504));
		key_hash_list.push_back(msghandle::type::fix_int32(1230966866));
		key_hash_list.push_back(msghandle::type::fix_int32(764680212));
		key_hash_list.push_back(msghandle::type::fix_int32(295321879));
		key_hash_list.push_back(msghandle::type::fix_int32(882968196));
		key_hash_list.push_back(msghandle::type::fix_int32(1046850025));
		key_hash_list.push_back(msghandle::type::fix_int32(580333350));
		key_hash_list.push_back(msghandle::type::fix_int32(1748769399));
		key_hash_list.push_back(msghandle::type::fix_int32(474707606));
		key_hash_list.push_back(msghandle::type::fix_int32(658575869));
		key_hash_list.push_back(msghandle::type::fix_int32(1928038422));
		key_hash_list.push_back(msghandle::type::fix_int32(394332));
		key_hash_list.push_back(msghandle::type::fix_int32(555488196));
		key_hash_list.push_back(msghandle::type::fix_int32(221332089));
		key_hash_list.push_back(msghandle::type::fix_int32(250832220));
		key_hash_list.push_back(msghandle::type::fix_int32(1334668562));
		key_hash_list.push_back(msghandle::type::fix_int32(1728672335));
		key_hash_list.push_back(msghandle::type::fix_int32(265045961));
		key_hash_list.push_back(msghandle::type::fix_int32(1219251167));
		Clear();
	}

void Clear(){
	header={};
	front_source={};
	output_mode={};
	adaptive_mode={};
	manual_bps={};
	manual_width={};
	manual_height={};
	remote_control_assist={};
	remote_control_alarm={};
	split_adaptive={};
	manual_split_main={};
	manual_split_left={};
	manual_split_right={};
	receiver_reported_fps={};
	receiver_reported_bps={};
	receiver_reported_ts={};
	points={};
	video_third_person={};
	split_area_parames={};
	auto_control_assist={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,front_source,output_mode,adaptive_mode,manual_bps,manual_width,manual_height,remote_control_assist,remote_control_alarm,split_adaptive,manual_split_main,manual_split_left,manual_split_right,receiver_reported_fps,receiver_reported_bps,receiver_reported_ts,points,video_third_person,split_area_parames,auto_control_assist);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","front_source","output_mode","adaptive_mode","manual_bps","manual_width","manual_height","remote_control_assist","remote_control_alarm","split_adaptive","manual_split_main","manual_split_left","manual_split_right","receiver_reported_fps","receiver_reported_bps","receiver_reported_ts","points","video_third_person","split_area_parames","auto_control_assist");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,front_source,output_mode,adaptive_mode,manual_bps,manual_width,manual_height,remote_control_assist,remote_control_alarm,split_adaptive,manual_split_main,manual_split_left,manual_split_right,receiver_reported_fps,receiver_reported_bps,receiver_reported_ts,points,video_third_person,split_area_parames,auto_control_assist);
} AppVideoProfile;
REGISTER(AppVideoProfile,AppVideoProfile_Func);
