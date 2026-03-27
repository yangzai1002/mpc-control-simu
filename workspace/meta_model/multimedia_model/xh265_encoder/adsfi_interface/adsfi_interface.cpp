
#include <fmt/format.h>

#include "DatetimeUtil.hpp"
#include "HexUtils.hpp"
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "config_loader/custom_stack.h"
#include "log/avos_node.h"

AdsfiInterface::AdsfiInterface()
    : _so_path("")
    , _use_constant_parameter(false)
    , _constant_bps_kb(1024)
    , _constant_fps(30)
    , _constant_width(1280)
    , _constant_height(720)
    , _constant_gop(30)
    , _using_bps_kb(1024)
    , _using_fps(30)
    , _using_frame_interval_us(33) // 1000000/_using_f;
    , _using_width(1280)
    , _using_height(720)
    , _using_gop(30)
    , _last_image_width(0)
    , _last_image_height(0)
    , _last_data_time(0)
    , mdc_mdcimg_interface_ptr(nullptr) {
    auto        ptr = CustomStack::Instance();
    std::string prefix = "media/";

    auto full_path = prefix + "video/encoder/so_path";
    if (!ptr->GetProjectConfigValue(full_path, _so_path) || _so_path.empty()) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    full_path = prefix + "video/encoder/use_constant_parameter";
    if (!ptr->GetProjectConfigValue(full_path, _use_constant_parameter)) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    full_path = prefix + "video/encoder/constant_bps";
    if (!ptr->GetProjectConfigValue(full_path, _constant_bps_kb) || _constant_bps_kb <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    full_path = prefix + "video/encoder/constant_fps";
    if (!ptr->GetProjectConfigValue(full_path, _constant_fps) || _constant_fps <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    full_path = prefix + "video/encoder/constant_width";
    if (!ptr->GetProjectConfigValue(full_path, _constant_width) || _constant_width <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    full_path = prefix + "video/encoder/constant_height";
    if (!ptr->GetProjectConfigValue(full_path, _constant_height) || _constant_height <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    full_path = prefix + "video/encoder/constant_gop";
    if (!ptr->GetProjectConfigValue(full_path, _constant_gop) || _constant_gop <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        auto error = fmt::format("read project config {} failed", full_path);
        throw std::runtime_error(error);
    }

    _ec.ec_remove(_ERRORCODE_CONFIG);

    ApInfo("h265_encoder") << "so_path: " << _so_path;
    ApInfo("h265_encoder") << "use_constant_parameter: " << _use_constant_parameter;
    ApInfo("h265_encoder") << "constant_bps: " << _constant_bps_kb;
    ApInfo("h265_encoder") << "constant_fps: " << _constant_fps;
    ApInfo("h265_encoder") << "constant_width: " << _constant_width;
    ApInfo("h265_encoder") << "constant_height: " << _constant_height;
    ApInfo("h265_encoder") << "constant_gop: " << _constant_gop;

    _thread = std::thread([&]() {
        pthread_setname_np(pthread_self(), "data_monitor");
        while (!_stopped) {
            auto now = app_common::DatetimeUtil::current_second_double();
            auto last = _last_data_time.load();
            if (fabs(now - last) >= 2.0) {
                std::lock_guard lg(_ec_mtx);
                _ec.ec_add(_ERRORCODE_NO_DATA);
            } else {
                std::lock_guard lg(_ec_mtx);
                _ec.ec_remove(_ERRORCODE_NO_DATA);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

AdsfiInterface::~AdsfiInterface() {
    _stopped = true;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void AdsfiInterface::Init() {
    SetScheduleInfo("message");

    // ApLogInterface::Instance()->Init("XH265Encoder");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &ptr) {
    image_deque.push(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::AppVideoProfile> &ptr) {
    param_deque.push(ptr);
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgImageData> &frame) {
    std::shared_ptr<ara::adsfi::MsgImageData> image_ptr;
    image_deque.waitAndPop(image_ptr);
    auto double_time = app_common::DatetimeUtil::double_time_from_sec_nsec(image_ptr->timestamp);
    _last_data_time = double_time;

    if (!_pre_param_ptr) {
        _pre_param_ptr = std::make_shared<ara::adsfi::AppVideoProfile>();
        _pre_param_ptr->manual_bps = 1024;
        _pre_param_ptr->manual_width = 1280;
        _pre_param_ptr->manual_height = 720;
    }

    param_deque.tryPop(_pre_param_ptr);
    if (__glibc_unlikely(nullptr == _pre_param_ptr)) {
        ApError("h265_encoder") << "param null";
        return -1;
    }

    if (__glibc_unlikely(!_use_constant_parameter && 0 == _pre_param_ptr->manual_bps)) {
        ApError("h265_encoder") << "error, use_constant_parameter: " << _use_constant_parameter
                                << ", encoding_parameter.bps: " << _pre_param_ptr->manual_bps;
        std::lock_guard lg(_ec_mtx);
        _ec.ec_add(_ERRORCODE_ENCODING_PARAM);
        return -1;
    }
    {
        std::lock_guard lg(_ec_mtx);
        _ec.ec_remove(_ERRORCODE_ENCODING_PARAM);
    }

    if (__glibc_unlikely(2 != image_ptr->imageType)) {
        ApError("h265_encoder") << "error, image_type: " << static_cast<std::int32_t>(image_ptr->imageType);
        std::lock_guard lg(_ec_mtx);
        _ec.ec_add(_ERRORCODE_IMAGE_FORMAT);
        return -1;
    }
    {
        std::lock_guard lg(_ec_mtx);
        _ec.ec_remove(_ERRORCODE_IMAGE_FORMAT);
    }

    const std::uint32_t FPS = 30;
    const std::uint32_t GOP = 30;

    bool c = false;
    if (nullptr == mdc_mdcimg_interface_ptr) {
        c = true;
    } else if ((!_use_constant_parameter &&
                (_using_bps_kb != _pre_param_ptr->manual_bps || _using_width != _pre_param_ptr->manual_width ||
                 _using_height != _pre_param_ptr->manual_height ||
                 _using_fps != FPS /*_pre_param_ptr->encoding_parameter.fps*/ ||
                 _using_gop != GOP /*_pre_param_ptr->encoding_parameter.encoding_gop*/)) ||
               _last_image_width != image_ptr->width || _last_image_height != image_ptr->height) {
        ApInfo("h265_encoder") << "recreate encoder, encoder: " << _using_bps_kb << ", " << _using_width << ", "
                               << _using_height << ", " << _using_fps << ", " << _using_gop;
        ApInfo("h265_encoder") << "recreate encoder, parameter: " << _pre_param_ptr->manual_bps << ", "
                               << _pre_param_ptr->manual_width << ", " << _pre_param_ptr->manual_height << ", "
                               << FPS /*_pre_param_ptr->encoding_parameter.fps*/ << ", "
                               << GOP /*_pre_param_ptr->encoding_parameter.encoding_gop*/;
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
            _using_bps_kb = _pre_param_ptr->manual_bps;
            _using_fps = FPS /*_pre_param_ptr->encoding_parameter.fps*/;
            _using_width = _pre_param_ptr->manual_width;
            _using_height = _pre_param_ptr->manual_height;
            _using_gop = GOP /*_pre_param_ptr->encoding_parameter.encoding_gop*/;
        }
        _last_image_width = image_ptr->width;
        _last_image_height = image_ptr->height;

        if (_using_bps_kb <= 0) {
            ApError("h265_encoder") << "invalid parameter bps: " << _using_bps_kb;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "bps=" + std::to_string(_using_bps_kb));
            return -1;
        }
        if (_using_fps <= 0) {
            ApError("h265_encoder") << "invalid parameter fps: " << _using_fps;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "fps=" + std::to_string(_using_fps));
            return -1;
        }
        if (_using_width <= 0) {
            ApError("h265_encoder") << "invalid parameter output width: " << _using_width;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "output_width=" + std::to_string(_using_width));
            return -1;
        }
        if (_using_height <= 0) {
            ApError("h265_encoder") << "invalid parameter output height: " << _using_height;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "output_height=" + std::to_string(_using_height));
            return -1;
        }
        if (_using_gop <= 0) {
            ApError("h265_encoder") << "invalid parameter gop: " << _using_gop;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "gop=" + std::to_string(_using_gop));
            return -1;
        }
        if (_last_image_width <= 0) {
            ApError("h265_encoder") << "invalid parameter input width: " << _last_image_width;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "input_height=" + std::to_string(_last_image_width));
            return -1;
        }
        if (_last_image_height <= 0) {
            ApError("h265_encoder") << "invalid parameter input height: " << _last_image_height;
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_ENCODING_PARAM, "input_width=" + std::to_string(_last_image_height));
            return -1;
        }
        {
            std::lock_guard lg(_ec_mtx);
            _ec.ec_remove(_ERRORCODE_ENCODING_PARAM);
        }
        _using_frame_interval_us = 1000000.0 / _using_fps;

        compile_fs::path cfg_so_path(_so_path);
        if (!cfg_so_path.is_absolute()) {
            cfg_so_path = custom_ptr->GetNodeConfigPath();
            cfg_so_path /= "config";
            cfg_so_path /= _so_path;
        }

        mdc_mdcimg_interface_ptr = std::make_shared<CMdcImgDvppInterface>(
            cfg_so_path.string(), _last_image_width, _last_image_height, _using_width, _using_height, _using_bps_kb);
        if (nullptr == mdc_mdcimg_interface_ptr) {
            ApError("h265_encoder") << "create mdc image dvpp interface failed";
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_CREATE_ENCODER);
            return -1;
        }
        if (mdc_mdcimg_interface_ptr->Init() < 0) {
            ApError("h265_encoder") << "init mdc image dvpp interface failed";
            std::lock_guard lg(_ec_mtx);
            _ec.ec_add(_ERRORCODE_CREATE_ENCODER);
            return -1;
        }
        {
            std::lock_guard lg(_ec_mtx);
            _ec.ec_remove(_ERRORCODE_CREATE_ENCODER);
        }
    }

    if (image_ptr->bufferType != 4) {
        ApError("h265_encoder") << "image buffer type invalid, buffertype: "
                                << static_cast<std::int32_t>(image_ptr->bufferType);
        std::lock_guard lg(_ec_mtx);
        _ec.ec_add(_ERRORCODE_IMAGE_FORMAT);
        return -1;
    }
    unsigned char *data = zero_shm_mem.GetAllocShmMem(image_ptr->frameID, image_ptr->dataSize, image_ptr->seq);
    if (nullptr == data) {
        ApError("h265_encoder") << "image acquire failed, buffertype: "
                                << static_cast<std::int32_t>(image_ptr->bufferType)
                                << ", frameid: " << image_ptr->frameID << ", datasize: " << image_ptr->dataSize
                                << ", seq: " << image_ptr->seq;
        std::lock_guard lg(_ec_mtx);
        _ec.ec_add(_ERRORCODE_IMAGE_FORMAT);
        return -1;
    }
    {
        std::lock_guard lg(_ec_mtx);
        _ec.ec_remove(_ERRORCODE_IMAGE_FORMAT);
    }

    {
        std::lock_guard lg(_ec_mtx);
        _actual_fps.store(_ec.check_freq(double_time, _using_fps));
    }

    auto s = app_common::DatetimeUtil::current_microsecond();
    // mdc_mdcimg_interface_ptr->SetYUV420SPImage((uint8_t*)image_ptr->data.data(), image_ptr->data.size());
    mdc_mdcimg_interface_ptr->SetYUV420SPImage((uint8_t *)data, image_ptr->dataSize);
    mdc_mdcimg_interface_ptr->GetH265Image(frame->rawData);
    auto e = app_common::DatetimeUtil::current_microsecond();
    _time_cost_indicator.add(e - s);
    auto avg_time = _time_cost_indicator.avg();
    if (avg_time > _using_frame_interval_us) {
        ApError("h265_encoder") << "time cost too long: " << avg_time;
        std::lock_guard lg(_ec_mtx);
        _ec.ec_add(_ERRORCODE_ENCODE_COST);
    } else {
        std::lock_guard lg(_ec_mtx);
        _ec.ec_remove(_ERRORCODE_ENCODE_COST);
    }

    frame->timestamp = image_ptr->timestamp;
    frame->frameID = "encoded_h265_" + std::to_string(image_ptr->seq);
    frame->width = _using_width;
    frame->height = _using_height;
    frame->dataSize = frame->rawData.size();
    frame->bufferType = 0;
    frame->imageType = 53;
    frame->seq = image_ptr->seq;
    frame->mbufData = 0;
    ApInfo("h265_encoder") << "frame encoded, timestamp: " << double_time << ", size: " << frame->dataSize;
    //   << ", hex: " << app_common::HexUtils::to_hex_string(frame->rawData.data(), std::max(frame->dataSize, 200u));

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VideoEncodingPerf> &frame) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    frame->fps = _actual_fps.load();
    return 0;
}
