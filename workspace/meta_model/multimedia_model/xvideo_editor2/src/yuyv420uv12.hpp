#ifndef DRAW_YUYV_H
#define DRAW_YUYV_H

#include <opencv2/imgproc/types_c.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <opencv2/imgproc.hpp>
#include <queue>
#include <thread>
#include <vector>

class YUYV420UV12 {
private:
    static uchar *pure_image_buffer(int width, int height, const uchar y[4], const uchar uv[2]) {
        int            total = width * height;
        int            size = total * 1.5;
        unsigned char *buffer = new unsigned char[size];
        auto           y_buffer = buffer;
        auto           uv_buffer = buffer + total;
        for (int i = 0; i < width * height; i += 4) {
            std::memcpy(y_buffer + i, y, 4);
            std::memcpy(uv_buffer + i / 2, uv, 2);
        }
        return buffer;
    }

public:
    static std::shared_ptr<YUYV420UV12> white_img(int width, int height) {
        constexpr unsigned char y[4] = {255, 255, 255, 255}; // Y=255 白色
        constexpr unsigned char uv[2] = {128, 128};          // U=128, V=128 中性

        // 分配原始 YUV420SP buffer
        auto *raw = YUYV420UV12::pure_image_buffer(width, height, y, uv);

        // 创建 shared_ptr，添加自定义 deleter 来释放 raw buffer
        return std::shared_ptr<YUYV420UV12>(new YUYV420UV12(width, height, raw, true), // shared_buffer = true 不再复制
                                            [raw](YUYV420UV12 *ptr) {
                                                delete[] raw; // 释放 YUV 数据
                                                delete ptr;   // 释放图像对象
                                            });
    }

    static std::shared_ptr<YUYV420UV12> blue_img(int width, int height) {
        constexpr unsigned char y[4] = {41, 41, 41, 41};
        constexpr unsigned char uv[2] = {240, 240};
        return std::make_shared<YUYV420UV12>(width, height, YUYV420UV12::pure_image_buffer(width, height, y, uv),
                                             false);
    }

private:
    void clear_buff() {
        if (is_selfbuf && nullptr != buffer_) {
            delete[] buffer_;
            buffer_ = nullptr;
        }
    }

public:
    void release_buff() {
        is_selfbuf = false;
    }

    // 将当前图像恢复为白底
    void resetToWhite() {
        if (!buffer_)
            return; // 安全检查
        constexpr unsigned char y[4] = {255, 255, 255, 255};
        constexpr unsigned char uv[2] = {128, 128};

        // 填充 Y 分量
        int y_size = width_ * height_;
        for (int i = 0; i < y_size; i += 4) {
            std::memcpy(buffer_ + i, y, 4);
        }

        // 填充 UV 分量
        int    uv_size = y_size / 2; // UV plane size = 1/2 Y plane
        uchar *uv_ptr = buffer_ + y_size;
        for (int i = 0; i < uv_size; i += 2) {
            std::memcpy(uv_ptr + i, uv, 2);
        }
    }

    YUYV420UV12(int width, int height, unsigned char *buffer, bool shared_buffer = true)
        : width_(width), height_(height), is_selfbuf(!shared_buffer) {
        size_ = width_ * height_ * 1.5;
        if (!shared_buffer) {
            buffer_ = new unsigned char[size_];
            memcpy(buffer_, buffer, size_);
        } else {
            buffer_ = buffer;
        }
    }

    YUYV420UV12(int width, int height, const unsigned char *buffer, bool shared_buffer = true)
        : YUYV420UV12(width, height, const_cast<unsigned char *>(buffer), shared_buffer) {
    }

    YUYV420UV12() {
    }

    // 赋值运算符
    YUYV420UV12 &operator=(const YUYV420UV12 &other) {
        if (this == &other) {
            return *this;
        }
        width_ = other.width_;
        height_ = other.height_;
        size_ = other.size_;

        if (buffer_ != other.buffer_ && is_selfbuf) {
            clear_buff();
        }
        is_selfbuf = true;

        buffer_ = new uint8_t[int(width_ * height_ * 1.5)];
        memcpy(buffer_, other.buffer_, size_);
        return *this;
    }

    void swap_psp() {
        size_t y_size = width_ * height_;
        size_t uv_plane_size = y_size / 4;

        uchar *u_plane = buffer_ + y_size;
        uchar *v_plane = u_plane + uv_plane_size;

        for (size_t i = 0; i < uv_plane_size; ++i) {
            u_plane[2 * i] = u_plane[i]; // safe 因为写入在前
            u_plane[2 * i + 1] = v_plane[i];
        }
    }

    void swap_spp() {
        size_t y_size = width_ * height_;
        size_t uv_plane_size = y_size / 4;

        uchar *uv_plane = buffer_ + y_size;
        uchar *u_plane = uv_plane;
        uchar *v_plane = uv_plane + uv_plane_size;

        for (int i = uv_plane_size - 1; i >= 0; --i) { // 反向拷贝，避免覆盖
            u_plane[i] = uv_plane[2 * i];
            v_plane[i] = uv_plane[2 * i + 1];
        }
    }

    // for bgr mat
    YUYV420UV12(cv::Mat &data)
        : width_(data.cols), height_(data.rows), size_(size_ = width_ * height_ * 1.5), is_selfbuf(true) {
        assert(data.channels() == 3);
        cv::Mat yuv420;
        cv::cvtColor(data, yuv420, CV_BGR2YUV_I420);
        buffer_ = new u_int8_t[size_];
        std::memcpy(buffer_, yuv420.data, size_);
        swap_psp();
    }

    YUYV420UV12(int width, int height) : width_(width), height_(height), is_selfbuf(true) {
        size_ = width_ * height_ * 1.5;
        buffer_ = new u_int8_t[size_];
    }

    ~YUYV420UV12() {
        clear_buff();
    }

    void updatabuffer(uint8_t *data) {
        memcpy(buffer_, data, int(width_ * height_ * 1.5));
    }

    void get_rgb(cv::Mat &bgr) {
        swap_spp();
        cv::Mat yuv420(height_ + height_ / 2, width_, CV_8UC1, buffer_);
        cv::cvtColor(yuv420, bgr, cv::COLOR_YUV2BGR_I420);
        return;
    }

    void MergeDataV2(std::shared_ptr<YUYV420UV12> other, int other_start_pos_left, int other_start_pos_top,
                     int other_width, int other_height, int start_pos_left, int start_pos_top, int width, int height) {
        int    x_step = other_width / width;
        int    y_step = other_height / height;
        uchar *uv_buffer = buffer_ + width_ * height_;
        uchar *other_uv_buffer = other->buffer_ + other->width_ * other->height_;
        for (size_t i = 0; i < height; i += 2) {
            size_t y_shift = (start_pos_top + i) * width_;
            size_t other_y_shift = (other_start_pos_top + i * y_step) * other->width_;
            for (size_t j = 0; j < width; j += 2) {
                size_t ptr_shift = y_shift + (start_pos_left + j);
                size_t other_ptr_shift = other_y_shift + (other_start_pos_left + j * x_step);
                uchar *y_ptr = buffer_ + ptr_shift;
                uchar *other_y_ptr = other->buffer_ + other_ptr_shift;
                memcpy(y_ptr, other_y_ptr, 2);
                memcpy(y_ptr + width_, other_y_ptr + other->width_, 2);
                uchar *uv_ptr = uv_buffer + int((y_shift / 2 + (start_pos_left + j)) & (~1));
                uchar *other_uv_ptr =
                    other_uv_buffer + int((other_y_shift / 2 + (other_start_pos_left + j * x_step)) & (~1));
                // uchar *uv_ptr = uv_buffer + int(y_shift >> 1 + start_pos_left & (~1) + j);
                // uchar *other_uv_ptr =
                // other_uv_buffer + int(other_y_shift >> 1 + other_start_pos_left & (~1) + j * x_step);
                memcpy(uv_ptr, other_uv_ptr, 2);
            }
        }
    }

    // after all, it works
    // void MergeDataV2(std::shared_ptr<YUYV420UV12> other, int other_start_pos_left, int other_start_pos_top,
    //                  int other_width, int other_height, int start_pos_left, int start_pos_top, int width, int height)
    //                  {
    //     int    x_step = other_width / width;
    //     int    y_step = other_height / height;
    //     uchar *uv_buffer = buffer_ + width_ * height_;
    //     uchar *other_uv_buffer = other->buffer_ + other->width_ * other->height_;
    //     for (size_t i = 0; i < height; i += 1) {
    //         size_t y_shift = (start_pos_top + i) * width_;
    //         size_t other_y_shift = (other_start_pos_top + i * y_step) * other->width_;
    //         for (size_t j = 0; j < width; j += 4) {
    //             size_t ptr_shift = y_shift + (start_pos_left + j);
    //             size_t other_ptr_shift = other_y_shift + (other_start_pos_left + j * x_step);
    //             uchar *y_ptr = buffer_ + ptr_shift;
    //             uchar *other_y_ptr = other->buffer_ + other_ptr_shift;
    //             memcpy(y_ptr, other_y_ptr, 4);
    //             // memcpy(y_ptr + width_, other_y_ptr + other->width_, 2);
    //             uchar *uv_ptr = uv_buffer + ptr_shift / 2;
    //             uchar *other_uv_ptr = other_uv_buffer + other_ptr_shift / 2;
    //             memcpy(uv_ptr, other_uv_ptr, 2);
    //         }
    //     }
    // }

    u_int8_t *get_data() {
        return buffer_;
    }

    const u_int8_t *get_data() const {
        return buffer_;
    }

    std::string getCurrentTime() {
        time_t now = time(0);
        char   timeStr[100];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return std::string(timeStr);
    }

    int width() const {
        return width_;
    }

    int height() const {
        return height_;
    }

    int size() const {
        return size_;
    }

private:
    int       width_;
    int       height_;
    int       size_;
    u_int8_t *buffer_ = nullptr;
    bool      is_selfbuf = false;
};

#endif
