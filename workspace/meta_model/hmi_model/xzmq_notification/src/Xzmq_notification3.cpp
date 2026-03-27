#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DatetimeUtil.hpp"
#include "Ec409.hpp"
#include "GeoUtils.hpp"
#include "StringUtils.hpp"
#include "VinUtils.hpp"
#include "XMessageSend.h"
#include "Xzmq_notification3_terrain_map_config.hpp"
#include "Xzmq_notification3_unpassable_fault_codes_config.hpp"
#include "config_loader/custom_stack.h"
// #include "geometry_tools.hpp"
#include <nlohmann/json.hpp>

#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_faultdata.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_videoencodingperf.h"
// ==================== 全局共享资源（用于100Hz线程） ====================
static std::shared_ptr<XMessageSend> g_loc_handler = nullptr;
static std::mutex                    g_mutex_handler;
static std::shared_ptr<XMessageSend> g_handler = nullptr;
static std::shared_ptr<XMessageSend> g_terrain_info_handler = nullptr;

using InsFaultMapType = std::unordered_map<std::string, uint16_t>;
static InsFaultMapType g_ins_fault_map{};
// 最新的融合定位和GNSS数据（主线程不断更新，100Hz线程读取）
static ara::adsfi::MsgHafLocation g_latest_imu{};
static ara::adsfi::MsgHafGnssInfo g_latest_location{};

// 数据健康异常标志（主线程更新，100Hz线程使用）
static bool g_imu_exception{false};
static bool g_loc_exception{false};

// 10Hz线程需要的共享数据
static ara::adsfi::MsgHafFusionOutArray g_latest_detected_objects{};
static ara::adsfi::MsgHafFusionOutArray g_latest_target_obj_out{};
static ara::adsfi::PerceptionStaticEnv  g_latest_semantic_map{};
static ara::adsfi::TaskManageStatus     g_latest_task_manage_status{};

static ara::adsfi::FaultData g_latest_fault_data{};

// 通行率"不可通行"故障码列表
static std::unordered_set<std::string> g_unpassable_fault_codes{};

// 地形图配置
static TerrainMapConfig              g_terrain_map_cfg{};
static std::shared_ptr<XMessageSend> g_fault_upload_handler = nullptr;

static std::mutex g_data_mutex;

static std::shared_ptr<Ec409> _ec = nullptr;

// 判断点是否在旋转矩形内的辅助函数
// 全局版本号
static uint64_t g_software_version = 0;

// INS 故障码配置结构体
struct InsFaultCodeConfig {
    std::string gyro_x;     // Bit 0: 陀螺仪X
    std::string gyro_y;     // Bit 1: 陀螺仪Y
    std::string gyro_z;     // Bit 2: 陀螺仪Z
    std::string accel_x;    // Bit 3: 加速度计X
    std::string accel_y;    // Bit 4: 加速度计Y
    std::string accel_z;    // Bit 5: 加速度计Z
    std::string sat_loss;   // Bit 6: 未收到卫星数据
    std::string bind_error; // Bit 7: 装订/其它故障 (1011022)
    std::string odom_fault; // Bit 8: 轮速计故障 (暂时为空)
};

static InsFaultCodeConfig g_ins_fault_cfg{};

// 设备驱动模块名称配置
struct ModuleNameConfig {
    std::string front_lidar;
    std::string left_front_lidar;
    std::string right_front_lidar;
    std::string front_radar;
    std::string left_front_radar;
    std::string right_front_radar;
    std::string mdc_cameralist;
    std::string front_middle_60;
    std::string front_middle_around_120;
    std::string left_front_around_100;
    std::string right_front_around_100;
    std::string left_front_60;
    std::string left_back_100;
    std::string right_front_60;
    std::string right_back_100;
    std::string back_middle_around_120;
    std::string front_fisheye_200;
    std::string left_fisheye_200;
    std::string right_fisheye_200;
    std::string back_fisheye_200;
    std::string front;
    std::string back;
    std::string imu;
};

static ModuleNameConfig g_module_name_cfg{};

// 初始化软件版本函数
void InitSoftwareVersion() {
    try {
        std::ifstream file("/opt/usr/ap_adsfi/version.json");
        if (!file.is_open()) {
            ApError("zmq_notification") << "--无法打开 ../version.json";
            return;
        }

        nlohmann::json j;
        file >> j;

        std::string version_str = j.value("version_info", "");
        ApError("zmq_notification") << "--读取到的原始字符串: [" << version_str << "]"; // 关键调试日志

        if (version_str.empty()) {
            return;
        }

        // 这样无论文件是 UTF-8 还是 GBK，只要是非数字就能匹配通过
        std::regex  pattern(R"(V(\d+)\D+(\d+)\D+(\d+)\D+(\d+)\D+(\d+)\D+(\d+).*)");
        std::smatch matches;

        if (std::regex_match(version_str, matches, pattern)) {
            // Byte 0: Stage
            uint8_t stage = static_cast<uint8_t>(std::stoi(matches[1]));
            // Byte 1: Review
            uint8_t review = static_cast<uint8_t>(std::stoi(matches[2]));
            // Byte 2: Internal
            uint8_t internal = static_cast<uint8_t>(std::stoi(matches[3]));

            // Byte 3: Year
            int     full_year = std::stoi(matches[4]);
            uint8_t year_offset = 0;
            if (full_year >= 1970) {
                year_offset = static_cast<uint8_t>(full_year - 1970);
            }

            // Byte 4: Month
            uint8_t month = static_cast<uint8_t>(std::stoi(matches[5]));

            // Byte 5: Day
            uint8_t day = static_cast<uint8_t>(std::stoi(matches[6]));

            ApError("zmq_notification") << "--解析结果: Stage=" << (int)stage << " Rev=" << (int)review
                                        << " Int=" << (int)internal << " Year=" << full_year << " Mon=" << (int)month
                                        << " Day=" << (int)day;

            g_software_version = static_cast<uint64_t>(stage) | (static_cast<uint64_t>(review) << 8) |
                                 (static_cast<uint64_t>(internal) << 16) | (static_cast<uint64_t>(year_offset) << 24) |
                                 (static_cast<uint64_t>(month) << 32) | (static_cast<uint64_t>(day) << 40);

            ApError("zmq_notification") << "--打包后 Hex: 0x" << std::hex << g_software_version << std::dec;

        } else {
            ApError("zmq_notification") << "--正则匹配失败! 请检查格式。字符串: " << version_str;
        }

    } catch (const std::exception& e) {
        ApError("zmq_notification") << "-- 解析 version.json 异常: " << e.what();
    }
}
// 判断点是否在旋转矩形内的辅助函数
bool isPointInRotatedRectangle(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3, float x4,
                               float y4) {
    // 使用叉积判断点是否在矩形内
    // 对于凸多边形，如果点在所有边的同一侧，则点在多边形内

    // 边1: (x1,y1) -> (x2,y2)
    float cross1 = (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);

    // 边2: (x2,y2) -> (x3,y3)
    float cross2 = (x3 - x2) * (py - y2) - (y3 - y2) * (px - x2);

    // 边3: (x3,y3) -> (x4,y4)
    float cross3 = (x4 - x3) * (py - y3) - (y4 - y3) * (px - x3);

    // 边4: (x4,y4) -> (x1,y1)
    float cross4 = (x1 - x4) * (py - y4) - (y1 - y4) * (px - x4);

    // 如果所有叉积的符号相同（都为正或都为负），则点在矩形内
    return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0 && cross4 >= 0) ||
           (cross1 <= 0 && cross2 <= 0 && cross3 <= 0 && cross4 <= 0);
}

inline uint64_t pack_protocol_time() {
    using namespace std::chrono;
    auto tp = system_clock::now();
    auto ms = time_point_cast<milliseconds>(tp).time_since_epoch().count();
    auto s = ms / 1000;
    auto milli = ms % 1000;

    std::tm     tm_local{};
    std::time_t tt = s;
    localtime_r(&tt, &tm_local);

    uint64_t y = tm_local.tm_year - 70;
    uint64_t M = tm_local.tm_mon + 1;
    uint64_t d = tm_local.tm_mday;
    uint64_t h = tm_local.tm_hour;
    uint64_t mi = tm_local.tm_min;
    uint64_t s2 = tm_local.tm_sec;
    uint64_t ms16 = milli;

    uint64_t packed = (y << 56) | (M << 48) | (d << 40) | (h << 32) | (mi << 24) | (s2 << 16) | ms16;

    return packed;
}

static protocol_common::auto2manager_vehicle_status get_auto2manager_vehicle_status(
    const ara::adsfi::VehicleInformation& vehicle_information, // 车辆状态信息
    const ara::adsfi::FaultData& fault_data, const ara::adsfi::BusinessCommand& business_command,
    const ara::adsfi::AppRemoteControl& remote_control);

static protocol_common::auto2manager_video_status get_auto2manager_video_status(
    const ara::adsfi::AppVideoProfile& video_profile, const ara::adsfi::VideoEncodingPerf& encoding_perf);

// 地形感知信息发布
static protocol_common::auto2manager_terrain_info get_auto2manager_terrain_info(
    const ara::adsfi::MsgHafLocation& latest_imu, const ara::adsfi::PerceptionStaticEnv& semantic_map,
    const ara::adsfi::MsgHafFusionOutArray& detected_objects);

// 感知目标发布
static protocol_common::auto2manager_perception_target_publish get_auto2manager_perception_target_publish(
    const ara::adsfi::MsgHafFusionOutArray& detected_objects, const ara::adsfi::MsgHafGnssInfo& location,
    const ara::adsfi::MsgHafFusionOutArray& target_obj_out);

// 偏移量发布
static protocol_common::auto2manager_offset_publish get_auto2manager_offset_release(
    const ara::adsfi::TaskManageStatus& status, const ara::adsfi::FaultData& fault_data);

// 传感器故障上报
static protocol_common::auto2infosec_fault_upload get_auto2infosec_fault_upload(const ara::adsfi::FaultData& fault_data,
                                                                                const ModuleNameConfig&      config);

// 定位状态上报
static protocol_common::loc2manager_status get_loc2manager_status(const ara::adsfi::FaultData& fault_data);

// INS错误上报值计算
static uint16_t GetInsErrorReportValue(const ara::adsfi::FaultData& fault_data) {
    uint16_t error_report = 0;

    if (fault_data.fault_info.empty()) {
        return 0;
    }

    // 遍历当前车辆报出的所有故障
    for (const auto& fault : fault_data.fault_info) {
        // 容错优化：虽然 fault.code 通常是机器生成的，但为了极致安全，
        // 如果你担心上游发来的故障码带空格，可以在这里做一个副本并 trim。
        // 但通常只需要保证配置文件是干净的即可，这里直接用引用。
        const std::string& code = fault.code;

        // 查表 (O(1) 复杂度)
        auto it = g_ins_fault_map.find(code);
        if (it != g_ins_fault_map.end()) {
            // 命中配置的故障码，直接按位或运算
            error_report |= it->second;
        }
    }

    return error_report;
}

static void Loc100HzPublishThread() {
    ApError("zmq_notification") << "zmq send Loc100HzPublishThread started ";
    auto next_wakeup = std::chrono::steady_clock::now();
    // 协议规定的无效值（集中定义，便于维护）
    constexpr int32_t  INVALID_LON = 181000000; // 181 * 1e6
    constexpr int32_t  INVALID_LAT = 91000000;  //  91 * 1e6
    constexpr int32_t  INVALID_ALT = 999999;
    constexpr uint16_t INVALID_HEADING = 36100;
    constexpr int16_t  INVALID_PITCH_ROLL = 9100;
    int                loop_count = 0;
    while (true) {
        next_wakeup += std::chrono::milliseconds(10);
        bool send_success = true;
        loop_count++; // 每次循环计数加 1
        try {
            protocol_common::loc2manager_basic_data basic_location;
            basic_location.header.timestamp = pack_protocol_time();
            basic_location.header.source_systemid = 4;
            basic_location.header.target_systemid = 1;
            // 安全拷贝共享数据
            ara::adsfi::MsgHafLocation imu_copy;
            ara::adsfi::MsgHafGnssInfo location_copy;
            ara::adsfi::FaultData      fault_data_copy;
            bool                       imu_exc;
            bool                       loc_exc;
            {
                std::lock_guard<std::mutex> lock(g_data_mutex);
                imu_copy = g_latest_imu;
                location_copy = g_latest_location;
                fault_data_copy = g_latest_fault_data;
                imu_exc = g_imu_exception;
                loc_exc = g_loc_exception;
            }
            const auto& imu = imu_copy;
            const auto& loc = location_copy;
            if (!imu_exc) {
                // 融合 + 组合定位
                const int32_t lon = static_cast<int32_t>(imu.llh.x * 1e6);
                const int32_t lat = static_cast<int32_t>(imu.llh.y * 1e6);
                const int32_t alt = static_cast<int32_t>(imu.llh.z * 10);

                basic_location.fuse_geo.longitude = lon;
                basic_location.fuse_geo.latitude = lat;
                basic_location.fuse_geo.altitude = alt;
                basic_location.nav_geo.longitude = lon;
                basic_location.nav_geo.latitude = lat;
                basic_location.nav_geo.altitude = alt;
                // 融合姿态
                basic_location.fuse_pose.pitch = static_cast<int16_t>(imu.pose.pose.orientation.x * 100);
                basic_location.fuse_pose.roll = static_cast<int16_t>(imu.pose.pose.orientation.y * 100);
                // 航向特殊处理
                double heading = 90.0 - imu.pose.pose.orientation.z;
                if (heading < 0.0) {
                    heading += 360.0;
                }

                const uint8_t imu_status = static_cast<uint8_t>(imu.locationState);
                if (imu_status != 0) {
                    basic_location.fuse_pose.heading = static_cast<uint16_t>(heading * 100);
                } else {
                    basic_location.fuse_pose.heading =
                        !loc_exc ? static_cast<uint16_t>(loc.attitude.z * 100) : INVALID_HEADING;
                }
                // 融合速度 & 角速度
                basic_location.fuse_vx = static_cast<int16_t>(imu.velocity.twist.linear.x * 10);
                basic_location.fuse_vy = static_cast<int16_t>(imu.velocity.twist.linear.y * 10);
                basic_location.fuse_vz = static_cast<int16_t>(imu.velocity.twist.linear.z * 10);
                basic_location.fuse_wx = static_cast<int16_t>(imu.velocity.twist.angular.x * 100);
                basic_location.fuse_wy = static_cast<int16_t>(imu.velocity.twist.angular.y * 100);
                basic_location.fuse_wz = static_cast<int16_t>(imu.velocity.twist.angular.z * 100);

                // // DR 递推位置 & 姿态
                basic_location.dr_x = static_cast<int32_t>(imu.dr_pose.pose.position.x * 10);
                basic_location.dr_y = static_cast<int32_t>(imu.dr_pose.pose.position.y * 10);
                basic_location.dr_z = static_cast<int32_t>(imu.dr_pose.pose.position.z * 10);

                basic_location.dr_pose.heading = static_cast<uint16_t>(imu.dr_pose.pose.orientation.z * 100);
                basic_location.dr_pose.pitch = static_cast<int16_t>(imu.dr_pose.pose.orientation.y * 100);
                basic_location.dr_pose.roll = static_cast<int16_t>(imu.dr_pose.pose.orientation.x * 100);

                // 状态
                basic_location.navstate = static_cast<uint8_t>(imu.locationState);
                basic_location.insstate = static_cast<uint8_t>(imu.locationState);
                basic_location.ins_error_report = GetInsErrorReportValue(fault_data_copy);
            } else {
                // IMU 异常：所有 IMU 相关字段使用协议无效值
                // 注意：即使 GPS 仍有有效数据，也强制融合结果无效
                // 融合定位 & 导航定位
                basic_location.fuse_geo.longitude = INVALID_LON;
                basic_location.fuse_geo.latitude = INVALID_LAT;
                basic_location.fuse_geo.altitude = INVALID_ALT;

                basic_location.nav_geo.longitude = INVALID_LON;
                basic_location.nav_geo.latitude = INVALID_LAT;
                basic_location.nav_geo.altitude = INVALID_ALT;

                // 融合姿态（全部置无效）
                basic_location.fuse_pose.heading = INVALID_HEADING;
                basic_location.fuse_pose.pitch = INVALID_PITCH_ROLL;
                basic_location.fuse_pose.roll = INVALID_PITCH_ROLL;

                // 融合速度
                basic_location.fuse_vx = 0;
                basic_location.fuse_vy = 0;
                basic_location.fuse_vz = 0;

                // 融合角速度
                basic_location.fuse_wx = 0;
                basic_location.fuse_wy = 0;
                basic_location.fuse_wz = 0;

                // DR 递推位置
                basic_location.dr_x = 0;
                basic_location.dr_y = 0;
                basic_location.dr_z = 0;

                // DR 递推姿态
                basic_location.dr_pose.heading = INVALID_HEADING;
                basic_location.dr_pose.pitch = INVALID_PITCH_ROLL;
                basic_location.dr_pose.roll = INVALID_PITCH_ROLL;

                // 状态全部置为无效/0
                basic_location.navstate = 0;
                basic_location.insstate = 0;
                basic_location.ins_error_report = 0;
            }

            // 2. 纯卫星相关（使用 GNSS 纯卫星数据，而非融合定位数据）
            if (!loc_exc) {
                basic_location.sate_geo.longitude = static_cast<int32_t>(loc.longitude * 1e6);
                basic_location.sate_geo.latitude = static_cast<int32_t>(loc.latitude * 1e6);
                basic_location.sate_geo.altitude = static_cast<int32_t>(loc.elevation * 10);
                basic_location.gnssstate = static_cast<uint8_t>(loc.positionType);
            } else {
                basic_location.sate_geo.longitude = INVALID_LON;
                basic_location.sate_geo.latitude = INVALID_LAT;
                basic_location.sate_geo.altitude = INVALID_ALT;
                basic_location.gnssstate = 0;
            }
            if (g_loc_handler) {
                g_loc_handler->sendTpImu(basic_location);
            }
            if (loop_count >= 100 && g_loc_handler) {
                protocol_common::loc2manager_status loc_status = get_loc2manager_status(fault_data_copy);
                g_loc_handler->sendLocStatus(loc_status);
                loop_count = 0;
            }
        } catch (const std::exception& e) {
            send_success = false;
            _ec->ec_add(Ec409::_ERRORCODE_ZMQ_SEND, "Loc100HzPublishThread");
            ApError("zmq_notification") << "Loc100HzPublishThread exception: " << e.what();
        } catch (...) {
            send_success = false;
            _ec->ec_add(Ec409::_ERRORCODE_ZMQ_SEND, "Loc100HzPublishThread");
            ApError("zmq_notification") << "Loc100HzPublishThread unknown exception";
        }
        if (send_success) {
            _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_SEND);
        }
        std::this_thread::sleep_until(next_wakeup);
    }
}

static void TenHzPublishThread() {
    ApError("zmq_notification") << "zmq send TenHzPublishThread started ";

    auto next_wakeup = std::chrono::steady_clock::now();

    while (true) {
        next_wakeup += std::chrono::milliseconds(100); // 固定100ms步进，防漂移
        bool ten_hz_success = true;

        try {
            // 加锁安全拷贝所有需要的最新数据
            ara::adsfi::MsgHafLocation       imu_copy;
            ara::adsfi::MsgHafGnssInfo       location_copy;
            ara::adsfi::MsgHafFusionOutArray detected_objects_copy;
            ara::adsfi::MsgHafFusionOutArray target_obj_out_copy;
            ara::adsfi::PerceptionStaticEnv  semantic_map_copy;
            ara::adsfi::TaskManageStatus     task_manage_status_copy;
            ara::adsfi::FaultData            fault_data_copy;

            {
                std::lock_guard<std::mutex> lock(g_data_mutex);
                imu_copy = g_latest_imu;
                location_copy = g_latest_location;
                detected_objects_copy = g_latest_detected_objects;
                target_obj_out_copy = g_latest_target_obj_out;
                semantic_map_copy = g_latest_semantic_map;
                task_manage_status_copy = g_latest_task_manage_status;
                fault_data_copy = g_latest_fault_data;
            }

            // 1. 地形感知信息（最耗CPU的部分）
            if (g_terrain_info_handler) {
                auto terrain_info = get_auto2manager_terrain_info(imu_copy, semantic_map_copy, detected_objects_copy);
                g_terrain_info_handler->sendTerrainInfo(terrain_info);
            }

            // 2. 感知目标发布
            {
                std::lock_guard<std::mutex> lg(g_mutex_handler);
                if (g_handler) {
                    auto perception_target = get_auto2manager_perception_target_publish(
                        detected_objects_copy, location_copy, target_obj_out_copy);
                    g_handler->sendPerceptionTarget(perception_target);
                }
            }

            // 3. 偏移量发布
            {
                std::lock_guard<std::mutex> lg(g_mutex_handler);
                if (g_handler) {
                    auto offset_publish = get_auto2manager_offset_release(task_manage_status_copy, fault_data_copy);
                    g_handler->sendOffSetRelease(offset_publish);
                }
            }

        } catch (const std::exception& e) {
            ten_hz_success = false;
            _ec->ec_add(Ec409::_ERRORCODE_ZMQ_SEND, "TenHzPublishThread");
            ApError("zmq_notification") << "TenHzPublishThread exception: " << e.what();
        } catch (...) {
            ten_hz_success = false;
            _ec->ec_add(Ec409::_ERRORCODE_ZMQ_SEND, "TenHzPublishThread");
            ApError("zmq_notification") << "TenHzPublishThread unknown exception";
        }
        if (ten_hz_success) {
            _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_SEND);
        }

        std::this_thread::sleep_until(next_wakeup);
    }
}

void Xzmq_notification3(const ara::adsfi::MsgHafLocation&       imu,
                        const ara::adsfi::MsgHafGnssInfo&       location, // 车辆当前位置
                        const ara::adsfi::FaultData&            fault_data,
                        const ara::adsfi::VehicleInformation&   vehicle_information, // 车辆状态信息
                        const ara::adsfi::VehicleActControl&    control,
                        const ara::adsfi::PerceptionStaticEnv&  semantic_map,
                        const ara::adsfi::MsgHafFusionOutArray& detected_objects, // 检测到的障碍物
                        const ara::adsfi::MsgHafFusionOutArray& target_obj_out,   // 当前跟踪目标输出
                        const ara::adsfi::BusinessCommand&      business_command, // 业务层当前命令
                        const ara::adsfi::TaskManageStatus&     task_manage_status,
                        const ara::adsfi::AppVideoProfile&      video_profile,
                        const ara::adsfi::AppRemoteControl&     remote_control, // 远程控制参数
                        const ara::adsfi::VideoEncodingPerf&    encoding_perf) {
    // add your code here
    static bool                                  _init = false;
    static int                                   _count = 0;
    static std::chrono::steady_clock::time_point last_1hz = std::chrono::steady_clock::now();

    // 数据健康监控变量（毫秒级超时）
    static double                                _last_imu_time = -1.0;
    static double                                _last_loc_time = -1.0;
    static std::chrono::steady_clock::time_point _last_imu_update = std::chrono::steady_clock::now();
    static std::chrono::steady_clock::time_point _last_loc_update = std::chrono::steady_clock::now();
    static constexpr int                         DATA_TIMEOUT_MS = 200; // 超时阈值：200毫秒

    if (!_ec) {
        _ec = std::make_shared<Ec409>();
        if (!_ec) {
            ApError("zmq_notification") << "cannot create Ec409";
            return;
        }
    }

    if (!_init) {
        // 统一获取配置路径前缀
        std::string sub_endpoint = "tcp://*:31000";
        std::string sub_loc_endpoint = "ipc:///opt/usr/FD_task_manager/paramsfile/ins_status";
        std::string pub_terrain_endpoint = "ipc:///opt/usr/FD_task_manager/paramsfile/OGM_map";
        std::string pub_sensor_fault_endpoint = "tcp://*:21002";

        auto        ptr = CustomStack::Instance();
        std::string prefix;

        auto full_path = prefix + "hmi/zmq/normal_status_endpoint";
        if (!ptr->GetProjectConfigValue(full_path, sub_endpoint) || sub_endpoint.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << sub_endpoint;

        full_path = prefix + "hmi/zmq/loc_status_endpoint";
        if (!ptr->GetProjectConfigValue(full_path, sub_loc_endpoint) || sub_loc_endpoint.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << sub_loc_endpoint;

        full_path = prefix + "hmi/zmq/terrain_endpoint";
        if (!ptr->GetProjectConfigValue(full_path, pub_terrain_endpoint) || pub_terrain_endpoint.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << pub_terrain_endpoint;

        full_path = prefix + "hmi/zmq/sensor_fault_endpoint";
        if (!ptr->GetProjectConfigValue(full_path, pub_sensor_fault_endpoint) || pub_sensor_fault_endpoint.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }

        // 读取INS故障码配置
        full_path = prefix + "hmi/ins_fault_codes/gyro_x";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.gyro_x)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.gyro_x;

        full_path = prefix + "hmi/ins_fault_codes/gyro_y";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.gyro_y)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.gyro_y;

        full_path = prefix + "hmi/ins_fault_codes/gyro_z";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.gyro_z)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.gyro_z;
        full_path = prefix + "hmi/ins_fault_codes/accel_x";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.accel_x)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.accel_x;

        full_path = prefix + "hmi/ins_fault_codes/accel_y";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.accel_y)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.accel_y;

        full_path = prefix + "hmi/ins_fault_codes/accel_z";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.accel_z)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.accel_z;

        full_path = prefix + "hmi/ins_fault_codes/sat_loss";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.sat_loss)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.sat_loss;

        full_path = prefix + "hmi/ins_fault_codes/bind_error";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.bind_error)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.bind_error;

        full_path = prefix + "hmi/ins_fault_codes/odom_fault";
        if (!ptr->GetProjectConfigValue(full_path, g_ins_fault_cfg.odom_fault)) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_ins_fault_cfg.odom_fault;

        app_common::StringUtils::trim(g_ins_fault_cfg.gyro_x);
        app_common::StringUtils::trim(g_ins_fault_cfg.gyro_y);
        app_common::StringUtils::trim(g_ins_fault_cfg.gyro_z);
        app_common::StringUtils::trim(g_ins_fault_cfg.accel_x);
        app_common::StringUtils::trim(g_ins_fault_cfg.accel_y);
        app_common::StringUtils::trim(g_ins_fault_cfg.accel_z);
        app_common::StringUtils::trim(g_ins_fault_cfg.sat_loss);
        app_common::StringUtils::trim(g_ins_fault_cfg.bind_error);
        app_common::StringUtils::trim(g_ins_fault_cfg.odom_fault);

        // 构建INS故障码映射表
        g_ins_fault_map.clear();
        if (!g_ins_fault_cfg.gyro_x.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.gyro_x] = 0x0001;
        }
        if (!g_ins_fault_cfg.gyro_y.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.gyro_y] = 0x0002;
        }
        if (!g_ins_fault_cfg.gyro_z.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.gyro_z] = 0x0004;
        }
        if (!g_ins_fault_cfg.accel_x.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.accel_x] = 0x0008;
        }
        if (!g_ins_fault_cfg.accel_y.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.accel_y] = 0x0010;
        }
        if (!g_ins_fault_cfg.accel_z.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.accel_z] = 0x0020;
        }
        if (!g_ins_fault_cfg.sat_loss.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.sat_loss] = 0x0040;
        }
        if (!g_ins_fault_cfg.bind_error.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.bind_error] = 0x0080;
        }
        if (!g_ins_fault_cfg.odom_fault.empty()) {
            g_ins_fault_map[g_ins_fault_cfg.odom_fault] = 0x0100;
        }

        ApError("zmq_notification") << "INS fault map loaded, size: " << g_ins_fault_map.size();

        // 获取驱动设备模块名称
        // TODO 协议确定后加上empty限制
        full_path = prefix + "hmi/module_name/front_lidar";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.front_lidar) ||
            g_module_name_cfg.front_lidar.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.front_lidar;

        full_path = prefix + "hmi/module_name/left_front_lidar";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.left_front_lidar) ||
            g_module_name_cfg.left_front_lidar.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.left_front_lidar;

        full_path = prefix + "hmi/module_name/right_front_lidar";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.right_front_lidar) ||
            g_module_name_cfg.right_front_lidar.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.right_front_lidar;

        full_path = prefix + "hmi/module_name/front_radar";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.front_radar) ||
            g_module_name_cfg.front_radar.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.front_radar;

        full_path = prefix + "hmi/module_name/left_front_radar";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.left_front_radar) ||
            g_module_name_cfg.left_front_radar.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.left_front_radar;

        full_path = prefix + "hmi/module_name/right_front_radar";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.right_front_radar) ||
            g_module_name_cfg.right_front_radar.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.right_front_radar;

        full_path = prefix + "hmi/module_name/mdc_cameralist";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.mdc_cameralist) ||
            g_module_name_cfg.mdc_cameralist.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.mdc_cameralist;

        full_path = prefix + "hmi/module_name/front_middle_60";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.front_middle_60) ||
            g_module_name_cfg.front_middle_60.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.front_middle_60;

        full_path = prefix + "hmi/module_name/front_middle_around_120";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.front_middle_around_120) ||
            g_module_name_cfg.front_middle_around_120.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.front_middle_around_120;

        full_path = prefix + "hmi/module_name/left_front_around_100";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.left_front_around_100) ||
            g_module_name_cfg.left_front_around_100.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.left_front_around_100;

        full_path = prefix + "hmi/module_name/right_front_around_100";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.right_front_around_100) ||
            g_module_name_cfg.right_front_around_100.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.right_front_around_100;

        full_path = prefix + "hmi/module_name/left_front_60";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.left_front_60) ||
            g_module_name_cfg.left_front_60.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.left_front_60;

        full_path = prefix + "hmi/module_name/left_back_100";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.left_back_100) ||
            g_module_name_cfg.left_back_100.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.left_back_100;

        full_path = prefix + "hmi/module_name/right_front_60";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.right_front_60) ||
            g_module_name_cfg.right_front_60.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.right_front_60;

        full_path = prefix + "hmi/module_name/right_back_100";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.right_back_100) ||
            g_module_name_cfg.right_back_100.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.right_back_100;

        full_path = prefix + "hmi/module_name/back_middle_around_120";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.back_middle_around_120) ||
            g_module_name_cfg.back_middle_around_120.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.back_middle_around_120;

        full_path = prefix + "hmi/module_name/front_fisheye_200";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.front_fisheye_200) ||
            g_module_name_cfg.front_fisheye_200.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.front_fisheye_200;

        full_path = prefix + "hmi/module_name/left_fisheye_200";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.left_fisheye_200) ||
            g_module_name_cfg.left_fisheye_200.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.left_fisheye_200;

        full_path = prefix + "hmi/module_name/right_fisheye_200";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.right_fisheye_200) ||
            g_module_name_cfg.right_fisheye_200.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.right_fisheye_200;

        full_path = prefix + "hmi/module_name/back_fisheye_200";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.back_fisheye_200) ||
            g_module_name_cfg.back_fisheye_200.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.back_fisheye_200;

        full_path = prefix + "hmi/module_name/front";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.front) || g_module_name_cfg.front.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.front;

        full_path = prefix + "hmi/module_name/back";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.back) || g_module_name_cfg.back.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.back;

        full_path = prefix + "hmi/module_name/imu";
        if (!ptr->GetProjectConfigValue(full_path, g_module_name_cfg.imu) || g_module_name_cfg.imu.empty()) {
            _ec->ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
            ApError("zmq_notification") << "read project config " + full_path + " failed";
            return;
        }
        ApError("zmq_notification") << full_path << "=" << g_module_name_cfg.imu;

        _ec->ec_remove(Ec409::_ERRORCODE_CONFIG);

        ApError("zmq_notification") << "INS Fault Config Loaded into Map. Size: " << g_ins_fault_map.size();

        // 初始化阶段加载不可通行故障码列表（用于 passable_rate 判定）
        load_unpassable_fault_codes_from_config(ptr, prefix, g_unpassable_fault_codes);
        // 初始化阶段加载地形图配置（用于 get_auto2manager_terrain_info）
        g_terrain_map_cfg = load_terrain_map_config_from_config(ptr, prefix, g_terrain_map_cfg);

        _ec->ec_remove(Ec409::_ERRORCODE_CONFIG);
        InitSoftwareVersion();

        if (!g_loc_handler) {
            try {
                auto zmq_loc_wrapper = std::make_shared<ZmqConstruct>(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto publier_loc = zmq_loc_wrapper->createPublisher(sub_loc_endpoint, true);
                g_loc_handler = std::make_shared<XMessageSend>(zmq_loc_wrapper, publier_loc);
                _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);
            } catch (const std::exception& e) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, sub_loc_endpoint);
                ApError("zmq_notification")
                    << "create publisher failed, endpoint: " << sub_loc_endpoint << ", error: " << e.what();
                return;
            } catch (...) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, sub_loc_endpoint);
                ApError("zmq_notification")
                    << "create publisher failed, endpoint: " << sub_loc_endpoint << ", error: unknown";
                return;
            }
        }

        if (!g_handler) {
            try {
                auto zmq_wrapper = std::make_shared<ZmqConstruct>(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto publier = zmq_wrapper->createPublisher(sub_endpoint, true);
                g_handler = std::make_shared<XMessageSend>(zmq_wrapper, publier);
                _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);
            } catch (const std::exception& e) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, sub_endpoint);
                ApError("zmq_notification")
                    << "zmq create publisher failed, endpoint: " << sub_endpoint << ", error: " << e.what();
                return;
            } catch (...) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, sub_endpoint);
                ApError("zmq_notification")
                    << "zmq create publisher failed, endpoint: " << sub_endpoint << ", error: unknown";
                return;
            }
        }

        if (!g_terrain_info_handler) {
            try {
                auto zmq_terrain_wrapper = std::make_shared<ZmqConstruct>(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto pub_terrain = zmq_terrain_wrapper->createPublisher(pub_terrain_endpoint, true);
                g_terrain_info_handler = std::make_shared<XMessageSend>(zmq_terrain_wrapper, pub_terrain);
                _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);
            } catch (const std::exception& e) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, pub_terrain_endpoint);
                ApError("zmq_notification")
                    << "zmq create publisher failed, endpoint: " << pub_terrain_endpoint << ", error: " << e.what();
                return;
            } catch (...) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, pub_terrain_endpoint);
                ApError("zmq_notification")
                    << "zmq create publisher failed, endpoint: " << pub_terrain_endpoint << ", error: unknown";
                return;
            }
        }
        if (!g_fault_upload_handler) {
            try {
                auto zmq_fault_wrapper = std::make_shared<ZmqConstruct>(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto pub_fault = zmq_fault_wrapper->createPublisher(pub_sensor_fault_endpoint, true);
                g_fault_upload_handler = std::make_shared<XMessageSend>(zmq_fault_wrapper, pub_fault);
                _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);
            } catch (const std::exception& e) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, pub_sensor_fault_endpoint);
                ApError("zmq_notification") << "zmq create publisher failed, endpoint: " << pub_sensor_fault_endpoint
                                            << ", error: " << e.what();
                return;
            } catch (...) {
                _ec->ec_add(Ec409::_ERRORCODE_ZMQ_LISTEN, pub_sensor_fault_endpoint);
                ApError("zmq_notification")
                    << "zmq create publisher failed, endpoint: " << pub_sensor_fault_endpoint << ", error: unknown";
                return;
            }
        }

        if (g_loc_handler && g_handler && g_terrain_info_handler && g_fault_upload_handler) {
            std::thread(Loc100HzPublishThread).detach();

            std::thread(TenHzPublishThread).detach();
        }
        _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_LISTEN);

        _init = true;
    }

    auto now = std::chrono::steady_clock::now();

    // 数据健康监控（毫秒级超时判定）
    {
        // 检查imu时间戳是否更新
        auto tmp_time = imu.header.timestamp.sec + imu.header.timestamp.nsec / 1e9;
        if (tmp_time > 0 && tmp_time != _last_imu_time) {
            _last_imu_time = tmp_time;
            _last_imu_update = now;
        }

        // 检查location时间戳是否更新
        auto loc_time = location.header.timestamp.sec + location.header.timestamp.nsec / 1e9;
        if (loc_time > 0 && loc_time != _last_loc_time) {
            _last_loc_time = loc_time;
            _last_loc_update = now;
        }

        // 计算时间差（毫秒）
        auto imu_dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_imu_update).count();
        auto loc_dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_loc_update).count();

        // 加锁更新所有共享数据
        {
            std::lock_guard<std::mutex> lock(g_data_mutex);
            g_latest_imu = imu;
            g_latest_location = location;
            g_latest_semantic_map = semantic_map;
            g_latest_fault_data = fault_data;
            g_imu_exception = (imu_dt > DATA_TIMEOUT_MS);
            g_loc_exception = (loc_dt > DATA_TIMEOUT_MS);

            g_latest_detected_objects = detected_objects;
            g_latest_target_obj_out = target_obj_out;
            g_latest_task_manage_status = task_manage_status;
        }
    }

    // 1HZ数据
    if (now - last_1hz >= std::chrono::seconds(1)) {
        last_1hz = now;
        bool one_hz_success = true;
        try {
            // 健康状态数据
            protocol_common::auto2manager_vehicle_status msg =
                get_auto2manager_vehicle_status(vehicle_information, fault_data, business_command, remote_control);
            {
                std::lock_guard<std::mutex> lg(g_mutex_handler);
                g_handler->sendHealthStatus(msg);
            }

            // 行动状态数据
            protocol_common::auto2manager_task_status task_status;
            task_status.header.timestamp = pack_protocol_time();
            task_status.header.source_systemid = 2;
            task_status.header.target_systemid = 1;

            task_status.task_type = task_manage_status.task_type;
            task_status.action_id = task_manage_status.action_id;
            task_status.task_status = task_manage_status.task_status;
            task_status.progress = static_cast<uint8_t>(task_manage_status.task_progress);
            {
                std::lock_guard<std::mutex> lg(g_mutex_handler);
                g_handler->sendTaskStatus(task_status);
            }

            // 视频状态信息数据
            protocol_common::auto2manager_video_status video_status =
                get_auto2manager_video_status(video_profile, encoding_perf);
            {
                std::lock_guard<std::mutex> lg(g_mutex_handler);
                g_handler->sendVideoStatus(video_status);
            }

            // 传感器故障状态信息数据
            protocol_common::auto2infosec_fault_upload fault_msg =
                get_auto2infosec_fault_upload(fault_data, g_module_name_cfg);
            g_fault_upload_handler->sendSensorFault(fault_msg);
        } catch (const std::exception& e) {
            one_hz_success = false;
            _ec->ec_add(Ec409::_ERRORCODE_ZMQ_SEND, "1 HZ send data error");
            ApError("zmq_notification") << "zmq send data failed, error: " << e.what();
        }
        if (one_hz_success) {
            _ec->ec_remove(Ec409::_ERRORCODE_ZMQ_SEND);
        }
    }
}

static protocol_common::auto2manager_vehicle_status get_auto2manager_vehicle_status(
    const ara::adsfi::VehicleInformation& vehicle_information, const ara::adsfi::FaultData& fault_data,
    const ara::adsfi::BusinessCommand& business_command, const ara::adsfi::AppRemoteControl& remote_control) {
    protocol_common::auto2manager_vehicle_status msg;
    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 2;
    msg.header.target_systemid = 1;
    msg.software_version = g_software_version;
    auto task_avoid_mode = business_command.param.task_avoid_mode;
    if (task_avoid_mode == 0) {
        msg.degrade_mode = 2;
    } else if (task_avoid_mode == 3) {
        msg.degrade_mode = 1;
    } else if (task_avoid_mode == 1) {
        msg.degrade_mode = 0;
    }
    auto business_mode = business_command.business_mode;
    if (business_mode == 2 || business_mode == 31 || business_mode == 11) {
        msg.auto_mobility_strategy = 1;
    } else {
        msg.auto_mobility_strategy = 0;
    }
    msg.perception_strategy = 2;
    ApError("zmq_notification") << "====== remotectrl_flag: " << remote_control.remotectrl_flag
                                << ", security_assist_enabled: " << remote_control.security_assist_enabled;
    if (1 == remote_control.remotectrl_flag && 0 == remote_control.security_assist_enabled) {
        msg.human_driving_mode = 2;
    } else if (1 == remote_control.remotectrl_flag && 1 == remote_control.security_assist_enabled) {
        msg.human_driving_mode = 3;
    } else {
        msg.human_driving_mode = 1;
    }
    msg.speed_limit = 0;
    msg.speed_limit = static_cast<uint16_t>(std::round(business_command.param.max_speed * 100.0 / 36.0));

    /* ---------- 故障信息赋值 ---------- */
    msg.fault_num = static_cast<uint32_t>(fault_data.fault_info.size());
    msg.fault_list.reserve(msg.fault_num);

    static const uint8_t HANDLE_TO_LEVEL[] = {
        3, // 0 -> 信息
        2, // 1 -> 警告
        2, // 2 -> 警告（限速刹停）
        1  // 3 -> 严重
    };

    for (const auto& fault_info : fault_data.fault_info) {
        protocol_common::auto2manager_faultdata fault_item{};

        /* 1. 故障码转换 */
        try {
            fault_item.fault_id = static_cast<uint32_t>(std::stoul(fault_info.code));
        } catch (const std::exception& e) {
            fault_item.fault_id = 0;
            ApError("zmq_notification") << "无法转换故障码: " << fault_info.code << ", 错误: " << e.what();
        }

        /* 2. 等级映射 + 越界保护 */
        if (fault_info.handle < std::size(HANDLE_TO_LEVEL)) {
            fault_item.fault_level = HANDLE_TO_LEVEL[fault_info.handle];
        } else {
            /* 未知 handle 一律当“信息”处理，避免协议非法值 */
            fault_item.fault_level = 3;
            ApError("zmq_notification") << "未知 handle 值: " << fault_info.handle << ", 故障码: " << fault_info.code;
        }

        msg.fault_list.push_back(fault_item);
    }

    return msg;
}

static protocol_common::auto2manager_video_status get_auto2manager_video_status(
    const ara::adsfi::AppVideoProfile& video_profile, const ara::adsfi::VideoEncodingPerf& encoding_perf) {
    protocol_common::auto2manager_video_status msg;
    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 2;
    msg.header.target_systemid = 1;
    // 分辨率状态，默认1-1080p
    if (1920 == video_profile.manual_width && 1080 == video_profile.manual_height) {
        msg.resolution = 1;
    } else if (1280 == video_profile.manual_width && 720 == video_profile.manual_height) {
        msg.resolution = 2;
    } else {
        msg.resolution = 1;
    }

    auto _bps = static_cast<int>(video_profile.manual_bps);
    switch (_bps) {
        case 256:
            msg.bps = 1;
            break;
        case 512:
            msg.bps = 2;
            break;
        case 1024:
            msg.bps = 3;
            break;
        case 2048:
            msg.bps = 4;
            break;
        default:
            break;
    }

    msg.fps = static_cast<uint8_t>(encoding_perf.fps + 0.5);
    msg.remote_assist_line = static_cast<uint8_t>(video_profile.remote_control_assist);
    msg.remote_warning_info = static_cast<uint8_t>(video_profile.remote_control_alarm);
    msg.auto_assist_line = static_cast<uint8_t>(video_profile.auto_control_assist);
    msg.drivable_area = static_cast<uint8_t>(video_profile.drivable_area);
    return msg;
}

static protocol_common::auto2manager_terrain_info get_auto2manager_terrain_info(
    const ara::adsfi::MsgHafLocation& latest_imu, const ara::adsfi::PerceptionStaticEnv& semantic_map,
    const ara::adsfi::MsgHafFusionOutArray& detected_objects) {
    const auto& cfg = g_terrain_map_cfg;

    protocol_common::auto2manager_terrain_info msg;
    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 2;
    msg.header.target_systemid = 1;
    msg.height = cfg.height;
    msg.width = cfg.width;
    msg.channel = cfg.channel;
    msg.resolution = cfg.resolution_cm_per_pixel; // 单位：cm/pixel（20 => 0.2m/pixel）

    const int    width = cfg.width;
    const int    height = cfg.height;
    const size_t total = static_cast<size_t>(width) * static_cast<size_t>(height);
    const float  scale = static_cast<float>(cfg.resolution_cm_per_pixel) / 100.0f;
    const float  center_u = static_cast<float>(width) / 2.0f;
    const float  center_v = static_cast<float>(height) / 2.0f;

    // 只画第三通道（障碍物通道）
    std::vector<uint8_t> ch_obstacle(total, 0);

    // ====================== 1. 使用传进来的融合定位数据 ======================
    const double imu_time = latest_imu.header.timestamp.sec + latest_imu.header.timestamp.nsec / 1e9;
    ApInfo("zmq_notification") << "Terrain map: imu time = " << imu_time;
    const bool has_imu = (imu_time > 0.0);

    double car_lon = 0.0, car_lat = 0.0, car_heading_rad = 0.0;
    if (has_imu) {
        car_lon = latest_imu.llh.x; // 经度
        car_lat = latest_imu.llh.y; // 纬度

        double yaw_deg = 90.0 - latest_imu.pose.pose.orientation.z;
        if (yaw_deg < 0.0)
            yaw_deg += 360.0;
        car_heading_rad = yaw_deg * M_PI / 180.0;
    } else {
        ApError("zmq_notification") << "Terrain map: no valid fusion imu, only dynamic objects will be drawn";
    }

    // ====================== 2. 人车不可被覆盖的安全绘制 ======================
    auto SafeDrawPixel = [&ch_obstacle, &cfg](size_t idx, uint8_t type) {
        if (idx >= ch_obstacle.size()) {
            ApError("zmq_notification") << "Terrain map: index out of range, idx = " << idx
                                        << ", size = " << ch_obstacle.size();
            return;
        }
        uint8_t& p = ch_obstacle[idx];
        if (cfg.protect_type_mask[p] != 0) {
            return; // 人(3)、车(4) 一旦画上，永不覆盖
        }
        p = type;
    };

    // ====================== 3. 旋转矩形绘制函数 ======================
    auto DrawRotatedRect = [&](float vcs_x, float vcs_y, float len, float wid, float heading_deg, uint8_t type) {
        if (type == 0) {
            return;
        }

        const float grid_u = center_u - vcs_y / scale; // Y左正 → u左大
        const float grid_v = center_v - vcs_x / scale; // X前正 → v上大

        const float rad = heading_deg * static_cast<float>(M_PI) / 180.0f;
        const float c = std::cos(rad), s = std::sin(rad);
        const float hl = len / scale / 2.0f;
        const float hw = wid / scale / 2.0f;

        struct Pt {
            float u, v;
        } corner[4] = {{grid_u + hl * c + hw * s, grid_v + hl * s - hw * c},
                       {grid_u + hl * c - hw * s, grid_v + hl * s + hw * c},
                       {grid_u - hl * c - hw * s, grid_v - hl * s + hw * c},
                       {grid_u - hl * c + hw * s, grid_v - hl * s - hw * c}};

        float min_u = corner[0].u, max_u = corner[0].u;
        float min_v = corner[0].v, max_v = corner[0].v;
        for (int i = 1; i < 4; ++i) {
            min_u = std::min(min_u, corner[i].u);
            max_u = std::max(max_u, corner[i].u);
            min_v = std::min(min_v, corner[i].v);
            max_v = std::max(max_v, corner[i].v);
        }

        const int u0 = std::max(0, static_cast<int>(std::floor(min_u)));
        const int u1 = std::min(width - 1, static_cast<int>(std::ceil(max_u)));
        const int v0 = std::max(0, static_cast<int>(std::floor(min_v)));
        const int v1 = std::min(height - 1, static_cast<int>(std::ceil(max_v)));

        for (int v = v0; v <= v1; ++v) {
            for (int u = u0; u <= u1; ++u) {
                if (isPointInRotatedRectangle(static_cast<float>(u), static_cast<float>(v), corner[0].u, corner[0].v,
                                              corner[1].u, corner[1].v, corner[2].u, corner[2].v, corner[3].u,
                                              corner[3].v)) {
                    const size_t idx = static_cast<size_t>(v) * static_cast<size_t>(width) + static_cast<size_t>(u);
                    SafeDrawPixel(idx, type);
                }
            }
        }
    };

    // ====================== 4. 先画动态目标（新架构：MsgHafFusionOutArray） ======================
    static double last_detected_objects_timestamp = 0;
    double        current_time = 0.0;
    const double  detected_time = detected_objects.stamp.sec + detected_objects.stamp.nsec / 1e9;

    if (detected_time != last_detected_objects_timestamp && detected_time > 0) {
        last_detected_objects_timestamp = detected_time;
        // 注意：MsgHafFusionOutArray 使用 stamp 字段
        current_time = detected_objects.stamp.sec + detected_objects.stamp.nsec / 1e9;

        for (const auto& obj : detected_objects.fusionOut) {
            uint8_t       type = 0;
            const uint8_t inner_type = static_cast<uint8_t>(obj.fusionType);
            if (inner_type < cfg.dynamic_type_map.size()) {
                type = cfg.dynamic_type_map[inner_type];
            }

            // 新架构：使用 rect.center (VCS坐标) 和 rect.size
            DrawRotatedRect(obj.rect.center.x, obj.rect.center.y, obj.rect.size.x, obj.rect.size.y,
                            obj.absRect.orientation, type);
        }
    }

    // ====================== 5. 再画静态语义障碍物（经纬度 → 相对坐标） ======================
    if (has_imu && current_time > 0.0) {
        static double last_semantic_map_timestamp = 0;
        const double  semantic_time = app_common::DatetimeUtil::double_time_from_sec_nsec(semantic_map.header.time);

        // 时间对齐检查：语义地图时间戳应该在动态目标时间戳的容差范围内
        constexpr double TIME_TOLERANCE_SEC = 1.0;
        const double     time_diff = std::abs(semantic_time - current_time);
        const bool       time_aligned = (time_diff <= TIME_TOLERANCE_SEC);

        // 如果时间不对齐，打印警告信息
        if (semantic_time > 0 && !time_aligned) {
            ApError("zmq_notification") << "Terrain map: semantic map time not aligned, semantic_time=" << semantic_time
                                        << ", current_time=" << current_time << ", time_diff=" << time_diff
                                        << " (tolerance=" << TIME_TOLERANCE_SEC << "s)";
        }

        if (semantic_time != last_semantic_map_timestamp && semantic_time > 0 && time_aligned) {
            last_semantic_map_timestamp = semantic_time;
            for (const auto& sem : semantic_map.semantic_objs) {
                uint8_t type = 0;
                if (cfg.semantic_positive_types.count(sem.type) > 0) {
                    type = 1; // 正障碍
                } else if (cfg.semantic_negative_types.count(sem.type) > 0) {
                    type = 2; // 负障碍
                } else {
                    continue;
                }

                /**
                 * 业务模式business_mode:
                 * 0/11/31: DR坐标系
                 * 1/3/4: UTM
                 */
                if (app_common::StringUtils::ends_with(semantic_map.header.data_name, "out_to_utm")) {
                    // UTM坐标系，需要转换到VCS
                    GeoPointGCCS car_gccs;
                    GeoPointGCCS obj_gccs{};
                    GeoPointVCS  obj_vcs{};
                    // 新架构：dr_pose 是 DR递推坐标
                    car_gccs.xg = latest_imu.dr_pose.pose.position.x;
                    car_gccs.yg = latest_imu.dr_pose.pose.position.y;
                    car_gccs.angle = latest_imu.dr_pose.pose.orientation.z;
                    obj_gccs.xg = sem.x;
                    obj_gccs.yg = sem.y;
                    obj_gccs.angle = sem.heading;
                    GeometryTool::GCCS2VCS(car_gccs, obj_gccs, obj_vcs);
                    const float max_abs_x = center_v * scale;
                    const float max_abs_y = center_u * scale;
                    if (std::abs(obj_vcs.x) <= max_abs_x && std::abs(obj_vcs.y) <= max_abs_y) {
                        DrawRotatedRect(obj_vcs.x, obj_vcs.y, sem.obj_size.length, sem.obj_size.width, sem.heading,
                                        type);
                    }
                } else {
                    // 已经是VCS坐标
                    DrawRotatedRect(sem.x, sem.y, sem.obj_size.length, sem.obj_size.width, sem.heading, type);
                }
            }
        }
    }

    // ====================== 6. 打包，只填第三通道 ======================
    msg.pixel_data.assign(total * static_cast<size_t>(cfg.channel), 0); // 仅障碍物通道填值，其余通道填0
    if (cfg.obstacle_channel_index >= 0 && cfg.obstacle_channel_index < cfg.channel) {
        std::memcpy(msg.pixel_data.data() + static_cast<size_t>(cfg.obstacle_channel_index) * total, ch_obstacle.data(),
                    total);
    }

    return msg;
}

// 感知目标发布信息
static protocol_common::auto2manager_perception_target_publish get_auto2manager_perception_target_publish(
    const ara::adsfi::MsgHafFusionOutArray& detected_objects, const ara::adsfi::MsgHafGnssInfo& location,
    const ara::adsfi::MsgHafFusionOutArray& target_obj_out) {
    protocol_common::auto2manager_perception_target_publish msg;
    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 2;
    msg.header.target_systemid = 1;
    // 目标个数
    msg.target_num = static_cast<uint32_t>(detected_objects.fusionOut.size());
    // TODO: 不限制大小吗？？？？
    msg.target_attributes.resize(msg.target_num);
    for (size_t i = 0; i < detected_objects.fusionOut.size(); i++) {
        const auto& obj = detected_objects.fusionOut[i];

        // 目标ID
        msg.target_attributes[i].target_id = static_cast<uint32_t>(obj.objectID);

        // 类型映射：cs字段到协议类型
        uint8_t protocol_type;
        if (obj.fusionType == 0 || obj.fusionType == 1) { // 算法：人
            protocol_type = 2;                            // 协议：人员
        } else if (obj.fusionType == 2 || obj.fusionType == 3 || obj.fusionType == 4 ||
                   obj.fusionType == 6) { // 算法：车
            protocol_type = 1;            // 协议：车辆
        } else {
            protocol_type = 0;
        }

        msg.target_attributes[i].target_type = protocol_type;
        msg.target_attributes[i].confidence = static_cast<uint8_t>(obj.confidence * 100); // 置信度 [0-1] -> [0-100]

        // 绝对坐标转GPS
        GeoPointGCCS gccs{};
        GeoPointGPS  gps{};
        gccs.xg = obj.absRect.center.x;
        gccs.yg = obj.absRect.center.y;
        GeometryTool::GCCS2GPS(gccs, gps, GeometryTool::GetLongZone(location.longitude));
        msg.target_attributes[i].geopoint.longitude = static_cast<int32_t>(gps.lon * 1e6);
        msg.target_attributes[i].geopoint.latitude = static_cast<int32_t>(gps.lat * 1e6);
        msg.target_attributes[i].geopoint.altitude = 0; // 高度， 默认0

        // 速度计算
        float rel_vx = obj.velocity.x;
        float rel_vy = obj.velocity.y;
        float speed_val = std::hypot(rel_vx, rel_vy);
        msg.target_attributes[i].speed = static_cast<uint16_t>(speed_val * 10.0f);

        // 航向角处理
        double heading = 90.0 - obj.absRect.orientation;
        if (heading < 0.0) {
            heading += 360.0;
        }
        msg.target_attributes[i].heading = static_cast<uint16_t>(heading * 100); // 航向角

        // 相对位置和速度（VCS坐标系）
        msg.target_attributes[i].relative_x = static_cast<int16_t>(obj.rect.center.x * 10);
        msg.target_attributes[i].relative_y = static_cast<int16_t>(obj.rect.center.y * 10);
        msg.target_attributes[i].relative_z = static_cast<int16_t>(obj.rect.center.z * 10);
        msg.target_attributes[i].relative_speed_x = static_cast<int16_t>(obj.velocity.x * 10);
        msg.target_attributes[i].relative_speed_y = static_cast<int16_t>(obj.velocity.y * 10);

        // 绝对位置和速度（GCCS坐标系）
        msg.target_attributes[i].absolute_x = static_cast<int32_t>(obj.absRect.center.x * 10);
        msg.target_attributes[i].absolute_y = static_cast<int32_t>(obj.absRect.center.y * 10);
        msg.target_attributes[i].absolute_speed_x = static_cast<int16_t>(obj.absVelocity.x * 10);
        msg.target_attributes[i].absolute_speed_y = static_cast<int16_t>(obj.absVelocity.y * 10);

        // 尺寸信息
        msg.target_attributes[i].length = static_cast<uint16_t>(obj.rect.size.x * 10);
        msg.target_attributes[i].width = static_cast<uint16_t>(obj.rect.size.y * 10);
        msg.target_attributes[i].height = static_cast<uint16_t>(obj.rect.size.z * 10);
    }
    return msg;
}

static bool has_road_blocked_fault(const ara::adsfi::FaultData& fault_data) {
    // 依据配置的“不可通行故障码列表”判断：
    // - 故障码命中 => 认为道路受阻（不可通行）
    // - 列表为空（未配置/读取失败/配置为空）=> 不进行道路阻断判断，恒返回 false
    for (const auto& fault_info : fault_data.fault_info) {
        if (g_unpassable_fault_codes.count(fault_info.code) > 0) {
            return true;
        }
    }
    return false;
}

static protocol_common::auto2manager_offset_publish get_auto2manager_offset_release(
    const ara::adsfi::TaskManageStatus& status, const ara::adsfi::FaultData& fault_data) {
    protocol_common::auto2manager_offset_publish msg;
    static constexpr uint8_t                     PASSABLE_RATE_BLOCKED = 0; // 道路受阻标志
    static constexpr uint8_t                     PASSABLE_RATE_CLEAR = 100; // 道路畅通标志

    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 2;
    msg.header.target_systemid = 1;
    msg.offset = static_cast<uint8_t>(status.distance_to_nearest * 1e1);
    msg.passable_rate = has_road_blocked_fault(fault_data) ? PASSABLE_RATE_BLOCKED : PASSABLE_RATE_CLEAR;
    return msg;
}

static protocol_common::auto2infosec_fault_upload get_auto2infosec_fault_upload(const ara::adsfi::FaultData& fault,
                                                                                const ModuleNameConfig&      config) {
    protocol_common::auto2infosec_fault_upload msg{};

    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 2;
    msg.header.target_systemid = 1;

    if (fault.fault_info.empty()) {
        return msg;
    }

    // 获取当前前后夜视相机状态 0:主 即前    1:从,即后, 默认为0
    // TODO: 这个参数后续可以通过配置项来指定，增加灵活性
    // int32_t nv_camera_pos = 0;
    // SHMParamApi::Instance()->QueryIntModuleParam("mdc_camera_nv_camera_pos", nv_camera_pos);
    // ApError("zmq_notification") << "mdc_camera_nv_camera_pos: " << nv_camera_pos;

    // 故障上报映射表，key 为设备名称，value 为上报字段的指针
    std::unordered_map<std::string, uint8_t*> fault_map = {
        {config.left_front_lidar, &msg.left_front_lidar},   // 左前激光雷达
        {config.front_lidar, &msg.front_lidar},             // 正前激光雷达
        {config.right_front_lidar, &msg.right_front_lidar}, // 右前激光雷达

        {config.left_front_radar, &msg.left_front_mmw_radar},   // 左前毫米波雷达
        {config.front_radar, &msg.front_mmw_radar},             // 正前毫米波雷达
        {config.right_front_radar, &msg.right_front_mmw_radar}, // 右前毫米波雷达
        {config.imu, &msg.imu}};

    // 故障上报映射表，camera_name -> fault flag, 前后夜视共用一个camera_name
    std::unordered_map<std::string, uint8_t*> camera_fault_map = {
        {config.front_middle_60, &msg.front_center_tele_camera_60},
        {config.front_middle_around_120, &msg.front_center_wide_camera_120},
        {config.left_front_around_100, &msg.front_left_wide_camera_100},
        {config.right_front_around_100, &msg.front_right_wide_camera_100},
        {config.left_front_60, &msg.left_front_camera_60},
        {config.left_back_100, &msg.left_rear_camera_100},
        {config.right_front_60, &msg.right_front_camera_60},
        {config.right_back_100, &msg.right_rear_camera_100},
        {config.back_middle_around_120, &msg.rear_center_wide_camera_120},
        {config.front_fisheye_200, &msg.front_fisheye_camera_200},
        {config.left_fisheye_200, &msg.left_fisheye_camera_200},
        {config.right_fisheye_200, &msg.right_fisheye_camera_200},
        {config.back_fisheye_200, &msg.rear_fisheye_camera_200},
        {config.front, &msg.front_lowlight_camera}};

    std::unordered_map<std::string, std::vector<std::string>> camera_faultcode_map = {
        {config.front_middle_60, {"1042001", "1042002", "1042003", "1042004", "1042005", "1042006"}},
        {config.front_middle_around_120, {"1042131", "1042132", "1042133", "1042134", "1042135", "1042136"}},
        {config.left_front_around_100, {"1042101", "1042102", "1042103", "1042104", "1042105", "1042106"}},
        {config.right_front_around_100, {"1042091", "1042092", "1042093", "1042094", "1042095", "1042096"}},
        {config.left_front_60, {"1042021", "1042022", "1042023", "1042024", "1042025", "1042026"}},
        {config.left_back_100, {"1042111", "1042112", "1042113", "1042114", "1042115", "1042116"}},
        {config.right_front_60, {"1042011", "1042012", "1042013", "1042014", "1042015", "1042016"}},
        {config.right_back_100, {"1042081", "1042082", "1042083", "1042084", "1042085", "1042086"}},
        {config.back_middle_around_120, {"1042121", "1042122", "1042123", "1042124", "1042125", "1042126"}},
        {config.front_fisheye_200, {"1042041", "1042042", "1042043", "1042044", "1042045", "1042046"}},
        {config.left_fisheye_200, {"1042071", "1042072", "1042073", "1042074", "1042075", "1042076"}},
        {config.right_fisheye_200, {"1042051", "1042052", "1042053", "1042054", "1042055", "1042056"}},
        {config.back_fisheye_200, {"1042061", "1042062", "1042063", "1042064", "1042065", "1042066"}},
        {config.front, {"1042031", "1042032", "1042033", "1042034", "1042035", "1042036"}}};

    // 遍历所有上报的故障
    for (const auto& info : fault.fault_info) {
        if (info.from.empty()) {
            continue;
        }

        auto it = fault_map.find(info.from);
        if (it != fault_map.end()) {
            *(it->second) = 1; // 有故障，置 1
        }
    }

    // 处理 camera 故障状态，fault_info 额外描述带有 camera_name
    for (const auto& info : fault.fault_info) {
        // 故障模块名为 mdc 相机驱动模块名称
        if (config.mdc_cameralist != info.from || info.code.empty()) {
            continue;
        }

        // 根据故障码筛出具体故障相机
        for (const auto& it : camera_fault_map) {
            auto iter = camera_faultcode_map.find(it.first);
            if (iter == camera_faultcode_map.end()) {
                continue; // 修复：原来用 break 会中断整个循环，应该用 continue 跳过当前项
            }
            // faultcode vector 中查找相机故障码
            if (std::find(iter->second.begin(), iter->second.end(), info.code) != iter->second.end()) {
                // 前微光相机故障 → 设置 front_lowlight_camera（camera_fault_map 已正确映射）
                // 直接使用映射表中的指针即可，无需特殊处理
                *(it.second) = 1; // 有故障，置 1
            }
        }
    }

    return msg;
}

static protocol_common::loc2manager_status get_loc2manager_status(const ara::adsfi::FaultData& fault_data) {
    protocol_common::loc2manager_status msg{};

    msg.header.timestamp = pack_protocol_time();
    msg.header.source_systemid = 4;
    msg.header.target_systemid = 1;
    msg.loc_software_version = g_software_version;
    ApError("zmq_notification") << "---version: " << g_software_version;
    msg.error_num = static_cast<uint32_t>(fault_data.fault_info.size());
    msg.fault_list.reserve(msg.error_num);

    static const uint8_t HANDLE_TO_LEVEL[] = {
        3, // 0 -> 信息
        2, // 1 -> 警告
        2, // 2 -> 警告（限速刹停）
        1  // 3 -> 严重
    };

    for (const auto& fault_info : fault_data.fault_info) {
        protocol_common::loc2manager_fault_data fault_item{};

        /* 1. 故障码转换 */
        try {
            fault_item.fault_code = static_cast<uint32_t>(std::stoul(fault_info.code));
        } catch (const std::exception& e) {
            fault_item.fault_code = 0;
            ApError("zmq_notification") << "无法转换故障码: " << fault_info.code << ", 错误: " << e.what();
        }

        /* 2. 等级映射 + 越界保护 */
        if (fault_info.handle < std::size(HANDLE_TO_LEVEL)) {
            fault_item.fault_level = HANDLE_TO_LEVEL[fault_info.handle];
        } else {
            /* 未知 handle 一律当“信息”处理，避免协议非法值 */
            fault_item.fault_level = 3;
            ApError("zmq_notification") << "未知 handle 值: " << fault_info.handle << ", 故障码: " << fault_info.code;
        }

        msg.fault_list.push_back(fault_item);
    }
    return msg;
}
