#pragma once

#include <string>
#include "ara/adsfi/impl_type_idpsensorimage.h"
#include "ara/adsfi/impl_type_idpsensorimagelist.h"
#include "custom_stack.h"
#include "adsfi_interface/src/dvpp_protocol/cusermdc610funcinterface.h"
#include "zero_copy_shm_mem.h"
#include "time_cost.hpp"

namespace image_list_data_recv_dvpp {
    inline void safe_release_image(ara::adsfi::IdpSensorImage &data) {
        if (!check_release(data)) {
            return;
        }
        std::cout << "[ZERO] safe_release(T &SensorImage)" << std::endl;
        release(data.data);
    }

    inline void safe_release_image_list(ara::adsfi::IdpSensorImageList &data) {
        std::cout << "[ZERO] safe_release(T &SensorImageList)" << data.images.size() << std::endl;
        for (size_t i = 0; i < data.images.size(); i++) {
            safe_release<ara::adsfi::IdpSensorImage>(data.images[i]);

            std::cout << "[ZERO] safe_release(T &SensorImageList) " << i << " i " << typeid(data.images[i]).name()
                      << std::endl;
        }
        std::cout << "[ZERO] safe_release(T &SensorImageList)" << data.images.size() << std::endl;
    }

    // 注册发生在 b.so 加载时
    inline struct Register {
        Register() {
            register_safe_release<ara::adsfi::IdpSensorImageList>(safe_release_image_list);
            register_safe_release<ara::adsfi::IdpSensorImage>(safe_release_image);
        }
    } _reg;
} // namespace image_list_data_recv_dvpp

namespace dvpp {

    class CameraClient {
    private:
        std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> _client;

        // 私有构造函数
        CameraClient() {
            _client = avos::idpware::CUserMdc610FuncInterface::Instance();
            _client->idpCamClientInit();
        }

    public:
        // 禁止拷贝与赋值
        CameraClient(const CameraClient &) = delete;
        CameraClient &operator=(const CameraClient &) = delete;

        // 单例获取接口
        static CameraClient &instance() {
            static CameraClient inst;
            return inst;
        }

        int query(const std::string &name, avos::idpware::SCameraClientData &data, bool block = false) {
            memset(&data, 0, sizeof(data));
            // 拷贝相机名
            size_t copy_len = std::min(name.size(), sizeof(data.m_cameara_name.camera_name) - 1);
            memcpy(data.m_cameara_name.camera_name, name.c_str(), copy_len);
            data.m_cameara_name.camera_name_len = static_cast<uint32_t>(copy_len);
            data.m_block = block;
            // 调用接口
            auto ret = _client->idpGetCamData(data);
            std::cout << "idpGetCamData return: " << ret << ", cam name: " << name << std::endl;
            return ret;
        }
    };

    static inline void sync(ara::adsfi::IdpSensorImage &image) {
        avos::idpware::SCameraClientData dvpp_image{};
        CameraClient::instance().query(image.header.data_name, dvpp_image);
        image.header.time = dvpp_image.m_camera_data.m_stamp;
        image.height = dvpp_image.m_camera_data.height;
        image.width = dvpp_image.m_camera_data.width;
        image.encoding = std::to_string(dvpp_image.m_camera_data.type);
        if (image.shm_buffer_info.buffer_size != dvpp_image.m_camera_data.m_ptr_len ||
            !dvpp_image.m_camera_data.m_ptr) {
            std::ostringstream oss;
            oss << "[sync!!!!!!] sync error!\n"
                << "  data_name                          : " << image.header.data_name << "\n"
                << "  shm_buffer_info.buffer_size        : " << image.shm_buffer_info.buffer_size << "\n"
                << "  dvpp_image.m_camera_data.m_ptr_len : " << dvpp_image.m_camera_data.m_ptr_len << "\n"
                << "  width                              : " << image.width << "\n"
                << "  height                             : " << image.height << "\n"
                << "  encoding(type)                     : " << image.encoding << "\n"
                << "  stamp                              : " << dvpp_image.m_camera_data.m_stamp << "\n"
                << "  dvpp_image.m_camera_data.m_ptr     : "
                << static_cast<const void *>(dvpp_image.m_camera_data.m_ptr) << "\n";
            std::cerr << oss.str();
            return;
        }
        zero_copy_data_vector(image, dvpp_image.m_camera_data.m_ptr, image.shm_buffer_info.buffer_size);
    }

    static inline void sync(ara::adsfi::IdpSensorImageList &image_list) {
        for (size_t i = 0; i < image_list.images.size(); i++) {
            dvpp::sync(image_list.images[i]);
        }
    }
} // namespace dvpp
