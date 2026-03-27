#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>

#define STITCH_NUM 4

namespace avos
{
    namespace idpware
    {
        namespace imgstitchdvpp
        {
            enum EIdpImgdvppPlatformIOType
            {
                IOCTL_INIT_DVPP,
                IOCTL_STITCH_DVPP,
            };

            enum EIdpInputImgType
            {
                IMGTYPE_YUV420NV12,
                IMGTYPE_BGR,
                IMGTYPE_RGB,
                IMGTYPE_YUV422SP,
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
            };

            struct  SImgDvppStitch
            {
                SImgDvppInData m_inputdataary[STITCH_NUM];
                SImgDvppOutData m_outputdata;
            };
            

            struct SImgDvppCfg
            {
                unsigned char m_pos; //0:前，1:后，2：左， 3：右
                unsigned char  m_channelid;//64~127
                unsigned char  m_inputtype;
                unsigned char  m_outputtype;
                int m_inwidth;
                int m_inheight;
                int m_outwidth;
                int m_outheight;
                int m_roileft;
                int m_roitop;
                int m_roiwidth;
                int m_roiheight;                      
                char m_datapath[256];
                int m_datapathlen;
            };

            struct  SImgDvppCfgList
            {
                SImgDvppCfg data[STITCH_NUM];
                int m_dstwidth;
                int m_dstheight;
            };
        } //imgdvpp
    } // namespace idpware
} // namespace avos