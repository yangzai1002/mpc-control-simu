#pragma once

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
// #include <vector>
#include "msgpack.hpp"

namespace avos {
namespace perception {

template <typename T> class DataTrans {
public:
    avos::idpware::PerDataInterface Serialize(const T &data,
                                              std::string &string_ret) {
        avos::idpware::PerDataInterface ret;
        std::stringstream buffer;
        msgpack::pack(buffer, data);
        string_ret = buffer.str();
        ret.m_ptr = (char *)string_ret.data();
        ret.m_ptr_data_len = string_ret.size();
        return ret;
    }

    T Deserialize(const avos::idpware::PerDataInterface &data) {
        msgpack::object_handle handle =
            msgpack::unpack(data.m_ptr, data.m_ptr_data_len);
        msgpack::object obj = handle.get();
        return obj.as<T>();
    }
};

enum EIdpVisionDectectionIOType {
    // init area
    IOCTL_INIT_INTRINSICS_PATH,

    IOCTL_INIT_CONFIG,

    // input area
    IOCTL_SET_IMAGE_LIST_DATA,

    IOCTL_SET_RESULT_GET_INTERVAL,

    // output area
    IOCTL_GET_VISION_DETECT_DATA,
};

struct GetResultInterval {
    bool synchronous;
    int result_wait_msecond;
    int result_wait_interval_msecond;
    MSGPACK_DEFINE(result_wait_msecond, result_wait_interval_msecond);
};

struct SStringConfig {
    std::string str;
    MSGPACK_DEFINE(str);
};

struct SVisionPerceptionConfig {
    std::string config_root;
    std::string config_file;
    MSGPACK_DEFINE(config_root, config_file);
};

struct SVisionImageData {
    double trigger_stamp;
    int height;
    int width;
    std::string camera_name;
    std::string encoding; // "bgr8  rgb8  YUYV422  UYUV422"
    uint64_t data_ptr;
    MSGPACK_DEFINE(trigger_stamp, height, width, camera_name, encoding,
                   data_ptr);
};

struct SVisionImageDataList {
    std::vector<SVisionImageData> camera_images;
    MSGPACK_DEFINE(camera_images);
};

} // namespace perception
} // namespace avos