#ifndef _CGI230_UDP_PROTOCOL_HPP_
#define _CGI230_UDP_PROTOCOL_HPP_

#include <cstdint>

struct HCHeader {
    std::uint8_t  sync[4];     // 0xAA, 0xCC, 'H', 'C'
    std::uint16_t length;      // 消息实际长度，不包含头和校验
    std::uint16_t msgid;       // 消息号
    std::uint16_t gps_week;    // GPS周
    std::uint32_t gps_second;  // GPS周内秒
    std::uint32_t sn;          // 设备序列号（0~999999999）
    std::uint8_t  reserver[4]; // 预留
} __attribute__((packed));

static_assert(sizeof(HCHeader) == 22, "HCHeader size error");

struct HCRAWGNSSPVATB {
    HCHeader header;

    double        lat;          // 纬度，deg
    double        lon;          // 经度，deg
    float         alt;          // 大地高，m
    float         undulation;   // 高程异常，m
    float         ve;           // 东向速度，m/s
    float         vn;           // 北向速度，m/s
    float         vu;           // 天向速度，m/s
    float         std_lat;      // lat位置标准差，m
    float         std_lon;      // lon位置标准差，m
    float         std_alt;      // alt位置标准差，m
    float         std_ve;       // 东向速度标准差，m/s
    float         std_vn;       // 北向速度标准差，m/s
    float         std_vu;       // 天向速度标准差，m/s
    std::uint8_t  pos_type;     // 主天线定位状态，Novatel格式
    std::uint16_t nsused;       // 主天线使用卫星颗数
    std::uint16_t ns;           // 主天线可见卫星颗数
    std::uint16_t leaps;        // 闰秒，s
    float         age;          // 差分龄期，s
    float         hdop;         // 水平精度因子
    float         pdop;         // 位置精度因子
    float         vdop;         // 垂直精度因子
    float         tdop;         // 钟差精度因子
    float         gdop;         // 几何精度因子
    float         speed;        // 地面速度，m/s
    float         heading;      // 航迹角，deg
    std::uint8_t  pos_type2;    // 辅天线定位状态，Novatel格式
    std::uint16_t nsused2;      // 辅天线使用卫星颗数
    std::uint16_t ns2;          // 辅天线可见卫星颗数
    float         baseline;     // 基线长度，m
    float         heading2;     // 双天线航向，deg
    float         pitch2;       // 双天线俯仰角，deg
    float         std_heading2; // 双天线航向标准差，deg
    float         std_pitch2;   // 双天线俯仰角标准差，deg
    float         std_heading;  // 航迹角标准差，deg
    std::uint8_t  Reserved[12]; // 预留
    std::uint32_t CRC32;        // CRC32校验位
} __attribute__((packed));

static_assert(sizeof(HCRAWGNSSPVATB) == 166, "HCRAWGNSSPVATB size error");

struct HCRAWIMUVB {
    HCHeader header;

    std::uint16_t week;   // IMU采样时间-GPS周
    double        second; // IMU采样时间-GPS周内秒
    float         gyro_x; // 车辆x轴角速率，deg/s
    float         gyro_y; // 车辆y轴角速率，deg/s
    float         gyro_z; // 车辆z轴角速率，deg/s
    float         acc_x;  // 车辆x轴角加速度，g
    float         acc_y;  // 车辆y轴角加速度，g
    float         acc_z;  // 车辆z轴角加速度，g
    float         temp;   // IMU温度，℃
    struct {
        std::uint8_t status_gyro : 1;
        std::uint8_t status_gyro_x : 1;
        std::uint8_t status_gyro_y : 1;
        std::uint8_t status_gyro_z : 1;
        std::uint8_t status_acc : 1;
        std::uint8_t status_acc_x : 1;
        std::uint8_t status_acc_y : 1;
        std::uint8_t status_acc_z : 1;
    } ErrStatus;            // 异常状态标志
    std::int16_t  yaw;      // Z轴陀螺积分航向，-180~180°，系数0.01
    std::uint8_t  Reserved; // 预留
    std::uint32_t CRC32;    // CRC32校验位
} __attribute__((packed));

static_assert(sizeof(HCRAWIMUVB) == 68, "HCRAWIMUVB size error");

struct HCINSPVATB {
    HCHeader header;

    std::uint16_t week;   // GPS周
    double        second; // GPS周内秒

    double lat;        // 纬度，deg
    double lon;        // 经度，deg
    float  alt;        // 海拔高，m
    float  undulation; // 高程异常，m
    float  ve;         // 东向速度，m/s
    float  vn;         // 北向速度，m/s
    float  vu;         // 天向速度，m/s
    float  pitch;      // 俯仰角，deg
    float  roll;       // 横滚角，deg
    float  yaw;        // 航向角，deg

    float std_lat;   // lat位置标准差，m
    float std_lon;   // lon位置标准差，m
    float std_alt;   // alt位置标准差，m
    float std_ve;    // 东向速度标准差，m/s
    float std_vn;    // 北向速度标准差，m/s
    float std_vu;    // 天向速度标准差，m/s
    float std_pitch; // 俯仰角标准差，deg
    float std_roll;  // 横滚角标准差，deg
    float std_yaw;   // 航向角标准差，deg

    float vgyro_x;   // 车辆x轴角速率，deg/s
    float vgyro_y;   // 车辆y轴角速率，deg/s
    float vgyro_z;   // 车辆z轴角速率，deg/s
    float vacccar_x; // 车辆x轴加速度，m/s^2
    float vacccar_y; // 车辆y轴加速度，m/s^2
    float vacccar_z; // 车辆z轴加速度，m/s^2

    float speed;    // 地面速度，m/s
    float heading;  // 航迹角，deg
    float heading2; // 航向角（可能用于不同目的），deg

    struct {
        std::uint16_t state_ins : 4;  // 组合状态
        std::uint16_t state_gnss : 4; // GNSS状态
        std::uint16_t reserved : 8;
    } stat;                    // 状态标志
    float         age;         // 差分龄期，s
    std::uint16_t ns;          // 主天线可见卫星颗数
    std::uint16_t ns2;         // 辅天线可见卫星颗数
    std::uint16_t leaps;       // 闰秒，s
    float         hdop;        // 水平精度因子
    std::uint16_t warning;     // 异常标志
    std::uint16_t sensor_used; // 传感器使用标志
    float         vacc_x;      // 车辆x轴加速度，g
    float         vacc_y;      // 车辆y轴加速度，g
    float         vacc_z;      // 车辆z轴加速度，g
    std::uint8_t  TimeValid;   // 时间可用标志（0：无效，1：有效）
    std::uint8_t  Reserved[3]; // 预留字段
    std::uint32_t CRC32;       // CRC32校验位
} __attribute__((packed));

static_assert(sizeof(HCINSPVATB) == 192, "HCINSPVATB size error");

#endif
