#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
    std::shared_ptr<YUYV420UV12> image;
    cv::Point2f                  view_left_top_point;
    cv::Point2f                  view_right_bottom_point;

    cv::Point2f crop_left_top_point;
    cv::Point2f crop_right_bottom_point;
    std::string channel_name;
};

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool(size_t num_threads) : done(false), active_tasks(0) {
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
    static void PasteYUV420SP(std::shared_ptr<YUYV420UV12> dst, std::shared_ptr<YUYV420UV12> src, int pos_x, int pos_y);

    /**
     * 静态缩放接口：将YUV420SP原始数据缩放到任意大小（最近邻），支持NV12格式
     * @param src 原始YUV420SP数据指针
     * @param src_w 原始宽度
     * @param src_h 原始高度
     * @param dst 目标YUV420SP数据指针
     * @param dst_w 目标宽度
     * @param dst_h 目标高度
     */
    static void ResizeYUV420SP(const uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h);

    /**
     * 静态缩放接口：输入原始YUV420SP数据和目标宽高，返回缩放后的YUYV420UV12对象
     * @param src 原始YUV420SP数据指针
     * @param src_w 原始宽度
     * @param src_h 原始高度
     * @param dst_w 目标宽度
     * @param dst_h 目标高度
     * @return std::shared_ptr<YUYV420UV12> 缩放后的对象
     */
    static std::shared_ptr<YUYV420UV12> ResizeYUV420SPToPtr(const uint8_t* src, int src_w, int src_h, int dst_w,
                                                            int dst_h);

    // YUYV420UV12 与 cv::Mat 转换方法
    static cv::Mat                      YUYV420UV12ToMat(const std::shared_ptr<YUYV420UV12>& yuv_ptr);
    static std::shared_ptr<YUYV420UV12> MatToYUYV420UV12(const cv::Mat& mat);

    // 在YUYV420UV12上绘制文本的便捷方法
    static void DrawText(std::shared_ptr<YUYV420UV12>& yuv_ptr, const std::string& text, const cv::Point& position,
                         double font_scale = 1.0, const cv::Scalar& color = cv::Scalar(255, 255, 255),
                         int thickness = 2, int font_face = cv::FONT_HERSHEY_SIMPLEX);

private:
    static ThreadPool& GetThreadPool() {
        static ThreadPool pool(
            std::min(std::thread::hardware_concurrency() == 0 ? 1u : std::thread::hardware_concurrency(), 8u));
        return pool;
    }
};