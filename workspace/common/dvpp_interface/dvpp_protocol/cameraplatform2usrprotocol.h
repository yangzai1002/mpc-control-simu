#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>

#define MAX_CAMERA_NUM 16

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
            IOCTL_SET_KEYCHANNELID_PARAM,
            IOCTL_GET_H264CAMERADATA,
            IOCTL_SET_H264CAMERADATA_FREQ,
            IOCTL_SET_TOGGLE_LLC_MASTER,
            IOCTL_SET_CAMERA_ENABLE,
            IOCTL_GET_CAMERA_ENABLE,
            IOCTL_RESTART_LLC_ENABLE,
            IOCTL_INIT_CLIENT_CAMERA,
            IOCTL_GET_CLIENT_CAMERADATA,
        };

        enum EIdpImgType {
            IMGTYPE_YUV420NV12,
            IMGTYPE_RGB,
            IMGTYPE_BGR,
            IMGTYPE_H265,
            IMGTYPE_H264,
            IMGTYPE_YUV422SP,
        };

        struct SCameraData {
            char*       m_ptr;
            size_t      m_ptr_len;
            double      m_stamp;
            double      m_fsyncTimeStamp;
            double      m_fsyncGnssTimeStamp;
            double      m_pubTimeStamp;
            double      m_sofTimeStamp;
            double      m_eofTimeStamp;
            double      m_ispTimeStamp;
            EIdpImgType type;
            int         width;
            int         height;
        };

        struct SCameraDataList {
            SCameraData  m_camera_data_list[MAX_CAMERA_NUM];
            int          size;
            unsigned int mask;
            unsigned int init_mask;
            void         initBit(int bitPosition) {
                std::unique_lock<std::mutex> lock(mutx);
                init_mask = init_mask | (1 << bitPosition);
            }
            bool isAllMask() {
                std::unique_lock<std::mutex> lock(mutx);
                return init_mask == mask;
            }
            void setBit(int bitPosition) {
                std::unique_lock<std::mutex> lock(mutx);
                mask = mask | (1 << bitPosition);
            }
            void clearBit(int bitPosition) {
                mask = mask & ~(1 << bitPosition);
            }
            bool checkBit(int bitPosition) {
                return (mask & (1 << bitPosition)) != 0;
            }
            bool checkInitBit(int bitPosition) {
                return (init_mask & (1 << bitPosition)) != 0;
            }
            std::mutex mutx;
        };

        struct SCameraDataTest {
            uint64_t m_ptr;
            size_t   m_ptr_len;
            double   m_stamp;
            int      type;
        };

        struct SCameraInitTest {
            std::string                  name;
            std::vector<SCameraDataTest> camevec;
            ;
        };

        struct SCameraInfo {
            int width;
            int height;
        };

        struct SCameraName {
            char camera_name[128];
            int  camera_name_len;
        };

        struct SCameraAllName {
            SCameraName camera_array[MAX_CAMERA_NUM];
            int         camera_len = 0;
        };

        struct SCameraClientData {
            SCameraName m_cameara_name;
            SCameraData m_camera_data;
            int         m_block = 0;
        };

    } // namespace idpware
} // namespace avos