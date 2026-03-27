#pragma once
#include <cstdio>
#include <cstring>
#include <nlohmann/json.hpp>
#include <vector>

#include "Protocol.h"
#include "common.h"

namespace protocol_common {

    // 经纬高结构体（经度、纬度、高度）
    struct __attribute__(()) manager2map_geo_point {
        // 经度，精确到小数点后6位，-180e6~180e6，181000000为无效值
        int32_t longitude = 181000000;

        // 纬度，精确到小数点后6位，-90e6~90*e6，91000000为无效值
        int32_t latitude = 181000000;

        // 高程，精确到小数点后1位，-1000e1~30000e1，999999为无效值
        int32_t altitude = 999999;

        //航向
        int32_t heading = 0;
    };

    struct __attribute__(()) manager2map_task_point {
        //   补充
        // 1-必经点（位置必达），2-必经点（位置、航向必达），3-途经点， 4-禁行点
        uint8_t point_type = 0;
        // 经纬高
        manager2map_geo_point geo_point;
        // 预留位
        uint32_t reserved1 = 0;
        uint32_t reserved2 = 0;
        uint32_t reserved3 = 0;
    };

    // 经纬高结构体（经度、纬度、高度）
    struct __attribute__(()) manager2auto_geo_point {
        // 经度，精确到小数点后6位，-180e6~180e6，181000000为无效值
        int32_t longitude = 181000000;

        // 纬度，精确到小数点后6位，-90e6~90*e6，91000000为无效值
        int32_t latitude = 91000000;

        // 高程，精确到小数点后1位，-1000e1~30000e1，999999为无效值
        int32_t altitude = 999999;
    };

    // 姿态角结构体（航向角、俯仰角、倾斜角）
    struct __attribute__(()) manager2auto_pose {
        // 航向角，单位度，精度小数点后2位，0e2~360e2，36100为无效值
        uint16_t heading = 36100;

        // 俯仰角，单位度，精度小数点后2位，-90e2~90e2，9100为无效值
        int16_t pitch = 9100;

        // 倾斜角，单位度，精度小数点后2位，-90e2~90e2，9100为无效值
        int16_t roll = 9100;
    };

    // 编队配置参数结构体（IP、横向偏移量、纵向偏移量）
    struct __attribute__(()) manager2auto_formation_config {
        // 行动成员ip， 将ip str通过 inet_addr 转换为uint32_t
        uint32_t member_ip = 0;

        // 横向相对偏移量，单位米，面向行使方向，左为正，精度小数点后1位，1e1
        int16_t offset_x = 0;

        // 纵向相对偏移量，单位米，面向行使方向，前为正，精度小数点后1位，1e1
        int16_t offset_y = 0;
    };

    // 任务点参数结构体（坐标系、路点属性、经纬高、姿态角、接受半径误差范围、接受姿态误差范围、编队机动车辆数量、多车队形信息）
    struct manager2auto_task_point {
        // 坐标系
        // 0-WGS84,1-墨卡托，2-LocalNED，3-LocalENU，4-CGCS2000(默认)
        uint8_t coordinate_system = 4;

        // 路点属性
        // 1-必经点（位置必达），2-必经点（位置、航向必达），3-途经点， 4-禁行点
        uint8_t point_type = 0;

        // 经纬高
        manager2auto_geo_point geo_point;

        // 姿态角
        manager2auto_pose pose;

        // 可接受到达半径误差范围，精度小数点后1位，1e1
        uint16_t acceptance_radius = 0;

        // 可接受到达姿态误差范围
        manager2auto_pose acceptance_pose;

        // 参与编队数量
        uint32_t formation_member_num = 0;

        // 多车队形信息
        std::vector<manager2auto_formation_config> formation_config;

        // 预留位
        uint32_t reserved1 = 0;
        uint32_t reserved2 = 0;
        uint32_t reserved3 = 0;
    };

    // 轨迹点信息结构体（时间戳、经纬高、速度、曲率、航向）
    struct __attribute__(()) manager2auto_trajectory_point {
        // 时间戳，轨迹预测的时间,utc时间，精度至少到毫秒级
        uint64_t timestamp = 0;

        // 经纬高
        manager2auto_geo_point geo_point;

        // 速度，m/s,精度小数点后1位，1e1
        int16_t speed = 0;

        // 曲率，左正右负，精度小数点后3位，1e3
        int16_t curvature = 0;

        // 航向，单位度，精度小数点后2位，0e2~360e2，36100为无效值
        uint16_t heading = 36100;
    };

    // 编队轨迹消息结构体（车ip、轨迹点数量、轨迹点列表）
    struct manager2auto_formation_trajectory {
        // 车ip, 将ip str通过inet_addr转换为uint32_t
        uint32_t member_ip = 0;

        // 轨迹点数量
        uint32_t trajectory_points_num = 0;

        // 轨迹点列表
        std::vector<manager2auto_trajectory_point> trajectory_point;
    };

    // 编队定位数据结构体（行动成员ip、定位数据）
    struct __attribute__(()) manager2auto_formation_localization {
        // 车ip， 将ip str通过 inet_addr 转换为uint32_t
        uint32_t member_ip = 0;

        // 定位数据, 结构体内curvature无意义
        manager2auto_trajectory_point trajectory_points;
    };
    // 视频处理位置结构体（左上角x坐标、左上角y坐标、右下角x坐标、右下角y坐标）
    struct __attribute__(()) manager2auto_video_point {
        // 左上角x坐标，单位像素
        uint16_t top_left_x = 0;

        // 左上角y坐标，单位像素
        uint16_t top_left_y = 0;

        // 右下角x坐标，单位像素
        uint16_t bottom_right_x = 1920;

        // 右下角y坐标，单位像素
        uint16_t bottom_right_y = 1080;
    };

    // 视频控制结构体（视窗位置、视频通道、视频裁减位置）
    struct __attribute__(()) manager2auto_video_splicing {
        // 视窗位置：视频在操控屏幕的显示位置（默认1920*1080）
        manager2auto_video_point video_position;

        // 视频通道
        // 0：无相机，1：前主相机，2：左前后视相机，3：右前后视相机，4：左前前视相机，5：右前前视相机，6：后视相机，7：环视相机
        // 8：前视夜视相机，9：后视夜视相机，10：栅格地图(暂定), 11: 左后前视相机（60），12:
        // 右后前视相机（60），13：前远视相机（60）
        uint8_t video_channel = 0;

        // 视频裁减位置
        manager2auto_video_point video_crop_position;
    };

    // 故障码结构体
    struct __attribute__(()) manager2auto_fault_data {
        // 故障码,参见具体故障码定义
        uint32_t fault_code;
        // 故障等级,1-严重（停车）；2-警告（限速）；3-提示
        uint8_t fault_level;
    };

    // 感知目标结构体（目标ID、目标类型、置信度、经纬高、速度、航向角、相对位置X、相对位置Y、相对位置Z、相对速度VX、相对速度VY、绝对位置X、绝对位置Y、绝对速度VX、绝对速度VY、观测尺寸长度、观测尺寸宽度、观测尺寸高度）
    struct __attribute__(()) manager2auto_perception_target {
        // 目标ID
        uint32_t target_id = 0;

        // 目标类型
        // 1-车辆，2-人员
        uint8_t target_type = 0;

        // 置信度，1~100
        uint8_t confidence = 0;

        // 经纬高
        manager2auto_geo_point geopoint;

        // 速度，单位米/秒，精度小数点后1位，1e1
        uint16_t speed = 0;

        // 航向角，单位度，精度小数点后2位，0e2~360e2，36100为无效值
        uint16_t heading = 36100;

        // 相对位置X，以几何中心为原点，正前方向，单位米，精确到小数点后1位，1e1
        int16_t relative_x = 0;

        // 相对位置Y，以几何中心为原点，正左方向，单位米，精确到小数点后1位，1e1
        int16_t relative_y = 0;

        // 相对位置Z，以几何中心为原点，正上方向，单位米，精确到小数点后1位，1e1
        int16_t relative_z = 0;

        // 相对速度VX，以几何中心为原点，正前方向，单位米/秒，精度小数点后1位，1e1
        int16_t relative_speed_x = 0;

        // 相对速度VY，以几何中心为原点，正左方向，单位米/秒，精度小数点后1位，1e1
        int16_t relative_speed_y = 0;

        // 绝对位置X，以组合导航设备上电时车辆位置为原点，正右方向，单位米，精确到小数点后1位，1e1
        int32_t absolute_x = 0;

        // 绝对位置Y，以组合导航设备上时车辆电位置为原点，正前方向，单位米，精确到小数点后1位，1e1
        int32_t absolute_y = 0;

        // 绝对速度VX，以组合导航设备上电时车辆位置为原点，正右方向，单位米/秒，精度小数点后1位，1e1
        int16_t absolute_speed_x = 0;

        // 绝对速度VY，以组合导航设备上电时车辆位置为原点，正前方向，单位米/秒，精度小数点后1位，1e1
        int16_t absolute_speed_y = 0;

        // 观测尺寸长度，单位米，精确到小数点后1位，1e1
        uint16_t length = 0;

        // 观测尺寸宽度，单位米，精确到小数点后1位，1e1
        uint16_t width = 0;

        // 观测尺寸高度，单位米，精确到小数点后1位，1e1
        uint16_t height = 0;
    };

    // 行动配置响应服务（行动编号、行动模式、降级模式）
    // （uint32）返回值0表示成功，返回值1表示失败
    class __attribute__(()) manager2auto_action_config {
#pragma region 行动模式配置
    public:
        // 帧头
        InternalFrameHeader header;

        // 降级模式
        // 0-正常模式（默认），1-突击模式，2-停障模式
        uint8_t degrade_mode = 0;
#pragma endregion
    };

    // 机动策略响应服务
    class __attribute__(()) manager2auto_maneuvering_strategy {
#pragma region 机动策略
    public:
        // 帧头
        InternalFrameHeader header;

        // 自主机动策略 0-循迹机动(默认)；1-自主通行
        uint8_t maneuvering_strategy_type = 0;
#pragma endregion
    };

    // 感知策略响应服务
    class __attribute__(()) manager2auto_pereception_strategy {
#pragma region 感知策略
    public:
        // 帧头
        InternalFrameHeader header;

        // 感知策略
        // 1-铺装路模式，2-越野模式（默认），3-草地模式，4-扬尘模式
        uint8_t pereception_strategy_type = 0;
#pragma endregion
    };

    // 定位使用策略配置响应服务(暂不使用)
    class __attribute__(()) manager2auto_positioning_usage {
#pragma region 定位使用策略
    public:
        // 帧头
        InternalFrameHeader header;

        // 定位使用策略
        // 0-北斗定位，1-惯导定位（默认），2-融合定位
        uint8_t positioning_type = 0;
#pragma endregion
    };

    // 目标点机动行动响应服务（行动编号、目标点数量、目标点列表、是否使用路网）
    // （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值801表示未设置行动点
    //(uint32)总里程，预估总里程，单位米
    //(uint32)全局路径点数量
    //(vector<GeoPoint>)全局路径点列表，经度、纬度、高度
    //(uint32)总里程，预估总里程，单位米
    //(uint32)全局路径点数量
    //(vector<GeoPoint>)全局路径点列表，经度、纬度、高度
    class manager2auto_target_points_action {
#pragma region 目标点机动行动
    public:
        // 帧头 16
        InternalFrameHeader header;

        // 行动编号 1
        uint8_t action_id = 0;

        // 目标点数量
        uint32_t target_points_num = 0;

        // 目标点列表
        std::vector<manager2auto_task_point> point_attributes;

        // 是否使用路网
        // 0-不使用路网，1-使用路网
        uint8_t use_roadnet = 0;

        // 绕圈模式 绕圈模式：0:不绕圈（默认值）1:一直绕圈；
        int8_t loop_mode = 0;
#pragma endregion
    };

    // 姿态调整响应服务（目标姿态、可接受到达姿态误差范围）
    // 152使用：先下发姿态调整，接收目标位置，再下发机动任务
    // cxy使用：直接进行调整
    // 姿态调整响应
    //(uint32),返回值，0表示失败，1表示成功
    //(GeoPoint)推荐位置，经度、纬度、高度
    class __attribute__(()) manager2auto_pose_adjust {
#pragma region 姿态调整
    public:
        // 帧头
        InternalFrameHeader header;

        // 目标姿态
        manager2auto_pose target_pose;

        // 可接受到达姿态误差范围
        manager2auto_pose acceptance_pose;
#pragma endregion
    };

    // 目标跟随行动响应服务（行动编号、目标像素位置x、目标像素位置y、最小安全距离、最大跟随距离）
    // 目标像素位置确定跟随目标，需要在退流视频中哦评估年显示+在目标中心
    // （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值803表示跟踪目标ID无效
    class __attribute__(()) manager2auto_follow_target_action {
#pragma region 目标跟随
    public:
        // 帧头
        InternalFrameHeader header;

        // 行动编号
        uint8_t action_id = 0;

        // 目标像素位置x, 在1920*1080下，点击位置距离原点x轴像素个数
        int16_t pixel_x = 0;

        // 目标像素位置y，在1920*1080下，点击位置距离原点y轴像素个数
        int16_t pixel_y = 0;

        // 最小安全距离，单位米，精度小数点后1位，1e1
        uint16_t safe_distance = 0;

        // 跟随时距参数
        uint16_t thw = 0;

        // 跟随策略使用 0-定位跟随，1-非定位跟随
        uint8_t follow_strategy = 0;
#pragma endregion
    };

    // 返航点设置响应服务
    // （uint32）返回值0表示成功，返回值1表示失败
    class __attribute__(()) manager2auto_return_point_set {
#pragma region 返航点设置
    public:
        // 帧头
        InternalFrameHeader header;

        // 返航标识
        // 0：无效，1：有效
        uint8_t flag = 0;
#pragma endregion
    };

    // 返航行动响应服务（行动编号、返航模式）
    // （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值801表示未设置行动点
    class __attribute__(()) manager2auto_return_action {
#pragma region 返航行动
    public:
        // 帧头
        InternalFrameHeader header;

        // 行动编号
        uint8_t action_id = 0;

        // 返航类型
        // 0：历史路径返航、1：重规划返航
        uint8_t return_type = 0;
#pragma endregion
    };

    // 编队机动行动响应服务（行动编号、编队模式、编队车辆数量、本相对于头车的队形信息、行动点数量、编队行动点）
    // （uint32）返回值0表示成功，返回值800表示有行动执行中，返回值801表示未设置行动点
    class manager2auto_formation_action {
#pragma region 编队机动
    public:
        // 帧头
        InternalFrameHeader header;

        // 行动编号
        uint8_t action_id = 0;

        // 编队模式，默认为1
        // 0=引导车模式；1=引导路径模式
        uint8_t formation_mode = 1;

        // 编队车辆数量
        uint32_t formation_member_num = 0;

        // 本相对于头车的队形信息
        // 当编队模式formation_mode为0时有效
        std::vector<manager2auto_formation_config> formation_config;

        // 行动点数量
        uint32_t formation_point_num = 0;

        // 编队行动点
        std::vector<manager2auto_task_point> formation_point;
#pragma endregion
    };

    // 编队定位发布（提供本次协同行动的其他车的实时定位信息）10Hz
    class manager2auto_formation_position {
#pragma region 编队定位发布
    public:
        // 帧头
        InternalFrameHeader header;

        // 其他参与编队数量
        uint32_t formation_member_num = 0;

        // 多车定位信息(不放本车定位，所有车的实时定位信息列表)
        std::vector<manager2auto_formation_localization> formation_positon;
#pragma endregion
    };

    // 行动指令控制响应服务（行动编号、行动控制指令）
    // （uint32）返回值0表示失败，返回值1表示成功
    class __attribute__(()) manager2auto_command_control {
#pragma region 行动指令控制
    public:
        // 帧头
        InternalFrameHeader header;

        // 行动编号
        uint8_t action_id = 0;

        // 行动控制指令
        // 1-开始，2-暂停，3-恢复，4-停止
        uint8_t commond = 0;

#pragma endregion
    };

    // 车辆速度上限配置响应服务（速度上限）
    // （uint32）返回值0表示失败，返回值1表示成功
    class __attribute__(()) manager2auto_speed_limit_config {
#pragma region 车辆速度上限配置
    public:
        // 帧头
        InternalFrameHeader header;

        // 速度上限，单位米/秒，精度小数点后1位，1e1
        uint16_t speed_limit = 0;
#pragma endregion
    };

    // 人机共驾模式订阅服务（心跳、人机共驾模式）
    // （uint32）返回值0表示失败，返回值1表示成功
    class __attribute__(()) manager2auto_human_driving_mode {
#pragma region 人机共驾模式
    public:
        // 帧头
        InternalFrameHeader header;

        // 人机共驾模式
        // 0-纯自主，1-遥控干预，2-主动应急避险
        uint8_t mode = 0;
#pragma endregion
    };

    // 摇杆控制订阅服务,服务类型：周期订阅，10Hz（纵向摇杆量、横向摇杆量、横摆角速度）
    class __attribute__(()) manager2auto_joystick_control {
#pragma region 摇杆控制
    public:
        // 帧头
        InternalFrameHeader header;

        // 档位, 0：驻车P，1：倒车档R，2：空挡N，3：前进档D，4：中心转向C
        uint8_t gear = 0;

        // 纵向摇杆量/油门制动，[-100,100]
        int8_t joystick_x = 0;

        // 横向摇杆量/方向盘，[-100,100]，左正右负
        int8_t joystick_y = 0;

        // 横摆角速度，[-100,100]，单位°/s，预留，默认发0
        int8_t angular_speed = 0;

        // 预留
        int8_t reserved1 = 0;
        int8_t reserved2 = 0;
        int8_t reserved3 = 0;
        int8_t reserved4 = 0;
#pragma endregion
    };
    // 紧急制动响应服务（紧急制动标识）
    // （uint32）返回值0表示失败，返回值1表示成功
    class __attribute__(()) manager2auto_emergency_brake {
#pragma region 紧急制动
    public:
        // 帧头
        InternalFrameHeader header;

        // 紧急制动标识
        // 0-不制动，1-紧急制动
        uint8_t emergancy_stop = 0;
#pragma endregion
    };
    // 视频数据传输使能响应服务（视频使能标识）
    // （uint32）返回值0表示失败，返回值1表示成功
    class __attribute__(()) manager2auto_video_enable {
#pragma region 视频数据传输使能
    public:
        // 帧头
        InternalFrameHeader header;

        // 视频使能标识，0-不传输，1-传输
        uint8_t video_enable = 0;
        // 视频传输目标IP
        uint32_t video_target_ip = 0; // 默认0，表示未设置

        // 视频传输目标端口
        uint16_t video_target_port = 0; // 默认0，表示未设置
#pragma endregion
    };

    // 视频画面切换响应服务（分辨率、码率、帧率、分割裁减标志位、拼接视频数量、视频拼接设置）
    // （uint32）返回值0表示失败，返回值1表示成功
    class manager2auto_video_set {
#pragma region 视频画面切换
    public:
        // 帧头
        InternalFrameHeader header;

        // 分辨率
        // 0-1920*1080 1-1280*720
        uint8_t resolution = 0;

        // 码率
        // 1-256k，2-512k，3-1M，4-2M
        uint8_t bps = 1;

        // 帧率
        uint8_t fps = 30;

        // 分割裁减标志位
        // 0-不调整拼接视频，1-调整拼接视频
        uint8_t splicing_flag = 0;

        // 拼接视频数量
        uint32_t video_num = 0;

        // 视频拼接设置
        std::vector<manager2auto_video_splicing> video_splicing = std::vector<manager2auto_video_splicing>();
#pragma endregion
    };

    // 视频辅助形式响应服务（遥控辅助线、遥控预警信息、自主辅助线）
    // （uint32）返回值0表示失败，返回值1表示成功
    class __attribute__(()) manager2auto_video_assist {
#pragma region 视频辅助形式
    public:
        // 帧头
        InternalFrameHeader header;

        // 遥控辅助线,遥控时无人车两侧预计通行区域，只显示在前主视（可见光、夜视）和后主视（可见光、夜视）视频上
        uint8_t remote_assist_line = 0;

        // 遥控预警信息,暂定只显示在前主视（可见光、夜视）相机上
        uint8_t remote_warning_info = 0;

        // 自主辅助线,暂定只显示在前主视（可见光、夜视）相机
        uint8_t auto_assist_line = 0;
#pragma endregion
    };

    class __attribute__(()) map2manager_global_point {
#pragma region 返回关键点数据
    public:
        // 返回值
        // 1- success 其他异常码
        uint32_t ad_return = 0;
        // 坐标系
        // 0-WGS84,1-墨卡托，2-LocalNED，3-LocalENU，4-CGCS2000
        uint8_t coordinate_system = 0;
        // 全局路径点数量
        uint32_t keypoint_num = 0;

        // 全局路径点列表
        std::vector<manager2map_task_point> keypoint_list;
#pragma endregion
    };

    class __attribute__(()) map2manager_global_path {
#pragma region 返回路点数据
    public:
        // 返回值
        // 1- success 其他异常码
        uint32_t ad_return = 0;
        // 坐标系
        // 0-WGS84,1-墨卡托，2-LocalNED，3-LocalENU，4-CGCS2000
        uint8_t coordinate_system = 0;
        // 全局路径点数量
        uint32_t pathpoint_num = 0;

        // 全局路径点列表
        std::vector<manager2map_geo_point> pathpoint_list;
#pragma endregion
    };

    class __attribute__(()) manager2auto_data_logger {
#pragma region 数据记录
    public:
        // 帧头
        InternalFrameHeader header;
        // 数据记录标识(0-有效，1-无效)记录前25s后5s的自主系统数据
        uint8_t data_logger = 0;
    };

    // manager2loc_ins_control	惯导相关控制
    class __attribute__(()) manager2loc_ins_control {
#pragma region 惯导相关控制
    public:
        // 枕头
        InternalFrameHeader header;
        // 惯导控制码 0-状态自检， 1-重新寻北
        uint8_t ins_control = 0;
#pragma endregion
    };

    // manager2loc_loc_mode 定位模式控制
    class __attribute__(()) manager2loc_loc_mode {
#pragma region 定位模式控制
    public:
        // 帧头
        InternalFrameHeader header;
        // 定位模式设置
        // Bit0：纯惯性，Bit1：轮速计，Bit2：卫星，Bit3：激光，Bit4：视觉，Bit5：毫米波，Bit6：预留，Bit7：自适应（默认为自适应，此时其他位无效）
        uint8_t loc_mode = 0;
    };

    template <>
    inline manager2auto_data_logger msg_parse<manager2auto_data_logger>(const uint8_t *buffer, size_t size,
                                                                        size_t &offset) {
        manager2auto_data_logger obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.data_logger, buffer, size, offset);
        return obj;
    }

    // manager2loc_ins_control	惯导相关控制
    template <>
    inline manager2loc_ins_control msg_parse<manager2loc_ins_control>(const uint8_t *buffer, size_t size,
                                                                      size_t &offset) {
        manager2loc_ins_control obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.ins_control, buffer, size, offset);
        return obj;
    }

    // manager2loc_loc_mode 定位模式控制

    template <>
    inline manager2loc_loc_mode msg_parse<manager2loc_loc_mode>(const uint8_t *buffer, size_t size, size_t &offset) {
        manager2loc_loc_mode obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.loc_mode, buffer, size, offset);
        return obj;
    }

    template <>
    inline manager2map_geo_point msg_parse<manager2map_geo_point>(const uint8_t *buffer, size_t size, size_t &offset) {
        manager2map_geo_point obj;
        read_le(obj.longitude, buffer, size, offset);
        read_le(obj.latitude, buffer, size, offset);
        read_le(obj.altitude, buffer, size, offset);
        read_le(obj.heading, buffer, size, offset);
        return obj;
    }

    template <>
    inline manager2map_task_point msg_parse<manager2map_task_point>(const uint8_t *buffer, size_t size,
                                                                    size_t &offset) {
        manager2map_task_point obj;
        read_le(obj.point_type, buffer, size, offset);
        obj.geo_point = msg_parse<manager2map_geo_point>(buffer, size, offset);
        read_le(obj.reserved1, buffer, size, offset);
        read_le(obj.reserved2, buffer, size, offset);
        read_le(obj.reserved3, buffer, size, offset);

        return obj;
    }

    template <>
    inline map2manager_global_point msg_parse<map2manager_global_point>(const uint8_t *buffer, size_t size,
                                                                        size_t &offset) {
        map2manager_global_point obj;

        // 返回值
        read_le(obj.ad_return, buffer, size, offset);
        read_le(obj.coordinate_system, buffer, size, offset);
        read_le(obj.keypoint_num, buffer, size, offset);
        obj.keypoint_list.resize(obj.keypoint_num);
        for (uint32_t i = 0; i < obj.keypoint_num; ++i) {
            obj.keypoint_list[i] = msg_parse<manager2map_task_point>(buffer, size, offset);
        }

        return obj;
    }

    template <>
    inline map2manager_global_path msg_parse<map2manager_global_path>(const uint8_t *buffer, size_t size,
                                                                      size_t &offset) {
        map2manager_global_path obj;

        read_le(obj.ad_return, buffer, size, offset);
        read_le(obj.coordinate_system, buffer, size, offset);
        read_le(obj.pathpoint_num, buffer, size, offset);
        obj.pathpoint_list.resize(obj.pathpoint_num);
        for (uint32_t i = 0; i < obj.pathpoint_num; ++i) {
            obj.pathpoint_list[i] = msg_parse<manager2map_geo_point>(buffer, size, offset);
        }

        return obj;
    }

    // manager2auto_geo_point
    template <>
    inline manager2auto_geo_point msg_parse<manager2auto_geo_point>(const uint8_t *buffer, size_t size,
                                                                    size_t &offset) {
        manager2auto_geo_point obj;
        read_le(obj.longitude, buffer, size, offset);
        read_le(obj.latitude, buffer, size, offset);
        read_le(obj.altitude, buffer, size, offset);
        return obj;
    }

    // manager2auto_pose
    template <>
    inline manager2auto_pose msg_parse<manager2auto_pose>(const uint8_t *buffer, size_t size, size_t &offset) {
        manager2auto_pose obj;
        read_le(obj.heading, buffer, size, offset);
        read_le(obj.pitch, buffer, size, offset);
        read_le(obj.roll, buffer, size, offset);
        return obj;
    }

    // manager2auto_formation_config
    template <>
    inline manager2auto_formation_config msg_parse<manager2auto_formation_config>(const uint8_t *buffer, size_t size,
                                                                                  size_t &offset) {
        manager2auto_formation_config obj;
        read_le(obj.member_ip, buffer, size, offset);
        read_le(obj.offset_x, buffer, size, offset);
        read_le(obj.offset_y, buffer, size, offset);
        return obj;
    }

    // manager2auto_task_point
    template <>
    inline manager2auto_task_point msg_parse<manager2auto_task_point>(const uint8_t *buffer, size_t size,
                                                                      size_t &offset) {
        manager2auto_task_point obj;
        read_le(obj.coordinate_system, buffer, size, offset);
        read_le(obj.point_type, buffer, size, offset);
        obj.geo_point = msg_parse<manager2auto_geo_point>(buffer, size, offset);
        obj.pose = msg_parse<manager2auto_pose>(buffer, size, offset);
        read_le(obj.acceptance_radius, buffer, size, offset);
        obj.acceptance_pose = msg_parse<manager2auto_pose>(buffer, size, offset);
        read_le(obj.formation_member_num, buffer, size, offset);

        auto vec_size = obj.formation_member_num;
        obj.formation_config.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.formation_config[i] = msg_parse<manager2auto_formation_config>(buffer, size, offset);
        }

        read_le(obj.reserved1, buffer, size, offset);
        read_le(obj.reserved2, buffer, size, offset);
        read_le(obj.reserved3, buffer, size, offset);
        return obj;
    }

    // manager2auto_trajectory_point
    template <>
    inline manager2auto_trajectory_point msg_parse<manager2auto_trajectory_point>(const uint8_t *buffer, size_t size,
                                                                                  size_t &offset) {
        manager2auto_trajectory_point obj;
        read_le(obj.timestamp, buffer, size, offset);
        obj.geo_point = msg_parse<manager2auto_geo_point>(buffer, size, offset);
        read_le(obj.speed, buffer, size, offset);
        read_le(obj.curvature, buffer, size, offset);
        read_le(obj.heading, buffer, size, offset);
        return obj;
    }

    // manager2auto_formation_trajectory
    template <>
    inline manager2auto_formation_trajectory msg_parse<manager2auto_formation_trajectory>(const uint8_t *buffer,
                                                                                          size_t size, size_t &offset) {
        manager2auto_formation_trajectory obj;
        read_le(obj.member_ip, buffer, size, offset);
        read_le(obj.trajectory_points_num, buffer, size, offset);

        auto vec_size = obj.trajectory_points_num;
        obj.trajectory_point.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.trajectory_point[i] = msg_parse<manager2auto_trajectory_point>(buffer, size, offset);
        }
        return obj;
    }

    // manager2auto_formation_localization
    template <>
    inline manager2auto_formation_localization msg_parse<manager2auto_formation_localization>(const uint8_t *buffer,
                                                                                              size_t         size,
                                                                                              size_t &       offset) {
        manager2auto_formation_localization obj;
        read_le(obj.member_ip, buffer, size, offset);
        obj.trajectory_points = msg_parse<manager2auto_trajectory_point>(buffer, size, offset);
        return obj;
    }

    // manager2auto_video_point
    template <>
    inline manager2auto_video_point msg_parse<manager2auto_video_point>(const uint8_t *buffer, size_t size,
                                                                        size_t &offset) {
        manager2auto_video_point obj;
        read_le(obj.top_left_x, buffer, size, offset);
        read_le(obj.top_left_y, buffer, size, offset);
        read_le(obj.bottom_right_x, buffer, size, offset);
        read_le(obj.bottom_right_y, buffer, size, offset);
        return obj;
    }

    // manager2auto_video_splicing
    template <>
    inline manager2auto_video_splicing msg_parse<manager2auto_video_splicing>(const uint8_t *buffer, size_t size,
                                                                              size_t &offset) {
        manager2auto_video_splicing obj;
        obj.video_position = msg_parse<manager2auto_video_point>(buffer, size, offset);
        read_le(obj.video_channel, buffer, size, offset);
        obj.video_crop_position = msg_parse<manager2auto_video_point>(buffer, size, offset);
        return obj;
    }

    // manager2auto_perception_target
    template <>
    inline manager2auto_perception_target msg_parse<manager2auto_perception_target>(const uint8_t *buffer, size_t size,
                                                                                    size_t &offset) {
        manager2auto_perception_target obj;
        read_le(obj.target_id, buffer, size, offset);
        read_le(obj.target_type, buffer, size, offset);
        read_le(obj.confidence, buffer, size, offset);
        obj.geopoint = msg_parse<manager2auto_geo_point>(buffer, size, offset);
        read_le(obj.speed, buffer, size, offset);
        read_le(obj.heading, buffer, size, offset);
        read_le(obj.relative_x, buffer, size, offset);
        read_le(obj.relative_y, buffer, size, offset);
        read_le(obj.relative_z, buffer, size, offset);
        read_le(obj.relative_speed_x, buffer, size, offset);
        read_le(obj.relative_speed_y, buffer, size, offset);
        read_le(obj.absolute_x, buffer, size, offset);
        read_le(obj.absolute_y, buffer, size, offset);
        read_le(obj.absolute_speed_x, buffer, size, offset);
        read_le(obj.absolute_speed_y, buffer, size, offset);
        read_le(obj.length, buffer, size, offset);
        read_le(obj.width, buffer, size, offset);
        read_le(obj.height, buffer, size, offset);
        return obj;
    }

    //  manager2auto_action_config
    template <>
    inline manager2auto_action_config msg_parse<manager2auto_action_config>(const uint8_t *buffer, size_t size,
                                                                            size_t &offset) {
        manager2auto_action_config obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.degrade_mode, buffer, size, offset);
        return obj;
    }

    // manager2auto_maneuvering_strategy
    template <>
    inline manager2auto_maneuvering_strategy msg_parse<manager2auto_maneuvering_strategy>(const uint8_t *buffer,
                                                                                          size_t size, size_t &offset) {
        manager2auto_maneuvering_strategy obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.maneuvering_strategy_type, buffer, size, offset);
        return obj;
    }

    // manager2auto_pereception_strategy
    template <>
    inline manager2auto_pereception_strategy msg_parse<manager2auto_pereception_strategy>(const uint8_t *buffer,
                                                                                          size_t size, size_t &offset) {
        manager2auto_pereception_strategy obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.pereception_strategy_type, buffer, size, offset);
        return obj;
    }

    // manager2auto_positioning_usage
    template <>
    inline manager2auto_positioning_usage msg_parse<manager2auto_positioning_usage>(const uint8_t *buffer, size_t size,
                                                                                    size_t &offset) {
        manager2auto_positioning_usage obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.positioning_type, buffer, size, offset);
        return obj;
    }

    // manager2auto_target_points_action
    template <>
    inline manager2auto_target_points_action msg_parse<manager2auto_target_points_action>(const uint8_t *buffer,
                                                                                          size_t size, size_t &offset) {
        manager2auto_target_points_action obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.action_id, buffer, size, offset);
        read_le(obj.target_points_num, buffer, size, offset);

        // uint32_t vec_size = 0;
        // read_le(vec_size, buffer, size, offset, sizeof(vec_size));
        // offset += sizeof(vec_size);
        auto vec_size = obj.target_points_num;
        obj.point_attributes.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.point_attributes[i] = msg_parse<manager2auto_task_point>(buffer, size, offset);
        }

        read_le(obj.use_roadnet, buffer, size, offset);
        read_le(obj.loop_mode, buffer, size, offset);
        return obj;
    }

    // manager2auto_pose_adjust
    template <>
    inline manager2auto_pose_adjust msg_parse<manager2auto_pose_adjust>(const uint8_t *buffer, size_t size,
                                                                        size_t &offset) {
        manager2auto_pose_adjust obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        obj.target_pose = msg_parse<manager2auto_pose>(buffer, size, offset);
        obj.acceptance_pose = msg_parse<manager2auto_pose>(buffer, size, offset);
        return obj;
    }

    // manager2auto_follow_target_action
    template <>
    inline manager2auto_follow_target_action msg_parse<manager2auto_follow_target_action>(const uint8_t *buffer,
                                                                                          size_t size, size_t &offset) {
        manager2auto_follow_target_action obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.action_id, buffer, size, offset);
        read_le(obj.pixel_x, buffer, size, offset);
        read_le(obj.pixel_y, buffer, size, offset);
        read_le(obj.safe_distance, buffer, size, offset);
        read_le(obj.thw, buffer, size, offset);
        read_le(obj.follow_strategy, buffer, size, offset);
        return obj;
    }

    // manager2auto_return_point_set
    template <>
    inline manager2auto_return_point_set msg_parse<manager2auto_return_point_set>(const uint8_t *buffer, size_t size,
                                                                                  size_t &offset) {
        manager2auto_return_point_set obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.flag, buffer, size, offset);
        return obj;
    }

    // manager2auto_return_action
    template <>
    inline manager2auto_return_action msg_parse<manager2auto_return_action>(const uint8_t *buffer, size_t size,
                                                                            size_t &offset) {
        manager2auto_return_action obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.action_id, buffer, size, offset);
        read_le(obj.return_type, buffer, size, offset);
        return obj;
    }

    // manager2auto_formation_action
    template <>
    inline manager2auto_formation_action msg_parse<manager2auto_formation_action>(const uint8_t *buffer, size_t size,
                                                                                  size_t &offset) {
        manager2auto_formation_action obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.action_id, buffer, size, offset);
        read_le(obj.formation_mode, buffer, size, offset);
        read_le(obj.formation_member_num, buffer, size, offset);

        auto vec_size = obj.formation_member_num;
        obj.formation_config.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.formation_config[i] = msg_parse<manager2auto_formation_config>(buffer, size, offset);
        }

        read_le(obj.formation_point_num, buffer, size, offset);

        uint32_t vec2_size = obj.formation_point_num;
        obj.formation_point.resize(vec2_size);
        for (uint32_t i = 0; i < vec2_size; ++i) {
            obj.formation_point[i] = msg_parse<manager2auto_task_point>(buffer, size, offset);
        }
        return obj;
    }

    // manager2auto_formation_position
    template <>
    inline manager2auto_formation_position msg_parse<manager2auto_formation_position>(const uint8_t *buffer,
                                                                                      size_t size, size_t &offset) {
        manager2auto_formation_position obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.formation_member_num, buffer, size, offset);

        auto vec_size = obj.formation_member_num;
        obj.formation_positon.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.formation_positon[i] = msg_parse<manager2auto_formation_localization>(buffer, size, offset);
        }
        return obj;
    }

    // manager2auto_command_control
    template <>
    inline manager2auto_command_control msg_parse<manager2auto_command_control>(const uint8_t *buffer, size_t size,
                                                                                size_t &offset) {
        manager2auto_command_control obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.action_id, buffer, size, offset);
        read_le(obj.commond, buffer, size, offset);
        return obj;
    }

    // manager2auto_speed_limit_config
    template <>
    inline manager2auto_speed_limit_config msg_parse<manager2auto_speed_limit_config>(const uint8_t *buffer,
                                                                                      size_t size, size_t &offset) {
        manager2auto_speed_limit_config obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.speed_limit, buffer, size, offset);
        return obj;
    }

    // manager2auto_human_driving_mode
    template <>
    inline manager2auto_human_driving_mode msg_parse<manager2auto_human_driving_mode>(const uint8_t *buffer,
                                                                                      size_t size, size_t &offset) {
        manager2auto_human_driving_mode obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.mode, buffer, size, offset);
        return obj;
    }

    // manager2auto_joystick_control
    template <>
    inline manager2auto_joystick_control msg_parse<manager2auto_joystick_control>(const uint8_t *buffer, size_t size,
                                                                                  size_t &offset) {
        manager2auto_joystick_control obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.gear, buffer, size, offset);
        read_le(obj.joystick_x, buffer, size, offset);
        read_le(obj.joystick_y, buffer, size, offset);
        read_le(obj.angular_speed, buffer, size, offset);
        read_le(obj.reserved1, buffer, size, offset);
        read_le(obj.reserved2, buffer, size, offset);
        read_le(obj.reserved3, buffer, size, offset);
        read_le(obj.reserved4, buffer, size, offset);
        return obj;
    }

    // manager2auto_emergency_brake
    template <>
    inline manager2auto_emergency_brake msg_parse<manager2auto_emergency_brake>(const uint8_t *buffer, size_t size,
                                                                                size_t &offset) {
        manager2auto_emergency_brake obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.emergancy_stop, buffer, size, offset);
        return obj;
    }

    // manager2auto_video_enable
    template <>
    inline manager2auto_video_enable msg_parse<manager2auto_video_enable>(const uint8_t *buffer, size_t size,
                                                                          size_t &offset) {
        manager2auto_video_enable obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.video_enable, buffer, size, offset);
        read_le(obj.video_target_ip, buffer, size, offset);
        read_le(obj.video_target_port, buffer, size, offset);
        return obj;
    }

    // manager2auto_video_set
    template <>
    inline manager2auto_video_set msg_parse<manager2auto_video_set>(const uint8_t *buffer, size_t size,
                                                                    size_t &offset) {
        manager2auto_video_set obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.resolution, buffer, size, offset);
        read_le(obj.bps, buffer, size, offset);
        read_le(obj.fps, buffer, size, offset);
        read_le(obj.splicing_flag, buffer, size, offset);
        read_le(obj.video_num, buffer, size, offset);

        auto vec_size = obj.video_num;
        obj.video_splicing.resize(vec_size);
        for (uint32_t i = 0; i < vec_size; ++i) {
            obj.video_splicing[i] = msg_parse<manager2auto_video_splicing>(buffer, size, offset);
        }
        return obj;
    }

    // manager2auto_video_assist
    template <>
    inline manager2auto_video_assist msg_parse<manager2auto_video_assist>(const uint8_t *buffer, size_t size,
                                                                          size_t &offset) {
        manager2auto_video_assist obj;
        obj.header = msg_parse<InternalFrameHeader>(buffer, size, offset);
        read_le(obj.remote_assist_line, buffer, size, offset);
        read_le(obj.remote_warning_info, buffer, size, offset);
        read_le(obj.auto_assist_line, buffer, size, offset);
        return obj;
    }

    // manager2auto_geo_point
    template <>
    inline void msg_pack<manager2auto_geo_point>(const manager2auto_geo_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.longitude, buffer);
        msg_pack(obj.latitude, buffer);
        msg_pack(obj.altitude, buffer);
    }

    // manager2auto_pose
    template <> inline void msg_pack<manager2auto_pose>(const manager2auto_pose &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.heading, buffer);
        msg_pack(obj.pitch, buffer);
        msg_pack(obj.roll, buffer);
    }

    // manager2auto_formation_config
    template <>
    inline void msg_pack<manager2auto_formation_config>(const manager2auto_formation_config &obj, uint8_t *buffer,
                                                        size_t &offset) {
        memcpy(buffer + offset, &obj.member_ip, sizeof(obj.member_ip));
        offset += sizeof(obj.member_ip);
        memcpy(buffer + offset, &obj.offset_x, sizeof(obj.offset_x));
        offset += sizeof(obj.offset_x);
        memcpy(buffer + offset, &obj.offset_y, sizeof(obj.offset_y));
        offset += sizeof(obj.offset_y);
    }

    // manager2auto_task_point
    template <>
    inline void msg_pack<manager2auto_task_point>(const manager2auto_task_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.coordinate_system, buffer);
        msg_pack(obj.point_type, buffer);
        msg_pack(obj.geo_point, buffer);
        msg_pack(obj.pose, buffer);
        msg_pack(obj.acceptance_radius, buffer);
        msg_pack(obj.acceptance_pose, buffer);
        msg_pack(obj.formation_member_num, buffer);
        for (const auto &fc : obj.formation_config) {
            msg_pack(fc, buffer);
        }

        msg_pack(obj.reserved1, buffer);
        msg_pack(obj.reserved2, buffer);
        msg_pack(obj.reserved3, buffer);
    }

    //新增关键点等结构
    template <>
    inline void msg_pack<manager2map_geo_point>(const manager2map_geo_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.longitude, buffer);
        msg_pack(obj.latitude, buffer);
        msg_pack(obj.altitude, buffer);
        msg_pack(obj.heading, buffer);
    }

    template <>
    inline void msg_pack<manager2map_task_point>(const manager2map_task_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.point_type, buffer);
        msg_pack(obj.geo_point, buffer);
        msg_pack(obj.reserved1, buffer);
        msg_pack(obj.reserved2, buffer);
        msg_pack(obj.reserved3, buffer);
    }

    template <>
    inline void msg_pack<map2manager_global_point>(const map2manager_global_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.ad_return, buffer);
        msg_pack(obj.coordinate_system, buffer);
        msg_pack(obj.keypoint_num, buffer);
        for (auto &pt : obj.keypoint_list) {
            msg_pack(pt, buffer);
        }
    }

    // manager2auto_trajectory_point
    template <>
    inline void msg_pack<manager2auto_trajectory_point>(const manager2auto_trajectory_point &obj, uint8_t *buffer,
                                                        size_t &offset) {
        memcpy(buffer + offset, &obj.timestamp, sizeof(obj.timestamp));
        offset += sizeof(obj.timestamp);
        msg_pack(obj.geo_point, buffer, offset);
        memcpy(buffer + offset, &obj.speed, sizeof(obj.speed));
        offset += sizeof(obj.speed);
        memcpy(buffer + offset, &obj.curvature, sizeof(obj.curvature));
        offset += sizeof(obj.curvature);
        memcpy(buffer + offset, &obj.heading, sizeof(obj.heading));
        offset += sizeof(obj.heading);
    }

    template <>
    inline void msg_pack<manager2auto_trajectory_point>(const manager2auto_trajectory_point &obj,
                                                        std::vector<uint8_t> &               buffer) {
        // 直接使用通用模板处理所有成员
        msg_pack(obj.timestamp, buffer);
        msg_pack(obj.geo_point, buffer);
        msg_pack(obj.speed, buffer);
        msg_pack(obj.curvature, buffer);
        msg_pack(obj.heading, buffer);
    }

    // manager2auto_formation_trajectory
    template <>
    inline void msg_pack<manager2auto_formation_trajectory>(const manager2auto_formation_trajectory &obj,
                                                            uint8_t *buffer, size_t &offset) {
        memcpy(buffer + offset, &obj.member_ip, sizeof(obj.member_ip));
        offset += sizeof(obj.member_ip);
        memcpy(buffer + offset, &obj.trajectory_points_num, sizeof(obj.trajectory_points_num));
        offset += sizeof(obj.trajectory_points_num);

        // uint32_t vec_size = obj.trajectory_point.size();
        // memcpy(buffer + offset, &vec_size, sizeof(vec_size));
        // offset += sizeof(vec_size);
        for (const auto &pt : obj.trajectory_point) {
            msg_pack(pt, buffer, offset);
        }
    }

    template <>
    inline void msg_pack<manager2auto_formation_trajectory>(const manager2auto_formation_trajectory &obj,
                                                            std::vector<uint8_t> &                   buffer) {
        msg_pack(obj.member_ip, buffer);
        msg_pack(obj.trajectory_points_num, buffer);
        for (auto &f : obj.trajectory_point) {
            msg_pack(f, buffer);
        }
    }

    // manager2auto_formation_localization
    template <>
    inline void msg_pack<manager2auto_formation_localization>(const manager2auto_formation_localization &obj,
                                                              uint8_t *buffer, size_t &offset) {
        memcpy(buffer + offset, &obj.member_ip, sizeof(obj.member_ip));
        offset += sizeof(obj.member_ip);
        msg_pack(obj.trajectory_points, buffer, offset);
    }

    // template <>
    // inline void msg_pack<manager2auto_formation_localization>(
    //     const manager2auto_formation_localization &obj,
    //     std::vector<uint8_t> &buffer) {
    //   msg_pack(obj.member_ip, buffer);
    //   msg_pack(obj.trajectory_points, buffer);
    // }

    // manager2auto_video_point
    template <>
    inline void msg_pack<manager2auto_video_point>(const manager2auto_video_point &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.top_left_x, buffer);
        msg_pack(obj.top_left_y, buffer);
        msg_pack(obj.bottom_right_x, buffer);
        msg_pack(obj.bottom_right_y, buffer);
    }

    // manager2auto_video_splicing
    template <>
    inline void msg_pack<manager2auto_video_splicing>(const manager2auto_video_splicing &obj, uint8_t *buffer,
                                                      size_t &offset) {
        msg_pack(obj.video_position, buffer, offset);
        memcpy(buffer + offset, &obj.video_channel, sizeof(obj.video_channel));
        offset += sizeof(obj.video_channel);
        msg_pack(obj.video_crop_position, buffer, offset);
    }

    // manager2auto_perception_target
    template <>
    inline void msg_pack<manager2auto_perception_target>(const manager2auto_perception_target &obj, uint8_t *buffer,
                                                         size_t &offset) {
        memcpy(buffer + offset, &obj.target_id, sizeof(obj.target_id));
        offset += sizeof(obj.target_id);
        memcpy(buffer + offset, &obj.target_type, sizeof(obj.target_type));
        offset += sizeof(obj.target_type);
        memcpy(buffer + offset, &obj.confidence, sizeof(obj.confidence));
        offset += sizeof(obj.confidence);
        msg_pack(obj.geopoint, buffer, offset);
        memcpy(buffer + offset, &obj.speed, sizeof(obj.speed));
        offset += sizeof(obj.speed);
        memcpy(buffer + offset, &obj.heading, sizeof(obj.heading));
        offset += sizeof(obj.heading);
        memcpy(buffer + offset, &obj.relative_x, sizeof(obj.relative_x));
        offset += sizeof(obj.relative_x);
        memcpy(buffer + offset, &obj.relative_y, sizeof(obj.relative_y));
        offset += sizeof(obj.relative_y);
        memcpy(buffer + offset, &obj.relative_z, sizeof(obj.relative_z));
        offset += sizeof(obj.relative_z);
        memcpy(buffer + offset, &obj.relative_speed_x, sizeof(obj.relative_speed_x));
        offset += sizeof(obj.relative_speed_x);
        memcpy(buffer + offset, &obj.relative_speed_y, sizeof(obj.relative_speed_y));
        offset += sizeof(obj.relative_speed_y);
        memcpy(buffer + offset, &obj.absolute_x, sizeof(obj.absolute_x));
        offset += sizeof(obj.absolute_x);
        memcpy(buffer + offset, &obj.absolute_y, sizeof(obj.absolute_y));
        offset += sizeof(obj.absolute_y);
        memcpy(buffer + offset, &obj.absolute_speed_x, sizeof(obj.absolute_speed_x));
        offset += sizeof(obj.absolute_speed_x);
        memcpy(buffer + offset, &obj.absolute_speed_y, sizeof(obj.absolute_speed_y));
        offset += sizeof(obj.absolute_speed_y);
        memcpy(buffer + offset, &obj.length, sizeof(obj.length));
        offset += sizeof(obj.length);
        memcpy(buffer + offset, &obj.width, sizeof(obj.width));
        offset += sizeof(obj.width);
        memcpy(buffer + offset, &obj.height, sizeof(obj.height));
        offset += sizeof(obj.height);
    }

    // template <>
    // inline void msg_pack<manager2auto_perception_target>(
    //     manager2auto_perception_target &obj, std::vector<uint8_t> &buffer) {
    //   msg_pack(obj.target_id, buffer);
    //   msg_pack(obj.target_type, buffer);
    //   msg_pack(obj.confidence, buffer);
    // }

    template <>
    inline void msg_pack<map2manager_global_path>(const map2manager_global_path &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.ad_return, buffer);
        msg_pack(obj.coordinate_system, buffer);
        msg_pack(obj.pathpoint_num, buffer);
        for (uint32_t i = 0; i < obj.pathpoint_list.size(); ++i) {
            msg_pack(obj.pathpoint_list[i], buffer);
        }
    }

    //  manager2auto_action_config
    template <>
    inline void msg_pack<manager2auto_action_config>(const manager2auto_action_config &obj,
                                                     std::vector<uint8_t> &            buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.degrade_mode, buffer);
    }

    // manager2auto_target_points_action
    template <>
    inline void msg_pack<manager2auto_target_points_action>(const manager2auto_target_points_action &obj,
                                                            uint8_t *buffer, size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
        offset += sizeof(obj.action_id);
        memcpy(buffer + offset, &obj.target_points_num, sizeof(obj.target_points_num));
        offset += sizeof(obj.target_points_num);

        // uint32_t vec_size = obj.point_attributes.size();
        // memcpy(buffer + offset, &vec_size, sizeof(vec_size));
        // offset += sizeof(vec_size);
        for (const auto &pt : obj.point_attributes) {
            msg_pack(pt, buffer, offset);
        }

        memcpy(buffer + offset, &obj.use_roadnet, sizeof(obj.use_roadnet));
        offset += sizeof(obj.use_roadnet);

        memcpy(buffer + offset, &obj.loop_mode, sizeof(obj.loop_mode));
        offset += sizeof(obj.loop_mode);
    }

    template <>
    inline void msg_pack<manager2auto_target_points_action>(const manager2auto_target_points_action &obj,
                                                            std::vector<uint8_t> &                   buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.action_id, buffer);
        msg_pack(obj.target_points_num, buffer);
        for (const auto &f : obj.point_attributes) {
            msg_pack(f, buffer);
        }
        msg_pack(obj.use_roadnet, buffer);

        msg_pack(obj.loop_mode, buffer);
    }

    // manager2auto_pose_adjust
    template <>
    inline void msg_pack<manager2auto_pose_adjust>(const manager2auto_pose_adjust &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.target_pose, buffer);
        msg_pack(obj.acceptance_pose, buffer);
    }

    // manager2auto_follow_target_action
    template <>
    inline void msg_pack<manager2auto_follow_target_action>(const manager2auto_follow_target_action &obj,
                                                            uint8_t *buffer, size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
        offset += sizeof(obj.action_id);
        memcpy(buffer + offset, &obj.pixel_x, sizeof(obj.pixel_x));
        offset += sizeof(obj.pixel_x);
        memcpy(buffer + offset, &obj.pixel_y, sizeof(obj.pixel_y));
        offset += sizeof(obj.pixel_y);
        memcpy(buffer + offset, &obj.safe_distance, sizeof(obj.safe_distance));
        offset += sizeof(obj.safe_distance);
        memcpy(buffer + offset, &obj.thw, sizeof(obj.thw));
        offset += sizeof(obj.thw);
        memcpy(buffer + offset, &obj.follow_strategy, sizeof(obj.follow_strategy));
        offset += sizeof(obj.follow_strategy);
    }

    // manager2auto_return_point_set
    template <>
    inline void msg_pack<manager2auto_return_point_set>(const manager2auto_return_point_set &obj, uint8_t *buffer,
                                                        size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.flag, sizeof(obj.flag));
        offset += sizeof(obj.flag);
    }

    // manager2auto_return_action
    template <>
    inline void msg_pack<manager2auto_return_action>(const manager2auto_return_action &obj,
                                                     std::vector<uint8_t> &            buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.action_id, buffer);
        msg_pack(obj.return_type, buffer);
    }

    // manager2auto_formation_action
    template <>
    inline void msg_pack<manager2auto_formation_action>(const manager2auto_formation_action &obj,
                                                        std::vector<uint8_t> &               buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.action_id, buffer);
        msg_pack(obj.formation_mode, buffer);
        msg_pack(obj.formation_member_num, buffer);
        for (const auto &fc : obj.formation_config) {
            msg_pack(fc, buffer);
        }
        msg_pack(obj.formation_point_num, buffer);
        for (const auto &pt : obj.formation_point) {
            msg_pack(pt, buffer);
        }
    }

    // manager2auto_formation_position
    template <>
    inline void msg_pack<manager2auto_formation_position>(const manager2auto_formation_position &obj,
                                                          std::vector<uint8_t> &                 buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.formation_member_num, buffer);
        for (const auto &pos : obj.formation_positon) {
            msg_pack(pos, buffer);
        }
    }

    // manager2auto_command_control
    template <>
    inline void msg_pack<manager2auto_command_control>(const manager2auto_command_control &obj, uint8_t *buffer,
                                                       size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.action_id, sizeof(obj.action_id));
        offset += sizeof(obj.action_id);
        memcpy(buffer + offset, &obj.commond, sizeof(obj.commond));
        offset += sizeof(obj.commond);
    }

    // manager2auto_speed_limit_config
    template <>
    inline void msg_pack<manager2auto_speed_limit_config>(const manager2auto_speed_limit_config &obj, uint8_t *buffer,
                                                          size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.speed_limit, sizeof(obj.speed_limit));
        offset += sizeof(obj.speed_limit);
    }

    // manager2auto_human_driving_mode
    template <>
    inline void msg_pack<manager2auto_human_driving_mode>(const manager2auto_human_driving_mode &obj, uint8_t *buffer,
                                                          size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        // memcpy(buffer + offset, &obj.heart, sizeof(obj.heart));
        // offset += sizeof(obj.heart);
        memcpy(buffer + offset, &obj.mode, sizeof(obj.mode));
        offset += sizeof(obj.mode);
    }

    // manager2auto_joystick_control
    template <>
    inline void msg_pack<manager2auto_joystick_control>(const manager2auto_joystick_control &obj, uint8_t *buffer,
                                                        size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.gear, sizeof(obj.gear));
        offset += sizeof(obj.gear);
        memcpy(buffer + offset, &obj.joystick_x, sizeof(obj.joystick_x));
        offset += sizeof(obj.joystick_x);
        memcpy(buffer + offset, &obj.joystick_y, sizeof(obj.joystick_y));
        offset += sizeof(obj.joystick_y);
        memcpy(buffer + offset, &obj.angular_speed, sizeof(obj.angular_speed));
        offset += sizeof(obj.angular_speed);
        memcpy(buffer + offset, &obj.reserved1, sizeof(obj.reserved1));
        offset += sizeof(obj.reserved1);
        memcpy(buffer + offset, &obj.reserved2, sizeof(obj.reserved2));
        offset += sizeof(obj.reserved2);
        memcpy(buffer + offset, &obj.reserved3, sizeof(obj.reserved3));
        offset += sizeof(obj.reserved3);
        memcpy(buffer + offset, &obj.reserved4, sizeof(obj.reserved4));
        offset += sizeof(obj.reserved4);
    }

    template <>
    inline void msg_pack<manager2auto_joystick_control>(const manager2auto_joystick_control &obj,
                                                        std::vector<uint8_t> &               buffer) {
        // 直接使用通用模板处理基本类型
        msg_pack(obj.header, buffer);
        msg_pack(obj.gear, buffer);
        msg_pack(obj.joystick_x, buffer);
        msg_pack(obj.joystick_y, buffer);
        msg_pack(obj.angular_speed, buffer);
        msg_pack(obj.reserved1, buffer);
        msg_pack(obj.reserved2, buffer);
        msg_pack(obj.reserved3, buffer);
        msg_pack(obj.reserved4, buffer);
    }

    // manager2auto_emergency_brake
    template <>
    inline void msg_pack<manager2auto_emergency_brake>(const manager2auto_emergency_brake &obj, uint8_t *buffer,
                                                       size_t &offset) {
        msg_pack(obj.header, buffer, offset);
        memcpy(buffer + offset, &obj.emergancy_stop, sizeof(obj.emergancy_stop));
        offset += sizeof(obj.emergancy_stop);
    }

    template <>
    inline void msg_pack<manager2auto_emergency_brake>(const manager2auto_emergency_brake &obj,
                                                       std::vector<uint8_t> &              buffer) {
        // 直接使用通用模板处理所有成员
        msg_pack(obj.header, buffer);
        msg_pack(obj.emergancy_stop, buffer);
    }

    // manager2auto_video_enable
    template <>
    inline void msg_pack<manager2auto_video_enable>(const manager2auto_video_enable &obj,
                                                    std::vector<uint8_t> &           buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.video_enable, buffer);
        msg_pack(obj.video_target_ip, buffer);
        msg_pack(obj.video_target_port, buffer);
    }

    // manager2auto_video_set
    template <>
    inline void msg_pack<manager2auto_video_set>(const manager2auto_video_set &obj, std::vector<uint8_t> &buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.resolution, buffer);
        msg_pack(obj.bps, buffer);
        msg_pack(obj.fps, buffer);
        msg_pack(obj.splicing_flag, buffer);
        msg_pack(obj.video_num, buffer);
        for (const auto &vs : obj.video_splicing) {
            msg_pack(vs, buffer);
        }
    }

    // manager2auto_video_assist
    template <>
    inline void msg_pack<manager2auto_video_assist>(const manager2auto_video_assist &obj,
                                                    std::vector<uint8_t> &           buffer) {
        msg_pack(obj.header, buffer);
        msg_pack(obj.remote_assist_line, buffer);
        msg_pack(obj.remote_warning_info, buffer);
        msg_pack(obj.auto_assist_line, buffer);
    }

} // namespace protocol_common
