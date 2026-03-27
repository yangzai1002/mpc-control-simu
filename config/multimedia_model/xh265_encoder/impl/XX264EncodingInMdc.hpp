#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

class CMdcImgDvppInterface {
public:
    CMdcImgDvppInterface(std::string _so_path, const int in_image_width, const int in_image_height,
                         const int out_image_width, const int out_image_height, const int out_image_bps) {
        so_path = _so_path;
        m_inimage_width = in_image_width;
        m_inimage_height = in_image_height;
        m_outimage_width = out_image_width;
        m_outimage_height = out_image_height;
        m_outimage_bps = out_image_bps;
    }

    ~CMdcImgDvppInterface() {
        mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_DEINIT_DVPP, nullptr);
        mdc_camera_ptr.reset();
    }

    int Init() {
        m_dvpp_cfg.m_inputtype = avos::idpware::imgdvpp::IMGTYPE_YUV420NV12;
        m_dvpp_cfg.m_outputtype = avos::idpware::imgdvpp::IMGTYPE_H264;
        m_dvpp_cfg.m_inwidth = m_inimage_width;
        m_dvpp_cfg.m_inheight = m_inimage_height;
        m_dvpp_cfg.m_outwidth = m_outimage_width;
        m_dvpp_cfg.m_outheight = m_outimage_height;
        m_dvpp_cfg.m_outbps = m_outimage_bps;

        mdc_camera_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        if (mdc_camera_ptr->open(so_path.c_str(), RTLD_LAZY) < 0) {
            AERROR << "load so failed";
            return -1;
        }
        if (mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_INIT_DVPP, &(m_dvpp_cfg)) < 0) {
            AERROR << "IOCTL_INIT_DVPP failed";
            return -1;
        }

        return 0;
    }

    void SetBGRImage(uint8_t* ptr, int size) {
        m_input_img.m_ptr = ptr;
        m_input_img.m_mode = 0;
        m_input_img.m_ptr_len = size;
        if (size != m_inimage_height * m_inimage_width * 3) {
            std::cout << "CMdcImgDvppInterface::SetBGRImage size is error size =" << size << std::endl;
        }

        mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_SET_INPUTIMG, &m_input_img);
    }

    void SetYUV420SPImage(uint8_t* ptr, int size) {
        m_input_img.m_ptr = ptr;
        m_input_img.m_mode = 0;
        m_input_img.m_ptr_len = size;
        if (size != m_inimage_height * m_inimage_width * 3 / 2) {
            std::cout << "CMdcImgDvppInterface::SetYUV420SPImage size is error size =" << size << std::endl;
        }

        mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_SET_INPUTIMG, &m_input_img);
    }

    void GetH264Image(std::vector<unsigned char>& frame_data) {
        std::cout << "GetH264Image <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
        mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_GET_OUTPUTIMG, &m_output_img);
        std::cout << "m_output_img.m_ptr_len = " << m_output_img.m_ptr_len << std::endl;
        frame_data.clear();
        frame_data.resize(m_output_img.m_ptr_len);
        memcpy(&frame_data[0], m_output_img.m_ptr, m_output_img.m_ptr_len);
    }

private:
    std::string so_path;
    int         m_inimage_width;
    int         m_inimage_height;
    int         m_outimage_width;
    int         m_outimage_height;
    int         m_outimage_bps;

    avos::idpware::imgdvpp::SImgDvppCfg                           m_dvpp_cfg;
    avos::idpware::imgdvpp::SImgDvppInData                        m_input_img;
    avos::idpware::imgdvpp::SImgDvppOutData                       m_output_img;
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_camera_ptr;

public:
};



class Ec {
public:
    Ec() = default;

    std::uint32_t ec_add(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            auto r = _ec_map.insert({ec, 0});
            if (!r.second) {
                return 0;
            }
            ptr = r.first;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec);
        }
        return ptr->second;
    }

    std::uint32_t ec_add(const std::string& ec, const std::string& ec_detail) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            _ec_map[ec] = 0;
        }
        ptr = _ec_map.find(ec);
        if (ptr == _ec_map.end()) {
            return 0;
        }

        ++ptr->second;
        if (ptr->second % 20 == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, ec_detail);
        }
        return ptr->second;
    }

    void ec_remove(const std::string& ec) {
        std::lock_guard<std::mutex> lg(_mtx);
        if (_ec_map.find(ec) != _ec_map.end()) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
            _ec_map.erase(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lg(_mtx);
        for (const auto& ec : _ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        _ec_map.clear();
    }

    void check_freq(double data_time, double threshold_freq) {
        double avg = 0;
        {
            std::lock_guard<std::mutex> lg(_mtx);
            _per_second_indicator.add(data_time * 1000, 1, 1);
            avg = _per_second_indicator.avg_per_second().first;
        }

        if (avg < threshold_freq) {
            ec_add(_ERRORCODE_FREQ, std::to_string(avg));
        } else {
            ec_remove(_ERRORCODE_FREQ);
        }
    }

private:
    std::mutex                              _mtx;
    std::map<std::string, std::uint32_t>    _ec_map;
    app_common::PerSecondIndicator<10, 100> _per_second_indicator;

    static const constexpr char* _ERRORCODE_CONFIG = "4022001";         // 配置参数读取失败
    static const constexpr char* _ERRORCODE_IMAGE_FORMAT = "4022002";   // 图像格式错误
    static const constexpr char* _ERRORCODE_ENCODING_PARAM = "4022003"; // 编码参数错误
    static const constexpr char* _ERRORCODE_CREATE_ENCODER = "4022004"; // 编码器创建失败
    static const constexpr char* _ERRORCODE_NO_DATA = "4022005";        // 无数据输入
    static const constexpr char* _ERRORCODE_FREQ = "4022006";           // 输入数据频率异常
    static const constexpr char* _ERRORCODE_ENCODE = "4022007";         // 编码失败
    static const constexpr char* _ERRORCODE_ENCODE_COST = "4022008";    // 编码耗时过长
};

// void XX264EncodingInMdc(const SensorImage&       sensor_Image_in, // 原始视频帧
//                         const AppVideoParameter& video_prameter,  // 视频编码控制参数
//                         VideoEncodedFrame&       encoded_frame,   // 编码后的视频帧
//                         VideoEncodingPerf&       encoding_perf) {
//     // add your code here
//     STATIC_DATA(std::string, _so_path, "");
//     STATIC_DATA(bool, _use_constant_parameter, false);
//     STATIC_DATA(std::int32_t, _constant_bps_kb, 1024);
//     STATIC_DATA(std::int32_t, _constant_fps, 30);
//     STATIC_DATA(std::int32_t, _constant_width, 1280);
//     STATIC_DATA(std::int32_t, _constant_height, 720);
//     STATIC_DATA(std::int32_t, _constant_gop, 30);
//     STATIC_DATA(std::int32_t, _using_bps_kb, 1024);
//     STATIC_DATA(std::int32_t, _using_fps, 30);
//     STATIC_DATA(std::int32_t, _using_frame_interval_us, 33); // 1000000/_using_fps
//     STATIC_DATA(std::int32_t, _using_width, 1280);
//     STATIC_DATA(std::int32_t, _using_height, 720);
//     STATIC_DATA(std::int32_t, _using_gop, 30);
//     STATIC_DATA(std::int32_t, _last_image_width, 0);
//     STATIC_DATA(std::int32_t, _last_image_height, 0);

//     STATIC_DATA(std::shared_ptr<std::atomic<double>>, _last_data_time, nullptr);

//     STATIC_DATA(bool, _init, false);
//     STATIC_DATA(std::shared_ptr<Ec>, _ec, nullptr);
//     STATIC_DATA(std::shared_ptr<app_common::TimeCostIndicator<100>>, _time_cost_indicator, nullptr);

//     STATIC_DATA(std::shared_ptr<CMdcImgDvppInterface>, mdc_mdcimg_interface_ptr, nullptr);

//     auto ptr = CustomStack::Instance();
//     if (!_init) {
//         _ec = std::make_shared<Ec>();
//         _time_cost_indicator = std::make_shared<app_common::TimeCostIndicator<100>>();
//         _last_data_time = std::make_shared<std::atomic<double>>(0);
//         if (nullptr == _ec || nullptr == _time_cost_indicator || nullptr == _last_data_time) {
//             AERROR << "cannot create ec manager";
//             // 一般不会失败，所以这里先不发送故障码，直接返回
//             return;
//         }

//         std::string prefix;
//         if (ptr->GetConfig("config_path_prefix", prefix) < 0) {
//             _ec->ec_add(_ERRORCODE_CONFIG, "config_path_prefix");
//             throw std::runtime_error("read local config config_path_prefix failed");
//         }
//         app_common::StringUtils::trim(prefix);
//         if (!prefix.empty() && prefix.back() != '/') {
//             prefix += "/";
//         }

//         auto full_path = prefix + "video/encoder/so_path";
//         if (!ptr->GetProjectConfigValue(full_path, _so_path) || _so_path.empty()) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         full_path = prefix + "video/encoder/use_constant_parameter";
//         if (!ptr->GetProjectConfigValue(full_path, _use_constant_parameter)) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         full_path = prefix + "video/encoder/constant_bps";
//         if (!ptr->GetProjectConfigValue(full_path, _constant_bps_kb) || _constant_bps_kb <= 0) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         full_path = prefix + "video/encoder/constant_fps";
//         if (!ptr->GetProjectConfigValue(full_path, _constant_fps) || _constant_fps <= 0) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         full_path = prefix + "video/encoder/constant_width";
//         if (!ptr->GetProjectConfigValue(full_path, _constant_width) || _constant_width <= 0) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         full_path = prefix + "video/encoder/constant_height";
//         if (!ptr->GetProjectConfigValue(full_path, _constant_height) || _constant_height <= 0) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         full_path = prefix + "video/encoder/constant_gop";
//         if (!ptr->GetProjectConfigValue(full_path, _constant_gop) || _constant_gop <= 0) {
//             _ec->ec_add(_ERRORCODE_CONFIG, full_path);
//             AERROR << "read project config " << full_path << "failed";
//             return;
//         }

//         _ec->ec_remove(_ERRORCODE_CONFIG);

//         AINFO << "so_path: " << _so_path;
//         AINFO << "use_constant_parameter: " << _use_constant_parameter;
//         AINFO << "constant_bps: " << _constant_bps_kb;
//         AINFO << "constant_fps: " << _constant_fps;
//         AINFO << "constant_width: " << _constant_width;
//         AINFO << "constant_height: " << _constant_height;
//         AINFO << "constant_gop: " << _constant_gop;

//         _init = true;
//     }

//     _last_data_time->store(sensor_Image_in.header.time);

//     if (!_use_constant_parameter && 0 == video_prameter.encoding_parameter.bps) {
//         return;
//     }

//     bool c = false;
//     if (nullptr == mdc_mdcimg_interface_ptr) {
//         c = true;
//     } else if ((!_use_constant_parameter && (_using_bps_kb != video_prameter.encoding_parameter.bps ||
//                 _using_width != video_prameter.encoding_parameter.width ||
//                 _using_height != video_prameter.encoding_parameter.height ||
//                 _using_fps != video_prameter.encoding_parameter.fps ||
//                 _using_gop != video_prameter.encoding_parameter.encoding_gop)) ||
//                _last_image_width != sensor_Image_in.width || _last_image_height != sensor_Image_in.height) {
//         AINFO << "recreate encoder, encoder: " << _using_bps_kb << ", " << _using_width << ", " << _using_height << ", "
//               << _using_fps << ", " << _using_gop;
//         AINFO << "recreate encoder, parameter: " << video_prameter.encoding_parameter.bps << ", "
//               << video_prameter.encoding_parameter.width << ", " << video_prameter.encoding_parameter.height << ", "
//               << video_prameter.encoding_parameter.fps << ", " << video_prameter.encoding_parameter.encoding_gop;
//         c = true;
//     }
//     if (c) {
//         auto ptr = CustomStack::Instance();
//         if (_use_constant_parameter) {
//             _using_bps_kb = _constant_bps_kb;
//             _using_fps = _constant_fps;
//             _using_width = _constant_width;
//             _using_height = _constant_height;
//             _using_gop = _constant_gop;
//         } else {
//             _using_bps_kb = video_prameter.encoding_parameter.bps;
//             _using_fps = video_prameter.encoding_parameter.fps;
//             _using_width = video_prameter.encoding_parameter.width;
//             _using_height = video_prameter.encoding_parameter.height;
//             _using_gop = video_prameter.encoding_parameter.encoding_gop;
//         }
//         _last_image_width = sensor_Image_in.width;
//         _last_image_height = sensor_Image_in.height;

//         if (_using_bps_kb <= 0) {
//             AERROR << "invalid parameter bps: " << _using_bps_kb;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "bps=" + std::to_string(_using_bps_kb));
//             return;
//         }
//         if (_using_fps <= 0) {
//             AERROR << "invalid parameter fps: " << _using_fps;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "fps=" + std::to_string(_using_fps));
//             return;
//         }
//         if (_using_width <= 0) {
//             AERROR << "invalid parameter output width: " << _using_width;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "output_width=" + std::to_string(_using_width));
//             return;
//         }
//         if (_using_height <= 0) {
//             AERROR << "invalid parameter output height: " << _using_height;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "output_height=" + std::to_string(_using_height));
//             return;
//         }
//         if (_using_gop <= 0) {
//             AERROR << "invalid parameter gop: " << _using_gop;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "gop=" + std::to_string(_using_gop));
//             return;
//         }
//         if (_last_image_width <= 0) {
//             AERROR << "invalid parameter input width: " << _last_image_width;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "input_height=" + std::to_string(_last_image_width));
//             return;
//         }
//         if (_last_image_height <= 0) {
//             AERROR << "invalid parameter input height: " << _last_image_height;
//             _ec->ec_add(_ERRORCODE_ENCODING_PARAM, "input_width=" + std::to_string(_last_image_height));
//             return;
//         }
//         _ec->ec_remove(_ERRORCODE_ENCODING_PARAM);
//         _using_frame_interval_us = 1000000.0 / _using_fps;

//         std::filesystem::path cfg_so_path(_so_path);
//         if (!cfg_so_path.is_absolute()) {
//             cfg_so_path = ptr->GetNodeConfigPath();
//             cfg_so_path /= _so_path;
//         }

//         mdc_mdcimg_interface_ptr = std::make_shared<CMdcImgDvppInterface>(
//                                        cfg_so_path.string(), _last_image_width, _last_image_height, _using_width, _using_height, _using_bps_kb);
//         if (nullptr == mdc_mdcimg_interface_ptr) {
//             AERROR << "create mdc image dvpp interface failed";
//             _ec->ec_add(_ERRORCODE_CREATE_ENCODER);
//             return;
//         }
//         if (mdc_mdcimg_interface_ptr->Init() < 0) {
//             AERROR << "init mdc image dvpp interface failed";
//             _ec->ec_add(_ERRORCODE_CREATE_ENCODER);
//             return;
//         }
//         _ec->ec_remove(_ERRORCODE_CREATE_ENCODER);

//         std::thread([_ec, _last_data_time]() {
//             while (true) {
//                 auto now = app_common::DatetimeUtil::current_second_double();
//                 auto last = _last_data_time->load();
//                 if (fabs(now - last) >= 2.0) {
//                     _ec->ec_add(_ERRORCODE_NO_DATA);
//                 } else {
//                     _ec->ec_remove(_ERRORCODE_NO_DATA);
//                 }
//                 std::this_thread::sleep_for(std::chrono::milliseconds(100));
//             }
//         }).detach();

//         return;
//     }

//     // ptr->GetConfig("num_threads", _num_threads);
//     // std::cout << "num_threads: " << _num_threads << std::endl;

//     // auto sensor_Image = sensor_Image_in;
//     // zero_shm_mem_ptr->SyncDataVectorInfo(sensor_Image);
//     // if (sensor_Image.data.empty()) {
//     //     std::cout << "sensor_image.data is empty" << std::endl;
//     //     return;
//     // }

//     // mdc_mdcimg_interface_ptr->SetBGRImage((uint8_t *)sensor_Image.data.data(),sensor_Image.data.size());

//     _ec->check_freq(sensor_Image_in.header.time, _using_fps);

//     auto s = app_common::DatetimeUtil::current_microsecond();
//     mdc_mdcimg_interface_ptr->SetYUV420SPImage((uint8_t*)sensor_Image_in.data.data(), sensor_Image_in.data.size());
//     mdc_mdcimg_interface_ptr->GetH264Image(encoded_frame.frame_data);
//     auto e = app_common::DatetimeUtil::current_microsecond();
//     _time_cost_indicator->add(e - s);
//     auto avg_time = _time_cost_indicator->avg();
//     if (avg_time > _using_frame_interval_us) {
//         AERROR << "time cost too long: " << avg_time;
//         _ec->ec_add(_ERRORCODE_ENCODE_COST);
//     } else {
//         _ec->ec_remove(_ERRORCODE_ENCODE_COST);
//     }

//     encoded_frame.header = sensor_Image_in.header;
//     encoded_frame.encoding = 0;
//     encoded_frame.fps = 30;
//     encoded_frame.pts = 30;
//     encoded_frame.key_frame = 1;
//     encoded_frame.frame_size = encoded_frame.frame_data.size();
//     if (encoded_frame.frame_size > 64) {
//         for (size_t i = 0; i < 64; i++) {
//             printf("%02x ", encoded_frame.frame_data[i]);
//         }
//         printf("\n");
//     }

//     AINFO << "encoded_frame.frame_data.size() = " << encoded_frame.frame_data.size();
//     AINFO << "encoded_frame.fps = " << encoded_frame.fps;

//     AINFO << "曝光时间：" << std::fixed << std::setprecision(6) << sensor_Image_in.header.time;
// }
