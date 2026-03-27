#include <chrono>
#include <map>

#include "CsvUtils.hpp"
#include "DatetimeUtil.hpp"
#include "EC400.hpp"
#include "StringUtils.hpp"
#include "VideoParameter.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_planningstatus.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "custom_stack.h"
#include "geometry_tools.hpp"
#include "ap_log/ap_log_interface.h"

#define STEER_ANGLE_MAX 180
struct EncodingProfile {
    EncodingProfile() : width(0), height(0), bps(0) {
    }
    std::int32_t width;
    std::int32_t height;
    std::int32_t bps;

    bool operator<(const EncodingProfile& rhs) const {
        return width < rhs.width || (width == rhs.width && height < rhs.height);
    }

    bool operator==(const EncodingProfile& rhs) const {
        return width == rhs.width && height == rhs.height && bps == rhs.bps;
    }

    std::string to_string() const {
        return std::to_string(width) + "x" + std::to_string(height) + "@" + std::to_string(bps);
    }
};

struct RowOfBandwidthProfile {
    EncodingProfile rgb;
    EncodingProfile gray;
    EncodingProfile binary;
    EncodingProfile feature;
};

using BandwidthProfile = std::vector<RowOfBandwidthProfile>;

template <size_t SAMPLE_COUNT> struct CostIndicator {
    CostIndicator() : _sum_costs(0.0) {
        reset();
    }

    void reset() {
        _sum_costs = 0;
        _costs.clear();
    }

    void add(double cost_in_ms) {
        if (_costs.size() >= SAMPLE_COUNT) {
            _sum_costs -= _costs.front();
            _costs.pop_front();
        }

        _costs.push_back(cost_in_ms);
        _sum_costs += cost_in_ms;
    }

    double avg() {
        if (_costs.empty()) {
            return 0;
        }
        return _sum_costs / _costs.size();
    }

    bool any(const std::function<bool(double)>& f) {
        for (auto x : _costs) {
            if (f(x)) {
                return true;
            }
        }

        return false;
    }

    bool all(const std::function<bool(double)>& f) {
        for (auto x : _costs) {
            if (!f(x)) {
                return false;
            }
        }
        return true;
    }

private:
    double            _sum_costs;
    std::list<double> _costs;
};

struct TimedCostIndicator {
    TimedCostIndicator(std::uint32_t max) : _max(max), _sum_costs(0.0) {
        reset();
    }

    void reset() {
        _sum_costs = 0;
        _timed_costs.clear();
    }

    void add(std::int64_t ms, double cost_in_ms) {
        _timed_costs[ms] = cost_in_ms;
        _sum_costs += cost_in_ms;
        if (_timed_costs.size() > _max) {
            auto iter = _timed_costs.begin();
            _sum_costs -= iter->second;
            _timed_costs.erase(iter);
        }
    }

    double avg() {
        if (_timed_costs.empty()) {
            return 0;
        }
        return _sum_costs / _timed_costs.size();
    }

    bool any(const std::function<bool(double)>& f) {
        for (auto x : _timed_costs) {
            if (f(x.second)) {
                return true;
            }
        }

        return false;
    }

    bool all(const std::function<bool(double)>& f) {
        for (auto x : _timed_costs) {
            if (!f(x.second)) {
                return false;
            }
        }
        return true;
    }

    std::string to_string() {
        std::stringstream ss;
        for (auto& x : _timed_costs) {
            ss << "(" << x.first << ", " << x.second << ") ";
        }
        return ss.str();
    }

private:
    const std::uint32_t            _max;
    std::map<std::int64_t, double> _timed_costs; // timestamp -> cost
    double                         _sum_costs;
};

struct EncodingSelectionCB {
    struct Quality {
        std::int32_t min_quality;
        std::int32_t max_quality;
        std::int32_t cur_quality;
    };

    EncodingSelectionCB(double threshold_bps, std::int32_t switch_interval, const std::vector<std::int32_t> qualities)
        : rr_fps(switch_interval / 1000)
        , _threshold_bps(threshold_bps)
        , _switch_interval(switch_interval)
        , _last_switch_ts(app_common::DatetimeUtil::current_millisecond()) {
        _quality_map.emplace(1, Quality{.min_quality = qualities[0], .max_quality = 51, .cur_quality = qualities[0]});
        _quality_map.emplace(2, Quality{.min_quality = qualities[1], .max_quality = 51, .cur_quality = qualities[1]});
        _quality_map.emplace(3, Quality{.min_quality = qualities[2], .max_quality = 51, .cur_quality = qualities[2]});
        _quality_map.emplace(4, Quality{.min_quality = qualities[3], .max_quality = 51, .cur_quality = qualities[3]});
    }

    EncodingSelectionCB(const BandwidthProfile& profile, double threshold_bps, std::int32_t switch_interval,
                        const std::vector<std::int32_t> qualities)
        : EncodingSelectionCB(threshold_bps, switch_interval, qualities) {
        for (auto& row : profile) {
            if (row.rgb.width > 0 && !exists(profiles[1], row.rgb)) {
                profiles[1].push_back(row.rgb);
            }

            if (row.gray.width > 0 && !exists(profiles[2], row.gray)) {
                profiles[2].push_back(row.gray);
            }

            if (row.binary.width > 0 && !exists(profiles[3], row.binary)) {
                profiles[3].push_back(row.binary);
            }

            if (row.feature.width > 0 && !exists(profiles[4], row.binary)) {
                profiles[4].push_back(row.binary);
            }
        }
        indexes[1] = 0;
        indexes[2] = 0;
        indexes[3] = 0;
        indexes[4] = 0;
        auto iter = std::find_if(profiles.begin(), profiles.end(),
                                 [](decltype(profiles)::value_type& e) { return e.second.empty(); });
        if (iter != profiles.end()) {
            throw std::runtime_error("profile data error");
        }
        std::for_each(profiles.begin(), profiles.end(), [](decltype(profiles)::value_type& e) {
            std::sort(e.second.begin(), e.second.end());
            std::reverse(e.second.begin(), e.second.end());
        });
    }

    void select(const ara::adsfi::AppVideoProfile& in, compatible::VideoParameter& out) {
        rr_fps.add(in.receiver_reported_ts, in.receiver_reported_fps);

        ApInfo("video_editor") << "rate_control_mode: " << out.encoding_parameter.rate_control_mode;
        if (1 == out.encoding_parameter.rate_control_mode) {
            try {
                auto& q = _quality_map.at(in.output_mode);
                ApInfo("video_editor") << "adaptive_mode: " << in.adaptive_mode << ", rr_fps: " << rr_fps.to_string();
                if (0 == in.adaptive_mode) {
                    out.encoding_parameter.quality = q.min_quality;
                    q.cur_quality = q.min_quality;
                } else {
                    auto now = app_common::DatetimeUtil::current_millisecond();
                    if ((_last_switch_ts + _switch_interval) > now) {
                        ApInfo("video_editor") << "too frequent switch, ignore";
                        return;
                    }
                    _last_switch_ts = now;

                    if (rr_fps.any([threshold = _threshold_bps](double x) { return (x != 0 && x < threshold); })) {
                        // 增加编码质量值（即质量降低）
                        if (q.cur_quality < q.max_quality) {
                            ++q.cur_quality;
                            ApInfo("video_editor") << "increase quality to " << q.cur_quality;
                        }
                    } else if (rr_fps.all([threshold = _threshold_bps](double x) { return x >= threshold; })) {
                        // 减小编码质量值（即质量提高）
                        if (q.cur_quality > q.min_quality) {
                            --q.cur_quality;
                            ApInfo("video_editor") << "decrease quality to " << q.cur_quality;
                        }
                    }
                    out.encoding_parameter.quality = q.cur_quality;
                }
            } catch (const std::out_of_range& e) {
                ApInfo("video_editor") << "invalid output_mode: " << in.output_mode;
            }
        } else if (0 == out.encoding_parameter.rate_control_mode) {
            auto iter1 = profiles.find(in.output_mode);
            if (iter1 == profiles.end()) {
                ApInfo("video_editor") << __LINE__ << ": invalid output_mode: " << in.output_mode;
                return;
            }
            auto iter2 = indexes.find(in.output_mode);
            if (iter2 == indexes.end()) {
                ApInfo("video_editor") << __LINE__ << ": invalid output_mode: " << in.output_mode;
                return;
            }

            auto now = app_common::DatetimeUtil::current_millisecond();
            if (_last_switch_ts + _switch_interval < now) {
                _last_switch_ts = now;
                ApInfo("video_editor") << __LINE__;
                ApInfo("video_editor") << __LINE__ << ":select ----- output_mode: " << in.output_mode
                                       << ", rr_fps: " << rr_fps.to_string()
                                       << ", current index: " << indexes[in.output_mode];
                ApInfo("video_editor") << __LINE__;
                auto old = indexes[in.output_mode];
                adjust_profile(profiles[in.output_mode], indexes[in.output_mode]);
                if (old != indexes[in.output_mode]) {
                    rr_fps.reset();
                }
            }

            if (in.output_mode != _last_outputmode) {
                set_suitable_profile(_last_outputmode, in.output_mode);
                _last_outputmode = in.output_mode;
            }

            set_encoding_parameter(profiles[in.output_mode], indexes[in.output_mode], out);
        }
    }

    void set_suitable_profile(std::int32_t old_mode, std::int32_t new_mode) {
        auto iter = indexes.find(old_mode);
        if (iter != indexes.end()) {
            auto p = profiles[old_mode][indexes[old_mode]];

            std::vector<std::int32_t> distances;
            std::for_each(profiles[new_mode].begin(), profiles[new_mode].end(),
                          [&distances, &p](const EncodingProfile& x) { distances.push_back(x.bps - p.bps); });
            std::int32_t min = std::numeric_limits<std::int32_t>::max();
            std::int32_t min_i = 0;
            for (int i = 0; i < distances.size(); ++i) {
                if (distances[i] < min) {
                    min = distances[i];
                    min_i = i;
                }
            }
            indexes[new_mode] = min_i;
        } else {
            indexes[new_mode] = 0;
        }
    }

    void adjust_profile(const std::vector<EncodingProfile>& in, std::uint32_t& idx) {
        if (rr_fps.any(
                [threshold = _threshold_bps](double x) { return (x != 0 && x < threshold); })) { // 降低码率和分辨率
            if (idx < (in.size() - 1)) {
                ++idx;
            }
        } else if (rr_fps.all([threshold = _threshold_bps](double x) { return x >= threshold; })) { // 提高码率和分辨率
            if (idx > 0) {
                --idx;
            }
        }
    }

    void set_encoding_parameter(const std::vector<EncodingProfile>& in, std::uint32_t idx,
                                compatible::VideoParameter& out) {
        ApInfo("video_editor") << __LINE__ << ":select ----- idx: " << idx << ", profile: " << in[idx].to_string()
                               << ", profiles: " << to_string(in);
        out.encoding_parameter.width = in[idx].width;
        out.encoding_parameter.height = in[idx].height;
        out.encoding_parameter.bps = in[idx].bps;
    }

    bool exists(const std::vector<EncodingProfile>& in, const EncodingProfile& x) {
        return in.end() != std::find_if(in.begin(), in.end(), [&x](const EncodingProfile& y) { return x == y; });
    }

    std::string to_string(const std::vector<EncodingProfile>& in) {
        std::stringstream ss;
        for (const auto& x : in) {
            ss << x.to_string() << " ";
        }
        return ss.str();
    }

private:
    std::map<std::int32_t, std::vector<EncodingProfile>> profiles;
    std::map<std::int32_t, std::uint32_t>                indexes;
    TimedCostIndicator                                   rr_fps; // receiver reported fps, 1 hz
    const std::int32_t                                   _switch_interval;
    const double                                         _threshold_bps;

    std::map<std::int32_t, Quality> _quality_map;

    std::int64_t _last_switch_ts;
    std::int32_t _last_outputmode;
};

void setCameraData(compatible::VideoParameter& video_parameter, int main, int left, int right) {
    video_parameter.editor_parameter.video_main_ = main;
    video_parameter.editor_parameter.video_left_ = left;
    video_parameter.editor_parameter.video_right_ = right;
}

//根据转向角切换视角
void changeCameraBySteerAngle(float angle, bool is_R, int main, compatible::VideoParameter& video_parameter) {
    // is_R  判断是否为倒档  angle 方向左正右负
    //判断前相机/夜视相机
    if (is_R) {
        //倒档
        if (angle >= STEER_ANGLE_MAX) { //左后拼接
            setCameraData(video_parameter, 3, 1, -1);
        } else if ((angle < STEER_ANGLE_MAX && angle > 0) || (angle < 0 && angle > -STEER_ANGLE_MAX)) { //左后右
            setCameraData(video_parameter, 3, 1, 2);
        } else if (angle <= -STEER_ANGLE_MAX) { //右后视角
            setCameraData(video_parameter, 3, -1, 2);
        }
    } else {
        if (angle >= STEER_ANGLE_MAX) { //右前拼接
            setCameraData(video_parameter, main, 1, -1);
        } else if ((angle < STEER_ANGLE_MAX && angle > 0) || (angle < 0 && angle > -STEER_ANGLE_MAX)) { //左前右
            setCameraData(video_parameter, main, 1, 2);
        } else if (angle <= -STEER_ANGLE_MAX) { //左前视角
            setCameraData(video_parameter, main, -1, 2);
        }
    }
}

#define cb_pi 3.1415926535897932384626433832795
constexpr double METERS_PER_DEGREE_LAT = 111195.0; // 纬度每度对应米数
#define CB_EARTH_RADIUS 6378.137                   //地球半径 KM
#define CB_NEAR_POINT_SIZE 2 // 任务点滤除用于判断当前点位置是里起点或终点两点以内

bool GetLatlog(double lat, double lon, double d, double angle, double& new_lat, double& new_lon) {
    // 将距离转换成经度的计算公式
    new_lon = lon + (d * sin(angle * M_PI / 180)) / (111 * cos(lat * M_PI / 180));
    // 将距离转换成纬度的计算公式
    new_lat = lat + (d * cos(angle * M_PI / 180)) / 111;

    // const double heading_rad = angle * M_PI / 180.0;

    // // 分解位移分量
    // const double north_dis = d * std::cos(heading_rad);  // 北向分量
    // const double east_dis = d * std::sin(heading_rad);   // 东向分量

    // // 计算纬度变化
    // new_lat = lat + (north_dis / METERS_PER_DEGREE_LAT);

    // // 计算经度变化
    // const double lat_rad = lat * M_PI / 180.0;  // 当前纬度转弧度
    // const double meters_per_degree_lon = METERS_PER_DEGREE_LAT * std::cos(lat_rad);
    // new_lon = lon + (east_dis / meters_per_degree_lon);
    // // 将结果转换为字符串格式
    return true;
}

#define PI 3.14159265358979323846

// 角度转弧度
double deg2rad(double deg) {
    return deg * PI / 180.0;
}

// 计算新的 UTM 坐标
void calculateNewEulerPosition(double x, double y, double yaw, double distance, double& newX, double& newY) {
    double yaw_rad = deg2rad(yaw); // 偏航角转换为弧度

    // 计算坐标增量
    double dx = distance * cos(yaw_rad);
    double dy = distance * sin(yaw_rad);

    // 计算新的坐标
    newX = x + dx;
    newY = y + dy;
}

size_t getValidTrajectorySize(const ara::adsfi::MsgHafEgoTrajectory& traj, float max_len = 50.0f) {
    if (traj.trajectoryPoints.empty())
        return 0;

    // 最后一个点的累计长度
    float last_len = traj.trajectoryPoints.back().length;

    // 如果最后长度 <= 50，全部使用
    if (last_len <= max_len) {
        return traj.trajectoryPoints.size();
    }

    // 否则只取 length <= 50 的点
    for (size_t i = 0; i < traj.trajectoryPoints.size(); ++i) {
        if (traj.trajectoryPoints[i].length > max_len) {
            return i; // i 之前的点全部有效
        }
    }

    return traj.trajectoryPoints.size();
}

//根据规划的点数据移动到车头位置
bool move2CarHead(const ara::adsfi::MsgHafEgoTrajectory& path, const float& car_length, const float& point_length,
                  ara::adsfi::MsgHafEgoTrajectory& new_path) {
    if (car_length < 0.0f || point_length < 0.0f) {
        ApError("video_editor") << "move2CarHead failed: invalid car_length = " << car_length;
        return false;
    }

    if (path.trajectoryPoints.empty()) {
        ApError("video_editor") << "move2CarHead failed: empty trajectory";
        new_path.trajectoryPoints.clear();
        return false;
    }

    GeoPointVCS target_pvcs;
    target_pvcs.x = car_length / 2.0f;
    target_pvcs.y = 0.0f;
    target_pvcs.angle = 0.0f;

    size_t valid_size = getValidTrajectorySize(path, point_length);
    if (valid_size < 2) {
        return false;
    }

    for (size_t i = 0; i < valid_size; ++i) {
        GeoPointGCCS car_pgccs;
        car_pgccs.xg = path.trajectoryPoints[i].wayPoint.x;
        car_pgccs.yg = path.trajectoryPoints[i].wayPoint.y;
        car_pgccs.angle = path.trajectoryPoints[i].wayPoint.angle;

        GeoPointGCCS output_pgccs;
        if (!GeometryTool::VCS2GCCS(car_pgccs, target_pvcs, output_pgccs)) {
            ApError("video_editor") << "VCS2GCCS failed, skip this point";
            continue;
        }
        // 只更新全局坐标，避免污染局部坐标
        auto point = path.trajectoryPoints[i];
        point.wayPoint.xg = output_pgccs.xg;
        point.wayPoint.yg = output_pgccs.yg;
        point.wayPoint.angle = output_pgccs.angle;
        new_path.trajectoryPoints.push_back(point);
    }

    return true;
}

bool filterTrajectoryByDistanceOnly(const ara::adsfi::MsgHafEgoTrajectory& src, ara::adsfi::MsgHafEgoTrajectory& dst,
                                    float min_dist = 0.5f) {
    dst.trajectoryPoints.clear();

    if (src.trajectoryPoints.size() < 2) {
        return false;
    }

    dst.trajectoryPoints.reserve(src.trajectoryPoints.size());

    dst.trajectoryPoints.push_back(src.trajectoryPoints.front());
    const auto* last = &dst.trajectoryPoints.back();

    float acc_dist = 0.0f;

    for (size_t i = 1; i + 1 < src.trajectoryPoints.size(); ++i) {
        const auto& curr = src.trajectoryPoints[i];

        float dx = curr.wayPoint.xg - last->wayPoint.xg;
        float dy = curr.wayPoint.yg - last->wayPoint.yg;
        float dist = std::hypot(dx, dy);

        acc_dist += dist;

        if (acc_dist >= min_dist) {
            dst.trajectoryPoints.push_back(curr);
            last = &dst.trajectoryPoints.back();
            acc_dist = 0.0f;
        }
    }

    // 保证最后一个点
    dst.trajectoryPoints.push_back(src.trajectoryPoints.back());

    return dst.trajectoryPoints.size() >= 2;
}

void XVideoSelector(const ara::adsfi::AppVideoProfile&      video_profile,
                    const ara::adsfi::MsgHafLocation&       localition_fusion,   //位置信息
                    const ara::adsfi::VehicleInformation&   vehicle_information, // 车辆状态
                    const ara::adsfi::MsgHafFusionOutArray& objects,             // 目标识别结果
                    const ara::adsfi::MsgHafEgoTrajectory&  remote_control,      // 遥控时的路径规划
                    const ara::adsfi::MsgHafEgoTrajectory&  path,                // 路径规划
                    const ara::adsfi::PlanningStatus&       planning_status,
                    const ara::adsfi::AppRemoteControl&     control,        // 遥控信息
                    compatible::VideoParameter&             video_parameter // 视频参数
) {
    static bool             _init{false};
    static int              _out_width{0};
    static int              _out_height{0};
    static int              _out_fps{0};
    static int              _out_bps{0};
    static int              _out_gop{0};
    static double           _threshold_bps{29.2};       // 默认自适应切换帧率阈值
    static int              _switchover_interval{2000}; // 默认自适应切换周期（单位：毫秒）
    static int              _rate_control_mode{0};      // 默认码率控制方式（0：码率，1：质量）
    static BandwidthProfile _profile{};
    static std::shared_ptr<EncodingSelectionCB> _cb{nullptr};
    static float                                _point_length{0.0f};
    static float                                _point_interval{0.0f};
    static float                                _car_length{0.0f};
    if (!_init) {
        auto ptr = CustomStack::Instance();

        std::string qualities;
        std::string prefix;
        auto        out_width_path = prefix + "video/select/out_width";
        if (!ptr->GetProjectConfigValue(out_width_path, _out_width)) {
            EC400::Instance()->ec_add(EC400::CONFIG, out_width_path);
            throw std::runtime_error("read project config " + out_width_path + " failed");
        }

        auto out_height_path = prefix + "video/select/out_height";
        if (!ptr->GetProjectConfigValue(out_height_path, _out_height)) {
            EC400::Instance()->ec_add(EC400::CONFIG, out_height_path);
            throw std::runtime_error("read project config " + out_height_path + " failed");
        }

        auto out_fps_path = prefix + "video/select/out_fps";
        if (!ptr->GetProjectConfigValue(out_fps_path, _out_fps)) {
            EC400::Instance()->ec_add(EC400::CONFIG, out_fps_path);
            throw std::runtime_error("read project config " + out_fps_path + " failed");
        }

        auto out_bps_path = prefix + "video/select/out_bps";
        if (!ptr->GetProjectConfigValue(out_bps_path, _out_bps)) {
            EC400::Instance()->ec_add(EC400::CONFIG, out_bps_path);
            throw std::runtime_error("read project config " + out_bps_path + " failed");
        }

        auto out_gop_path = prefix + "video/select/out_gop";
        if (!ptr->GetProjectConfigValue(out_gop_path, _out_gop)) {
            EC400::Instance()->ec_add(EC400::CONFIG, out_gop_path);
            throw std::runtime_error("read project config " + out_gop_path + " failed");
        }

        auto threshold_bps_path = prefix + "video/select/threshold_bps";
        if (!ptr->GetProjectConfigValue(threshold_bps_path, _threshold_bps)) {
            EC400::Instance()->ec_add(EC400::CONFIG, threshold_bps_path);
            throw std::runtime_error("read project config " + threshold_bps_path + " failed");
        }

        auto switchover_interval_path = prefix + "video/select/switchover_interval";
        if (!ptr->GetProjectConfigValue(switchover_interval_path, _switchover_interval)) {
            EC400::Instance()->ec_add(EC400::CONFIG, switchover_interval_path);
            throw std::runtime_error("read project config " + switchover_interval_path + " failed");
        }

        auto rate_control_mode_path = prefix + "video/select/rate_control_mode";
        if (!ptr->GetProjectConfigValue(rate_control_mode_path, _rate_control_mode)) {
            EC400::Instance()->ec_add(EC400::CONFIG, rate_control_mode_path);
            throw std::runtime_error("read project config " + rate_control_mode_path + " failed");
        }

        auto qualities_path = prefix + "video/select/qualities";
        if (!ptr->GetProjectConfigValue(qualities_path, qualities)) {
            EC400::Instance()->ec_add(EC400::CONFIG, qualities_path);
            throw std::runtime_error("read project config " + qualities_path + " failed");
        }

        auto point_length_path = prefix + "video/select/point_length";
        if (!ptr->GetProjectConfigValue(point_length_path, _point_length)) {
            EC400::Instance()->ec_add(EC400::CONFIG, point_length_path);
            throw std::runtime_error("read project config " + point_length_path + " failed");
        }

        auto point_interval_path = prefix + "video/select/point_interval";
        if (!ptr->GetProjectConfigValue(point_interval_path, _point_interval)) {
            EC400::Instance()->ec_add(EC400::CONFIG, point_interval_path);
            throw std::runtime_error("read project config " + point_interval_path + " failed");
        }

        auto car_length_path = prefix + "video/select/car_length";
        if (!ptr->GetProjectConfigValue(car_length_path, _car_length)) {
            EC400::Instance()->ec_add(EC400::CONFIG, car_length_path);
            throw std::runtime_error("read project config " + car_length_path + " failed");
        }

        ApInfo("video_editor") << "out_width: " << _out_width;
        ApInfo("video_editor") << "out_height: " << _out_height;
        ApInfo("video_editor") << "out_fps: " << _out_fps;
        ApInfo("video_editor") << "out_bps: " << _out_bps;
        ApInfo("video_editor") << "out_gop: " << _out_gop;
        ApInfo("video_editor") << "threshold_bps: " << _threshold_bps;
        ApInfo("video_editor") << "switchover_interval: " << _switchover_interval;
        ApInfo("video_editor") << "rate_control_mode: " << _rate_control_mode;
        ApInfo("video_editor") << "point_length: " << _point_length;
        ApInfo("video_editor") << "point_interval: " << _point_interval;
        ApInfo("video_editor") << "qualities: " << qualities;

        auto values = app_common::StringUtils::tokenize<std::vector>((char*)qualities.c_str(), ",");
        if (values.size() != 4) {
            ApInfo("video_editor") << "error: qualities size != 4";
            EC400::Instance()->ec_add(EC400::CONFIG, qualities_path);
            throw std::runtime_error("read project config " + qualities_path + " failed");
        }
        std::vector<std::int32_t> i_qualities;
        for (const auto& v : values) {
            i_qualities.push_back(std::stoi(v));
        }

        auto c = ptr->GetNodeConfigPath() + "/resolution_bps.csv";
        try {
            auto m = app_common::CSVUtils::read(c, ',', 1);
            for (const auto& row : m) {
                RowOfBandwidthProfile p;
                p.rgb.width = std::stoi(row[0]);
                p.rgb.height = std::stoi(row[1]);
                p.rgb.bps = std::stoi(row[2]);
                p.gray.width = std::stoi(row[3]);
                p.gray.height = std::stoi(row[4]);
                p.gray.bps = std::stoi(row[5]);
                p.binary.width = std::stoi(row[6]);
                p.binary.height = std::stoi(row[7]);
                p.binary.bps = std::stoi(row[8]);
                p.feature.width = std::stoi(row[9]);
                p.feature.height = std::stoi(row[10]);
                p.feature.bps = std::stoi(row[11]);
                _profile.emplace_back(std::move(p));
            }
            _cb = std::make_shared<EncodingSelectionCB>(_profile, _threshold_bps, _switchover_interval, i_qualities);

            _init = true;

            EC400::Instance()->ec_remove(EC400::CONFIG);
        } catch (const std::runtime_error& e) {
            ApInfo("video_editor") << "error: read " << c << " " << e.what();
            EC400::Instance()->ec_add(EC400::CONFIG, qualities_path);
            throw std::runtime_error("read project config " + qualities_path + " failed");
        } catch (const std::exception& e) {
            ApInfo("video_editor") << "error: read " << c << " " << e.what();
            EC400::Instance()->ec_add(EC400::CONFIG, qualities_path);
            throw std::runtime_error("read project config " + qualities_path + " failed");
        } catch (...) {
            ApInfo("video_editor") << "error: read " << c;
            EC400::Instance()->ec_add(EC400::CONFIG, qualities_path);
            throw std::runtime_error("read project config " + qualities_path + " failed");
        }
    }

    if (video_profile.split_area_parames.size() > 0) {
        video_parameter.editor_parameter.split_parames_ = video_profile.split_area_parames;
    } else {
        ara::adsfi::AppVideoSplitParam split_parames;
        split_parames.channel_name = "front";
        ara::adsfi::AppVideoSplitPoint view_points;
        view_points.left_top_point_x = 0;
        view_points.left_top_point_y = 0;
        view_points.right_bottom_point_x = 1920;
        view_points.right_bottom_point_y = 1080;

        ara::adsfi::AppVideoSplitPoint crop_points;
        crop_points.left_top_point_x = 0;
        crop_points.left_top_point_y = 0;
        crop_points.right_bottom_point_x = 3840;
        crop_points.right_bottom_point_y = 2160;
        split_parames.view_area_points = view_points;
        split_parames.crop_area_points = crop_points;
        video_parameter.editor_parameter.split_parames_.push_back(split_parames);
    }

    video_parameter.encoding_parameter.rate_control_mode = _rate_control_mode;
    video_parameter.encoding_parameter.encoding_gop = _out_gop;
    video_parameter.encoding_parameter.fps = _out_fps;
    ApInfo("video_editor") << "quality: " << video_parameter.encoding_parameter.quality;
    if (0 == video_profile.output_mode) { // 说明要么没有上游输入、要么出错了
        ApInfo("video_editor") << "no input: ";
        video_parameter.editor_parameter.output_mode = 1;
        video_parameter.editor_parameter.remote_control_assist = 0;
        video_parameter.editor_parameter.remote_control_alarm = 0;
        video_parameter.editor_parameter.split_adaptive = 0;
        video_parameter.editor_parameter.video_main_ = 0;
        video_parameter.editor_parameter.video_left_ = -1;
        video_parameter.editor_parameter.video_right_ = -1;
        video_parameter.editor_parameter.video_third_person = 0;

        video_parameter.encoding_parameter.adaptive_mode = 0;
        video_parameter.encoding_parameter.bps = _out_bps;
        video_parameter.encoding_parameter.width = _out_width;
        video_parameter.encoding_parameter.height = _out_height;
    } else {
        video_parameter.editor_parameter.output_mode = video_profile.output_mode;
        video_parameter.editor_parameter.remote_control_assist = video_profile.remote_control_assist;
        video_parameter.editor_parameter.remote_control_alarm = video_profile.remote_control_alarm;
        video_parameter.editor_parameter.split_adaptive = video_profile.split_adaptive;
        video_parameter.editor_parameter.video_main_ = video_profile.manual_split_main;
        video_parameter.editor_parameter.video_left_ = video_profile.manual_split_left;
        video_parameter.editor_parameter.video_right_ = video_profile.manual_split_right;
        video_parameter.editor_parameter.video_third_person = video_profile.video_third_person;

        video_parameter.encoding_parameter.adaptive_mode = video_profile.adaptive_mode;
        video_parameter.encoding_parameter.bps = video_profile.manual_bps;
        video_parameter.encoding_parameter.width = video_profile.manual_width;
        video_parameter.encoding_parameter.height = video_profile.manual_height;
    }

    if (video_profile.remote_control_alarm == 1) {
        video_parameter.editor_parameter.objects = objects;
    } else {
        video_parameter.editor_parameter.objects = {};
    }

    if (video_profile.remote_control_assist == 1) {
        ApInfo("video_editor") << "remote_control: " << remote_control.trajectoryPoints.size();
        video_parameter.editor_parameter.remote_control = remote_control;
    } else {
        video_parameter.editor_parameter.remote_control.trajectoryPoints.resize(0);
    }

    auto now = std::chrono::system_clock::now();

    // 转换为 Unix 时间戳（秒为单位的 double）
    auto   duration = now.time_since_epoch();
    double timestamp_ = std::chrono::duration<double>(duration).count();
    double time_ = timestamp_ - app_common::DatetimeUtil::double_time_from_sec_nsec(path.egoTrajectoryHeader.timestamp);

    if (video_profile.auto_control_assist == 1) {
        ApInfo("video_editor") << "here";
        ara::adsfi::MsgHafEgoTrajectory out_path;
        float length = (1 != vehicle_information.vehicle_act_state.shift_position) ? _car_length : 0.0;
        if (move2CarHead(path, length, _point_length, out_path)) {
            ara::adsfi::MsgHafEgoTrajectory path_draw;
            if (filterTrajectoryByDistanceOnly(out_path, path_draw, _point_interval)) {
                video_parameter.editor_parameter.path = out_path;
            }
            video_parameter.editor_parameter.path = out_path;
        }

        if (control.remotectrl_flag != 0 || time_ > 1) {
            ApInfo("video_editor") << "here";
            video_parameter.editor_parameter.path.trajectoryPoints.resize(0);
        }
    } else {
        ApInfo("video_editor") << "here";
        video_parameter.editor_parameter.path.trajectoryPoints.resize(0);
    }

    if (control.remotectrl_flag != 0 || time_ > 1) {
        ApInfo("video_editor") << "here";
        video_parameter.editor_parameter.path.trajectoryPoints.resize(0);
    }

    ApInfo("video_editor") << "size of path: " << path.trajectoryPoints.size();
    ApInfo("video_editor") << "size of video_parameter.editor_parameter.path: "
                           << video_parameter.editor_parameter.path.trajectoryPoints.size();

    //自适应切换视角
    if (video_profile.split_adaptive == 1) {
        bool shift_position_R = vehicle_information.vehicle_act_state.shift_position == 1 ? true : false;
        changeCameraBySteerAngle(vehicle_information.vehicle_act_state.steer_angle, shift_position_R,
                                 video_profile.front_source, video_parameter);
    }

    if (1 == _rate_control_mode) {
        _cb->select(video_profile, video_parameter);
        ApInfo("video_editor") << "quality: " << video_parameter.encoding_parameter.quality;
        ApInfo("video_editor") << "rate_control_mode: " << video_parameter.encoding_parameter.rate_control_mode;
        ApInfo("video_editor") << "rate_control_mode: " << _rate_control_mode;
    } else if (video_parameter.encoding_parameter.adaptive_mode != 0) {
        _cb->select(video_profile, video_parameter);
    } else if (0 == video_parameter.encoding_parameter.bps || 0 == video_parameter.encoding_parameter.width) {
        // 理论上不应该出现这种情况
        ApInfo("video_editor") << "revert to default resolution and bps";
        video_parameter.encoding_parameter.bps = _out_bps;
        video_parameter.encoding_parameter.width = _out_width;
        video_parameter.encoding_parameter.height = _out_height;
    } else {
        ApInfo("video_editor") << "keep bps and resolution ";
    }
}
