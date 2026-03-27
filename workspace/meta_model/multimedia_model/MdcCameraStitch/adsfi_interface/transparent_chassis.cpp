#include "transparent_chassis.h"
#include <cstring>
#include <algorithm>
#include <cmath>

// ─────────────────────────────────────────────
// 构造 / 配置
// ─────────────────────────────────────────────

TransparentChassis::TransparentChassis()
    : has_enqueued_(false)
{
    config_.pixel_per_meter        = 20.0f;
    config_.surround_width         = 1280;
    config_.surround_height        = 1920;
    config_.chassis_rect           = {480, 760, 320, 400};
    config_.vehicle_anchor_x       = 640;
    config_.vehicle_anchor_y       = 960;
    config_.default_chassis_data   = nullptr;
    config_.default_chassis_width  = 0;
    config_.default_chassis_height = 0;
    config_.min_enqueue_distance   = 0.1f;   // 10cm
    config_.min_enqueue_heading    = 0.05f;  // ~3°
}

void TransparentChassis::setConfig(const TransparentChassisConfig& cfg)
{
    config_ = cfg;
}

// ─────────────────────────────────────────────
// 3. setVehiclePose（可在独立线程调用）
// ─────────────────────────────────────────────

void TransparentChassis::setVehiclePose(double timestamp,
                                        double x, double y, double heading)
{
    VehiclePose pose;
    pose.timestamp = timestamp;
    pose.x         = x;
    pose.y         = y;
    pose.heading   = heading;

    std::lock_guard<std::mutex> lk(pose_mutex_);
    pose_history_.push_back(pose);
    if (static_cast<int>(pose_history_.size()) > kMaxPoseHistory)
        pose_history_.pop_front();
}

// ─────────────────────────────────────────────
// 位姿查找（操作快照，调用方已持锁或传入副本）
// ─────────────────────────────────────────────

bool TransparentChassis::findNearestPose(double timestamp,
                                         const std::deque<VehiclePose>& snapshot,
                                         VehiclePose& out_pose) const
{
    if (snapshot.empty())
        return false;

    const VehiclePose* best = nullptr;
    double best_diff = 1e18;
    for (const auto& p : snapshot) {
        double diff = std::abs(p.timestamp - timestamp);
        if (diff < best_diff) {
            best_diff = diff;
            best = &p;
        }
    }
    out_pose = *best;
    return true;
}

// ─────────────────────────────────────────────
// 坐标变换
// ─────────────────────────────────────────────

void TransparentChassis::pixelToWorld(const VehiclePose& pose,
                                      int px, int py,
                                      double& wx, double& wy) const
{
    // BEV coordinate system:
    //   image +X (right) = world -Y
    //   image +Y (down)  = world -X
    // So: dx_m (world-forward) = -dy_pix / scale
    //     dy_m (world-left)    = -dx_pix / scale
    float dx_pix = static_cast<float>(px - config_.vehicle_anchor_x);
    float dy_pix = static_cast<float>(py - config_.vehicle_anchor_y);

    float dx_m = -dy_pix / config_.pixel_per_meter;  // world X (forward)
    float dy_m = -dx_pix / config_.pixel_per_meter;  // world Y (left)

    double cos_h = std::cos(pose.heading);
    double sin_h = std::sin(pose.heading);

    // rotate from vehicle frame to world frame
    wx = pose.x + cos_h * dx_m - sin_h * dy_m;
    wy = pose.y + sin_h * dx_m + cos_h * dy_m;
}

bool TransparentChassis::worldToPixel(const FrameRecord& frame,
                                      double wx, double wy,
                                      float& px, float& py) const
{
    const VehiclePose& pose = frame.pose;

    double dx_w = wx - pose.x;
    double dy_w = wy - pose.y;

    double cos_h = std::cos(pose.heading);
    double sin_h = std::sin(pose.heading);

    // rotate from world frame to vehicle frame
    double dx_v =  cos_h * dx_w + sin_h * dy_w;  // forward
    double dy_v = -sin_h * dx_w + cos_h * dy_w;  // left

    // BEV: image_x = anchor_x - dy_v * scale
    //      image_y = anchor_y - dx_v * scale
    px = static_cast<float>(config_.vehicle_anchor_x - dy_v * config_.pixel_per_meter);
    py = static_cast<float>(config_.vehicle_anchor_y - dx_v * config_.pixel_per_meter);

    if (px < 0 || px >= frame.width - 1 || py < 0 || py >= frame.height - 1)
        return false;

    // 跳过历史帧的空洞区域（车体遮挡，无有效地面像素）
    const SelfRect& cr = config_.chassis_rect;
    if (px >= cr.x && px < cr.x + cr.width &&
        py >= cr.y && py < cr.y + cr.height)
        return false;

    return true;
}

// ─────────────────────────────────────────────
// 双线性插值采样（操作 FrameRecord 内部 pixels）
// ─────────────────────────────────────────────

uint8_t TransparentChassis::sampleY(const FrameRecord& img, float px, float py) const
{
    int x0 = static_cast<int>(px);
    int y0 = static_cast<int>(py);
    int x1 = std::min(x0 + 1, img.width  - 1);
    int y1 = std::min(y0 + 1, img.height - 1);

    float fx = px - x0, fy = py - y0;
    const uint8_t* Y = img.y_plane();
    float v = (1 - fy) * ((1 - fx) * Y[y0 * img.width + x0] + fx * Y[y0 * img.width + x1])
            +      fy  * ((1 - fx) * Y[y1 * img.width + x0] + fx * Y[y1 * img.width + x1]);
    return static_cast<uint8_t>(std::min(std::max(v, 0.0f), 255.0f));
}

void TransparentChassis::sampleUV(const FrameRecord& img, float px, float py,
                                   uint8_t& u, uint8_t& v) const
{
    float upx = px / 2.0f, upy = py / 2.0f;
    int uv_w = img.width / 2, uv_h = img.height / 2;

    int x0 = static_cast<int>(upx), y0 = static_cast<int>(upy);
    int x1 = std::min(x0 + 1, uv_w - 1);
    int y1 = std::min(y0 + 1, uv_h - 1);
    float fx = upx - x0, fy = upy - y0;

    const uint8_t* UV = img.uv_plane();
    auto getU = [&](int xi, int yi) -> float { return UV[(yi * uv_w + xi) * 2];     };
    auto getV = [&](int xi, int yi) -> float { return UV[(yi * uv_w + xi) * 2 + 1]; };

    float fu = (1-fy)*((1-fx)*getU(x0,y0)+fx*getU(x1,y0)) + fy*((1-fx)*getU(x0,y1)+fx*getU(x1,y1));
    float fv = (1-fy)*((1-fx)*getV(x0,y0)+fx*getV(x1,y0)) + fy*((1-fx)*getV(x0,y1)+fx*getV(x1,y1));
    u = static_cast<uint8_t>(std::min(std::max(fu, 0.0f), 255.0f));
    v = static_cast<uint8_t>(std::min(std::max(fv, 0.0f), 255.0f));
}

// ─────────────────────────────────────────────
// 默认车体回退（写入 YuvFrame 输出）
// ─────────────────────────────────────────────

void TransparentChassis::copyDefaultChassis(YuvFrame& output) const
{
    const SelfRect& cr  = config_.chassis_rect;
    const uint8_t* def = config_.default_chassis_data;
    int dw = config_.default_chassis_width;
    int dh = config_.default_chassis_height;

    if (!def || dw <= 0 || dh <= 0) {
        // 无默认图：填灰
        for (int row = 0; row < cr.height; ++row)
            std::memset(output.y_plane() + (cr.y + row) * output.width + cr.x, 128, cr.width);
        int uv_x = cr.x/2, uv_y = cr.y/2, uv_w = cr.width/2, uv_h = cr.height/2;
        for (int row = 0; row < uv_h; ++row)
            std::memset(output.uv_plane() + (uv_y+row)*(output.width/2)*2 + uv_x*2, 128, uv_w*2);
        return;
    }

    // 最近邻缩放到 chassis_rect
    const uint8_t* def_uv = def + dw * dh;
    int def_uv_w = dw / 2, def_uv_h = dh / 2;

    for (int row = 0; row < cr.height; ++row) {
        int sr = row * dh / cr.height;
        for (int col = 0; col < cr.width; ++col) {
            int sc = col * dw / cr.width;
            output.y_plane()[(cr.y + row) * output.width + (cr.x + col)] = def[sr * dw + sc];
        }
    }
    int uv_h = cr.height/2, uv_w = cr.width/2;
    int uv_x = cr.x/2,      uv_y = cr.y/2;
    for (int row = 0; row < uv_h; ++row) {
        int sr = row * def_uv_h / uv_h;
        for (int col = 0; col < uv_w; ++col) {
            int sc = col * def_uv_w / uv_w;
            uint8_t* d       = output.uv_plane() + (uv_y+row)*(output.width/2)*2 + (uv_x+col)*2;
            const uint8_t* s = def_uv + sr * def_uv_w * 2 + sc * 2;
            d[0] = s[0]; d[1] = s[1];
        }
    }
}

// ─────────────────────────────────────────────
// 核心混合
// ─────────────────────────────────────────────

void TransparentChassis::blendChassisRegion(YuvFrame& output,
                                             const VehiclePose& current_pose) const
{
    const SelfRect& cr = config_.chassis_rect;
    std::vector<bool> filled(cr.width * cr.height, false);
    int remaining = cr.width * cr.height;

    for (int fi = static_cast<int>(history_.size()) - 1;
         fi >= 0 && remaining > 0; --fi)
    {
        const FrameRecord& frame = history_[fi];

        for (int row = 0; row < cr.height && remaining > 0; ++row) {
            for (int col = 0; col < cr.width; ++col) {
                int idx = row * cr.width + col;
                if (filled[idx]) continue;

                int dst_px = cr.x + col;
                int dst_py = cr.y + row;

                double wx, wy;
                pixelToWorld(current_pose, dst_px, dst_py, wx, wy);

                float src_px, src_py;
                if (!worldToPixel(frame, wx, wy, src_px, src_py))
                    continue;

                output.y_plane()[dst_py * output.width + dst_px] =
                    sampleY(frame, src_px, src_py);

                if ((dst_px % 2 == 0) && (dst_py % 2 == 0)) {
                    uint8_t u, v;
                    sampleUV(frame, src_px, src_py, u, v);
                    uint8_t* uv_ptr = output.uv_plane()
                                    + (dst_py/2) * (output.width/2) * 2 + (dst_px/2) * 2;
                    uv_ptr[0] = u; uv_ptr[1] = v;
                }

                filled[idx] = true;
                --remaining;
            }
        }
    }

}

// ─────────────────────────────────────────────
// 2+4. processFrame：输入环视图，输出透明地盘图
// ─────────────────────────────────────────────

bool TransparentChassis::processFrame(double timestamp, YuvFrame& in_frame)
{
    // 加锁拷贝位姿快照，立即释放锁
    std::deque<VehiclePose> pose_snapshot;
    {
        std::lock_guard<std::mutex> lk(pose_mutex_);
        pose_snapshot = pose_history_;
    }

    if (pose_snapshot.empty())
        return false;

    VehiclePose current_pose;
    if (!findNearestPose(timestamp, pose_snapshot, current_pose))
        return false;

    // 判断是否需要入队：与上次入队位姿相比，位移或转角超过阈值才拷贝图像
    size_t frame_size = static_cast<size_t>(in_frame.width) * in_frame.height * 3 / 2;
    bool need_enqueue = !has_enqueued_;
    if (!need_enqueue) {
        double dx      = current_pose.x - last_enqueued_pose_.x;
        double dy      = current_pose.y - last_enqueued_pose_.y;
        double dist    = std::sqrt(dx * dx + dy * dy);
        double dh      = std::abs(current_pose.heading - last_enqueued_pose_.heading);
        // 角度差归一化到 [0, π]
        if (dh > M_PI) dh = 2.0 * M_PI - dh;
        need_enqueue = (dist >= config_.min_enqueue_distance);
    }

    if (need_enqueue) {
        FrameRecord rec;
        rec.timestamp = timestamp;
        rec.pose      = current_pose;
        rec.width     = in_frame.width;
        rec.height    = in_frame.height;
        rec.pixels.assign(in_frame.data, in_frame.data + frame_size);

        // 在副本中用灰色填充空洞区域，避免历史帧空洞被采样到黑色
        const SelfRect& cr = config_.chassis_rect;
        for (int row = 0; row < cr.height; ++row) {
            memset(rec.pixels.data() + (cr.y + row) * rec.width + cr.x, 128, cr.width);
        }
        int uv_x = cr.x / 2, uv_y = cr.y / 2;
        int uv_w = cr.width / 2, uv_h = cr.height / 2;
        uint8_t* uv_base = rec.pixels.data() + rec.width * rec.height;
        for (int row = 0; row < uv_h; ++row) {
            memset(uv_base + (uv_y + row) * (rec.width / 2) * 2 + uv_x * 2, 128, uv_w * 2);
        }

        history_.push_back(std::move(rec));
        if (static_cast<int>(history_.size()) > kMaxHistory)
            history_.pop_front();

        last_enqueued_pose_ = current_pose;
        has_enqueued_       = true;
    }

    // 原地修改 in_frame：填充车体区域
    if (static_cast<int>(history_.size()) >= 2)
        blendChassisRegion(in_frame, current_pose);
   

    return true;
}

// ─────────────────────────────────────────────
// 像素读写接口
// ─────────────────────────────────────────────

// YUV → RGB (BT.601 全范围)
static void yuv_to_rgb(uint8_t y, uint8_t u, uint8_t v,
                       uint8_t& r, uint8_t& g, uint8_t& b)
{
    int c = static_cast<int>(y);
    int d = static_cast<int>(u) - 128;
    int e = static_cast<int>(v) - 128;

    auto clamp = [](int x) -> uint8_t {
        return static_cast<uint8_t>(x < 0 ? 0 : (x > 255 ? 255 : x));
    };
    r = clamp(c + 1.402   * e);
    g = clamp(c - 0.344136 * d - 0.714136 * e);
    b = clamp(c + 1.772   * d);
}

// RGB → YUV (BT.601 全范围)
static void rgb_to_yuv(uint8_t r, uint8_t g, uint8_t b,
                       uint8_t& y, uint8_t& u, uint8_t& v)
{
    int ri = r, gi = g, bi = b;
    y = static_cast<uint8_t>( 0.299    * ri + 0.587    * gi + 0.114    * bi);
    u = static_cast<uint8_t>(-0.168736 * ri - 0.331264 * gi + 0.5      * bi + 128);
    v = static_cast<uint8_t>( 0.5      * ri - 0.418688 * gi - 0.081312 * bi + 128);
}

bool TransparentChassis::setPixYuv(YuvFrame& frame, int x, int y,
                                    uint8_t y_val, uint8_t u_val, uint8_t v_val,
                                    float alpha)
{
    if (!frame.data || x < 0 || x >= frame.width || y < 0 || y >= frame.height)
        return false;

    uint8_t* yp = frame.y_plane() + y * frame.width + x;
    if (alpha >= 1.0f) {
        *yp = y_val;
    } else {
        float a = alpha < 0.0f ? 0.0f : alpha;
        *yp = static_cast<uint8_t>(*yp * (1.0f - a) + y_val * a + 0.5f);
    }

    if ((x % 2 == 0) && (y % 2 == 0)) {
        uint8_t* uv = frame.uv_plane() + (y / 2) * (frame.width / 2) * 2 + (x / 2) * 2;
        if (alpha >= 1.0f) {
            uv[0] = u_val;
            uv[1] = v_val;
        } else {
            float a = alpha < 0.0f ? 0.0f : alpha;
            uv[0] = static_cast<uint8_t>(uv[0] * (1.0f - a) + u_val * a + 0.5f);
            uv[1] = static_cast<uint8_t>(uv[1] * (1.0f - a) + v_val * a + 0.5f);
        }
    }
    return true;
}

bool TransparentChassis::setPixRgb(YuvFrame& frame, int x, int y,
                                    uint8_t r, uint8_t g, uint8_t b, float alpha)
{
    uint8_t y_val, u_val, v_val;
    rgb_to_yuv(r, g, b, y_val, u_val, v_val);
    return setPixYuv(frame, x, y, y_val, u_val, v_val, alpha);
}

bool TransparentChassis::getPixYuv(const YuvFrame& frame, int x, int y,
                                    uint8_t& y_val, uint8_t& u_val, uint8_t& v_val) const
{
    if (!frame.data || x < 0 || x >= frame.width || y < 0 || y >= frame.height)
        return false;

    y_val = frame.y_plane()[y * frame.width + x];

    const uint8_t* uv = frame.uv_plane()
                      + (y / 2) * (frame.width / 2) * 2 + (x / 2) * 2;
    u_val = uv[0];
    v_val = uv[1];
    return true;
}

bool TransparentChassis::getPixRgb(const YuvFrame& frame, int x, int y,
                                    uint8_t& r, uint8_t& g, uint8_t& b) const
{
    uint8_t y_val, u_val, v_val;
    if (!getPixYuv(frame, x, y, y_val, u_val, v_val))
        return false;

    yuv_to_rgb(y_val, u_val, v_val, r, g, b);
    return true;
}
