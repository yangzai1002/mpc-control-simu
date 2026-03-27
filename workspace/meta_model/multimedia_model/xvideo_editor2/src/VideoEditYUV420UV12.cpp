#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#include "VideoEditYUV420UV12.hpp"
#include "ap_log/ap_log_interface.h"
#include "ap_log/ap_log_interface.h"

#ifndef QUALITY_MODE
#define QUALITY_MODE 1
#endif

void VideoEditYUV420UV12::drawRectangleNV12(uint8_t* data, int width, int height, int x0, int y0, int x1, int y1,
                                            int thickness) {
    if (!data || width <= 0 || height <= 0 || thickness <= 0)
        return;

    // -------- clamp & normalize --------
    x0 = std::clamp(x0, 0, width - 1);
    y0 = std::clamp(y0, 0, height - 1);
    x1 = std::clamp(x1, 0, width - 1);
    y1 = std::clamp(y1, 0, height - 1);
    if (x0 > x1)
        std::swap(x0, x1);
    if (y0 > y1)
        std::swap(y0, y1);

    // -------- NV12 安全红色 --------
    constexpr uint8_t Y_RED = 76;
    constexpr uint8_t U_RED = 84;
    constexpr uint8_t V_RED = 255;

    uint8_t*  y_plane = data;
    uint8_t*  uv_plane = data + width * height;
    const int uv_w = width >> 1;
    const int uv_h = height >> 1;

    // =========================
    // 1️⃣ Y 平面：画矩形边框
    // =========================
    auto set_y = [&](int x, int y) {
        if ((unsigned)x < (unsigned)width && (unsigned)y < (unsigned)height) {
            y_plane[y * width + x] = Y_RED;
        }
    };

    // top & bottom
    for (int t = 0; t < thickness; ++t) {
        int yt = y0 + t;
        int yb = y1 - t;
        for (int x = x0; x <= x1; ++x) {
            set_y(x, yt);
            set_y(x, yb);
        }
    }

    // left & right
    for (int t = 0; t < thickness; ++t) {
        int xl = x0 + t;
        int xr = x1 - t;
        for (int y = y0; y <= y1; ++y) {
            set_y(xl, y);
            set_y(xr, y);
        }
    }

    // =========================
    // 2️⃣ UV 平面：单线程 + 2×2 对齐
    // =========================
    auto set_uv = [&](int x, int y) {
        x &= ~1;
        y &= ~1;
        int uv_x = x >> 1;
        int uv_y = y >> 1;
        if ((unsigned)uv_x >= (unsigned)uv_w || (unsigned)uv_y >= (unsigned)uv_h)
            return;

        size_t uv_index = ((size_t)uv_y * uv_w + uv_x) * 2;
        uv_plane[uv_index + 0] = U_RED;
        uv_plane[uv_index + 1] = V_RED;
    };

    // top & bottom
    for (int x = x0; x <= x1; x += 2) {
        set_uv(x, y0);
        set_uv(x, y1);
    }

    // left & right
    for (int y = y0; y <= y1; y += 2) {
        set_uv(x0, y);
        set_uv(x1, y);
    }
}

// 绘制线条（Bresenham）- 多线程优化，注意资源释放
void VideoEditYUV420UV12::DrawLine(void* img, const int& width, const int& height, const std::vector<cv::Point>& points,
                                   uint8_t y_value, uint8_t u_value, uint8_t v_value, int thickness) {
    uint8_t* data = static_cast<uint8_t*>(img);

    if (!data || width <= 0 || height <= 0 || points.size() < 2 || thickness <= 0)
        return;

    // 安全像素绘制函数
    auto set_pixel = [&](int x, int y) noexcept {
        if ((unsigned)x >= (unsigned)width || (unsigned)y >= (unsigned)height)
            return;

        data[y * width + x] = y_value;

        int      uv_x = x >> 1, uv_y = y >> 1;
        int      uv_w = width >> 1;
        uint8_t* uv = data + width * height;
        size_t   uv_index = ((size_t)uv_y * uv_w + uv_x) * 2;
        if (uv_index + 1 < (size_t)width * height / 2) {
            uv[uv_index + 0] = u_value;
            uv[uv_index + 1] = v_value;
        }
    };

    // 安全绘制单段线
    auto draw_segment = [&](int x0, int y0, int x1, int y1) noexcept {
        // Clamp endpoints
        x0 = std::clamp(x0, 0, width - 1);
        x1 = std::clamp(x1, 0, width - 1);
        y0 = std::clamp(y0, 0, height - 1);
        y1 = std::clamp(y1, 0, height - 1);

        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        int       steps = 0;
        const int max_steps = width + height; // fail-safe

        while (true) {
            // 画线宽范围
            for (int tx = -thickness / 2; tx <= thickness / 2; ++tx)
                for (int ty = -thickness / 2; ty <= thickness / 2; ++ty)
                    set_pixel(x0 + tx, y0 + ty);

            if (x0 == x1 && y0 == y1)
                break;

            int e2 = err * 2;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }

            if (++steps > max_steps)
                break; // 防止异常数据死循环
        }
    };

    size_t       segment_count = points.size() - 1;
    const size_t thread_threshold = 32;

    if (segment_count >= thread_threshold) {
        const unsigned int thread_count = std::min<unsigned int>(std::thread::hardware_concurrency(), 8U);
        size_t             chunk_size = (segment_count + thread_count - 1) / thread_count;

        ThreadPool&         pool = GetThreadPool();
        std::atomic<size_t> tasks_remaining(0);

        for (size_t t = 0; t < thread_count; ++t) {
            size_t start = t * chunk_size + 1;
            size_t end = std::min(points.size(), (t + 1) * chunk_size + 1);
            if (start >= end)
                continue;

            tasks_remaining.fetch_add(1);
            pool.enqueue([&, start, end] {
                for (size_t i = start; i < end; ++i)
                    draw_segment(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y);
                tasks_remaining.fetch_sub(1);
            });
        }

        while (tasks_remaining.load() > 0)
            std::this_thread::yield();
    } else {
        for (size_t i = 1; i < points.size(); ++i)
            draw_segment(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y);
    }
}

void VideoEditYUV420UV12::FillPolygonYUV420(YUYV420UV12& img, const std::vector<cv::Point>& polygon, uint8_t y_value,
                                            uint8_t u_value, uint8_t v_value,
                                            float alpha) // 透明度：0.0（完全透明）~ 1.0（完全不透明）
{
    int      width = img.width(), height = img.height();
    uint8_t* data = img.get_data();

    if (!data || polygon.size() < 3 || alpha <= 0.0f)
        return;

    // 1. 计算多边形的 y 范围
    int min_y = height, max_y = 0;
    for (const auto& pt : polygon) {
        min_y = std::min(min_y, pt.y);
        max_y = std::max(max_y, pt.y);
    }
    min_y = std::max(0, min_y);
    max_y = std::min(height - 1, max_y);

    // 2. 对每一行做扫描线填充
    for (int y = min_y; y <= max_y; ++y) {
        std::vector<int> nodes; // 存储交点 x

        // 遍历多边形边
        for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
            int y0 = polygon[i].y, y1 = polygon[j].y;
            int x0 = polygon[i].x, x1 = polygon[j].x;

            if ((y0 < y && y1 >= y) || (y1 < y && y0 >= y)) {
                int x = x0 + (y - y0) * (x1 - x0) / (y1 - y0);
                nodes.push_back(x);
            }
        }

        std::sort(nodes.begin(), nodes.end());

        // 3. 两两成对填充
        for (size_t k = 0; k + 1 < nodes.size(); k += 2) {
            int x_start = std::max(0, nodes[k]);
            int x_end = std::min(width - 1, nodes[k + 1]);

            for (int x = x_start; x <= x_end; ++x) {
                // 修改 Y 分量（亮度）
                uint8_t& y_dst = data[y * width + x];
                y_dst = static_cast<uint8_t>(y_dst * (1.0f - alpha) + y_value * alpha);

                // 修改 UV 分量（每 2x2 一组）
                if ((y % 2 == 0) && (x % 2 == 0)) {
                    int uv_x = x / 2;
                    int uv_y = y / 2;
                    int uv_w = width / 2;

                    uint8_t* uv = data + width * height;
                    uint8_t& u_dst = uv[(uv_y * uv_w + uv_x) * 2 + 0];
                    uint8_t& v_dst = uv[(uv_y * uv_w + uv_x) * 2 + 1];

                    u_dst = static_cast<uint8_t>(u_dst * (1.0f - alpha) + u_value * alpha);
                    v_dst = static_cast<uint8_t>(v_dst * (1.0f - alpha) + v_value * alpha);
                }
            }
        }
    }
}

void VideoEditYUV420UV12::FillPolygonYUV420_MultiThread(void* img, const int& width, const int& height,
                                                        const std::vector<cv::Point>& polygon, uint8_t y_value,
                                                        uint8_t u_value, uint8_t v_value, float alpha) {
    uint8_t* data = static_cast<uint8_t*>(img);
    if (!data || width <= 0 || height <= 0 || polygon.size() < 3)
        return;

    alpha = std::clamp(alpha, 0.0f, 1.0f);
    if (alpha <= 0.0f)
        return;

    int min_y = height, max_y = -1;
    for (const auto& pt : polygon) {
        if (pt.y < 0 || pt.y >= height)
            continue;
        min_y = std::min(min_y, pt.y);
        max_y = std::max(max_y, pt.y);
    }
    if (min_y >= height || max_y < 0)
        return;

    min_y = std::max(0, min_y);
    max_y = std::min(height - 1, max_y);

    const int total_rows = max_y - min_y + 1;
    if (total_rows <= 0)
        return;

    const unsigned int max_threads = std::min(8u, std::thread::hardware_concurrency());
    const unsigned int thread_count = std::min(max_threads, (unsigned int)total_rows);
    const int          chunk_size = (total_rows + thread_count - 1) / thread_count;

    ThreadPool&      pool = GetThreadPool();
    std::atomic<int> pending_tasks(0);

    for (unsigned int t = 0; t < thread_count; ++t) {
        int y_start = min_y + t * chunk_size;
        int y_end = std::min(max_y + 1, y_start + chunk_size);
        if (y_start >= y_end)
            continue;

        pending_tasks.fetch_add(1);
        pool.enqueue([=, &polygon, &pending_tasks]() noexcept {
            std::vector<int> nodes;
            nodes.reserve(polygon.size());

            for (int y = y_start; y < y_end; ++y) {
                nodes.clear();

                for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
                    int x0 = polygon[j].x, y0 = polygon[j].y;
                    int x1 = polygon[i].x, y1 = polygon[i].y;

                    // 排除水平边 & 越界
                    if (y0 == y1 || (y < std::min(y0, y1)) || (y >= std::max(y0, y1)))
                        continue;

                    float t_ratio = (float)(y - y0) / (float)(y1 - y0);
                    int   x = (int)(x0 + t_ratio * (x1 - x0) + 0.5f);
                    nodes.push_back(x);
                }

                if (nodes.size() < 2)
                    continue;

                std::sort(nodes.begin(), nodes.end());

                for (size_t k = 0; k + 1 < nodes.size(); k += 2) {
                    int x_start = std::clamp(nodes[k], 0, width - 1);
                    int x_end = std::clamp(nodes[k + 1], 0, width - 1);
                    if (x_start > x_end)
                        std::swap(x_start, x_end);

                    uint8_t* y_row = data + y * width;

                    for (int x = x_start; x <= x_end; ++x) {
                        uint8_t& y_dst = y_row[x];
                        y_dst = static_cast<uint8_t>(y_dst * (1.0f - alpha) + y_value * alpha);

                        if ((y & 1) == 0 && (x & 1) == 0) {
                            int      uv_x = x >> 1;
                            int      uv_y = y >> 1;
                            int      uv_w = width >> 1;
                            uint8_t* uv = data + width * height;
                            size_t   uv_index = ((size_t)uv_y * uv_w + uv_x) * 2;
                            uint8_t& u_dst = uv[uv_index + 0];
                            uint8_t& v_dst = uv[uv_index + 1];
                            u_dst = static_cast<uint8_t>(u_dst * (1.0f - alpha) + u_value * alpha);
                            v_dst = static_cast<uint8_t>(v_dst * (1.0f - alpha) + v_value * alpha);
                        }
                    }
                }
            }

            pending_tasks.fetch_sub(1);
        });
    }

    while (pending_tasks.load() > 0)
        std::this_thread::yield();
}

// void VideoEditYUV420UV12::FillPolygonYUV420_MultiThread(void* img, const int& width, const int& height,
//                                                         const std::vector<cv::Point>& polygon, uint8_t y_value,
//                                                         uint8_t u_value, uint8_t v_value, float alpha) {
//     // uint8_t* data = img.get_data();
//     uint8_t* data = static_cast<uint8_t*>(img);
//     if (!data || polygon.size() < 3)
//         return;
//     if (alpha <= 0.0f)
//         return;
//     if (alpha > 1.0f)
//         alpha = 1.0f;

//     // 计算Y范围
//     int min_y = height, max_y = 0;
//     for (const auto& pt : polygon) {
//         min_y = std::min(min_y, pt.y);
//         max_y = std::max(max_y, pt.y);
//     }
//     min_y = std::max(0, min_y);
//     max_y = std::min(height - 1, max_y);

//     int total_rows = max_y - min_y + 1;
//     if (total_rows <= 0)
//         return;

//     unsigned int thread_count = std::min(8u, (unsigned int)total_rows);
//     size_t       chunk_size = (total_rows + thread_count - 1) / thread_count;

//     ThreadPool& pool_ = GetThreadPool();

//     for (unsigned int t = 0; t < thread_count; ++t) {
//         int y_start = min_y + t * chunk_size;
//         int y_end = std::min(max_y + 1, y_start + (int)chunk_size);
//         if (y_start >= y_end)
//             continue;

//         // 注意全值捕获，防止引用风险
//         pool_.enqueue([=, &polygon]() {
//             for (int y = y_start; y < y_end; ++y) {
//                 std::vector<int> nodes;
//                 for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
//                     int y0 = polygon[i].y, y1 = polygon[j].y;
//                     int x0 = polygon[i].x, x1 = polygon[j].x;
//                     if ((y0 < y && y1 >= y) || (y1 < y && y0 >= y)) {
//                         int x = x0 + (y - y0) * (x1 - x0) / (y1 - y0);
//                         nodes.push_back(x);
//                     }
//                 }
//                 std::sort(nodes.begin(), nodes.end());
//                 for (size_t k = 0; k + 1 < nodes.size(); k += 2) {
//                     int x_start = std::max(0, nodes[k]);
//                     int x_end = std::min(width - 1, nodes[k + 1]);
//                     for (int x = x_start; x <= x_end; ++x) {
//                         uint8_t& y_dst = data[y * width + x];
//                         y_dst = static_cast<uint8_t>(y_dst * (1.0f - alpha) + y_value * alpha);
//                         if ((y % 2 == 0) && (x % 2 == 0)) {
//                             int      uv_x = x / 2;
//                             int      uv_y = y / 2;
//                             int      uv_w = width / 2;
//                             uint8_t* uv = data + width * height;
//                             uint8_t& u_dst = uv[(uv_y * uv_w + uv_x) * 2 + 0];
//                             uint8_t& v_dst = uv[(uv_y * uv_w + uv_x) * 2 + 1];
//                             u_dst = static_cast<uint8_t>(u_dst * (1.0f - alpha) + u_value * alpha);
//                             v_dst = static_cast<uint8_t>(v_dst * (1.0f - alpha) + v_value * alpha);
//                         }
//                     }
//                 }
//             }
//         });
//     }

//     pool_.wait_for_all_tasks();
// }

template <typename T> T xclamp(T v, T lo, T hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

void VideoEditYUV420UV12::drawRectangle(void* ptr, const int& width, const int& height, const std::string& color,
                                        const cv::Point& top_left, const cv::Point& bottom_right, int thickness) {
    if (ptr == nullptr) {
        ApError("video_editor") << "YUV data is null";
        return;
    }

    ApError("video_editor") << "color" << color;
    const YUVColor yuv = getYUVFromColor(color);

    int       w = width;
    int       h = height;
    cv::Point tl{xclamp(top_left.x, 0, w - 1), xclamp(top_left.y, 0, h - 1)};
    cv::Point br{xclamp(bottom_right.x, 0, w - 1), xclamp(bottom_right.y, 0, h - 1)};
    cv::Point tr(br.x, tl.y); // top right
    cv::Point bl(tl.x, br.y); // bottom left

    // 画四条边
    VideoEditYUV420UV12::DrawLine(ptr, w, h, {tl, tr}, yuv.y, yuv.u, yuv.v, thickness); // top
    VideoEditYUV420UV12::DrawLine(ptr, w, h, {tr, br}, yuv.y, yuv.u, yuv.v, thickness); // right
    VideoEditYUV420UV12::DrawLine(ptr, w, h, {br, bl}, yuv.y, yuv.u, yuv.v, thickness); // bottom
    VideoEditYUV420UV12::DrawLine(ptr, w, h, {bl, tl}, yuv.y, yuv.u, yuv.v, thickness); // left
}

void VideoEditYUV420UV12::drawPathplan(void* ptr, const int& width, const int& height, YUVColor yuv,
                                       const std::vector<cv::Point>& l_points, const std::vector<cv::Point>& r_points,
                                       float alpha) {
    // const YUVColor yuv = getYUVFromColor(color);
    VideoEditYUV420UV12::DrawLine(ptr, width, height, l_points, yuv.y, yuv.u, yuv.v, 2);

    VideoEditYUV420UV12::DrawLine(ptr, width, height, r_points, yuv.y, yuv.u, yuv.v, 2);

    std::vector<cv::Point> polygon(l_points);
    polygon.insert(polygon.end(), r_points.rbegin(), r_points.rend());
    VideoEditYUV420UV12::FillPolygonYUV420_MultiThread(ptr, width, height, polygon, yuv.y, yuv.u, yuv.v, alpha);
}

/**
 * yuv图像转其他图像模式，该函数会直接修改指针后的数据，慎用
 * param[in] img_mode：1全彩不处理，2灰度，3二值图
 */
bool VideoEditYUV420UV12::DoYUV420YU12ToImgMode(uint8_t* p_data, size_t data_len, int img_width, int img_height,
                                                int img_mode) {
    ApInfo("video_editor") << "DoYUV420YU12ToImgMode data_len=" << data_len << ",img_width=" << img_width
                           << ",img_height=" << img_height << ",img_mode" << img_mode;
    if (img_mode == 1) {
        return true;
    }
    if (img_width <= 0 || img_height <= 0) {
        ApInfo("video_editor") << "DoYUV420YU12ToImgMode img width or height is error";
        return false;
    }
    size_t total_size = img_width * img_height * 3 / 2;
    if (total_size > data_len) {
        ApInfo("video_editor") << "DoYUV420YU12ToImgMode data_len is shorter";
        return false;
    } else if (total_size != data_len) {
        // 数据长度不等，可能哪儿有问题
        ApInfo("video_editor") << "DoYUV420YU12ToImgMode data len is longer";
    }
    switch (img_mode) {
        case 2:
            return DoYUV420UV12ToGray(p_data, img_width, img_height);
        case 3:
            return DoYUV420YU12ToBinaryImg(p_data, img_width, img_height);
        default:
            ApInfo("video_editor") << "DoYUV420YU12ToImgMode unknown img mode" << img_mode;
            break;
    }
    return true;
}

bool VideoEditYUV420UV12::DoYUV420UV12ToGray(uint8_t* p_data, int img_width, int img_height) {
    ApInfo("video_editor") << "DoYUV420YU12ToImgMode DoYUV420UV12ToGray";
    memset(p_data + img_width * img_height, 128, img_width * img_height / 2);
    return true;
}

bool VideoEditYUV420UV12::DoYUV420YU12ToBinaryImg(uint8_t* p_data, int img_width, int img_height) {
    ApInfo("video_editor") << "DoYUV420YU12ToImgMode DoYUV420YU12ToBinaryImg";
    size_t y_size = img_width * img_height;
    for (size_t index = 0; index < y_size; ++index) {
        *(p_data + index) = *(p_data + index) >= 128 ? 255 : 0;
    }
    memset(p_data + img_width * img_height, 128, y_size / 2);
    return true;
}

YUVColor VideoEditYUV420UV12::getYUVFromColor(const std::string& color) {
    if (color == "blue") {
        return {149, 43, 21}; // RGB(0,0,255)
    } else if (color == "red") {
        return {76, 84, 255}; // RGB(255,0,0)
    } else if (color == "yellow") {
        return {210, 16, 146}; // RGB(255,255,0)
    } else {
        // default green
        return {76, 84, 255}; // RGB(0,255,0)
    }
    return {76, 84, 255};
}