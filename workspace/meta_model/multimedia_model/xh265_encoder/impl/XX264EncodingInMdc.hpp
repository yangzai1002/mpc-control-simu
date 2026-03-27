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
#include "ap_log/ap_log_interface.h"
#include "config_loader/custom_stack.h"
#include "dvpp_interface/cidpabstractplatforminterface.h"
#include "dvpp_interface/dvpp_protocol/imgprocessplatform2usrprotocol.h"
#include "monitor/faulthandle_api.hpp"
#include "shm_util/zero_copy_shm_mem.h"

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
        m_dvpp_cfg.m_outputtype = avos::idpware::imgdvpp::IMGTYPE_H265;
        m_dvpp_cfg.m_inwidth = m_inimage_width;
        m_dvpp_cfg.m_inheight = m_inimage_height;
        m_dvpp_cfg.m_outwidth = m_outimage_width;
        m_dvpp_cfg.m_outheight = m_outimage_height;
        m_dvpp_cfg.m_outbps = m_outimage_bps;

        mdc_camera_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        if (mdc_camera_ptr->open(so_path.c_str(), RTLD_LAZY) < 0) {
            ApError("h265_encoder") << "load so failed";
            return -1;
        }
        if (mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_INIT_DVPP, &(m_dvpp_cfg)) < 0) {
            ApError("h265_encoder") << "IOCTL_INIT_DVPP failed";
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

    void GetH265Image(std::vector<unsigned char>& frame_data) {
        std::cout << "GetH265Image <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
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

    double check_freq(double data_time, double threshold_freq) {
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

        return avg;
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
