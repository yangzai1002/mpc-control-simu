#include "VideoEditYUV420UV12.hpp"
#include "ap_log/ap_log_interface.h"
#include "ap_log/ap_log_interface.h"

void VideoEditYUV420UV12::PasteYUV420SP(std::shared_ptr<YUYV420UV12> dst, std::shared_ptr<YUYV420UV12> src, int pos_x,
                                        int pos_y) {
    int dst_w = dst->width(), dst_h = dst->height();
    int src_w = src->width(), src_h = src->height();

    uint8_t* dst_data = dst->get_data();
    uint8_t* src_data = src->get_data();

    // 拷贝Y分量
    for (int y = 0; y < src_h; ++y) {
        int dst_y = pos_y + y;
        if (dst_y >= dst_h)
            continue;
        memcpy(dst_data + dst_y * dst_w + pos_x, src_data + y * src_w, std::min(src_w, dst_w - pos_x));
    }

    // 拷贝UV分量
    int      dst_uv_w = dst_w / 2, src_uv_w = src_w / 2;
    uint8_t* dst_uv = dst_data + dst_w * dst_h;
    uint8_t* src_uv = src_data + src_w * src_h;
    for (int y = 0; y < src_h / 2; ++y) {
        int dst_y = pos_y / 2 + y;
        if (dst_y >= dst_h / 2)
            continue;
        memcpy(dst_uv + (dst_y * dst_uv_w + pos_x / 2) * 2, src_uv + y * src_uv_w * 2,
               std::min(src_uv_w, dst_uv_w - pos_x / 2) * 2);
    }
}

//优化缩放代码
void VideoEditYUV420UV12::ResizeYUV420SP(const uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h) {
    // 缩放系数（避免除法，预先计算）
    const int y_ratio = (src_h << 16) / dst_h;
    const int x_ratio = (src_w << 16) / dst_w;

    const uint8_t* src_y_plane = src;
    const uint8_t* src_uv_plane = src + src_w * src_h;

    uint8_t* dst_y_plane = dst;
    uint8_t* dst_uv_plane = dst + dst_w * dst_h;

    // 拷贝 Y 分量（亮度）
    for (int y = 0; y < dst_h; ++y) {
        int            src_y = (y * y_ratio) >> 16;
        const uint8_t* src_row = src_y_plane + src_y * src_w;
        uint8_t*       dst_row = dst_y_plane + y * dst_w;

        int x = 0;
        for (; x <= dst_w - 4; x += 4) {
            int sx0 = (x * x_ratio) >> 16;
            int sx1 = ((x + 1) * x_ratio) >> 16;
            int sx2 = ((x + 2) * x_ratio) >> 16;
            int sx3 = ((x + 3) * x_ratio) >> 16;

            dst_row[x + 0] = src_row[sx0];
            dst_row[x + 1] = src_row[sx1];
            dst_row[x + 2] = src_row[sx2];
            dst_row[x + 3] = src_row[sx3];
        }
        for (; x < dst_w; ++x) {
            int sx = (x * x_ratio) >> 16;
            dst_row[x] = src_row[sx];
        }
    }

    // 缩放 UV 分量（色度，NV12: UVUV...）
    const int uv_src_w = src_w / 2;
    const int uv_src_h = src_h / 2;
    const int uv_dst_w = dst_w / 2;
    const int uv_dst_h = dst_h / 2;

    const int uv_y_ratio = (uv_src_h << 16) / uv_dst_h;
    const int uv_x_ratio = (uv_src_w << 16) / uv_dst_w;

    for (int y = 0; y < uv_dst_h; ++y) {
        int            src_y = (y * uv_y_ratio) >> 16;
        const uint8_t* src_uv_row = src_uv_plane + src_y * src_w;
        uint8_t*       dst_uv_row = dst_uv_plane + y * dst_w;

        int x = 0;
        for (; x <= uv_dst_w - 2; x += 2) {
            int sx0 = (x * uv_x_ratio) >> 16;
            int sx1 = ((x + 1) * uv_x_ratio) >> 16;

            dst_uv_row[x * 2 + 0] = src_uv_row[sx0 * 2 + 0]; // U
            dst_uv_row[x * 2 + 1] = src_uv_row[sx0 * 2 + 1]; // V

            dst_uv_row[(x + 1) * 2 + 0] = src_uv_row[sx1 * 2 + 0];
            dst_uv_row[(x + 1) * 2 + 1] = src_uv_row[sx1 * 2 + 1];
        }
        for (; x < uv_dst_w; ++x) {
            int sx = (x * uv_x_ratio) >> 16;
            dst_uv_row[x * 2 + 0] = src_uv_row[sx * 2 + 0];
            dst_uv_row[x * 2 + 1] = src_uv_row[sx * 2 + 1];
        }
    }
}

std::shared_ptr<YUYV420UV12> VideoEditYUV420UV12::ResizeYUV420SPToPtr(const uint8_t* src, int src_w, int src_h,
                                                                      int dst_w, int dst_h) {
    auto dst_ptr = std::make_shared<YUYV420UV12>(dst_w, dst_h);
    ResizeYUV420SP(src, src_w, src_h, dst_ptr->get_data(), dst_w, dst_h);
    return dst_ptr;
}

// YUYV420UV12 与 cv::Mat 转换方法实现

cv::Mat VideoEditYUV420UV12::YUYV420UV12ToMat(const std::shared_ptr<YUYV420UV12>& yuv_ptr) {
    if (!yuv_ptr || !yuv_ptr->get_data()) {
        ApError("status_recorder") << "YUYV420UV12ToMat: 输入指针为空";
        return cv::Mat();
    }

    // 创建临时对象进行格式转换（YUV420SP -> YUV420P）
    YUYV420UV12 temp_yuv = *yuv_ptr;
    temp_yuv.swap_spp(); // 转换为I420格式

    // 创建cv::Mat对象，使用YUV420P格式
    cv::Mat yuv420(temp_yuv.height() + temp_yuv.height() / 2, temp_yuv.width(), CV_8UC1, temp_yuv.get_data());

    // 转换为BGR格式
    cv::Mat bgr;
    cv::cvtColor(yuv420, bgr, cv::COLOR_YUV2BGR_I420);

    return bgr;
}

std::shared_ptr<YUYV420UV12> VideoEditYUV420UV12::MatToYUYV420UV12(const cv::Mat& mat) {
    if (mat.empty()) {
        ApError("status_recorder") << "MatToYUYV420UV12: 输入Mat为空";
        return nullptr;
    }

    // 确保输入是BGR格式
    cv::Mat bgr_mat;
    if (mat.channels() == 1) {
        cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
    } else if (mat.channels() == 3) {
        bgr_mat = mat.clone();
    } else {
        ApError("status_recorder") << "MatToYUYV420UV12: 不支持的通道数: " << mat.channels();
        return nullptr;
    }

    // 转换为YUV420P格式
    cv::Mat yuv420;
    cv::cvtColor(bgr_mat, yuv420, cv::COLOR_BGR2YUV_I420);

    // 创建YUYV420UV12对象
    auto yuv_ptr = std::make_shared<YUYV420UV12>(bgr_mat.cols, bgr_mat.rows);

    // 复制数据
    memcpy(yuv_ptr->get_data(), yuv420.data, yuv_ptr->size());

    // 转换为YUV420SP格式（NV12）
    yuv_ptr->swap_psp();

    return yuv_ptr;
}

void VideoEditYUV420UV12::DrawText(std::shared_ptr<YUYV420UV12>& yuv_ptr, const std::string& text,
                                   const cv::Point& position, double font_scale, const cv::Scalar& color, int thickness,
                                   int font_face) {
    if (!yuv_ptr || !yuv_ptr->get_data()) {
        ApError("status_recorder") << "DrawText: 输入指针为空";
        return;
    }

    if (text.empty()) {
        ApError("status_recorder") << "DrawText: 文本为空";
        return;
    }

    // 转换为cv::Mat进行文本绘制
    cv::Mat mat = YUYV420UV12ToMat(yuv_ptr);
    if (mat.empty()) {
        ApError("status_recorder") << "DrawText: 转换Mat失败";
        return;
    }

    // 在Mat上绘制文本
    cv::putText(mat, text, position, font_face, font_scale, color, thickness);

    // 转换回YUYV420UV12
    auto new_yuv_ptr = MatToYUYV420UV12(mat);
    if (new_yuv_ptr) {
        yuv_ptr = new_yuv_ptr;
    } else {
        ApError("status_recorder") << "DrawText: 转换回YUYV420UV12失败";
    }
}
