#include "adsfi_interface.h"

#include "ap_log/ap_log_interface.h"
#include "common/time/node_time.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

namespace {

static bool IsDataLine(const std::string &line) {
    std::size_t i = 0;
    while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) {
        ++i;
    }
    if (i >= line.size()) return false;
    char c = line[i];
    if (c == '#') return false;
    return (c == '-' || c == '.' || (c >= '0' && c <= '9'));
}

static bool ParseDoubles(const std::string &line, std::vector<double> &out) {
    out.clear();
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (item.empty()) continue;
        try {
            out.push_back(std::stod(item));
        } catch (...) {
            return false;
        }
    }
    return !out.empty();
}

static ara::adsfi::MsgHafTime SecToHafTime(double t) {
    ara::adsfi::MsgHafTime out{};
    if (!std::isfinite(t) || t <= 0.0) {
        out.sec = 0U;
        out.nsec = 0U;
        return out;
    }
    double sec_d = 0.0;
    double frac = std::modf(t, &sec_d);
    if (sec_d < 0.0) {
        out.sec = 0U;
        out.nsec = 0U;
        return out;
    }
    double nsec_d = frac * 1e9;
    if (nsec_d < 0.0) nsec_d = 0.0;
    if (nsec_d > 999999999.0) nsec_d = 999999999.0;
    out.sec = static_cast<std::uint32_t>(sec_d);
    out.nsec = static_cast<std::uint32_t>(nsec_d);
    return out;
}

} // namespace

// DR_ 核心算法函数声明
extern void DR_(const ara::adsfi::MsgHafIMU& IMU,
                const ara::adsfi::MsgHafWheelSpeedList& WHEEL,
                const ara::adsfi::MsgHafLocation& TPIMU,
                ara::adsfi::MsgHafLocation& LIO_pose);

void AdsfiInterface::Init()
{
    // DR 期望 50 输入
    SetScheduleInfo("timmer", 20);
    // ApLogInterface::Instance()->Init("dr");

    
    if (simulation_mode_) {
        InitSimulationData();
        ApInfo("dr") << "init DR [SIMULATION MODE]";
    } else {
        ApInfo("dr") << "init DR [NORMAL MODE]";
    }
    ApInfo("dr") << "init DR, simulation_mode=" << simulation_mode_;
}

void AdsfiInterface::DeInit()
{
    ApInfo("dr") << "deinit DR";
}

// ================== 仿真数据初始化 ==================
void AdsfiInterface::InitSimulationData()
{
    sim_seq_ = 0;
    sim_time_ = avos::common::NodeTime::Now().ToSecond();
    sim_x_ = 448494.0;       // 初始 UTM X
    sim_y_ = 4417860.0;      // 初始 UTM Y
    sim_yaw_ = 90.0;         // 初始航向角 (朝北)
    sim_speed_ = 1.0;        // 初始速度 1 m/s
    
    ApInfo("dr") << "DR Simulation data initialized:";
    ApInfo("dr") << "  Initial position: x=" << sim_x_ << ", y=" << sim_y_;
    ApInfo("dr") << "  Initial yaw: " << sim_yaw_ << " deg";
    ApInfo("dr") << "  Initial speed: " << sim_speed_ << " m/s";
}

// ================== 生成虚拟 IMU 数据 ==================
void AdsfiInterface::GenerateSimIMU(ara::adsfi::MsgHafIMU &imu)
{
    imu = ara::adsfi::MsgHafIMU();
    
    double now = avos::common::NodeTime::Now().ToSecond();
    imu.imuHeader.timestamp.sec = static_cast<uint32_t>(now);
    imu.imuHeader.timestamp.nsec = static_cast<uint32_t>((now - imu.imuHeader.timestamp.sec) * 1e9);
    imu.imuHeader.seq = sim_seq_;
    imu.imuHeader.frameID = "simulation";
    
    // 直线行驶，角速度为 0
    imu.angularVelocity.x = 0.0f;
    imu.angularVelocity.y = 0.0f;
    imu.angularVelocity.z = 0.0f;  // deg/s
    
    // 加速度
    imu.linearAcceleration.x = 0.0f;  // 无加速
    imu.linearAcceleration.y = 0.0f;
    imu.linearAcceleration.z = 9.8f;  // 重力
    
    imu.imuStatus = 1;  // 有效
    imu.temperature = 25.0f;
}

// ================== 生成虚拟轮速数据 ==================
void AdsfiInterface::GenerateSimWheel(ara::adsfi::MsgHafWheelSpeedList &wheel)
{
    wheel = ara::adsfi::MsgHafWheelSpeedList();
    
    double now = avos::common::NodeTime::Now().ToSecond();
    wheel.header.timestamp.sec = static_cast<uint32_t>(now);
    wheel.header.timestamp.nsec = static_cast<uint32_t>((now - wheel.header.timestamp.sec) * 1e9);
    wheel.header.seq = sim_seq_;
    wheel.header.frameID = "simulation";
    
    // 四轮等速
    auto make_wheel = [this](uint8_t pos) {
        ara::adsfi::MsgHafWheelSpeed w;
        w.wheelSpeedMps = static_cast<float>(sim_speed_);
        w.wheelSpeedMpsValid = 1U;
        w.wheelSpeedPos = pos;
        return w;
    };
    
    wheel.wheel_speed_vec.clear();
    wheel.wheel_speed_vec.push_back(make_wheel(0));  // LF
    wheel.wheel_speed_vec.push_back(make_wheel(1));  // RF
    wheel.wheel_speed_vec.push_back(make_wheel(2));  // LR
    wheel.wheel_speed_vec.push_back(make_wheel(3));  // RR
}

// ================== 生成虚拟 TPIMU 数据 ==================
void AdsfiInterface::GenerateSimTpimu(ara::adsfi::MsgHafLocation &tpimu)
{
    tpimu = ara::adsfi::MsgHafLocation();
    
    double now = avos::common::NodeTime::Now().ToSecond();
    double dt = now - sim_time_;
    if (dt > 0.0 && dt < 1.0) {
        // 更新位置（直线行驶）
        double yaw_rad = sim_yaw_ * M_PI / 180.0;
        sim_x_ += sim_speed_ * dt * cos(yaw_rad);
        sim_y_ += sim_speed_ * dt * sin(yaw_rad);
    }
    sim_time_ = now;
    
    // Header
    tpimu.header.timestamp.sec = static_cast<uint32_t>(now);
    tpimu.header.timestamp.nsec = static_cast<uint32_t>((now - tpimu.header.timestamp.sec) * 1e9);
    tpimu.header.seq = sim_seq_;
    tpimu.header.frameID = "simulation";
    
    // Pose (UTM 坐标)
    tpimu.pose.pose.position.x = sim_x_;
    tpimu.pose.pose.position.y = sim_y_;
    tpimu.pose.pose.position.z = 0.0;
    tpimu.pose.pose.orientation.x = 0.0;   // roll
    tpimu.pose.pose.orientation.y = 0.0;   // pitch
    tpimu.pose.pose.orientation.z = sim_yaw_;  // yaw
    
    // DR Pose
    tpimu.dr_pose = tpimu.pose;
    
    // Velocity
    tpimu.velocity.twist.linear.x = sim_speed_;
    tpimu.velocity.twist.linear.y = 0.0;
    tpimu.velocity.twist.linear.z = 0.0;
    tpimu.velocity.twist.angular.x = 0.0;
    tpimu.velocity.twist.angular.y = 0.0;
    tpimu.velocity.twist.angular.z = 0.0;
    
    // 其他状态
    tpimu.locationState = 1;  // 有效
    tpimu.v = sim_speed_;
}

// ================== Callbacks ==================
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafIMU> &ptr)
{
    (void)name;
    safebuf_imu_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &ptr)
{
    (void)name;
    safebuf_wheel_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr)
{
    (void)name;
    safebuf_tpimu_.Set(ptr);
}

// ================== Process ==================
int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr)
{
    ApInfo("dr") << "PROCESS DR, simulation_mode=" << simulation_mode_;
    
    ara::adsfi::MsgHafIMU imu_msg;
    ara::adsfi::MsgHafWheelSpeedList wheel_msg;
    ara::adsfi::MsgHafLocation tpimu_msg;
    
    if (simulation_mode_) {
        // 仿真模式：生成虚拟数据
        GenerateSimIMU(imu_msg);
        GenerateSimWheel(wheel_msg);
        GenerateSimTpimu(tpimu_msg);
        
        sim_seq_++;
        
        ApInfo("dr") << "[DR SIM] seq=" << sim_seq_ 
              << ", pos=(" << sim_x_ << "," << sim_y_ << ")"
              << ", yaw=" << sim_yaw_ << ", speed=" << sim_speed_;
    } else {
        // 正常模式：从缓存获取数据
        std::shared_ptr<ara::adsfi::MsgHafIMU> imu_ptr;
        std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> wheel_ptr;
        std::shared_ptr<ara::adsfi::MsgHafLocation> tpimu_ptr;

        bool b_imu = safebuf_imu_.Get(imu_ptr);
        bool b_wheel = safebuf_wheel_.Get(wheel_ptr);
        bool b_tpimu = safebuf_tpimu_.Get(tpimu_ptr);

        if (!b_imu || !b_wheel || !b_tpimu) {
            ApInfo("dr") << "[DR] Waiting for data: imu=" << b_imu 
                  << ", wheel=" << b_wheel << ", tpimu=" << b_tpimu;
            return -1;
        }
        
        imu_msg = *imu_ptr;
        wheel_msg = *wheel_ptr;
        tpimu_msg = *tpimu_ptr;
    }

    // 调用 DR 算法
    ara::adsfi::MsgHafLocation out_loc;
    DR_(imu_msg, wheel_msg, tpimu_msg, out_loc);

    loc_ptr = std::make_shared<ara::adsfi::MsgHafLocation>(out_loc);
    
    ApInfo("dr") << "[DR] Output: pose=(" << loc_ptr->pose.pose.position.x 
          << "," << loc_ptr->pose.pose.position.y << ")"
          << ", dr_pose=(" << loc_ptr->dr_pose.pose.position.x 
          << "," << loc_ptr->dr_pose.pose.position.y << ")";

    return 0;
}


// ================== File Simulation ==================

bool AdsfiInterface::LoadSimData(const std::string &imu_csv, const std::string &wheel_csv,
                                 const std::string &tpimu_csv)
{
    sim_imu_.clear();
    sim_wheel_.clear();
    sim_tpimu_.clear();
    sim_index_ = 0;
    sim_loaded_ = false;
    sim_tp_x_ = 0.0;
    sim_tp_y_ = 0.0;
    sim_tp_last_t_ = -1.0;

    auto load_imu = [&](const std::string &path) -> bool {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            ApError("dr") << "[DR SIM] Open imu csv failed: " << path;
            return false;
        }
        std::string line;
        std::vector<double> vals;
        while (std::getline(ifs, line)) {
            if (!IsDataLine(line)) continue;
            if (!ParseDoubles(line, vals)) continue;
            if (vals.size() < 5) continue;
            ImuCsvRow r;
            r.t = vals[0];
            r.gyro_z_deg = vals[1];
            r.acc_x = vals[2];
            r.acc_y = vals[3];
            r.acc_z = vals[4];
            sim_imu_.push_back(r);
        }
        return !sim_imu_.empty();
    };

    auto load_wheel = [&](const std::string &path) -> bool {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            ApError("dr") << "[DR SIM] Open wheel csv failed: " << path;
            return false;
        }
        std::string line;
        std::vector<double> vals;
        while (std::getline(ifs, line)) {
            if (!IsDataLine(line)) continue;
            if (!ParseDoubles(line, vals)) continue;
            if (vals.size() < 5) continue;
            WheelCsvRow r;
            r.t = vals[0];
            r.v_lf = vals[1];
            r.v_rf = vals[2];
            r.v_lr = vals[3];
            r.v_rr = vals[4];
            sim_wheel_.push_back(r);
        }
        return !sim_wheel_.empty();
    };

    auto load_tpimu = [&](const std::string &path) -> bool {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            ApError("dr") << "[DR SIM] Open tpimu csv failed: " << path;
            return false;
        }
        std::string line;
        std::vector<double> vals;
        while (std::getline(ifs, line)) {
            if (!IsDataLine(line)) continue;
            if (!ParseDoubles(line, vals)) continue;
            if (vals.size() < 6) continue;
            TpimuCsvRow r;
            r.t = vals[0];
            r.x = vals[1];
            r.y = vals[2];
            r.z = vals[3];
            r.roll_deg = vals[4];
            r.pitch_deg = vals[5];
            sim_tpimu_.push_back(r);
        }
        return !sim_tpimu_.empty();
    };

    if (!load_imu(imu_csv)) {
        return false;
    }
    if (!load_wheel(wheel_csv)) {
        return false;
    }
    if (!tpimu_csv.empty()) {
        load_tpimu(tpimu_csv);
    }

    std::size_t n = std::min(sim_imu_.size(), sim_wheel_.size());
    if (!sim_tpimu_.empty()) {
        n = std::min(n, sim_tpimu_.size());
    }
    if (n == 0) {
        ApError("dr") << "[DR SIM] No valid samples loaded.";
        return false;
    }
    sim_imu_.resize(n);
    sim_wheel_.resize(n);
    if (!sim_tpimu_.empty()) {
        sim_tpimu_.resize(n);
    }

    sim_loaded_ = true;
    ApInfo("dr") << "[DR SIM] Loaded imu=" << sim_imu_.size()
          << ", wheel=" << sim_wheel_.size()
          << ", tpimu=" << sim_tpimu_.size();
    return true;
}

int AdsfiInterface::ProcessFromFile(std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr)
{
    if (!sim_loaded_ || sim_index_ >= sim_imu_.size() || sim_index_ >= sim_wheel_.size()) {
        return -1;
    }

    const auto &imu_r = sim_imu_[sim_index_];
    const auto &wh_r = sim_wheel_[sim_index_];

    ara::adsfi::MsgHafIMU imu_msg;
    imu_msg.imuHeader.timestamp = SecToHafTime(imu_r.t);
    imu_msg.imuHeader.seq = static_cast<std::uint32_t>(sim_index_);
    imu_msg.angularVelocity.z = static_cast<float>(imu_r.gyro_z_deg);
    imu_msg.linearAcceleration.x = static_cast<float>(imu_r.acc_x);
    imu_msg.linearAcceleration.y = static_cast<float>(imu_r.acc_y);
    imu_msg.linearAcceleration.z = static_cast<float>(imu_r.acc_z);

    ara::adsfi::MsgHafWheelSpeedList wheel_msg;
    wheel_msg.header.timestamp = SecToHafTime(wh_r.t);
    wheel_msg.header.seq = static_cast<std::uint32_t>(sim_index_);
    wheel_msg.wheel_speed_vec.clear();

    auto make_wheel = [](float speed, std::uint8_t pos) {
        ara::adsfi::MsgHafWheelSpeed w;
        w.wheelSpeedMps = speed;
        w.wheelSpeedMpsValid = 1U;
        w.wheelSpeedPos = pos;
        return w;
    };

    wheel_msg.wheel_speed_vec.push_back(make_wheel(static_cast<float>(wh_r.v_lf), 0));
    wheel_msg.wheel_speed_vec.push_back(make_wheel(static_cast<float>(wh_r.v_rf), 1));
    wheel_msg.wheel_speed_vec.push_back(make_wheel(static_cast<float>(wh_r.v_lr), 2));
    wheel_msg.wheel_speed_vec.push_back(make_wheel(static_cast<float>(wh_r.v_rr), 3));

    ara::adsfi::MsgHafLocation tpimu_msg;
    if (!sim_tpimu_.empty()) {
        const auto &tp = sim_tpimu_[sim_index_];
        tpimu_msg.header.timestamp = SecToHafTime(tp.t);
        tpimu_msg.header.seq = static_cast<std::uint32_t>(sim_index_);
        tpimu_msg.pose.pose.position.x = tp.x;
        tpimu_msg.pose.pose.position.y = tp.y;
        tpimu_msg.pose.pose.position.z = tp.z;
        tpimu_msg.pose.pose.orientation.x = tp.roll_deg;
        tpimu_msg.pose.pose.orientation.y = tp.pitch_deg;
    } else {
        // 简易合成 TPIMU：直线积分，防止 RTK 跳变误判
        double t = wh_r.t;
        if (sim_tp_last_t_ > 0.0) {
            double dt = t - sim_tp_last_t_;
            double v = 0.25 * (wh_r.v_lf + wh_r.v_rf + wh_r.v_lr + wh_r.v_rr);
            if (dt > 0.0 && dt < 1.0) {
                sim_tp_x_ += v * dt;
            }
        }
        sim_tp_last_t_ = t;
        tpimu_msg.header.timestamp = SecToHafTime(t);
        tpimu_msg.header.seq = static_cast<std::uint32_t>(sim_index_);
        tpimu_msg.pose.pose.position.x = sim_tp_x_;
        tpimu_msg.pose.pose.position.y = sim_tp_y_;
        tpimu_msg.pose.pose.position.z = 0.0;
        tpimu_msg.pose.pose.orientation.x = 0.0;
        tpimu_msg.pose.pose.orientation.y = 0.0;
    }

    ara::adsfi::MsgHafLocation out_loc;
    DR_(imu_msg, wheel_msg, tpimu_msg, out_loc);

    loc_ptr = std::make_shared<ara::adsfi::MsgHafLocation>(out_loc);

    sim_index_++;
    return 0;
}
