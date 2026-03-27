#pragma once

#include <fmt/format.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "EC204.hpp"
#include "H265RotatingFileReceiver.hpp"
#include "PerfIndicators.hpp"
#include "SafeQueue.hpp"
#include "VideoEditYUV420UV12.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msgimagedatalist.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "dvpp_interface/cidpabstractplatforminterface.h"
#include "dvpp_interface/dvpp_protocol/cusermdc610funcinterface.h"
#include "dvpp_interface/dvpp_protocol/imgprocessplatform2usrprotocol.h"
#include "image_data_interface/image_data_interface.hpp"
#include "monitor/faulthandle_api.hpp"
#include "opencv2/freetype.hpp"
#include "yuyv420uv12.hpp"

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
            ApError("status_recorder") << "load so failed";
            return -1;
        }
        if (mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_INIT_DVPP, &(m_dvpp_cfg)) < 0) {
            ApError("status_recorder") << "IOCTL_INIT_DVPP failed";
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
        m_output_img = {};
        mdc_camera_ptr->ioctl(avos::idpware::imgdvpp::IOCTL_GET_OUTPUTIMG, &m_output_img);
        ApInfo("status_recorder") << fmt::format("IOCTL_GET_OUTPUTIMG, ptr: {}, len: {}",
                                                 static_cast<void*>(m_output_img.m_ptr), m_output_img.m_ptr_len);
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
};

class RecorderH265 {
public:
    RecorderH265() {
        std::string font_file_name;
        auto        ptr = CustomStack::Instance();

        // ── 字体文件路径 ──────────────────────────────────────────────────
        std::string full_name = "system/recorder/video/compose/ttc_filename";
        if (!ptr->GetProjectConfigValue(full_name, font_file_name) || font_file_name.empty()) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << font_file_name;

        _ft2 = cv::freetype::createFreeType2();
        if (nullptr == _ft2) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_FREETYPE_LOAD_FONT, "createFreeType2 returned null");
            throw std::runtime_error("create freetype failed");
        }
        try {
            _ft2->loadFontData(ptr->GetProjectConfigPath() + "/" + font_file_name, 0);
        } catch (const std::exception& e) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_FREETYPE_LOAD_FONT, e.what());
            throw std::runtime_error(fmt::format("freetype loadFontData failed: {}", e.what()));
        }
        FaultHandle::FaultApi::Instance()->ResetFaultCode(EC204::_ERRORCODE_FREETYPE_LOAD_FONT);

        // ── 采样间隔 ──────────────────────────────────────────────────────
        full_name = "system/recorder/video/resample_interval";
        if (!ptr->GetProjectConfigValue(full_name, _resample_interval) || 0 == _resample_interval ||
            _resample_interval > 30) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _resample_interval;
        _resampled_fps = 30 / _resample_interval;
        _resampled_interval_us = 1e6 / _resampled_fps;

        // ── 摄像头名称（允许为空） ─────────────────────────────────────────
        full_name = "system/recorder/video/compose/left_front";
        if (!ptr->GetProjectConfigValue(full_name, _left_front_name)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _left_front_name;

        full_name = "system/recorder/video/compose/right_front";
        if (!ptr->GetProjectConfigValue(full_name, _right_front_name)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _right_front_name;

        full_name = "system/recorder/video/compose/left_back";
        if (!ptr->GetProjectConfigValue(full_name, _left_back_name)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _left_back_name;

        full_name = "system/recorder/video/compose/right_back";
        if (!ptr->GetProjectConfigValue(full_name, _right_back_name)) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _right_back_name;

        // ── 编码器参数 ────────────────────────────────────────────────────
        full_name = "system/recorder/video/encoder/so_path";
        if (!ptr->GetProjectConfigValue(full_name, _so_path) || _so_path.empty()) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _so_path;

        full_name = "system/recorder/video/encoder/constant_bps";
        if (!ptr->GetProjectConfigValue(full_name, _constant_bps_kb) || _constant_bps_kb <= 0) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _constant_bps_kb;

        full_name = "system/recorder/video/encoder/constant_width";
        if (!ptr->GetProjectConfigValue(full_name, _constant_width) || _constant_width <= 0) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _constant_width;

        full_name = "system/recorder/video/encoder/constant_height";
        if (!ptr->GetProjectConfigValue(full_name, _constant_height) || _constant_height <= 0) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _constant_height;

        full_name = "system/recorder/video/encoder/constant_gop";
        if (!ptr->GetProjectConfigValue(full_name, _constant_gop) || _constant_gop <= 0) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR, full_name);
            throw std::runtime_error(fmt::format("read project config {} failed", full_name));
        }
        ApInfo("status_recorder") << full_name << ": " << _constant_gop;

        FaultHandle::FaultApi::Instance()->ResetFaultCode(EC204::_ERRORCODE_PARAMETER_ERROR);

        // ── 摄像头名称 → EC204 错误码映射（仅对非空名称建立映射） ────────────
        if (!_left_front_name.empty()) {
            _no_data_ec_map[_left_front_name]    = EC204::_ERRORCODE_NO_DATA_LEFT_FRONT;
            _image_format_ec_map[_left_front_name] = EC204::_ERRORCODE_IMAGE_FORMAT_LEFT_FRONT;
        }
        if (!_right_front_name.empty()) {
            _no_data_ec_map[_right_front_name]    = EC204::_ERRORCODE_NO_DATA_RIGHT_FRONT;
            _image_format_ec_map[_right_front_name] = EC204::_ERRORCODE_IMAGE_FORMAT_RIGHT_FRONT;
        }
        if (!_left_back_name.empty()) {
            _no_data_ec_map[_left_back_name]    = EC204::_ERRORCODE_NO_DATA_LEFT_BACK;
            _image_format_ec_map[_left_back_name] = EC204::_ERRORCODE_IMAGE_FORMAT_LEFT_BACK;
        }
        if (!_right_back_name.empty()) {
            _no_data_ec_map[_right_back_name]    = EC204::_ERRORCODE_NO_DATA_RIGHT_BACK;
            _image_format_ec_map[_right_back_name] = EC204::_ERRORCODE_IMAGE_FORMAT_RIGHT_BACK;
        }

        // ── 编码器初始化 ──────────────────────────────────────────────────
        _img_process_ptr = avos::idpware::CUserMdc610FuncInterface::Instance();

        _canvas = YUYV420UV12::white_img(CANVAS_WIDTH, CANVAS_HEIGHT);
        if (nullptr == _canvas) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_SHM_ALLOC_FAILED,
                                                            "create white canvas failed");
            throw std::runtime_error("create white image failed");
        }

        std::filesystem::path cfg_so_path(_so_path);
        if (!cfg_so_path.is_absolute()) {
            cfg_so_path = ptr->GetNodeConfigPath();
            cfg_so_path /= "config";
            cfg_so_path /= _so_path;
        }

        _mdc_mdcimg_interface_ptr = std::make_shared<CMdcImgDvppInterface>(
            cfg_so_path.string(), CANVAS_WIDTH, CANVAS_HEIGHT, _constant_width, _constant_height, _constant_bps_kb);
        if (nullptr == _mdc_mdcimg_interface_ptr) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_CREATE_ENCODER);
            throw std::runtime_error("create mdc image dvpp interface failed");
        }
        if (_mdc_mdcimg_interface_ptr->Init() < 0) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(EC204::_ERRORCODE_CREATE_ENCODER);
            throw std::runtime_error("mdc image dvpp interface Init failed");
        }
        FaultHandle::FaultApi::Instance()->ResetFaultCode(EC204::_ERRORCODE_CREATE_ENCODER);

        _receiver = std::make_shared<H265RotatingFileReceiver>();

        _thread = std::thread([this]() { loop(); });
    }

    ~RecorderH265() {
        _stopped = true;
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    // ── 公开接口 ────────────────────────────────────────────────────────

    void add(std::shared_ptr<ara::adsfi::MsgImageDataList> n) {
        _queues.push(std::move(n));
    }

    void update_vehicle_info(const ara::adsfi::VehicleInformation& msg) {
        std::lock_guard lg(_mtx);
        _shift_position = msg.vehicle_act_state.shift_position;
        _drive_mode     = msg.vehicle_act_state.drive_mode;
        _speed_virtual  = msg.vehicle_act_state.speed;
    }

    void update_location(const ara::adsfi::MsgHafLocation& msg) {
        std::lock_guard lg(_mtx);
        _location_status = msg.locationState;
        _lat             = msg.llh.y; // llh: latitude(x), longitude(y), height(z)
        _lon             = msg.llh.x;
    }

    void update_fault_data(const ara::adsfi::FaultData& msg) {
        std::lock_guard lg(_mtx);
        _fault_level = msg.fault_level;
    }

    void update_vehicle_control(const ara::adsfi::VehicleActControl& msg) {
        std::lock_guard lg(_mtx);
        _speed_expected = msg.lon_control.target_speed;
    }

private:
    void loop() {
        pthread_setname_np(pthread_self(), "h265_file_writer");
        while (!_stopped) {
            std::shared_ptr<ara::adsfi::MsgImageDataList> image_list;
            if (!_queues.try_move_pop(image_list)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            if (_resample_interval > 0 && _resample_counter < _resample_interval) {
                ++_resample_counter;
                continue;
            }
            _resample_counter = 1;

            dvpp::sync(*image_list);

            ApInfo("status_recorder") << fmt::format(
                "image_list: {:20.8f}",
                app_common::DatetimeUtil::double_time_from_sec_nsec(image_list->header.timestamp));
            for (const auto& img : image_list->images) {
                ApInfo("status_recorder") << fmt::format(
                    "image, timestamp: {:20.8f}, frameID: {}, width: {}, height: {}, datasize: {}, bufftype: {}, "
                    "imagetype: {}, rawdata.size(): {}",
                    app_common::DatetimeUtil::double_time_from_sec_nsec(img.timestamp), img.frameID, img.width,
                    img.height, img.dataSize, static_cast<std::int32_t>(img.bufferType),
                    static_cast<std::int32_t>(img.imageType), img.rawData.size());
            }

            try {
                _canvas->resetToWhite();

                auto s    = app_common::DatetimeUtil::current_microsecond();
                auto yuv  = compose(image_list);
                auto e    = app_common::DatetimeUtil::current_microsecond();
                auto cost = e - s;
                ApInfo("status_recorder") << "compose, start: " << s << ", end: " << e << ", cost: " << cost;

                // compose 产物异常（空指针）
                if (!yuv) {
                    EC204::Instance().ec_add(EC204::_ERRORCODE_DATARESULT_ERROR);
                    continue;
                }
                EC204::Instance().ec_remove(EC204::_ERRORCODE_DATARESULT_ERROR);

                s    = app_common::DatetimeUtil::current_microsecond();
                auto frame = encode(yuv);
                e    = app_common::DatetimeUtil::current_microsecond();
                cost = e - s;
                ApInfo("status_recorder") << "encode, start: " << s << ", end: " << e << ", cost: " << cost;

                _time_cost_indicator.add(cost);
                auto avg_time = _time_cost_indicator.avg();
                ApInfo("status_recorder") << fmt::format("average encoding cost: {:20.8f}", avg_time);
                if (avg_time > _resampled_interval_us) {
                    ApError("status_recorder") << "time cost too long: " << avg_time;
                    EC204::Instance().ec_add(EC204::_ERRORCODE_ENCODE_COST, std::to_string(avg_time));
                } else {
                    EC204::Instance().ec_remove(EC204::_ERRORCODE_ENCODE_COST);
                }

                s    = app_common::DatetimeUtil::current_microsecond();
                record(frame);
                e    = app_common::DatetimeUtil::current_microsecond();
                cost = e - s;
                ApInfo("status_recorder") << "record, start: " << s << ", end: " << e << ", cost: " << cost;
            } catch (const std::exception& e) {
                ApError("status_recorder") << e.what();
            }

            safe_release(*image_list);
        }
    }

    // 从 image_list 中按 frameID 取出单路图像；
    // 数据缺失时上报 _ERRORCODE_NO_DATA_*，格式非法时上报 _ERRORCODE_IMAGE_FORMAT_*，
    // 两者恢复时对应 ec_remove。
    std::shared_ptr<YUYV420UV12> get_image_data(std::shared_ptr<ara::adsfi::MsgImageDataList> image_list,
                                                const std::string&                            name) {
        auto it = std::find_if(image_list->images.begin(), image_list->images.end(),
                               [&](const ara::adsfi::MsgImageData& img) { return img.frameID == name; });

        // ── 无数据 ────────────────────────────────────────────────────────
        if (it == image_list->images.end() || 0 == it->dataSize || it->rawData.empty()) {
            auto ec_it = _no_data_ec_map.find(name);
            if (ec_it != _no_data_ec_map.end()) {
                EC204::Instance().ec_add(ec_it->second);
            }
            return YUYV420UV12::white_img(FRAME_WIDTH, FRAME_HEIGHT);
        }
        // 数据恢复
        {
            auto ec_it = _no_data_ec_map.find(name);
            if (ec_it != _no_data_ec_map.end()) {
                EC204::Instance().ec_remove(ec_it->second);
            }
        }

        // ── 图像格式检查：rawData 大小 < YUV420SP 所需（width × height × 3/2） ──
        const std::size_t expected_size =
            static_cast<std::size_t>(it->width) * static_cast<std::size_t>(it->height) * 3 / 2;
        if (it->rawData.size() < expected_size) {
            auto ec_it = _image_format_ec_map.find(name);
            if (ec_it != _image_format_ec_map.end()) {
                EC204::Instance().ec_add(ec_it->second,
                              fmt::format("rawData.size:{} < expected:{}", it->rawData.size(), expected_size));
            }
            return YUYV420UV12::white_img(FRAME_WIDTH, FRAME_HEIGHT);
        }
        // 格式恢复
        {
            auto ec_it = _image_format_ec_map.find(name);
            if (ec_it != _image_format_ec_map.end()) {
                EC204::Instance().ec_remove(ec_it->second);
            }
        }

        return std::make_shared<YUYV420UV12>(it->width, it->height, it->rawData.data());
    }

    std::shared_ptr<YUYV420UV12> compose(std::shared_ptr<ara::adsfi::MsgImageDataList> image_list) noexcept(true) {
        std::vector<std::shared_ptr<YUYV420UV12>> crops(4);
        crops[0] = get_image_data(image_list, _left_front_name);
        crops[1] = get_image_data(image_list, _right_front_name);
        crops[2] = get_image_data(image_list, _left_back_name);
        crops[3] = get_image_data(image_list, _right_back_name);
        std::vector<std::string> camear_names = {_left_front_name, _right_front_name, _left_back_name,
                                                 _right_back_name};
        std::uint32_t            index = 0;
        for (const auto& crop : crops) {
            avos::idpware::SImgProcessInfo mem_info;
            avos::idpware::SImgProcess     param;
            std::string                    process_name;
            if (FRAME_WIDTH == crop->width() && FRAME_HEIGHT == crop->height()) {
                process_name = "crop_" + camear_names[index];
            } else {
                process_name = "resize_" + camear_names[index];
            }
            param.SetProcessName(process_name);
            param.InitInputImage(crop->get_data(), crop->size(), crop->width(), crop->height(),
                                 avos::idpware::BUF_DVPP_TYPE, avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            param.InitOutputImage(_canvas->get_data(), _canvas->size(), _canvas->width(), _canvas->height(),
                                  avos::idpware::BUF_DVPP_TYPE, avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            param.SetResizePaste(crop->width(), crop->height(), FRAME_WIDTH, FRAME_HEIGHT, 0 + index % 2 * FRAME_WIDTH,
                                 0 + index / 2 * FRAME_HEIGHT);
            _img_process_ptr->idpImgProcess(param);
            ++index;
        }

        cv::Mat mat = VideoEditYUV420UV12::YUYV420UV12ToMat(_canvas);
        draw(mat);
        return VideoEditYUV420UV12::MatToYUYV420UV12(mat);
    }

    void draw(cv::Mat& img) {
        std::string error_level;
        std::string gear_value;
        std::string virtual_speed_value;
        std::string expected_speed_value;
        std::string mode_value;
        std::string local_status;
        std::string lon_value;
        std::string lat_value;
        std::string time_value;

        {
            std::lock_guard lg(_mtx);
            error_level = std::to_string(_fault_level);
            gear_value  = "P";
            if (_shift_position == 1) {
                gear_value = "R";
            } else if (_shift_position == 2) {
                gear_value = "N";
            } else if (_shift_position == 3) {
                gear_value = "D";
            }
            virtual_speed_value  = fmt::format("{:.3f}", _speed_virtual);
            expected_speed_value = fmt::format("{:.3f}", _speed_expected);

            mode_value = "自主模式";
            if (_drive_mode == 0) {
                mode_value = "手动模式";
            }

            local_status = std::to_string(_location_status);
            lon_value    = fmt::format("{:.8f}", _lon);
            lat_value    = fmt::format("{:.8f}", _lat);

            auto          now         = std::chrono::system_clock::now();
            std::time_t   now_time_t  = std::chrono::system_clock::to_time_t(now);
            std::tm       now_tm{};
            if (::localtime_r(&now_time_t, &now_tm) != nullptr) {
                char time_buf[32] = {};
                std::snprintf(time_buf, sizeof(time_buf), "%04d%02d%02d %02d:%02d:%02d", now_tm.tm_year + 1900,
                              now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
                time_value = time_buf;
            }
        }

        cv::Scalar  color = cv::Scalar(0, 0, 255);
        std::string erro_str = "故障:";
        _ft2->putText(img, erro_str, cv::Point2f(10, 50), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, error_level, cv::Point2f(100, 50), 30, color, -1, cv::LINE_AA, true);

        std::string gear_ = "档位:";
        _ft2->putText(img, gear_, cv::Point2f(10, 80), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, gear_value, cv::Point2f(100, 80), 30, color, -1, cv::LINE_AA, true);

        std::string speed_str = "车速:";
        _ft2->putText(img, speed_str, cv::Point2f(10, 110), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, virtual_speed_value, cv::Point2f(100, 110), 30, color, -1, cv::LINE_AA, true);
        speed_str = "期速:";
        _ft2->putText(img, speed_str, cv::Point2f(10, 140), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, expected_speed_value, cv::Point2f(100, 140), 30, color, -1, cv::LINE_AA, true);

        std::string mode_str = "模式:";
        _ft2->putText(img, mode_str, cv::Point2f(10, 170), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, mode_value, cv::Point2f(100, 170), 30, color, -1, cv::LINE_AA, true);

        std::string loc_str = "定位:";
        _ft2->putText(img, loc_str, cv::Point2f(10, 200), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, local_status, cv::Point2f(100, 200), 30, color, -1, cv::LINE_AA, true);

        std::string lon_str = "经度:";
        _ft2->putText(img, lon_str, cv::Point2f(10, 230), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, lon_value, cv::Point2f(100, 230), 30, color, -1, cv::LINE_AA, true);

        std::string lat_str = "纬度:";
        _ft2->putText(img, lat_str, cv::Point2f(10, 260), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, lat_value, cv::Point2f(100, 260), 30, color, -1, cv::LINE_AA, true);

        std::string time_str = "日期:";
        _ft2->putText(img, time_str, cv::Point2f(10, 290), 25, color, -1, cv::LINE_AA, true);
        _ft2->putText(img, time_value, cv::Point2f(100, 290), 30, color, -1, cv::LINE_AA, true);
    }

    // 对 compose 后的 YUV420SP 图像进行 H.265 编码；
    // 编码返回空帧时上报 _ERRORCODE_ENCODING_ERROR，成功时恢复。
    std::vector<std::uint8_t> encode(std::shared_ptr<YUYV420UV12> img) {
        ApInfo("status_recorder") << "yuv420 size: " << img->size();
        std::vector<std::uint8_t> frame_data;
        _mdc_mdcimg_interface_ptr->SetYUV420SPImage(img->get_data(), img->size());
        _mdc_mdcimg_interface_ptr->GetH265Image(frame_data);

        if (frame_data.empty()) {
            EC204::Instance().ec_add(EC204::_ERRORCODE_ENCODING_ERROR);
        } else {
            EC204::Instance().ec_remove(EC204::_ERRORCODE_ENCODING_ERROR);
        }

        return frame_data;
    }

    void record(const std::vector<std::uint8_t>& frame) {
        _receiver->push(frame.data(), frame.size());
    }

private:
    // ── 常量 ──────────────────────────────────────────────────────────────
    static constexpr const std::uint32_t CANVAS_WIDTH  = 1920U;
    static constexpr const std::uint32_t CANVAS_HEIGHT = 1080U;
    static constexpr const std::uint32_t FRAME_WIDTH   = CANVAS_WIDTH / 2;
    static constexpr const std::uint32_t FRAME_HEIGHT  = CANVAS_HEIGHT / 2;

    // ── 配置 ──────────────────────────────────────────────────────────────
    std::uint32_t _resample_interval{};
    std::uint32_t _resampled_fps{};
    double        _resampled_interval_us{};

    std::string  _left_front_name;
    std::string  _right_front_name;
    std::string  _left_back_name;
    std::string  _right_back_name;
    std::string  _so_path;
    std::int32_t _constant_bps_kb{};
    std::int32_t _constant_width{};
    std::int32_t _constant_height{};
    std::int32_t _constant_gop{};

    // ── 运行时状态 ────────────────────────────────────────────────────────
    std::mutex   _mtx;
    std::int32_t _fault_level{0};
    std::int32_t _drive_mode{};
    std::int32_t _shift_position{};
    std::int32_t _location_status{};
    double       _speed_virtual{};
    double       _speed_expected{};
    double       _lat{};
    double       _lon{};

    app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageDataList>> _queues;
    std::uint32_t                                                        _resample_counter{1};

    // ── 摄像头名称 → 错误码映射（构造函数中初始化） ─────────────────────────
    std::map<std::string, const char*> _no_data_ec_map;
    std::map<std::string, const char*> _image_format_ec_map;

    // ── 编码器相关 ────────────────────────────────────────────────────────
    std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> _img_process_ptr;
    cv::Ptr<cv::freetype::FreeType2>                         _ft2;
    std::shared_ptr<YUYV420UV12>                             _canvas;
    std::shared_ptr<CMdcImgDvppInterface>                    _mdc_mdcimg_interface_ptr;
    app_common::TimeCostIndicator<100>                       _time_cost_indicator;
    std::shared_ptr<H265RotatingFileReceiver>                _receiver;

    std::atomic_bool _stopped{false};
    std::thread      _thread;
};
