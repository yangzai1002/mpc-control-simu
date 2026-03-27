#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*

*/

typedef struct VideoEncodingPerf{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	double	avg_ms_scaling;	//平均调整图像大小耗时（单位：毫秒）
	double	avg_ms_color_converting;	//平均颜色空间转换耗时（单位：毫秒）
	double	avg_ms_encoding;	//平均编码耗时（单位：毫秒）
	double	avg_ms_sending;	//平均发送耗时（单位：毫秒）
	double	avg_ms_span_from_driver_to_scale;	//平均时间跨度：从驱动到开始调整图像大小（单位：毫秒）
	double	avg_ms_span_from_driver_to_send_done;	//平均时间跨度：从驱动到发送完成（单位：毫秒）

	VideoEncodingPerf(){
		key_hash_list.resize(10);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1852952470));
		key_hash_list.push_back(msghandle::type::fix_int32(340949661));
		key_hash_list.push_back(msghandle::type::fix_int32(2059312328));
		key_hash_list.push_back(msghandle::type::fix_int32(127982413));
		key_hash_list.push_back(msghandle::type::fix_int32(618328270));
		key_hash_list.push_back(msghandle::type::fix_int32(484309942));
		Clear();
	}

void Clear(){
	avg_ms_scaling={};
	avg_ms_color_converting={};
	avg_ms_encoding={};
	avg_ms_sending={};
	avg_ms_span_from_driver_to_scale={};
	avg_ms_span_from_driver_to_send_done={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(0)
	MSGHANDLE_DEFINE(key_hash_list,avg_ms_scaling,avg_ms_color_converting,avg_ms_encoding,avg_ms_sending,avg_ms_span_from_driver_to_scale,avg_ms_span_from_driver_to_send_done);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","avg_ms_scaling","avg_ms_color_converting","avg_ms_encoding","avg_ms_sending","avg_ms_span_from_driver_to_scale","avg_ms_span_from_driver_to_send_done");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,avg_ms_scaling,avg_ms_color_converting,avg_ms_encoding,avg_ms_sending,avg_ms_span_from_driver_to_scale,avg_ms_span_from_driver_to_send_done);
} VideoEncodingPerf;
REGISTER(VideoEncodingPerf,VideoEncodingPerf_Func);
