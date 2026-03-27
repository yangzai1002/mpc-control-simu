#include "png_image.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>

PngImage::PngImage() : width_(0), height_(0) {}

bool PngImage::load(const std::string& path)
{
    // IMREAD_UNCHANGED 保留 alpha 通道（如果有）
    img_ = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img_.empty())
        return false;

    width_  = img_.cols;
    height_ = img_.rows;

    // 统一转为 BGRA，简化后续采样逻辑
    if (img_.channels() == 1)
        cv::cvtColor(img_, img_, cv::COLOR_GRAY2BGRA);
    else if (img_.channels() == 3)
        cv::cvtColor(img_, img_, cv::COLOR_BGR2BGRA);
    // 4 通道直接使用

    return true;
}

void PngImage::bilinear(float sx, float sy,
                         uint8_t& b, uint8_t& g, uint8_t& r, uint8_t& a) const
{
    int x0 = static_cast<int>(sx);
    int y0 = static_cast<int>(sy);
    int x1 = std::min(x0 + 1, width_  - 1);
    int y1 = std::min(y0 + 1, height_ - 1);

    float fx = sx - x0, fy = sy - y0;

    auto px = [&](int x, int y) -> const uint8_t* {
        return img_.ptr<uint8_t>(y) + x * 4;
    };

    for (int c = 0; c < 4; ++c) {
        float v = (1 - fy) * ((1 - fx) * px(x0, y0)[c] + fx * px(x1, y0)[c])
                +      fy  * ((1 - fx) * px(x0, y1)[c] + fx * px(x1, y1)[c]);
        uint8_t val = static_cast<uint8_t>(std::min(std::max(v, 0.0f), 255.0f));
        switch (c) {
            case 0: b = val; break;
            case 1: g = val; break;
            case 2: r = val; break;
            case 3: a = val; break;
        }
    }
}

bool PngImage::getpix(int resize_w, int resize_h, int px, int py,
                       uint8_t& b, uint8_t& g, uint8_t& r, uint8_t& a) const
{
    if (img_.empty())
        return false;
    if (px < 0 || px >= resize_w || py < 0 || py >= resize_h)
        return false;

    // 虚拟 resize：将目标坐标映射回原图浮点坐标
    // 使用像素中心对齐：src = (dst + 0.5) * (src_size / dst_size) - 0.5
    float src_x = (px + 0.5f) * static_cast<float>(width_)  / resize_w - 0.5f;
    float src_y = (py + 0.5f) * static_cast<float>(height_) / resize_h - 0.5f;

    src_x = std::max(0.0f, std::min(src_x, static_cast<float>(width_  - 1)));
    src_y = std::max(0.0f, std::min(src_y, static_cast<float>(height_ - 1)));

    bilinear(src_x, src_y, b, g, r, a);
    return true;
}
