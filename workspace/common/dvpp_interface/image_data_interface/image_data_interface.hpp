#pragma once

#include <string>
#include "ara/adsfi/impl_type_msgimagedatalist.h"

#include "dvpp_protocol/cusermdc610funcinterface.h"
#include "shm_util/zero_copy_shm_mem.h"

namespace image_list_data_recv_dvpp {
inline void safe_release_image(ara::adsfi::MsgImageData &data) {
    if (!check_release(data)) {
        return;
    }
    std::cout << "[ZERO] safe_release(T &SensorImage)" << std::endl;
    release(data.rawData);
}

inline void safe_release_image_list(ara::adsfi::MsgImageDataList &data) {
    std::cout << "[ZERO] safe_release(T &SensorImageList)" << data.images.size() << std::endl;
    for (size_t i = 0; i < data.images.size(); i++) {
        safe_release<ara::adsfi::MsgImageData>(data.images[i]);

        std::cout << "[ZERO] safe_release(T &SensorImageList) " << i << " i " << typeid(data.images[i]).name()
                  << std::endl;
    }
    std::cout << "[ZERO] safe_release(T &SensorImageList)" << data.images.size() << std::endl;
}

// 注册发生在 b.so 加载时
inline struct Register {
    Register() {
        register_safe_release<ara::adsfi::MsgImageDataList>(safe_release_image_list);
        register_safe_release<ara::adsfi::MsgImageData>(safe_release_image);
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

static inline void sync(ara::adsfi::MsgImageData &image) {
    avos::idpware::SCameraClientData dvpp_image{};
    // std::cout<<"image.frameID "<<image.frameID<<"\n";
    CameraClient::instance().query(image.frameID, dvpp_image);
    image.timestamp.sec = (unsigned int)dvpp_image.m_camera_data.m_stamp;
    image.timestamp.nsec = (dvpp_image.m_camera_data.m_stamp-image.timestamp.sec)*1e9;
    image.height = dvpp_image.m_camera_data.height;
    image.width = dvpp_image.m_camera_data.width;
    // image.encoding = std::to_string(dvpp_image.m_camera_data.type);
    if (image.dataSize != dvpp_image.m_camera_data.m_ptr_len ||
            !dvpp_image.m_camera_data.m_ptr) {
        std::ostringstream oss;
        // oss << "[sync!!!!!!] sync error!\n"
        //     << "  data_name                          : " << image.frameID << "\n"
        //     << "  dataSize        : " << image.dataSize << "\n"
        //     << "  dvpp_image.m_camera_data.m_ptr_len : " << dvpp_image.m_camera_data.m_ptr_len << "\n"
        //     << "  width                              : " << image.width << "\n"
        //     << "  height                             : " << image.height << "\n"
        //     << "  encoding(type)                     : " << std::to_string(dvpp_image.m_camera_data.type) << "\n"
        //     << "  stamp                              : " << dvpp_image.m_camera_data.m_stamp << "\n"
        //     << "  dvpp_image.m_camera_data.m_ptr     : "
        //     << static_cast<const void *>(dvpp_image.m_camera_data.m_ptr) << "\n";
        std::cerr << oss.str();
        return;
    }
    zero_copy_data_vector(image, dvpp_image.m_camera_data.m_ptr, image.dataSize);
}

static inline void sync(ara::adsfi::MsgImageDataList &image_list) {
    for (size_t i = 0; i < image_list.images.size(); i++) {
        dvpp::sync(image_list.images[i]);
    }
}
} // namespace dvpp
