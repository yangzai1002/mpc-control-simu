#pragma once
#include <cstdint>
#include <vector>
#include <deque>
#include <cmath>
#include <mutex>
#include "find_inner_rect.h"

// YUV420SP (NV12) 帧视图：不拥有内存，仅持有外部指针
struct YuvFrame {
    int            width;
    int            height;
    uint8_t*       data;    // 外部缓冲区：Y平面(width*height) + UV交织平面(width*height/2)
    size_t         size;    // 总字节数，应 == width * height * 3 / 2

    YuvFrame() : width(0), height(0), data(nullptr), size(0) {}
    YuvFrame(int w, int h, uint8_t* buf, size_t sz)
        : width(w), height(h), data(buf), size(sz) {}

    uint8_t*       y_plane()        { return data; }
    uint8_t*       uv_plane()       { return data + width * height; }
    const uint8_t* y_plane()  const { return data; }
    const uint8_t* uv_plane() const { return data + width * height; }
};

// 车辆位置信息
struct VehiclePose {
    double timestamp;   // 秒
    double x;           // 米，世界坐标
    double y;           // 米，世界坐标
    double heading;     // 弧度，车头朝向（相对世界坐标系 x 轴正方向）
};



// 透明地盘配置
struct TransparentChassisConfig {
    float pixel_per_meter;              // 每米对应像素数，默认 20（即5cm/pixel）
    int   surround_width;               // 环视图宽，默认 1280
    int   surround_height;              // 环视图高，默认 1920
    SelfRect  chassis_rect;                 // 车体在图中的矩形区域（像素）
    int   vehicle_anchor_x;             // 车辆记录位置对应的像素 x
    int   vehicle_anchor_y;             // 车辆记录位置对应的像素 y


    // 默认车体图像（无历史帧时的回退，可为 nullptr）
    // 格式：YUV420SP (NV12)，尺寸 default_chassis_width × default_chassis_height
    const uint8_t* default_chassis_data;
    int            default_chassis_width;
    int            default_chassis_height;

    // 历史帧入队阈值：位移或转角任一超过阈值才拷贝图像入队
    float min_enqueue_distance; // 米，默认 0.1m
    float min_enqueue_heading;  // 弧度，默认 0.05rad（约3°）
};

/**
 * TransparentChassis
 *
 * 功能：利用多帧累积的环视拼接图 + 车辆位姿，将车体区域替换为历史帧中
 *       对应地面像素，实现"透明地盘"效果。
 *
 * 图像格式：YUV420SP (NV12)，不依赖 OpenCV。
 *
 * 线程模型：
 *   - setVehiclePose 可在独立线程调用（内部加锁）
 *   - processFrame（原 setSurroundImage+compute）在图像线程调用
 *   - setConfig 在初始化阶段调用，不与其他接口并发
 */
class TransparentChassis {
public:
    TransparentChassis();
    ~TransparentChassis() = default;

    // 1. 配置接口（初始化阶段调用，非线程安全）
    void setConfig(const TransparentChassisConfig& config);

    // 2+4. 合并接口：直接在 in_frame 缓冲区上原地修改，写入透明地盘结果
    //   in_frame : 不含车体的环视拼接图；缓冲区必须可写，函数返回前不可释放
    //              函数返回后 in_frame.data 即为合成结果，无需额外输出缓冲区
    //   返回 false 表示位姿数据不足，车体区域保持原样
    bool processFrame(double timestamp, YuvFrame& in_frame);

    // 3. 设置车辆位置信息（可在独立线程调用）
    void setVehiclePose(double timestamp, double x, double y, double heading);

    // 像素读写接口（操作传入的 YuvFrame，坐标越界时返回 false）
    // YUV 版：y_val=Y, u_val=U, v_val=V（NV12 格式，BT.601）
    // RGB 版：r, g, b（BT.601 全范围转换）
    // alpha：混合比例 [0,1]，1.0 直接赋值，否则 dst = src*(1-alpha) + new*alpha
    bool setPixYuv(YuvFrame& frame, int x, int y,
                   uint8_t y_val, uint8_t u_val, uint8_t v_val, float alpha = 1.0f);
    bool setPixRgb(YuvFrame& frame, int x, int y,
                   uint8_t r, uint8_t g, uint8_t b, float alpha = 1.0f);
    bool getPixYuv(const YuvFrame& frame, int x, int y,
                   uint8_t& y_val, uint8_t& u_val, uint8_t& v_val) const;
    bool getPixRgb(const YuvFrame& frame, int x, int y,
                   uint8_t& r, uint8_t& g, uint8_t& b) const;

private:
    // 内部历史帧：自持图像数据
    struct FrameRecord {
        double                timestamp;
        VehiclePose           pose;
        int                   width;
        int                   height;
        std::vector<uint8_t>  pixels;  // YUV420SP 完整拷贝

        const uint8_t* y_plane()  const { return pixels.data(); }
        const uint8_t* uv_plane() const { return pixels.data() + width * height; }
    };

    bool worldToPixel(const FrameRecord& frame,
                      double wx, double wy,
                      float& px, float& py) const;

    uint8_t sampleY (const FrameRecord& img, float px, float py) const;
    void    sampleUV(const FrameRecord& img, float px, float py,
                     uint8_t& u, uint8_t& v) const;

    void pixelToWorld(const VehiclePose& pose,
                      int px, int py,
                      double& wx, double& wy) const;

    void blendChassisRegion(YuvFrame& frame, const VehiclePose& current_pose) const;
    void copyDefaultChassis(YuvFrame& frame) const;

    // 从位姿缓存快照中找时间戳最近的位姿
    bool findNearestPose(double timestamp,
                         const std::deque<VehiclePose>& snapshot,
                         VehiclePose& out_pose) const;

    TransparentChassisConfig config_;

    // 位姿缓存（多线程共享，用 pose_mutex_ 保护）
    mutable std::mutex       pose_mutex_;
    static constexpr int     kMaxPoseHistory = 200;
    std::deque<VehiclePose>  pose_history_;

    // 历史帧队列（仅图像线程访问，无需加锁）
    static constexpr int     kMaxHistory = 30;
    std::deque<FrameRecord>  history_;

    // 上次入队时的位姿（用于判断是否需要入队）
    VehiclePose last_enqueued_pose_;
    bool        has_enqueued_;
};
