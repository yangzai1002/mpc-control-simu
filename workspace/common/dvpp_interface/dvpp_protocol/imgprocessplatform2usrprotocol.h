#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>

namespace avos {
    namespace idpware {
        enum EIdpImgProcessIOType {
            IOCTL_IMGCROP_RESIZE_PASTE_FUNC,
            IOCTL_DVPP_MALLOC,
            IOCTL_DVPP_FREE,
        };

        enum EIdpImgBufType { BUF_CPU_TYPE, BUF_DVPP_TYPE };

        enum EIdpMdc610ImgType {
            IMG_FORMAT_YUV_400 = 0,                     // YUV400 8bit
            IMG_FORMAT_YUV_SEMIPLANAR_420 = 1,          // YUV420SP NV12 8bit
            IMG_FORMAT_YVU_SEMIPLANAR_420 = 2,          // YUV420SP NV21 8bit
            IMG_FORMAT_YUV_SEMIPLANAR_422 = 3,          // YUV422SP 8bit
            IMG_FORMAT_YVU_SEMIPLANAR_422 = 4,          // YVU422SP 8bit
            IMG_FORMAT_YUV_SEMIPLANAR_444 = 5,          // YUV444SP 8bit
            IMG_FORMAT_YVU_SEMIPLANAR_444 = 6,          // YVU444SP 8bit
            IMG_FORMAT_YUYV_PACKED_422 = 7,             // YUV422 Package YUYV 8bit
            IMG_FORMAT_UYVY_PACKED_422 = 8,             // YUV422 Package  UYVY 8bit
            IMG_FORMAT_YVYU_PACKED_422 = 9,             // YUV422 Package  YVYU 8bit
            IMG_FORMAT_VYUY_PACKED_422 = 10,            // YUV422 Package  VYUY 8bit
            IMG_FORMAT_YUV_PACKED_444 = 11,             // YUV444 Package  8bit
            IMG_FORMAT_RGB_888 = 12,                    // RGB888
            IMG_FORMAT_BGR_888 = 13,                    // BGR888
            IMG_FORMAT_ARGB_8888 = 14,                  // ARGB8888
            IMG_FORMAT_ABGR_8888 = 15,                  // ABGR8888
            IMG_FORMAT_RGBA_8888 = 16,                  // RGBA8888
            IMG_FORMAT_BGRA_8888 = 17,                  // BGRA8888
            IMG_FORMAT_YUV_SEMI_PLANNER_420_10BIT = 18, // YUV420SP 10bit
            IMG_FORMAT_YVU_SEMI_PLANNER_420_10BIT = 19, // YVU420sp 10bit
            IMG_FORMAT_YVU_PLANAR_420 = 20,             // YVU420P 8bit
            IMG_FORMAT_YVU_PLANAR_422 = 21,             // YVU422P 8bit
            IMG_FORMAT_YVU_PLANAR_444 = 22,             // YVU444P 8bit
            IMG_FORMAT_RGB_444 = 23,                    // RGB444  R:4bit G:4bit B:4bit，当前不支持该格式
            IMG_FORMAT_BGR_444 = 24,                    // BGR444  R:4bit G:4bit B:4bit，当前不支持该格式
            IMG_FORMAT_ARGB_4444 = 25,                  // ARGB4444 A:4bit R:4bit G:4bit B:4bit
            IMG_FORMAT_ABGR_4444 = 26,  // ABGR4444 A:4bit B:4bit G:4bit R:4bit，当前不支持该格式
            IMG_FORMAT_RGBA_4444 = 27,  // RGBA4444 R:4bit G:4bit B:4bit A:4bit，当前不支持该格式
            IMG_FORMAT_BGRA_4444 = 28,  // BGRA4444 B:4bit G:4bit R:4bit A:4bit，当前不支持该格式
            IMG_FORMAT_RGB_555 = 29,    // RGB555 R:5bit G:5bit B:5bit，当前不支持该格式
            IMG_FORMAT_BGR_555 = 30,    // BGR555 B:5bit G:5bit R:5bit，当前不支持该格式
            IMG_FORMAT_RGB_565 = 31,    // RGB565 R:5bit G:6bit B:5bit，当前不支持该格式
            IMG_FORMAT_BGR_565 = 32,    // BGR565 B:5bit G:6bit R:5bit，当前不支持该格式
            IMG_FORMAT_ARGB_1555 = 33,  // ARGB1555 A:1bit R:5bit G:6bit B:5bit
            IMG_FORMAT_ABGR_1555 = 34,  // ABGR1555 A:1bit B:5bit G:6bit R:5bit，当前不支持该格式
            IMG_FORMAT_RGBA_1555 = 35,  // RGBA1555 A:1bit B:5bit G:6bit R:5bit，当前不支持该格式
            IMG_FORMAT_BGRA_1555 = 36,  // BGRA1555 A:1bit B:5bit G:6bit R:5bit，当前不支持该格式
            IMG_FORMAT_ARGB_8565 = 37,  // ARGB8565 A:8bit R:5bit G:6bit B:5bit，当前不支持该格式
            IMG_FORMAT_ABGR_8565 = 38,  // ABGR8565 A:8bit B:5bit G:6bit R:5bit，当前不支持该格式
            IMG_FORMAT_RGBA_8565 = 39,  // RGBA8565 A:8bit R:5bit G:6bit B:5bit，当前不支持该格式
            IMG_FORMAT_BGRA_8565 = 40,  // BGRA8565 A:8bit B:5bit G:6bit R:5bit，当前不支持该格式
            IMG_FORMAT_ARGB_CLUT2 = 41, // ARGB Color Lookup Table 2bit
            IMG_FORMAT_ARGB_CLUT4 = 42, // ARGB Color Lookup Table 4bit
            IMG_FORMAT_RGB_BAYER_8BPP = 50,
            IMG_FORMAT_RGB_BAYER_10BPP = 51,
            IMG_FORMAT_RGB_BAYER_12BPP = 52,
            IMG_FORMAT_RGB_BAYER_14BPP = 53,
            IMG_FORMAT_RGB_BAYER_16BPP = 54,    // RGB Bayer 16bit，Bayer图像，当前不支持该格式
            IMG_FORMAT_YUV_PLANAR_420 = 55,     // YUV420P 8bit
            IMG_FORMAT_YUV_PLANAR_422 = 56,     // YUV422P 8bit
            IMG_FORMAT_YUV_PLANAR_444 = 57,     // YUV444P 8bit
            IMG_FORMAT_YVU_PACKED_444 = 58,     // YVU444 Package 8bit
            IMG_FORMAT_XYUV_PACKED_444 = 59,    // AYUV444 Package 8bit
            IMG_FORMAT_XYVU_PACKED_444 = 60,    // AYVU444 Package 8bit
            IMG_FORMAT_YUV_SEMIPLANAR_411 = 61, // YUV411SP 8bit
            IMG_FORMAT_YVU_SEMIPLANAR_411 = 62, // YVU411SP 8bit
            IMG_FORMAT_YUV_PLANAR_411 = 63,     // YUV411P 8bit
            IMG_FORMAT_YVU_PLANAR_411 = 64,     // YVU411P 8bit
            IMG_FORMAT_YUV_PLANAR_440 = 65,     // YUV440P 8bit
            IMG_FORMAT_YVU_PLANAR_440 = 66,     // YVU440P 8bit
            IMG_FORMAT_RGB_888_PLANAR = 69,     // RGB888 Planar
            IMG_FORMAT_BGR_888_PLANAR = 70,     // BGR888 Planar
            IMG_FORMAT_HSV_888_PACKAGE = 71,    // HSV Package，HSV图像package格式，当前不支持该格式
            IMG_FORMAT_HSV_888_PLANAR = 72,     // HSV Planar，HSV图像Planar格式，当前不支持该格式
            IMG_FORMAT_LAB_888_PACKAGE = 73,    // LAB Package，LAB图像package格式，当前不支持该格式
            IMG_FORMAT_LAB_888_PLANAR = 74,     // LAB Planar，LAB图像Planar格式，当前不支持该格式
            IMG_FORMAT_S8C1 =
                75, // Signed 8bit for 1pixel 1Channel，每个像素用1个8bit有符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_S8C2_PACKAGE =
                76, // Signed 8bit for 1pixel 2Channel
                    // Package，每个像素用2个8bit有符号数表示的双通道图像Package格式，当前不支持该格式
            IMG_FORMAT_S8C2_PLANAR =
                77, // Signed 8bit for 1pixel 2Channel
                    // Planar，每个像素用2个8bit有符号数据表的双通道图像Planar格式，当前不支持该格式
            IMG_FORMAT_S16C1 =
                78, // Signed 16bit 1pixel 1Channel，每个像素用1个16bit有符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_U8C1 =
                79, // Unsigned 8bit 1pixel 1Channel，每个像素用1个8bit无符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_U16C1 =
                80, // Unsigned 16bit 1pixel 1Channel，每个像素用1个16bit无符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_S32C1 =
                81, // Signed 32bit 1pixel 1Channel，每个像素用1个32bit有符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_U32C1 =
                82, // Unsigned 32bit 1pixel 1Channel，每个像素用1个32bit无符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_U64C1 =
                83, // Unsigned 64bit 1pixel 1Channel，每个像素用1个64bit无符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_S64C1 =
                84, // Signed 64bit 1pixel 1Channel，每个像素用1个64bit有符号数据表示的单通道图像，当前不支持该格式
            IMG_FORMAT_RGB_888_INT8 = 110,         // RGB888 Package 每个像素的单分量占用1个8bit有符号数
            IMG_FORMAT_BGR_888_INT8 = 111,         // BGR888 Package 每个像素的单分量占用1个8bit有符号数
            IMG_FORMAT_RGB_888_INT16 = 112,        // RGB888 Package 每个像素的单分量占用1个16bit有符号数
            IMG_FORMAT_BGR_888_INT16 = 113,        // BGR888 Package 每个像素的单分量占用1个16bit有符号数
            IMG_FORMAT_RGB_888_INT32 = 114,        // RGB888 Package 每个像素的单分量占用1个32bit有符号数
            IMG_FORMAT_BGR_888_INT32 = 115,        // BGR888 Package 每个像素的单分量占用1个32bit有符号数
            IMG_FORMAT_RGB_888_UINT16 = 116,       // RGB888 Package 每个像素的单分量占用1个16bit无符号数
            IMG_FORMAT_BGR_888_UINT16 = 117,       // BGR888 Package 每个像素的单分量占用1个16bit无符号数
            IMG_FORMAT_RGB_888_UINT32 = 118,       // RGB888 Package 每个像素的单分量占用1个32bit无符号数
            IMG_FORMAT_BGR_888_UINT32 = 119,       // BGR888 Package 每个像素的单分量占用1个32bit无符号数
            IMG_FORMAT_RGB_888_PLANAR_INT8 = 120,  // RGB888 Planar 每个像素的单分量占用1个8bit有符号数
            IMG_FORMAT_BGR_888_PLANAR_INT8 = 121,  // BGR888 Planar 每个像素的单分量占用1个8bit有符号数
            IMG_FORMAT_RGB_888_PLANAR_INT16 = 122, // RGB888 Planar 每个像素的单分量占用1个16bit有符号数
            IMG_FORMAT_BGR_888_PLANAR_INT16 = 123, // BGR888 Planar 每个像素的单分量占用1个16bit有符号数
            IMG_FORMAT_RGB_888_PLANAR_INT32 = 124, // RGB888 Planar 每个像素的单分量占用1个32bit有符号数
            IMG_FORMAT_BGR_888_PLANAR_INT32 = 125, // BGR888 Planar 每个像素的单分量占用1个32bit有符号数
            IMG_FORMAT_RGB_888_PLANAR_UINT16 = 126, // RGB888 Planar 每个像素的单分量占用1个16bit无符号数
            IMG_FORMAT_BGR_888_PLANAR_UINT16 = 127, // BGR888 Planar 每个像素的单分量占用1个16bit无符号数
            IMG_FORMAT_RGB_888_PLANAR_UINT32 = 128, // RGB888 Planar 每个像素的单分量占用1个32bit无符号数
            IMG_FORMAT_BGR_888_PLANAR_UINT32 = 129, // BGR888 Planar 每个像素的单分量占用1个32bit无符号数
            IMG_FORMAT_YUV400_UINT16 = 130, // YUV400 Package 每个像素的单分量占用1个16bit无符号数
            IMG_FORMAT_YUV400_UINT32 = 131, // YUV400 Package 每个像素的单分量占用1个32bit无符号数
            IMG_FORMAT_YUV400_UINT64 = 132, // YUV400 Package 每个像素的单分量占用1个64bit无符号数
            IMG_FORMAT_YUV400_INT8 = 133,   // YUV400 Package 每个像素的单分量占用1个8bit有符号数
            IMG_FORMAT_YUV400_INT16 = 134,  // YUV400 Package 每个像素的单分量占用1个16bit有符号数
            IMG_FORMAT_YUV400_INT32 = 135,  // YUV400 Package 每个像素的单分量占用1个32bit有符号数
            IMG_FORMAT_YUV400_INT64 = 136,  // YUV400 Package 每个像素的单分量占用1个64bit有符号数
            IMG_FORMAT_YUV400_FP16 = 137,   // YUV400 Package 每个像素用1个float16数据表示
            IMG_FORMAT_YUV400_FP32 = 138,   // YUV400 Package 每个像素用1个float32数据表示
            IMG_FORMAT_YUV400_FP64 = 139,   // YUV400 Package 每个像素用1个float64数据表示
            IMG_FORMAT_YUV400_BF16 = 140, // YUV400 Package 每个像素用1个BFloat16数据表示，当前不支持该格式
            IMG_FORMAT_YUV_SEMIPLANAR_440 = 1000, // YUV440SP 8bit
            IMG_FORMAT_YVU_SEMIPLANAR_440 = 1001, // YVU440SP 8bit
            IMG_FORMAT_FLOAT32 = 1002, // Float 32bit for 1pixel，每个像素用1个float32数据表示，当前不支持该格式
            IMG_FORMAT_BUTT = 1003,
            IMG_FORMAT_RGB_888_PLANAR_FP16 = 1004, // RGB888 Planar 每个像素用1个float16数据表示
            IMG_FORMAT_BGR_888_PLANAR_FP16 = 1005, // BGR888 Planar 每个像素用1个float16数据表示
            IMG_FORMAT_RGB_888_PLANAR_FP32 = 1006, // RGB888 Planar 每个像素用1个float32数据表示
            IMG_FORMAT_BGR_888_PLANAR_FP32 = 1007, // BGR888 Planar 每个像素用1个float32数据表示
            IMG_FORMAT_RGB_888_PLANAR_BF16 = 1008, // RGB888 Planar 每个像素用1个BFloat16数据表示，当前不支持该格式
            IMG_FORMAT_BGR_888_PLANAR_BF16 = 1009, // BGR888 Planar 每个像素用1个BFloat16数据表示，当前不支持该格式
            IMG_FORMAT_RGB_888_FP16 = 1010, // RGB888 Package，每个像素用1个float16数据表示
            IMG_FORMAT_BGR_888_FP16 = 1011, // BGR888 Package，每个像素用1个float16数据表示
            IMG_FORMAT_RGB_888_FP32 = 1012, // RGB888 Package，每个像素用1个float32数据表示
            IMG_FORMAT_BGR_888_FP32 = 1013, // BGR888 Package，每个像素用1个float32数据表示
            IMG_FORMAT_RGB_888_BF16 = 1014, // RGB888 Package 每个像素用1个BFloat16数据表示，当前不支持该格式
            IMG_FORMAT_BGR_888_BF16 = 1015, // BGR888 Package 每个像素用1个BFloat16数据表示，当前不支持该格式
            IMG_FORMAT_UNKNOWN = 10000
        };

        typedef struct SVpcCropRegion {
            u_int32_t top_offset;
            u_int32_t left_offset;
            u_int32_t crop_width;
            u_int32_t crop_height;

            SVpcCropRegion() {
                top_offset = 0;
                left_offset = 0;
                crop_width = 0;
                crop_height = 0;
            }
        } SVpcCropRegion;

        typedef struct SVpcResizeInfo {
            u_int32_t resize_width;
            u_int32_t resize_height;
            u_int32_t interpolation;
            SVpcResizeInfo() {
                resize_width = 0;
                resize_height = 0;
                interpolation = 0;
            }
        } SVpcResizeInfo;

        struct SImgProcessInfo {
            void *            m_buf;
            size_t            m_buf_len;
            EIdpImgBufType    m_buf_memtype;
            EIdpMdc610ImgType m_buf_imgtype;
            u_int32_t         m_img_width;
            u_int32_t         m_img_height;

            SImgProcessInfo() {
                m_buf = nullptr;
                m_buf_len = 0;
                m_buf_memtype = BUF_DVPP_TYPE;
                m_buf_imgtype = IMG_FORMAT_YUV_SEMIPLANAR_420;
                m_img_width = 0;
                m_img_height = 0;
            }

            void Init(void *data, size_t _m_buf_len, u_int32_t _m_img_width, u_int32_t _m_img_height,
                      EIdpImgBufType    _m_buf_memtype = BUF_DVPP_TYPE,
                      EIdpMdc610ImgType _m_buf_imgtype = IMG_FORMAT_YUV_SEMIPLANAR_420) {
                m_buf = data;
                m_buf_len = _m_buf_len;
                m_img_width = _m_img_width;
                m_img_height = _m_img_height;
                m_buf_memtype = _m_buf_memtype;
                m_buf_imgtype = _m_buf_imgtype;
            }
        };

        typedef struct SVpcCropResizePasteRegion {
            SImgProcessInfo m_dest_pic_info;
            SVpcCropRegion  crop_region;
            SVpcResizeInfo  resize_info;
            u_int32_t       dest_top_offset;
            u_int32_t       dest_left_offset;

            SVpcCropResizePasteRegion() {
                dest_top_offset = 0;
                dest_left_offset = 0;
            }

            void SetResizeParam(u_int32_t origin_width, u_int32_t origin_height, u_int32_t resize_width,
                                u_int32_t resize_height, u_int32_t interpolation = 0) {
                crop_region.top_offset = 0;
                crop_region.left_offset = 0;
                crop_region.crop_width = origin_width;
                crop_region.crop_height = origin_height;

                resize_info.resize_width = resize_width;
                resize_info.resize_height = resize_height;
                resize_info.interpolation = interpolation;
                dest_top_offset = 0;
                dest_left_offset = 0;
            }

            void SetCropParam(u_int32_t crop_left_offset, u_int32_t crop_top_offset, u_int32_t dest_width,
                              u_int32_t dest_height) {
                crop_region.top_offset = crop_top_offset;
                crop_region.left_offset = crop_left_offset;
                crop_region.crop_width = dest_width;
                crop_region.crop_height = dest_height;

                resize_info.resize_width = dest_width;
                resize_info.resize_height = dest_height;
                resize_info.interpolation = 0;
                dest_top_offset = 0;
                dest_left_offset = 0;
            }

            void SetResizePaste(u_int32_t origin_width, u_int32_t origin_height, u_int32_t resize_width,
                                u_int32_t resize_height, u_int32_t paste_left_offset, u_int32_t paste_top_offset) {
                crop_region.top_offset = 0;
                crop_region.left_offset = 0;
                crop_region.crop_width = origin_width;
                crop_region.crop_height = origin_height;

                resize_info.resize_width = resize_width;
                resize_info.resize_height = resize_height;
                resize_info.interpolation = 0;
                dest_top_offset = paste_top_offset;
                dest_left_offset = paste_left_offset;
            }

            void SetCropPaste(u_int32_t crop_width, u_int32_t crop_height, u_int32_t crop_left_offset,
                              u_int32_t crop_top_offset, u_int32_t paste_left_offset, u_int32_t paste_top_offset) {
                crop_region.top_offset = crop_top_offset;
                crop_region.left_offset = crop_left_offset;
                crop_region.crop_width = crop_width;
                crop_region.crop_height = crop_height;

                resize_info.resize_width = crop_width;
                resize_info.resize_height = crop_height;
                resize_info.interpolation = 0;
                dest_top_offset = paste_top_offset;
                dest_left_offset = paste_left_offset;
            }

            void SetCropResizePaste(u_int32_t crop_width, u_int32_t crop_height, u_int32_t crop_left_offset,
                                    u_int32_t crop_top_offset, u_int32_t resize_width, u_int32_t resize_height,
                                    u_int32_t paste_left_offset, u_int32_t paste_top_offset) {
                crop_region.top_offset = crop_top_offset;
                crop_region.left_offset = crop_left_offset;
                crop_region.crop_width = crop_width;
                crop_region.crop_height = crop_height;

                resize_info.resize_width = resize_width;
                resize_info.resize_height = resize_height;
                resize_info.interpolation = 0;
                dest_top_offset = paste_top_offset;
                dest_left_offset = paste_left_offset;
            }
        } SVpcCropResizePasteRegion;

        struct SImgProcess {
            char                      m_process_name[64];
            size_t                    m_process_name_len;
            SImgProcessInfo           m_in_img;
            SVpcCropResizePasteRegion m_out_img;

            void InitInputImage(void *data, size_t _m_buf_len, u_int32_t _m_img_width, u_int32_t _m_img_height,
                                EIdpImgBufType    _m_buf_memtype = BUF_DVPP_TYPE,
                                EIdpMdc610ImgType _m_buf_imgtype = IMG_FORMAT_YUV_SEMIPLANAR_420) {
                m_in_img.Init(data, _m_buf_len, _m_img_width, _m_img_height, _m_buf_memtype, _m_buf_imgtype);
            }

            void InitOutputImage(void *data, size_t _m_buf_len, u_int32_t _m_img_width, u_int32_t _m_img_height,
                                 EIdpImgBufType    _m_buf_memtype = BUF_DVPP_TYPE,
                                 EIdpMdc610ImgType _m_buf_imgtype = IMG_FORMAT_YUV_SEMIPLANAR_420) {
                m_out_img.m_dest_pic_info.Init(data, _m_buf_len, _m_img_width, _m_img_height, _m_buf_memtype,
                                               _m_buf_imgtype);
            }

            SImgProcess() {
                memset(m_process_name, 0, 64);
                m_process_name_len = 0;
            }

            std::string GetProcessName() const {
                return std::string(m_process_name);
            }

            void SetProcessName(const std::string &str) {
                memset(m_process_name, 0, 64);
                m_process_name_len = str.size() > (63) ? (63) : str.size();
                memcpy(m_process_name, str.c_str(), m_process_name_len);
            }

            void SetResizeParam(u_int32_t origin_width, u_int32_t origin_height, u_int32_t resize_width,
                                u_int32_t resize_height, u_int32_t interpolation = 0) {
                m_out_img.SetResizeParam(origin_width, origin_height, resize_width, resize_height, interpolation);
            }

            void SetCropParam(u_int32_t crop_left_offset, u_int32_t crop_top_offset, u_int32_t dest_width,
                              u_int32_t dest_height) {
                m_out_img.SetCropParam(crop_left_offset, crop_top_offset, dest_width, dest_height);
            }

            void SetResizePaste(u_int32_t origin_width, u_int32_t origin_height, u_int32_t resize_width,
                                u_int32_t resize_height, u_int32_t paste_left_offset, u_int32_t paste_top_offset) {
                m_out_img.SetResizePaste(origin_width, origin_height, resize_width, resize_height, paste_left_offset,
                                         paste_top_offset);
            }

            void SetCropPaste(u_int32_t crop_width, u_int32_t crop_height, u_int32_t crop_left_offset,
                              u_int32_t crop_top_offset, u_int32_t paste_left_offset, u_int32_t paste_top_offset) {
                m_out_img.SetCropPaste(crop_width, crop_height, crop_left_offset, crop_top_offset, paste_left_offset,
                                       paste_top_offset);
            }

            void SetCropResizePaste(u_int32_t crop_width, u_int32_t crop_height, u_int32_t crop_left_offset,
                                    u_int32_t crop_top_offset, u_int32_t resize_width, u_int32_t resize_height,
                                    u_int32_t paste_left_offset, u_int32_t paste_top_offset) {
                m_out_img.SetCropResizePaste(crop_width, crop_height, crop_left_offset, crop_top_offset, resize_width,
                                             resize_height, paste_left_offset, paste_top_offset);
            }
        };

        // yixingwang 2026-01-31
        namespace imgdvpp {
            enum EIdpImgdvppPlatformIOType {
                IOCTL_INIT_DVPP,
                IOCTL_DEINIT_DVPP,
                IOCTL_SET_INPUTIMG,
                IOCTL_GET_OUTPUTIMG,
            };

            enum EIdpInputImgType {
                IMGTYPE_YUV420NV12,
                IMGTYPE_BGR,
                IMGTYPE_RGB,
                IMGTYPE_YUV422SP,
            };

            enum EIdpOutputImgType {
                IMGTYPE_H264,
                IMGTYPE_H265,
            };

            struct SImgDvppOutData {
                u_int8_t *m_ptr;
                size_t    m_ptr_len;
                double    m_stamp;
                int       m_type;
            };

            struct SImgDvppInData {
                u_int8_t *m_ptr;
                size_t    m_ptr_len;
                double    m_stamp;
                int       m_type;
                int m_mode; // 0:线程安全，需要copy  1：线程不安全，但高效，低层指针直接给到m_ptr
            };

            struct SImgDvppCfg {
                int m_inputtype;
                int m_outputtype;
                int m_inwidth;
                int m_inheight;
                int m_outwidth;
                int m_outheight;
                int m_outbps;
            };
        } // namespace imgdvpp
    }     // namespace idpware
} // namespace avos
