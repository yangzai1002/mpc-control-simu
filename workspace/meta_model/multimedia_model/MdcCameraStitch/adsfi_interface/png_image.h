#pragma once
#include <string>
#include <cstdint>
#include <opencv2/core.hpp>

/**
 * PngImage
 *
 * 读取一张 PNG 图像，提供虚拟 resize 后的像素查询接口。
 * 内部不实际 resize，而是通过双线性插值在原图上采样，等效于 resize 后取像素。
 */
class PngImage {
public:
    PngImage();

    // 加载 PNG 文件，返回 false 表示失败
    bool load(const std::string& path);

    int width()  const { return width_;  }
    int height() const { return height_; }
    bool loaded() const { return !img_.empty(); }

    /**
     * 获取图像在虚拟 resize 到 resize_w × resize_h 后，
     * 坐标 (px, py) 处的像素值（双线性插值）。
     *
     * 返回值：
     *   - 灰度图：bgr[0]=bgr[1]=bgr[2]=灰度值，alpha=255
     *   - BGR 图：bgr[0]=B, bgr[1]=G, bgr[2]=R，alpha=255
     *   - BGRA 图：bgr[0]=B, bgr[1]=G, bgr[2]=R，alpha=A
     *
     * px/py 超出 [0, resize_w/h) 范围时返回 false。
     */
    bool getpix(int resize_w, int resize_h, int px, int py,
                uint8_t& b, uint8_t& g, uint8_t& r, uint8_t& a) const;

private:
    cv::Mat img_;   // 原始图像（保持加载时的通道数）
    int     width_;
    int     height_;

    // 双线性插值，src_x/src_y 为原图浮点坐标
    void bilinear(float src_x, float src_y,
                  uint8_t& b, uint8_t& g, uint8_t& r, uint8_t& a) const;
};
