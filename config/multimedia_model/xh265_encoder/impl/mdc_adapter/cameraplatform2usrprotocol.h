#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>

#include "msgpack.hpp"

namespace avos {
    namespace idpware {
        enum EIdpCameraPlatformIOType {
            IOCTL_INIT_CAMERA,
            IOCTL_SET_DATATYPE_PARAM,
            IOCTL_SET_CHANNELID_PARAM,
            IOCTL_GET_CAMERADATA,
            IOCTL_START_SERVICE,
            IOCTL_FREEDATA,
            IOCTL_SET_IMGTYPE_PARAM,
        };

        enum EIdpImgType {
            IMGTYPE_YUV420NV12,
            IMGTYPE_RGB,
            IMGTYPE_BGR,
            IMGTYPE_H265,
        };

        struct SCameraData {
            char*  m_ptr;
            size_t m_ptr_len;
            double m_stamp;
            int    type;
        };

        struct SCameraDataTest {
            uint64_t m_ptr;
            size_t   m_ptr_len;
            double   m_stamp;
            int      type;
            MSGPACK_DEFINE(m_ptr, m_ptr_len, m_stamp, type);
        };

        struct SCameraInitTest {
            std::string                  name;
            std::vector<SCameraDataTest> camevec;
            MSGPACK_DEFINE(name, camevec);
        };

        struct SCameraTest {
            char*  m_ptr;
            size_t m_ptr_len;
        };

    } // namespace idpware
} // namespace avos