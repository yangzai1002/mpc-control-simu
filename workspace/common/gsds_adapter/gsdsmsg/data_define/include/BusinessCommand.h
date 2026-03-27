#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "CommonHeader.h"
#include "BusinessPath.h"
#include "PlanningTrajectory.h"
#include "BusniessRemoteControl.h"
#include "BusinessCommandParameter.h"
/*

*/

typedef struct BusinessCommand{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	CommonHeader	header;	//数据头，包含时间戳等信息
	int	business_mode;	//当前任务类型 0:none 1:track  2:arrive  3:follow_utm 31:follow_dr  4: 找平 5:航向调整 6:姿态调整
	BusinessPath	business_path;	//当前任务路径
	PlanningTrajectory	last_path;	//前一帧任务路径
	BusniessRemoteControl	remote_ctl;	//遥控数据
	BusinessCommandParameter	param;	//任务相关参数
	int	estop_flag;	//急停标志位 0-关闭 1-开启
	int	record_trigger_flag;	//数据记录信号标志（0：关闭；1：开启）

	BusinessCommand(){
		key_hash_list.resize(12);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(701792648));
		key_hash_list.push_back(msghandle::type::fix_int32(686058829));
		key_hash_list.push_back(msghandle::type::fix_int32(407603714));
		key_hash_list.push_back(msghandle::type::fix_int32(689902910));
		key_hash_list.push_back(msghandle::type::fix_int32(1506666417));
		key_hash_list.push_back(msghandle::type::fix_int32(677504241));
		key_hash_list.push_back(msghandle::type::fix_int32(611403842));
		key_hash_list.push_back(msghandle::type::fix_int32(1281977179));
		Clear();
	}

void Clear(){
	header={};
	business_mode={};
	business_path={};
	last_path={};
	remote_ctl={};
	param={};
	estop_flag={};
	record_trigger_flag={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,header,business_mode,business_path,last_path,remote_ctl,param,estop_flag,record_trigger_flag);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","header","business_mode","business_path","last_path","remote_ctl","param","estop_flag","record_trigger_flag");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,header,business_mode,business_path,last_path,remote_ctl,param,estop_flag,record_trigger_flag);
} BusinessCommand;
REGISTER(BusinessCommand,BusinessCommand_Func);
