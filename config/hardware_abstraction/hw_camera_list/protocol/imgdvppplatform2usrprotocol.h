#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>

namespace avos
{
    namespace idpware
    {
        namespace imgdvpp
        {
            enum EIdpImgdvppPlatformIOType
            {
                IOCTL_INIT_DVPP,
                IOCTL_SET_INPUTIMG,
                IOCTL_GET_OUTPUTIMG,
            };

            enum EIdpInputImgType
            {
                IMGTYPE_YUV420NV12,
                IMGTYPE_BGR,
                IMGTYPE_RGB,
            };

            enum EIdpOutputImgType
            {
                IMGTYPE_H264,
                IMGTYPE_H265,
            };

            struct SImgDvppOutData
            {
                u_int8_t* m_ptr;
                size_t m_ptr_len;
                double m_stamp;
                int m_type;
            };

            struct SImgDvppInData
            {
                u_int8_t* m_ptr;
                size_t m_ptr_len;
                double m_stamp;
                int m_type;
                int m_mode; //0:线程安全，需要copy  1：线程不安全，但高效，低层指针直接给到m_ptr
            };

            struct SImgDvppCfg
            {
                int m_inputtype;
                int m_outputtype;
                int m_inwidth;
                int m_inheight;
                int m_outwidth;
                int m_outheight;
            };
        } //imgdvpp
    } // namespace idpware
} // namespace avos