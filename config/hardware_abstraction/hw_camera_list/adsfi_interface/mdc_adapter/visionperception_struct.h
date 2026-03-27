#ifndef __VISION_PERCEPTION_STRUCT_H__
#define __VISION_PERCEPTION_STRUCT_H__

#include <map>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>   // cv::line
#include <opencv2/highgui.hpp>   // cv::imshow, cv::waitKey
#include <string>
#include <vector>

namespace avos {
namespace perception {

#define VISION_PEOPLE 0         // 人
#define VISION_RIDER 1          // 骑行者
#define VISION_CAR 2            // 乘用车
#define VISION_BUS 3            // 公交车
#define VISION_TRUCK 4          // 货车
#define VISION_CROWD 5          // 人群
#define VISION_TRICYCLE 6       // 三轮车
#define VISION_BIKE 7           // 两轮车
#define VISION_TRICYCLE_RIDER 8 // 人骑机动三轮车
#define VISION_BIKE_RIDER 9     // 人骑摩托车
#define VISION_SUBULATE 10      // 锥形筒
#define VISION_TRIANGLE 11      // 三角牌
#define VISION_ROADBLOCK 12     // 路障
#define VISION_OTHER 13
#define VISION_WHEEL 14       // 车轮
#define VISION_WHEEL_POINT 15 // 车轮接地点
#define PERMISSION_ARROW 16 // 允许通行<左右转、直行、等>，合并后类型为arrow
// #define SPEED_LIMIT 17       // 限速，合并到19
#define FORBISSANCE_ARROW 18 // 禁止通行<左右转、直行、等>，合并到16
#define WORD 19              // 普通文字，对下游输出
#define NOTICE 20 // 人行横道、减速慢行<倒三角>，合并到16
#define ISOLATION_PILE 21     // 隔离桩
#define SHIDUN 22             // 石墩
#define VEHICLE_HEAD 23       // 车头
#define VEHICLE_TAIL 24       // 车尾
#define VISION_TRAFFICSIGN 25 // 交通标识牌
#define VISION_UNSUPPORT 255  // 不支持，不输出

// 需要进行二次分类的任务，做单独名称与任务id映射关系
static std::map<std::string, int> OBJECT_ID_MAP = {
    {"VISION_TRAFFICSIGN", VISION_TRAFFICSIGN}};

const cv::Scalar OBJECT_COLORS[] = {
    cv::Scalar(0, 255, 255),   // "people"
    cv::Scalar(0, 0, 255),     // "rider"
    cv::Scalar(255, 0, 255),   // "car"
    cv::Scalar(255, 0, 255),   // "bus"
    cv::Scalar(255, 0, 255),   // "truck"
    cv::Scalar(0, 255, 255),   // "crowd"
    cv::Scalar(0, 0, 255),     // "三轮车"
    cv::Scalar(0, 0, 255),     // "bike"
    cv::Scalar(0, 0, 255),     // "骑三轮车的人"
    cv::Scalar(0, 0, 255),     // "bikeRider"
    cv::Scalar(255, 255, 255), // "subulate"
    cv::Scalar(255, 255, 255), // "triangle"
    cv::Scalar(255, 255, 255), // "roadblock"
    cv::Scalar(0, 0, 0),       // "others"
    cv::Scalar(255, 0, 255),   // "wheel"
    cv::Scalar(255, 0, 255),   // "wheel_point"
    cv::Scalar(255, 255, 0),   // "permission_arrow"
    cv::Scalar(255, 255, 0),   // "speed_limit"
    cv::Scalar(255, 255, 0),   // "forbiddance_arrow"
    cv::Scalar(255, 255, 0),   // "word"
    cv::Scalar(255, 255, 0),   // "notice"
    cv::Scalar(255, 255, 0),   // "isolation_pile"
    cv::Scalar(255, 255, 0),   // "shidun"
    cv::Scalar(0, 0, 200),     // "head"
    cv::Scalar(0, 200, 200),   // "tail"
    cv::Scalar(255, 0, 0),     // "sign"
};

// new rules
#define SEG_BACKGROUND 0     // background
#define SEG_ROAD 1           // road 可通行道路
#define SEG_CURB 2           // curb+fence+wall 路缘石+围栏+墙壁
#define SEG_ROADBLOCK 3      // road_block+guide_board 路障
#define SEG_TRAFFICSIGN 4    // traffic_sign
#define SEG_CAR 5            // car
#define SEG_PEDCYC 6         // pedstrain,cyclist
#define SEG_EGOCAR 7         // ego car  自车
#define SEG_DEFLECTOR 8      // Deflector area 导流区
#define SEG_ZEBRA 9          // zebra crossing 人行横道
#define RESA_BACKGROUND 0    // background
#define RESA_POLE 1          // pole 杆子
#define RESA_LANE_TYPE_0 2   // lane type 0 实线
#define RESA_LANE_TYPE_1 3   // lane type 1 虚线
#define RESA_LANE_STOPLINE 4 // stopline 停止线

#define SEGMENT_LABEL_NUM 10
#define LANESEG_LABEL_NUM 5

const cv::Vec3b SEGMENT_COLORS[] = {
    cv::Vec3b(0, 0, 0),       // 0: background
    cv::Vec3b(255, 0, 0),     // 1: road
    cv::Vec3b(0, 120, 0),     // 2: curb+fence+wall
    cv::Vec3b(255, 255, 0),   // 3: road_block+guide_board
    cv::Vec3b(0, 0, 255),     // 4: traffic_sign
    cv::Vec3b(255, 0, 255),   // 5: car, Magenta
    cv::Vec3b(0, 255, 0),     // 6: pedstrain,cyclist
    cv::Vec3b(100, 0, 150),   // 7: ego car
    cv::Vec3b(150, 150, 150), // 8: Deflector area 导流区
    cv::Vec3b(255, 255, 100)  // 9: zebra crossing 人行横道
};

const cv::Vec3b LANE_COLORS[] = {
    cv::Vec3b(0, 0, 0),      // 0: background
    cv::Vec3b(255, 120, 0),  // 1: pole 杆子
    cv::Vec3b(0, 120, 255),  // 2: lane type 0 实线
    cv::Vec3b(0, 250, 100),  // 3: lane type 1 虚线
    cv::Vec3b(250, 250, 100) // 4: stopline 停止线
};

// 视觉目标检测2D相关字段
struct Object2D {
    uint32_t obj_id{0};   // 目标类型
    uint32_t child_id{0}; // 目标细分类型
    float prob{0};        // 得分置信度
    float prob2d{0};      // 得分置信度
    float prob3d{0};      // 得分置信度
    float x{0};           // 2D框的坐标
    float y{0};
    float w{0};
    float h{0};

    bool orientation_enable{false};
    uint32_t orientation_id{0}; // 目标航向类别
    float orientation_score{0}; // 目标航向置信度

    bool vrotate_enable{false};
    float vrotate{0};       // 图像坐标系下，目标航向角度，
    float vrotate_score{0}; // 图像坐标系下，目标航向角度置信度

    bool key_points_enable{false};
    std::vector<cv::Point2f> key_points; // 图像坐标系下，目标伪3D的关键点
    float vheight;                       // 图像坐标系下，目标的高度

    float occlusion{-1}; // 遮挡比例, -1表示无效，程序并未做该功能
    float truncation{-1}; // 截断比例, -1表示无效，程序并未做该功能
};

// 视觉目标检测跟踪相关字段
struct TrackState {
    bool enable{false};
    uint64_t track_id{0};
    uint64_t fusion_id{0};
    uint32_t track_age{0};
    uint32_t track_time_since_update{0};
    float track_score{0};
};

struct Object3D {
    bool enable{false};
    float x{0}; // 目标在3d坐标系下的中线点 (x,y,z)
    float y{0};
    float z{0};
    float width{0}; // 目标的物理尺寸
    float height{0};
    float length{0};
    float protate{0}; // 目标的在3D坐标下的航向角度
    float cx_score{
        -1}; // 以上信息的置信度，小于0时，表示置信度信息无效，程序并未做该功能
    float cy_score{-1};
    float cz_score{-1};
    float width_score{-1};
    float height_score{-1};
    float length_score{-1};
    float protate_score{-1};
};

// 目标实例分割结果
struct InstanceMask {
    bool enable{false};
    uint32_t mask_w{0};
    uint32_t mask_h{0};
    std::vector<uint8_t> instance_mask;
};

// 车辆尾灯状态
struct CarLightState {
    bool enable{false};
    uint32_t tail_light_state;
    float tail_light_score;
};

// 行人状态
struct PedestrianState {
    bool enable{false};
    uint32_t pedestrian_pose{0};
    float pedestrian_pose_score{0};
};

// 目标检测，单个目标信息结构体
struct VisionObject {
    int source_crop{0}; // 1:crop
    Object2D object_2d;
    TrackState track_state;
    Object3D object_3d;
    InstanceMask instance_mask;
    CarLightState car_light_state;
    PedestrianState pedestrian_state;
    // TrafficSign trafficsign;
};

// 感知模块输出的语义分割结果，转proto之前的结构体
struct SegmentMaskSplitInfo {
    uint32_t camera_id;
    std::vector<uint32_t> camera_roi;
    std::vector<uint32_t> mask_roi;
    uint32_t mask_type;
};

struct SegmetResult {
    bool valid{false};
    uint32_t camera_num;
    std::vector<std::string> camera_names;
    std::vector<uint32_t> camera_heights;
    std::vector<uint32_t> camera_widths;
    std::vector<bool> valids;
    std::vector<SegmentMaskSplitInfo> mask_split_infos;
    bool mask_compressed;
    uint8_t *mask_data;
    uint32_t data_size;
};

// 感知模块输出的车道线实例结果，转proto之前的结构体
struct LaneInstResult {
    bool valid{false};
    std::vector<int> camera_indexs;
    std::vector<int> point_counts;
    float *points;
    int32_t points_size;
};

// 感知模块输出的目标检测结果，转proto之前的结构体
struct CameraDetectResult {
    std::vector<VisionObject> vision_objects;
};

struct DetectionResult {
    bool valid{false};
    std::vector<CameraDetectResult> camera_vision_objects;
};

// 感知模块输出的实例分割结果，转proto前的结构体
struct CameraInstSegResult {
    int width;
    int height;
    int inst_num{0};
    std::vector<int> inst_class;
    std::vector<float> inst_score;
    uint8_t *mask{nullptr};
};

struct InstSegmentResult {
    bool valid{false};
    std::vector<CameraInstSegResult> camera_instseg_result;
};

struct PairObject2D {
    uint32_t obj_id{0};
    float prob{0};
    float x{0};
    float y{0};
    float w{0};
    float h{0};
    float offset_x{0};
    float offset_y{0};
};

struct WheelResult {
    bool valid{false};
    std::vector<std::vector<PairObject2D>> camera_wheel_objects;
};

struct VehicleHTResult {
    bool valid{false};
    std::vector<std::vector<PairObject2D>> camera_vehicle_ht_objects;
};

struct WheelPoints {
    uint32_t obj_id{0};
    float prob{0};
    float x{0};
    float y{0};
    float offset_x{0};
    float offset_y{0};
};

struct WheelPointsResult {
    bool valid{false};
    std::vector<std::vector<WheelPoints>> camera_wheel_points;
};

struct CameraObjectsNum {
    int objects_num = 0;
    int objects_num_vehicel = 0;
    int objects_num_person = 0;
    int objects_num_other = 0;
    int objects_num_static_obj = 0;
    int objects_num_ground_mark = 0;
};

struct VPResult {
    std::string model_version;
    DetectionResult detectstrut;
    SegmetResult segmentstrut;
    SegmetResult lanesegstrut;
    LaneInstResult laneInst;
    InstSegmentResult instSegStrut;
    WheelResult wheelsStrut;
    WheelPointsResult wheelPointStrut;
    VehicleHTResult vehicleHeadTailsStrut;
};

} // namespace perception
} // namespace avos

#endif