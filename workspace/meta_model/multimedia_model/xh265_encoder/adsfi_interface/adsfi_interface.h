
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <arpa/inet.h>
#include <dlfcn.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "DatetimeUtil.hpp"
#include "PerfIndicators.hpp"
#include "StringUtils.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_videoencodingperf.h"
#include "config_loader/custom_stack.h"
#include "impl/XX264EncodingInMdc.hpp"
#include "ap_log/ap_log_interface.h"
#include "monitor/faulthandle_api.hpp"
#include "shm_util/zero_copy_shm_mem.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init() override;

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::AppVideoProfile>& ptr);

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgImageData>& ptr);

    int Process(const std::string& name, std::shared_ptr<ara::adsfi::MsgImageData>& frame);

    int Process(const std::string& name, std::shared_ptr<ara::adsfi::VideoEncodingPerf>& frame);

private:
    ThreadSafeQueue<std::shared_ptr<ara::adsfi::AppVideoProfile>> param_deque;
    ThreadSafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>>    image_deque;

    // configuration
    std::string  _so_path;
    bool         _use_constant_parameter;
    std::int32_t _constant_bps_kb;
    std::int32_t _constant_fps;
    std::int32_t _constant_width;
    std::int32_t _constant_height;
    std::int32_t _constant_gop;
    std::int32_t _using_bps_kb;
    std::int32_t _using_fps;
    std::int32_t _using_frame_interval_us; // 1000000/_using_f;
    std::int32_t _using_width;
    std::int32_t _using_height;
    std::int32_t _using_gop;
    std::int32_t _last_image_width;
    std::int32_t _last_image_height;

    // runtime state
    std::atomic_bool                             _stopped;
    std::thread                                  _thread;
    std::atomic<double>                          _last_data_time;
    std::mutex                                   _ec_mtx;
    Ec                                           _ec;
    std::atomic<double>                          _actual_fps;
    std::shared_ptr<ara::adsfi::AppVideoProfile> _pre_param_ptr;
    app_common::TimeCostIndicator<100>           _time_cost_indicator;

    std::shared_ptr<CMdcImgDvppInterface> mdc_mdcimg_interface_ptr;

    static const constexpr char* _ERRORCODE_CONFIG = "4022001";         // 配置参数读取失败
    static const constexpr char* _ERRORCODE_IMAGE_FORMAT = "4022002";   // 图像格式错误
    static const constexpr char* _ERRORCODE_ENCODING_PARAM = "4022003"; // 编码参数错误
    static const constexpr char* _ERRORCODE_CREATE_ENCODER = "4022004"; // 编码器创建失败
    static const constexpr char* _ERRORCODE_NO_DATA = "4022005";        // 无数据输入
    static const constexpr char* _ERRORCODE_FREQ = "4022006";           // 输入数据频率异常
    static const constexpr char* _ERRORCODE_ENCODE = "4022007";         // 编码失败
    static const constexpr char* _ERRORCODE_ENCODE_COST = "4022008";    // 编码耗时过长
    ZeroCopyShmMem               zero_shm_mem;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
