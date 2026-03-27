#ifndef DRAW_YUYV_H
#define DRAW_YUYV_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "color.hpp"

typedef struct ColorInfo {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char y;
    unsigned char u;
    unsigned char v;

    ColorInfo() {
        r = 0;
        g = 0;
        b = 0;
        y = 0;
        u = 0;
        v = 0;
    }

    void MergeColor(const ColorInfo &color_info, double rate = 1.0) {
        y = rate * this->y + (1 - rate) * color_info.y;
        u = rate * this->u + (1 - rate) * color_info.u;
        v = rate * this->v + (1 - rate) * color_info.v;
    }
    ColorInfo(unsigned char _r, unsigned char _g, unsigned char _b) {
        r = _r;
        g = _g;
        b = _b;

        y = 0.183 * r + 0.614 * g + 0.062 * b + 16;
        u = 0.439 * r - 0.399 * g - 0.040 * b + 128;
        v = -0.101 * b - 0.338 * g + 0.439 * b + 128;
    }

    void Set(unsigned char _r, unsigned char _g, unsigned char _b) {
        r = _r;
        g = _g;
        b = _b;

        y = 0.183 * r + 0.614 * g + 0.062 * b + 16;
        u = 0.439 * r - 0.399 * g - 0.040 * b + 128;
        v = -0.101 * b - 0.338 * g + 0.439 * b + 128;
    }

    void Set2RGB(unsigned char _y, unsigned char _u, unsigned char _v) {
        r = 1.164 * (_y - 16) + 1.792 * (_u - 128);
        g = 1.164 * (_y - 16) - 0.213 * (_v - 128) - 0.534 * (_u - 128);
        b = 1.164 * (_y - 16) + 2.114 * (_v - 128);
    }

} ColorInfo;

// 线程池类
class ThreadPool {
public:
    // 定义任务队列
    using Task = std::function<void()>;
    std::queue<Task>        task_queue;
    std::mutex              queue_mutex;
    std::condition_variable cond_var;
    bool                    done = false;

    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this] {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cond_var.wait(lock, [this] { return done || !task_queue.empty(); });
                        if (done && task_queue.empty()) {
                            return;
                        }
                        task = std::move(task_queue.front());
                        task_queue.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            done = true;
        }
        cond_var.notify_all();
        for (auto &thread : threads_) {
            thread.join();
        }
    }

    void enqueue(Task task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            task_queue.push(std::move(task));
        }
        cond_var.notify_one();
    }

    void wait_for_all_tasks() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        cond_var.wait(lock, [this] { return task_queue.empty() && done; });
    }

private:
    std::vector<std::thread> threads_;
};

class YUYV_Operator {
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
    static std::shared_ptr<YUYV_Operator> white_img(int width, int height) {
        constexpr unsigned char white_yuyv[4] = {255, 128, 255, 128}; // 两个白色像素：YUYV
        int                     size = width * height * 2;
        unsigned char *         buffer = new unsigned char[size];
        for (int i = 0; i < size; i += 4) {
            std::memcpy(buffer + i, white_yuyv, 4);
        }
        return std::make_shared<YUYV_Operator>(width, height, buffer, false);
    }

    static std::shared_ptr<YUYV_Operator> blue_img(int width, int height) {
        constexpr unsigned char blue_yuyv[4] = {41, 240, 41, 240}; // Y0 U Y1 V
        int                     size = width * height * 2;
        unsigned char *         buffer = new unsigned char[size];
        for (int i = 0; i < size; i += 4) {
            std::memcpy(buffer + i, blue_yuyv, 4);
        }
        return std::make_shared<YUYV_Operator>(width, height, buffer, false);
    }

public:
    YUYV_Operator(int width, int height, unsigned char *buffer, bool shared_buffer = true)
        : width_(width), height_(height), is_selfbuf(!shared_buffer) {
        size_ = width_ * height_ * 2;
        if (!shared_buffer) {
            buffer_ = new unsigned char[size_];
            memcpy(buffer_, buffer, size_);
        } else {
            buffer_ = buffer;
        }
    }

    YUYV_Operator(int width, int height, const unsigned char *buffer, bool shared_buffer = true)
        : YUYV_Operator(width, height, const_cast<unsigned char *>(buffer), shared_buffer) {
    }

    YUYV_Operator() {
    }
    // 赋值运算符
    YUYV_Operator &operator=(const YUYV_Operator &other) {
        if (this == &other) {
            return *this;
        }
        width_ = other.width_;
        height_ = other.height_;
        is_selfbuf = true;
        size_ = other.size_;

        if (buffer_ != other.buffer_) {
            clear_buff();
        }

        buffer_ = new uint8_t[width_ * height_ * 2];
        std::memcpy(buffer_, other.buffer_, size_);
        return *this;
    }

    // for bgr mat
    YUYV_Operator(cv::Mat &data, size_t num_threads = 4) : color(num_threads), is_selfbuf(true) {
        assert(data.channels() == 3);
        width_ = data.cols;
        height_ = data.rows;
        cv::Mat yuyv_data;
        color.rgb2yuyv(data, yuyv_data);
        size_ = width_ * height_ * 2;  // YUYV格式，每个像素2个字节
        buffer_ = new u_int8_t[size_]; //(u_int8_t * )malloc(size_);
        memcpy(buffer_, yuyv_data.ptr<unsigned char>(), size_);
    }

    YUYV_Operator(int width, int height) : width_(width), height_(height) {
        size_ = width_ * height_ * 2;  // YUYV格式，每个像素2个字节
        buffer_ = new u_int8_t[size_]; //(u_int8_t * )malloc(size_);
        std::cout << size_ << "==================init yuyv buffer  ==================\n";
        is_selfbuf = true;
    }

    ~YUYV_Operator() {
        clear_buff();
    }

    void updatabuffer(uint8_t *data) {
        memcpy(buffer_, data, width_ * height_ * 2);
    }

    void get_rgb(cv::Mat &rgb) {
        rgb.create(height_, width_, CV_8UC3);
        cv::Mat yuyv;
        yuyv.create(height_, width_, CV_8UC2);
        memcpy(yuyv.ptr<unsigned char>(), buffer_, size_);
        color.yuyv2rgb(yuyv, rgb);
    }

    void MergeData(std::shared_ptr<YUYV_Operator> other, int other_start_pos_left, int other_start_pos_top,
                   int other_width, int other_height, int start_pos_left, int start_pos_top, int width, int height,
                   bool bflip = false) {
        if (start_pos_left > this->width_) {
            std::cout << "cannot merge width pos is error " << start_pos_left << "," << other->width_ << std::endl;
            return;
        }

        if (start_pos_top > this->height_) {
            std::cout << "cannot merge height pos is error " << start_pos_top << "," << other->height_ << std::endl;
            return;
        }

        ColorInfo temp_info;

        int x_step = other_width / width;
        int y_step = other_height / height;

        int t_i = start_pos_left;
        if (bflip) {
            t_i = start_pos_left + width - 1;
        }

        for (int i = other_start_pos_left; i < other_start_pos_left + other_width; i += x_step) {
            if (t_i >= width_) {
                continue;
            }
            int t_j = start_pos_top;
            for (int j = other_start_pos_top; j < other_start_pos_top + other_height; j += y_step) {
                if (t_j >= height_) {
                    continue;
                }

                if (other->GetPixel(i, j, temp_info) == 0) {
                    drawPixel(t_i, t_j, temp_info);
                }
                t_j++;
            }
            if (bflip)
                t_i--;
            else
                t_i++;
        }
    }

    void MergeData(YUYV_Operator &other, int other_start_pos_left, int other_start_pos_top, int other_width,
                   int other_height, int start_pos_left, int start_pos_top, int width, int height, bool bflip = false) {
        if (start_pos_left > this->width_) {
            std::cout << "cannot merge width pos is error " << start_pos_left << "," << other.width_ << std::endl;
            return;
        }

        if (start_pos_top > this->height_) {
            std::cout << "cannot merge height pos is error " << start_pos_top << "," << other.height_ << std::endl;
            return;
        }

        ColorInfo temp_info;

        int x_step = other_width / width;
        int y_step = other_height / height;

        int t_i = start_pos_left;
        if (bflip) {
            t_i = start_pos_left + width - 1;
        }

        for (int i = other_start_pos_left; i < other_start_pos_left + other_width; i += x_step) {
            if (t_i >= width_) {
                continue;
            }
            int t_j = start_pos_top;
            for (int j = other_start_pos_top; j < other_start_pos_top + other_height; j += y_step) {
                if (t_j >= height_) {
                    continue;
                }

                if (other.GetPixel(i, j, temp_info) == 0) {
                    drawPixel(t_i, t_j, temp_info);
                }
                t_j++;
            }
            if (bflip)
                t_i--;
            else
                t_i++;
        }
    }

    void MergeDataV2(std::shared_ptr<YUYV_Operator> other, int other_start_pos_left, int other_start_pos_top,
                     int other_width, int other_height, int start_pos_left, int start_pos_top, int width, int height) {
        int x_step = other_width / width;
        int y_step = other_height / height;
        for (size_t i = 0; i < height; i++) {
            uchar *y_ptr = buffer_ + (start_pos_top + i) * width_ * 2;
            uchar *other_y_ptr = other->buffer_ + (other_start_pos_top + i * y_step) * other->width_ * 2;
            for (size_t j = 0; j < width; j += 2) {
                uchar *x_ptr = y_ptr + (start_pos_left + j) * 2;
                uchar *other_x_ptr = other_y_ptr + (other_start_pos_left + j * x_step) * 2;
                memcpy(x_ptr, other_x_ptr, 4);
            }
        }
    }

    void MergeDataV3(std::shared_ptr<YUYV_Operator> other, int other_start_pos_left, int other_start_pos_top,
                     int other_width, int other_height, int start_pos_left, int start_pos_top, int width, int height) {
        // 基础校验
        if (!other || !other->buffer_ || !this->buffer_) {
            std::cerr << "MergeDataV2: invalid buffer pointer\n";
            return;
        }

        if (width % 2 != 0 || other_width % 2 != 0 || start_pos_left % 2 != 0 || other_start_pos_left % 2 != 0) {
            std::cerr << "MergeDataV2: width and start positions must be even for YUYV format\n";
            return;
        }

        // 防止越界
        if (start_pos_left < 0 || start_pos_top < 0 || start_pos_left + width > width_ ||
            start_pos_top + height > height_) {
            std::cerr << "MergeDataV2: target area out of bounds\n";
            return;
        }
        if (other_start_pos_left < 0 || other_start_pos_top < 0 || other_start_pos_left + other_width > other->width_ ||
            other_start_pos_top + other_height > other->height_) {
            std::cerr << "MergeDataV2: source area out of bounds\n";
            return;
        }

        // 缩放步长（采样间隔）
        int x_step = std::max(1, 2 * other_width / width); // 2字节=1像素
        int y_step = std::max(1, other_height / height);   // 垂直采样步长

        for (int i = 0; i < height; ++i) {
            // 当前图目标位置起始行指针
            uchar *dst_row = buffer_ + (start_pos_top + i) * width_ * 2 + start_pos_left * 2;

            // 来源图起始行
            int    src_y = other_start_pos_top + i * y_step;
            uchar *src_row = other->buffer_ + src_y * other->width_ * 2 + other_start_pos_left * 2;

            for (int j = 0; j < width; j += 2) {
                int    src_x = j * x_step;
                uchar *dst_ptr = dst_row + j * 2;
                uchar *src_ptr = src_row + src_x;

                // 拷贝 4 字节（两个像素）
                memcpy(dst_ptr, src_ptr, 4);
            }
        }
    }

    void MergeDataByRaw(std::shared_ptr<YUYV_Operator> other, int other_start_pos_left, int other_start_pos_top,
                        int other_width, int other_height, int start_pos_left, int start_pos_top, int width,
                        int height) {
        if (start_pos_left > this->width_ || start_pos_left + width > this->width_) {
            std::cout << "cannot merge width pos is error " << start_pos_left << "," << this->width_ << std::endl;
            return;
        }

        if (start_pos_top > this->height_ || start_pos_top + height > this->height_) {
            std::cout << "cannot merge height pos is error " << start_pos_top << "," << this->height_ << std::endl;
            return;
        }

        if (other_start_pos_left > other->width_ || other_start_pos_left + other_width > other->width_) {
            std::cout << "cannot merge width pos is error " << other_start_pos_left << "," << other->width_
                      << std::endl;
            return;
        }

        if (other_start_pos_top > other->height_ || other_start_pos_top + other_height > other->height_) {
            std::cout << "cannot merge height pos is error " << other_start_pos_top << "," << other->height_
                      << std::endl;
            return;
        }

        int other_start_index = (other_start_pos_top * other->width_ + other_start_pos_left) * 2;
        int this_start_index = (start_pos_top * this->width_ + start_pos_left) * 2;

        int row_size = width * 2; // 每行的字节数

        for (int i = 0; i < height; ++i) {
            int src_row_start = other_start_index + i * other->width_ * 2;
            int dst_row_start = this_start_index + i * this->width_ * 2;

            memcpy(this->buffer_ + dst_row_start, other->buffer_ + src_row_start, other_width * 2);
            // // 每次拷贝两个像素的数据（4 个字节）
            // for (int j = 0; j < width; j += 2) {
            //     memcpy(this->buffer_ + dst_row_start + j * 2,
            //            other->buffer_ + src_row_start + j * 2,
            //            4); // 每两个像素占用 4 个字节
            // }
        }
    }
    void MergeData(const YUYV_Operator &other, int start_pos_left, int start_pos_top, int width, int height,
                   ThreadPool &pool) {
        if (start_pos_left > this->width_) {
            std::cout << "cannot merge width pos is error " << start_pos_left << "," << other.width_ << std::endl;
            return;
        }

        if (start_pos_top > this->height_) {
            std::cout << "cannot merge height pos is error " << start_pos_top << "," << other.height_ << std::endl;
            return;
        }

        if (other.width_ < width) {
            std::cout << "cannot get bigger width " << std::endl;
            return;
        }

        if (other.height_ < height) {
            std::cout << "cannot get bigger height " << std::endl;
            return;
        }

        int current_width = width;
        int current_height = height;

        if (start_pos_left + width > other.width_) {
            current_width = other.width_ - start_pos_left;
        }

        if (start_pos_top + height > other.height_) {
            current_height = other.height_ - start_pos_top;
        }

        int x_step = other.width_ / width;
        int y_step = other.height_ / height;

        // 将任务分解为100x100的块
        const int        BLOCK_SIZE = 200;
        std::atomic<int> tasks_count(0);

        for (int i = 0; i < current_width; i += BLOCK_SIZE) {
            for (int j = 0; j < current_height; j += BLOCK_SIZE) {
                int block_width = std::min(BLOCK_SIZE, current_width - i);
                int block_height = std::min(BLOCK_SIZE, current_height - j);
                tasks_count.fetch_add(1);
                pool.enqueue(
                    [this, &other, start_pos_left, start_pos_top, i, j, block_width, block_height, &tasks_count] {
                        ColorInfo temp_info;
                        for (int x = 0; x < block_width; ++x) {
                            for (int y = 0; y < block_height; ++y) {
                                int src_x = start_pos_left + i + x;
                                int src_y = start_pos_top + j + y;
                                int dst_x = src_x;
                                int dst_y = src_y;

                                if (other.GetPixel(src_x, src_y, temp_info) == 0) {
                                    this->drawPixel(dst_x, dst_y, temp_info);
                                }
                            }
                        }
                        tasks_count.fetch_sub(1);
                    });
            }
        }

        // 等待所有任务完成
        while (tasks_count.load() > 0) {
            std::this_thread::yield();
        }
    }

    void drawText(int sx, int sy, const std::string &text, const cv::Scalar &color, const cv::Scalar &bgcolor,
                  int fontFace = cv::FONT_HERSHEY_DUPLEX, double fontScale = 1.2, int thickness = 1) {
        int      baseline = 0;
        cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
        baseline += thickness;

        cv::Mat   image((baseline + 20 + textSize.height) * fontScale, textSize.width + 10, CV_8UC3,
                      cv::Scalar(255, 255, 255));
        cv::Point textOrg(10, (baseline + textSize.height) * fontScale);

        // 在白色背景的图像上绘制文本
        cv::putText(image, text, textOrg, fontFace, fontScale, color, thickness);

        ColorInfo temp;
        // 遍历图像，找到所有非白色像素的位置
        for (int y = 0; y < image.rows; ++y) {
            for (int x = 0; x < image.cols; ++x) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                if (pixel[0] != 255 || pixel[1] != 255 || pixel[2] != 255) {
                    temp.Set(pixel[0], pixel[1], pixel[2]);
                    drawPixel(sx + x, sy + y, temp);
                } else {
                    temp.Set(bgcolor[0], bgcolor[1], bgcolor[2]);
                    drawPixel(sx + x, sy + y, temp);
                }
            }
        }
    }

    int GetPixel(int x, int y, ColorInfo &pixel_info) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            std::cerr << "Pixel coordinates are out of range." << std::endl;
            return -1;
        }

        int index = (y * width_ + x) * 2;

        if (x % 2 == 0) {
            if ((index + 3) > (width_ * height_ * 2 - 1)) {
                std::cout << "Pixel coordinates are out of range." << std::endl;
                return -1;
            }
            pixel_info.y = buffer_[index];
            pixel_info.u = buffer_[index + 1];
            pixel_info.v = buffer_[index + 3];
        } else {
            if ((index + 1) > (width_ * height_ * 2 - 1)) {
                std::cout << "Pixel coordinates are out of range." << std::endl;
                return -1;
            }
            pixel_info.y = buffer_[index];
            pixel_info.u = buffer_[index - 1];
            pixel_info.v = buffer_[index + 1];
        }

        return 0;
    }

    int GetRgb(int x, int y, ColorInfo &pixel_info) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            std::cerr << "Pixel coordinates are out of range." << std::endl;
            return -1;
        }

        int index = (y * width_ + x) * 2;

        if (x % 2 == 0) {
            if ((index + 3) > (width_ * height_ * 2 - 1)) {
                std::cout << "Pixel coordinates are out of range." << std::endl;
                return -1;
            }
            pixel_info.y = buffer_[index];
            pixel_info.u = buffer_[index + 1];
            pixel_info.v = buffer_[index + 3];
        } else {
            if ((index + 1) > (width_ * height_ * 2 - 1)) {
                std::cout << "Pixel coordinates are out of range." << std::endl;
                return -1;
            }
            pixel_info.y = buffer_[index];
            pixel_info.u = buffer_[index - 1];
            pixel_info.v = buffer_[index + 1];
        }

        return 0;
    }

    void FlipHor() {
        ColorInfo in_color_info;
        ColorInfo out_color_info;
        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_ / 2; j++) {
                GetPixel(j, i, in_color_info);
                GetPixel(width_ - j - 1, i, out_color_info);
                drawPixel(width_ - j - 1, i, in_color_info);
                drawPixel(j, i, out_color_info);
            }
        }
    }

    void FlipVer() {
        ColorInfo in_color_info;
        ColorInfo out_color_info;
        for (int i = 0; i < width_; i++) {
            for (int j = 0; j < height_ / 2; j++) {
                GetPixel(i, j, in_color_info);
                GetPixel(i, height_ - j - 1, out_color_info);
                drawPixel(i, height_ - j - 1, in_color_info);
                drawPixel(i, j, out_color_info);
            }
        }
    }

    void drawPixel(int x, int y, const ColorInfo &color, double rate = 1.0) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            // std::cout << "Pixel coordinates are out of range." << std::endl;
            return;
        }
        // if(x==0)
        //     printf("drawPixel buffer_ = 0x%x\n",buffer_);
        int index = (y * width_ + x) * 2;

        if ((index + 1) > (width_ * height_ * 2 - 1)) {
            std::cout << "Pixel coordinates are out of range." << std::endl;
            return;
        }
        if (x % 2 == 0) {
            // Y0 U Y1 V
            buffer_[index] = rate * color.y + (1 - rate) * buffer_[index];
            buffer_[index + 1] = rate * color.u + (1 - rate) * buffer_[index + 1];
        } else {
            buffer_[index] = rate * color.y + (1 - rate) * buffer_[index];
            buffer_[index + 1] = rate * color.v + (1 - rate) * buffer_[index + 1];
        }
    }

    void drawPixel(int x, int y, unsigned char y_value, unsigned char u_value, unsigned char v_value, double rate = 1.0,
                   int fix_x_offset = 0) {
        x = x + fix_x_offset;
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            // std::cout << "Pixel coordinates are out of range." << std::endl;
            return;
        }

        int index = (y * width_ + x) * 2;
        if (x % 2 == 0) {
            // Y0 U Y1 V
            buffer_[index] = rate * y_value + (1 - rate) * buffer_[index];
            buffer_[index + 1] = rate * u_value + (1 - rate) * buffer_[index + 1];
        } else {
            buffer_[index] = rate * y_value + (1 - rate) * buffer_[index];
            buffer_[index + 1] = rate * v_value + (1 - rate) * buffer_[index + 1];
        }
    }

    u_int8_t *get_data() {
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
    int width_;
    int height_;
    int size_;
    // std::vector<unsigned char> buffer_;
    u_int8_t *buffer_ = nullptr;
    bool      is_selfbuf = false;
    Color     color;
};

#endif
