
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <list>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "DatetimeUtil.hpp"
#include "PerfIndicators.hpp"
#include "StringUtils.hpp"
#include "config_loader/custom_stack.h"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"
#include "shm_util/zero_copy_shm_mem.h"


#include "mdc_adapter/cidpabstractplatforminterface.h"
#include "mdc_adapter/imgdvppplatform2usrprotocol.h"


// #include "ara/adsfi/impl_type_idpappvideoparameter.h"
#include "ara/adsfi/impl_type_idpappvideoparameter.h"
#include "ara/adsfi/impl_type_idpvideoencodedframe.h"
#include "ara/adsfi/impl_type_idpsensorimage.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

#include "impl/XX264EncodingInMdc.hpp"



class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {
		_so_path = "";
		_use_constant_parameter = false;
		_constant_bps_kb = 1024;
		_constant_fps = 30;
		_constant_width = 1280;
		_constant_height = 720;
		_constant_gop = 30;
		_using_bps_kb = 1024;
		_using_fps = 30;
		_using_frame_interval_us = 33; // 1000000/_using_f;
		_using_width = 1280;
		_using_height = 720;
		_using_gop = 30;
		_last_image_width = 0;
		_last_image_height = 0;

		_last_data_time = nullptr;

		_ec = nullptr;
		_time_cost_indicator = nullptr;

		mdc_mdcimg_interface_ptr = nullptr;
		zero_shm_mem_ptr = new ZeroCopyShmMem();
	}
	~AdsfiInterface() {

	}

	void Init() override;



	void Callback(const std::shared_ptr<ara::adsfi::IdpAppVideoParameter> &ptr) ;


	void Callback(const std::shared_ptr<ara::adsfi::IdpSensorImage> &ptr) ;

	int Process(std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame> &frame);




private:
	// ThreadSafeQueue<std::shared_ptr<std::shared_ptr<ara::adsfi::IdpAppVideoParameter>>> param_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::IdpAppVideoParameter>> param_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::IdpSensorImage>> image_deque;



	std::string _so_path;
	bool _use_constant_parameter ;
	std::int32_t _constant_bps_kb ;
	std::int32_t _constant_fps ;
	std::int32_t _constant_width ;
	std::int32_t _constant_height ;
	std::int32_t _constant_gop ;
	std::int32_t _using_bps_kb ;
	std::int32_t _using_fps ;
	std::int32_t _using_frame_interval_us ; // 1000000/_using_f;
	std::int32_t _using_width ;
	std::int32_t _using_height ;
	std::int32_t _using_gop;
	std::int32_t _last_image_width;
	std::int32_t _last_image_height ;

	std::shared_ptr<std::atomic<double>> _last_data_time ;

	std::shared_ptr<Ec> _ec ;
	std::shared_ptr<app_common::TimeCostIndicator<100>> _time_cost_indicator ;

	std::shared_ptr<CMdcImgDvppInterface> mdc_mdcimg_interface_ptr ;


	std::shared_ptr<ara::adsfi::IdpAppVideoParameter> pre_param_ptr;
	static const constexpr char* _ERRORCODE_CONFIG = "4022001";         // 配置参数读取失败
    static const constexpr char* _ERRORCODE_IMAGE_FORMAT = "4022002";   // 图像格式错误
    static const constexpr char* _ERRORCODE_ENCODING_PARAM = "4022003"; // 编码参数错误
    static const constexpr char* _ERRORCODE_CREATE_ENCODER = "4022004"; // 编码器创建失败
    static const constexpr char* _ERRORCODE_NO_DATA = "4022005";        // 无数据输入
    static const constexpr char* _ERRORCODE_FREQ = "4022006";           // 输入数据频率异常
    static const constexpr char* _ERRORCODE_ENCODE = "4022007";         // 编码失败
    static const constexpr char* _ERRORCODE_ENCODE_COST = "4022008";    // 编码耗时过长
ZeroCopyShmMem *zero_shm_mem_ptr;
	std::shared_ptr<ara::adsfi::IdpSensorImage> image_ptr;

	int frame_num = 0;
	BASE_TEMPLATE_FUNCION
};
#endif








