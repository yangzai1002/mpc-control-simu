#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "yuyv420uv12.hpp"

struct YUVColor {
    uint8_t y, u, v;
};

struct SplitImageParame {
    double                       width;
    double                       height;
    int                          index; //序号
    std::shared_ptr<YUYV420UV12> image;
    cv::Point2f                  view_left_top_point;
    cv::Point2f                  view_right_bottom_point;

    cv::Point2f crop_left_top_point;
    cv::Point2f crop_right_bottom_point;
    std::string channel_name;

    int   totalSize;
    void* src_ptr;
};

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t num_threads) : done(false), active_tasks(0) {
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
                        active_tasks++;
                    }
                    task();
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        active_tasks--;
                        if (task_queue.empty() && active_tasks == 0) {
                            finished_cond.notify_all();
                        }
                    }
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
        for (auto& thread : threads_) {
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

    // 等待所有任务完成（队列空且无活跃任务）
    void wait_for_all_tasks() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        finished_cond.wait(lock, [this] { return task_queue.empty() && active_tasks == 0; });
    }

private:
    std::vector<std::thread> threads_;
    std::queue<Task>         task_queue;
    std::mutex               queue_mutex;
    std::condition_variable  cond_var;
    std::condition_variable  finished_cond;
    bool                     done;
    size_t                   active_tasks;
};

class VideoEditYUV420UV12 {
public:
    static void drawRectangleNV12(uint8_t* data, int width, int height, int x0, int y0, int x1, int y1, int thickness);

    static void DrawLine(void* img, const int& width, const int& height, const std::vector<cv::Point>& points,
                         uint8_t y_value, uint8_t u_value, uint8_t v_value, int thickness = 1);

    /**
     * 静态填充接口：将YUV420SP原始数据绘制的两条线段中的区域进行填充
     * @param src 原始YUV420SP数据指针
     * @param polygon 填充区域的顶点坐标集合
     * @param y_value y颜色
     * @param u_value u颜色值
     * @param v_value z颜色值
     *  @param alpha 透明度，范围0-1.0f
     */
    static void FillPolygonYUV420(YUYV420UV12& img, const std::vector<cv::Point>& polygon, uint8_t y_value,
                                  uint8_t u_value, uint8_t v_value, float alpha);

    static void FillPolygonYUV420_MultiThread(void* img, const int& width, const int& height,
                                              const std::vector<cv::Point>& polygon, uint8_t y_value, uint8_t u_value,
                                              uint8_t v_value, float alpha);

    // YUV颜色获取
    static YUVColor getYUVFromColor(const std::string& color);

    static void drawRectangle(void* ptr, const int& width, const int& height, const std::string& color,
                              const cv::Point& top_left, const cv::Point& bottom_right, int thickness = 2);

    static int align_down_even(int v) {
        return v & (~1);
    }

    static void drawPathplan(void* ptr, const int& width, const int& height, YUVColor color,
                             const std::vector<cv::Point>& l_points, const std::vector<cv::Point>& r_points,
                             float alpha = 0.2);

    /**
     * yuv图像转其他图像模式，该函数会直接修改指针后的数据，慎用
     * param[in] img_mode：1全彩不处理，2灰度，3二值图
     */
    static bool DoYUV420YU12ToImgMode(uint8_t* p_data, size_t data_len, int img_width, int img_height, int img_mode);

private:
    // yuv图像灰度化，该函数会直接修改指针后的数据，慎用
    static bool DoYUV420UV12ToGray(uint8_t* p_data, int img_width, int img_height);

    // yuv图像二值化
    static bool DoYUV420YU12ToBinaryImg(uint8_t* p_data, int img_width, int img_height);

private:
    static ThreadPool& GetThreadPool() {
        static ThreadPool pool(
            std::min(std::thread::hardware_concurrency() == 0 ? 1u : std::thread::hardware_concurrency(), 8u));
        return pool;
    }
};
