
#include "adsfi_interface.h"
#include "log/avos_node.h"
#include "config_loader/custom_stack.h"


void AdsfiInterface::Init() {
	SetScheduleInfo("message",50);

	avos::common::AvosNode::Init("xh265_encoder");
	std::cout << "xh265_encoder init" << std::endl;

	_ec = std::make_shared<Ec>();
	_time_cost_indicator = std::make_shared<app_common::TimeCostIndicator<100>>();
	_last_data_time = std::make_shared<std::atomic<double>>(0);
	if (nullptr == _ec || nullptr == _time_cost_indicator || nullptr == _last_data_time) {
		AERROR << "cannot create ec manager";
		// 一般不会失败，所以这里先不发送故障码，直接返回
		return;
	}

    auto ptr = CustomStack::Instance();
	std::string prefix;

	if (ptr->GetConfig("XH265Encoder","config_path_prefix", prefix) < 0) {
		_ec->ec_add(_ERRORCODE_CONFIG, "config_path_prefix");
		throw std::runtime_error("read local config config_path_prefix failed");
	}
	app_common::StringUtils::trim(prefix);
	if (!prefix.empty() && prefix.back() != '/') {
		prefix += "/";
	}

	auto full_path = "video/encoder/so_path";
	if (!ptr->GetProjectConfigValue(full_path, _so_path) || _so_path.empty()) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	full_path = "video/encoder/use_constant_parameter";
	if (!ptr->GetProjectConfigValue(full_path, _use_constant_parameter)) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	full_path = "video/encoder/constant_bps";
	if (!ptr->GetProjectConfigValue(full_path, _constant_bps_kb) || _constant_bps_kb <= 0) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	full_path = "video/encoder/constant_fps";
	if (!ptr->GetProjectConfigValue(full_path, _constant_fps) || _constant_fps <= 0) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	full_path = "video/encoder/constant_width";
	if (!ptr->GetProjectConfigValue(full_path, _constant_width) || _constant_width <= 0) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	full_path = "video/encoder/constant_height";
	if (!ptr->GetProjectConfigValue(full_path, _constant_height) || _constant_height <= 0) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	full_path = "video/encoder/constant_gop";
	if (!ptr->GetProjectConfigValue(full_path, _constant_gop) || _constant_gop <= 0) {
		_ec->ec_add(_ERRORCODE_CONFIG, full_path);
		AERROR << "read project config " << full_path << "failed";
		return;
	}

	_ec->ec_remove(_ERRORCODE_CONFIG);

	AINFO << "so_path: " << _so_path;
	AINFO << "use_constant_parameter: " << _use_constant_parameter;
	AINFO << "constant_bps: " << _constant_bps_kb;
	AINFO << "constant_fps: " << _constant_fps;
	AINFO << "constant_width: " << _constant_width;
	AINFO << "constant_height: " << _constant_height;
	AINFO << "constant_gop: " << _constant_gop;


}


void AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpSensorImage> &ptr) {


	image_deque.push(ptr);


}


void AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpAppVideoParameter> &ptr) {

	param_deque.push(ptr);
}

// 函数：将 YUV 数据追加到文件中
void appendYUVToFile(void* yuvDataAddress, int yuvDataSize, const std::string& fileName) {
    // 打开文件，以二进制追加模式
    std::ofstream outFile(fileName, std::ios::binary | std::ios::app);
    if (!outFile) {
        std::cerr << "无法打开文件用于追加：" << fileName << std::endl;
        return;
    }

    // 将数据追加到文件
    outFile.write(static_cast<char*>(yuvDataAddress), yuvDataSize);

    // 关闭文件
    outFile.close();
}

int AdsfiInterface::Process(std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame> &frame) {

	frame_num++;
	image_deque.waitAndPop(image_ptr);

	param_deque.tryPop(pre_param_ptr);
	_last_data_time->store(image_ptr->header.time);

	if (!_use_constant_parameter && 0 == pre_param_ptr->encoding_parameter.bps) {
		return -1;
	}

	bool c = false;
	if (nullptr == mdc_mdcimg_interface_ptr) {
		c = true;
	} else if ((!_use_constant_parameter && (_using_bps_kb != pre_param_ptr->encoding_parameter.bps ||
	            _using_width != pre_param_ptr->encoding_parameter.width ||
	            _using_height != pre_param_ptr->encoding_parameter.height ||
	            _using_fps != pre_param_ptr->encoding_parameter.fps ||
	            _using_gop != pre_param_ptr->encoding_parameter.encoding_gop)) ||
	           _last_image_width != image_ptr->width || _last_image_height != image_ptr->height) {
		AINFO << "recreate encoder, encoder: " << _using_bps_kb << ", " << _using_width << ", " << _using_height << ", "
		      << _using_fps << ", " << _using_gop;
		AINFO << "recreate encoder, parameter: " << pre_param_ptr->encoding_parameter.bps << ", "
		      << pre_param_ptr->encoding_parameter.width << ", " << pre_param_ptr->encoding_parameter.height << ", "
		      << pre_param_ptr->encoding_parameter.fps << ", " << pre_param_ptr->encoding_parameter.encoding_gop;
		c = true;
	}
	if (c) {
		auto custom_ptr = CustomStack::Instance();
		if (_use_constant_parameter) {
			_using_bps_kb = _constant_bps_kb;
			_using_fps = _constant_fps;
			_using_width = _constant_width;
			_using_height = _constant_height;
			_using_gop = _constant_gop;
		} else {
			_using_bps_kb = pre_param_ptr->encoding_parameter.bps;
			_using_fps = pre_param_ptr->encoding_parameter.fps;
			_using_width = pre_param_ptr->encoding_parameter.width;
			_using_height = pre_param_ptr->encoding_parameter.height;
			_using_gop = pre_param_ptr->encoding_parameter.encoding_gop;
		}
		_last_image_width = image_ptr->width;
		_last_image_height = image_ptr->height;

		if (_using_bps_kb <= 0) {
			AERROR << "invalid parameter bps: " << _using_bps_kb;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "bps=" + std::to_string(_using_bps_kb));
			return -1;
		}
		if (_using_fps <= 0) {
			AERROR << "invalid parameter fps: " << _using_fps;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "fps=" + std::to_string(_using_fps));
			return -1;
		}
		if (_using_width <= 0) {
			AERROR << "invalid parameter output width: " << _using_width;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "output_width=" + std::to_string(_using_width));
			return -1;
		}
		if (_using_height <= 0) {
			AERROR << "invalid parameter output height: " << _using_height;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "output_height=" + std::to_string(_using_height));
			return -1;
		}
		if (_using_gop <= 0) {
			AERROR << "invalid parameter gop: " << _using_gop;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "gop=" + std::to_string(_using_gop));
			return -1;
		}
		if (_last_image_width <= 0) {
			AERROR << "invalid parameter input width: " << _last_image_width;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "input_height=" + std::to_string(_last_image_width));
			return -1;
		}
		if (_last_image_height <= 0) {
			AERROR << "invalid parameter input height: " << _last_image_height;
			_ec->ec_add(_ERRORCODE_ENCODING_PARAM, "input_width=" + std::to_string(_last_image_height));
			return -1;
		}
		_ec->ec_remove(_ERRORCODE_ENCODING_PARAM);
		_using_frame_interval_us = 1000000.0 / _using_fps;

		std::__fs::filesystem::path cfg_so_path(_so_path);
		if (!cfg_so_path.is_absolute()) {
			cfg_so_path = custom_ptr->GetNodeConfigPath();
			cfg_so_path /= _so_path;
		}

		mdc_mdcimg_interface_ptr = std::make_shared<CMdcImgDvppInterface>(
		                               cfg_so_path.string(), _last_image_width, _last_image_height, _using_width, _using_height, _using_bps_kb);
		if (nullptr == mdc_mdcimg_interface_ptr) {
			AERROR << "create mdc image dvpp interface failed";
			_ec->ec_add(_ERRORCODE_CREATE_ENCODER);
			return -1;
		}
		if (mdc_mdcimg_interface_ptr->Init() < 0) {
			AERROR << "init mdc image dvpp interface failed";
			_ec->ec_add(_ERRORCODE_CREATE_ENCODER);
			return -1;
		}
		_ec->ec_remove(_ERRORCODE_CREATE_ENCODER);

		std::thread([&]() {
			while (true) {
				auto now = app_common::DatetimeUtil::current_second_double();
				auto last = _last_data_time->load();
				if (fabs(now - last) >= 2.0) {
					_ec->ec_add(_ERRORCODE_NO_DATA);
				} else {
					_ec->ec_remove(_ERRORCODE_NO_DATA);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}).detach();

		return 0;
	}

	// image_ptr->GetConfig("num_threads", _num_threads);
	// std::cout << "num_threads: " << _num_threads << std::endl;


   unsigned char *data =
            zero_shm_mem_ptr->GetAllocShmMem(image_ptr->shm_buffer_info.shm_key, image_ptr->shm_buffer_info.buffer_size,
                                           image_ptr->shm_buffer_info.current_index, image_ptr->shm_buffer_info.queue_len);
	// zero_shm_mem_ptr->SyncDataVectorInfo(*image_ptr);
	// if (image_ptr->data.empty()) {
	//     std::cout << "sensor_image.data is empty" << std::endl;
	//     return;
	// }

	// mdc_mdcimg_interface_ptr->SetBGRImage((uint8_t *)sensor_Image.data.data(),sensor_Image.data.size());

	_ec->check_freq(image_ptr->header.time, _using_fps);

	if(frame_num == 100)
		appendYUVToFile((uint8_t*)data, image_ptr->shm_buffer_info.buffer_size,"/opt/usr/img_encoder_src.yuv420");

	auto s = app_common::DatetimeUtil::current_microsecond();
	// mdc_mdcimg_interface_ptr->SetYUV420SPImage((uint8_t*)image_ptr->data.data(), image_ptr->data.size());
	mdc_mdcimg_interface_ptr->SetYUV420SPImage((uint8_t*)data, image_ptr->shm_buffer_info.buffer_size);
	mdc_mdcimg_interface_ptr->GetH264Image(frame->frame_data);
	auto e = app_common::DatetimeUtil::current_microsecond();
	_time_cost_indicator->add(e - s);
	auto avg_time = _time_cost_indicator->avg();
	if (avg_time > _using_frame_interval_us) {
		AERROR << "time cost too long: " << avg_time;
		_ec->ec_add(_ERRORCODE_ENCODE_COST);
	} else {
		_ec->ec_remove(_ERRORCODE_ENCODE_COST);
	}

	frame->header = image_ptr->header;
	frame->encoding = 0;
	frame->fps = 30;
	frame->pts = 30;
	frame->key_frame = 1;
	frame->frame_size = frame->frame_data.size();
	if (frame->frame_size > 64) {
		for (size_t i = 0; i < 64; i++) {
			printf("%02x ", frame->frame_data[i]);
		}
		printf("\n");
	}

	AINFO << "frame->frame_data.size() = " << frame->frame_data.size();
	AINFO << "frame->fps = " << frame->fps;

	AINFO << "曝光时间：" << std::fixed << std::setprecision(6) << image_ptr->header.time;
	
	return 0;
}
