#ifndef __COLOR__
#define __COLOR__

#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <memory>
#include <opencv2/core.hpp>
#include <atomic>

#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <mutex>
#include <functional>
#include <condition_variable>
#include <thread>
#include <queue>

class ThreadPoolColor
{
public:
    using Task = std::function<void()>;

private:
    std::queue<Task> task_queue;
    std::mutex queue_mutex;
    std::condition_variable cond_var;
    std::condition_variable completion_var;
    bool done = false;
    std::atomic<int> active_tasks{0};
    std::vector<std::thread> threads_;

public:
    ThreadPoolColor(size_t num_threads)
    {
        for (size_t i = 0; i < num_threads; ++i)
        {
            threads_.emplace_back([this]
                                  {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cond_var.wait(lock, [this] { 
                            return done || !task_queue.empty(); 
                        });
                        
                        if (done && task_queue.empty()) {
                            return;
                        }
                        
                        task = std::move(task_queue.front());
                        task_queue.pop();
                        active_tasks++;
                    }
                    
                    task();
                    
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        active_tasks--;
                        if (active_tasks == 0 && task_queue.empty()) {
                            completion_var.notify_all();
                        }
                    }
                } });
        }
    }

    ~ThreadPoolColor()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            done = true;
        }
        cond_var.notify_all();
        for (auto &thread : threads_)
        {
            thread.join();
        }
    }

    void enqueue(Task task)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            task_queue.push(std::move(task));
        }
        cond_var.notify_one();
    }

    void wait_for_all_tasks()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        completion_var.wait(lock, [this]
                            { return task_queue.empty() && active_tasks == 0; });
    }
};
#endif

template <typename T>
T clamp(T v, T lo, T hi)
{
    return (v < lo) ? lo : (v > hi) ? hi
                                    : v;
}

class Color
{
private:
    std::array<std::array<double, 4>, 6> _cvt_param;
    std::array<int, 6> _bias;
    size_t _num_threads;
#ifdef __USE_SMART_PTR__
    static std::unique_ptr<unsigned char[]> _color_table_rgb2yuyv;
    static bool _initialized;
#else
    static unsigned char *_color_table_rgb2yuyv;
    static unsigned char *_color_table_yuyv2rgb;
#endif

    inline uint32_t color_index(unsigned char r, unsigned char g, unsigned char b) const
    {
        return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b) * 3;
    }

    void build_color_table_rgb2yuyv()
    {
#ifdef __USE_SMART_PTR__
        if (_initialized)
            return;
        _color_table_rgb2yuyv.reset(new unsigned char[256 * 256 * 256 * 3]);
        _initialized = true;
#else
        if (_color_table_rgb2yuyv != nullptr)
            return;

        _color_table_rgb2yuyv = static_cast<unsigned char *>(aligned_alloc(64, 256 * 256 * 256 * 3));
        _color_table_yuyv2rgb = static_cast<unsigned char *>(aligned_alloc(64, 256 * 256 * 256 * 3));
        if (!_color_table_rgb2yuyv)
        {
            throw std::bad_alloc();
        }

        for (int r = 0; r < 256; ++r)
        {
            for (int g = 0; g < 256; ++g)
            {
                for (int b = 0; b < 256; ++b)
                {
                    uint32_t index = color_index(r, g, b);
                    get_yuv_float(r, g, b,
                                  _color_table_rgb2yuyv[index],
                                  _color_table_rgb2yuyv[index + 1],
                                  _color_table_rgb2yuyv[index + 2]);
                }
            }
        }
        for (int y = 0; y < 256; ++y)
        {
            for (int u = 0; u < 256; ++u)
            {
                for (int v = 0; v < 256; ++v)
                {
                    uint32_t index = color_index(y, u, v);
                    get_rgb_float(y, u, v,
                                  _color_table_yuyv2rgb[index],
                                  _color_table_yuyv2rgb[index + 1],
                                  _color_table_yuyv2rgb[index + 2]);
                }
            }
        }

#endif
    }

public:
    Color(size_t num_threads = 2) : _num_threads(num_threads)
    {
        _cvt_param = {
            // rgb2yuv
            std::array<double, 4>{0.257, 0.504, 0.098, 16},
            std::array<double, 4>{-0.148, -0.291, 0.439, 128},
            std::array<double, 4>{0.439, -0.368, -0.071, 128},
            // yuv2rgb
            std::array<double, 4>{1.164, 0.0, 1.596, -223},
            std::array<double, 4>{1.164, -0.391, -0.813, 135},
            std::array<double, 4>{1.164, 2.018, 0.0, -277},
        };

        for (int i = 0; i < 6; ++i)
        {
            _bias[i] = static_cast<int>(_cvt_param[i][3]);
        }

        build_color_table_rgb2yuyv();
    }

    inline bool get_yuv_float(const unsigned char r, const unsigned char g, const unsigned char b, unsigned char &y, unsigned char &u, unsigned char &v)
    {
        double dy = 0.257 * r + 0.504 * g + 0.098 * b + 16;
        double du = -0.148 * r - 0.291 * g + 0.439 * b + 128;
        double dv = 0.439 * r - 0.368 * g - 0.071 * b + 128;
        y = clamp<int>(dy, 0, 255);
        u = clamp<int>(du, 0, 255);
        v = clamp<int>(dv, 0, 255);
        return true;
    }

    inline bool get_rgb_float(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char &r, unsigned char &g, unsigned char &b)
    {
        double dr = 1.164 * y + 0.0 * u + 1.596 * v - 223;
        double dg = 1.164 * y - 0.391 * u - 0.813 * v + 135;
        double db = 1.164 * y + 2.018 * u + 0.0 * v - 277;
        r = clamp<int>(dr, 0, 255);
        g = clamp<int>(dg, 0, 255);
        b = clamp<int>(db, 0, 255);
        return true;
    }

    inline bool get_yuv(const unsigned char r, const unsigned char g, const unsigned char b, unsigned char &y, unsigned char &u, unsigned char &v)
    {
        uint32_t index0 = color_index(r, g, b);
        y = _color_table_rgb2yuyv[index0];
        u = _color_table_rgb2yuyv[index0 + 1];
        v = _color_table_rgb2yuyv[index0 + 2];
        return true;
    }

    inline bool get_rgb(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char &r, unsigned char &g, unsigned char &b)
    {
        uint32_t index0 = color_index(y, u, v);
        r = _color_table_yuyv2rgb[index0];
        g = _color_table_yuyv2rgb[index0 + 1];
        b = _color_table_yuyv2rgb[index0 + 2];
        return true;
    }

    inline bool rgb2yuyv(const cv::Mat &rgb, cv::Mat &yuyv)
    {
        CV_Assert(rgb.type() == CV_8UC3);
        const int width = rgb.cols;
        const int height = rgb.rows;
        yuyv.create(height, width, CV_8UC2);

        const int block_size = 200;
        ThreadPoolColor pool(_num_threads);

        for (int i = 0; i < height; i += block_size)
        {
            const int y_min = i;
            const int y_max = std::min(i + block_size, height);

            pool.enqueue([&, y_min, y_max]()
                         {
            for (int y = y_min; y < y_max; ++y) {
                const uchar* rgb_row = rgb.ptr<uchar>(y);
                uchar* yuyv_row = yuyv.ptr<uchar>(y);

                for (int x = 0; x < width; x += 2) {
                    uchar y0, u0, v0;
                    uchar y1, u1, v1;
                    
                    get_yuv(rgb_row[x*3+2], rgb_row[x*3+1], rgb_row[x*3], y0, u0, v0);
                    get_yuv(rgb_row[(x+1)*3+2], rgb_row[(x+1)*3+1], rgb_row[(x+1)*3], y1, u1, v1);
                    
                    // 修正UV计算方式
                    yuyv_row[x*2] = y0;         // Y0
                    yuyv_row[x*2+1] = (u0 + u1)/2; // U (平均值)
                    yuyv_row[x*2+2] = y1;       // Y1
                    yuyv_row[x*2+3] = (v0 + v1)/2; // V (平均值)
                }
            } });
        }
        pool.wait_for_all_tasks();
        return true;
    }

    inline bool yuyv2rgb(const cv::Mat &yuyv, cv::Mat &rgb)
    {
        CV_Assert(yuyv.type() == CV_8UC2);
        const int width = yuyv.cols;
        const int height = yuyv.rows;
        rgb.create(height, width, CV_8UC3);

        const int block_size = 200;
        ThreadPoolColor pool(_num_threads);

        for (int i = 0; i < height; i += block_size)
        {
            const int y_min = i;
            const int y_max = std::min(i + block_size, height);

            pool.enqueue([&, y_min, y_max]()
                         {
            for (int y = y_min; y < y_max; ++y) {
                const uchar* yuyv_row = yuyv.ptr<uchar>(y);
                uchar* rgb_row = rgb.ptr<uchar>(y);

                for (int x = 0; x < width; x += 2) {
                    // 读取YUYV数据（每4字节对应2个像素）
                    uchar y0 = yuyv_row[x*2];
                    uchar u = yuyv_row[x*2+1];
                    uchar y1 = yuyv_row[x*2+2];
                    uchar v = yuyv_row[x*2+3];

                    uchar r0, g0, b0, r1, g1, b1;
                    get_rgb(y0, u, v, r0, g0, b0);
                    get_rgb(y1, u, v, r1, g1, b1);

                    // 写入RGB数据
                    rgb_row[x*3] = b0;
                    rgb_row[x*3+1] = g0;
                    rgb_row[x*3+2] = r0;
                    
                    rgb_row[(x+1)*3] = b1;
                    rgb_row[(x+1)*3+1] = g1;
                    rgb_row[(x+1)*3+2] = r1;
                }
            } });
        }
        pool.wait_for_all_tasks();
        return true;
    }
};



#endif