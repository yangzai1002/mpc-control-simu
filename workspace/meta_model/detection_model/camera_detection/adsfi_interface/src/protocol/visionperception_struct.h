/*
 * 该文件中定义的数据结构，需要通过 MSGPACK 进行序列化和反序列化
 * 对所有需要MSGPACK进行序列化和反序列化的类和结构体需要遵循以下准则:
 * 1:自定义的默认构造函数中不能通过new，malloc函数动态申请内存（会导致内存泄露）
 */
#ifndef __GSDS_VISION_PERCEPTION_STRUCT_H__
#define __GSDS_VISION_PERCEPTION_STRUCT_H__


#include <cstring>
#include <iostream>
#include <map>
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
#define PERMISSION_ARROW 16   // 允许通行<左右转、直行、等>，合并后类型为arrow
// #define SPEED_LIMIT 17       // 限速，合并到19
#define FORBISSANCE_ARROW 18  // 禁止通行<左右转、直行、等>，合并到16
#define WORD 19               // 普通文字，对下游输出
#define NOTICE 20             // 人行横道、减速慢行<倒三角>，合并到16
#define ISOLATION_PILE 21     // 隔离桩
#define SHIDUN 22             // 石墩
#define VEHICLE_HEAD 23       // 车头
#define VEHICLE_TAIL 24       // 车尾
#define VISION_TRAFFICSIGN 25 // 交通标识牌
#define VISION_UNSUPPORT 255  // 不支持，不输出

// 需要进行二次分类的任务，做单独名称与任务id映射关系
static std::map<std::string, int> OBJECT_ID_MAP = {
    {"VISION_TRAFFICSIGN", VISION_TRAFFICSIGN}};

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

struct Point2f {
    float x;
    float y;
    MSGPACK_DEFINE(x, y);
};

struct Rect {
    int x;
    int y;
    int width;
    int height;
    MSGPACK_DEFINE(x, y, width, height);
};

/*
 * Mat简化版实现，用于存储返回结果中的图像数据
 * 只存储单通道灰度图像
 */
class Mat {
public:
    Mat() {
        // 这里曾今动态申请了refcount_内存
        // 导致严重的内存泄露
    }

    Mat(int rows, int cols) : rows(rows), cols(cols) {
        refcount_ = reinterpret_cast<uint64_t>(new int(1));
        data_ = reinterpret_cast<uint64_t>(malloc(rows * cols));
    }

    Mat(const Mat &m) {
        data_ = m.data_;
        refcount_ = m.refcount_;
        rows = m.rows;
        cols = m.cols;
        (*refcount())++;
    }

    Mat clone() const {
        Mat clone(rows, cols);
        memcpy(clone.data(), data(), rows * cols);
        return clone;
    }

    Mat &operator=(const Mat &m) {
        if (this != &m) {
            if (nullptr != refcount() && --(*refcount()) == 0) {
                if (nullptr != data())
                    free(data());
                delete refcount();
            }
            data_ = m.data_;
            refcount_ = m.refcount_;
            rows = m.rows;
            cols = m.cols;
            add_refcount();
        }
        return *this;
    }

    ~Mat() {
        if (nullptr != refcount() && --(*refcount()) == 0) {
            if (nullptr != data())
                free(data());
            delete refcount();
        }
    }

    void *data() const { return reinterpret_cast<void *>(data_); }

    int rows = 0;
    int cols = 0;

    void add_refcount() {
        if (nullptr != refcount())
            (*refcount())++;
    }

private:
    int *refcount() { return reinterpret_cast<int *>(refcount_); }
    uint64_t data_ = reinterpret_cast<uint64_t>(nullptr);
    uint64_t refcount_ = reinterpret_cast<uint64_t>(nullptr);

public:
    MSGPACK_DEFINE(rows, cols, data_, refcount_);
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
    std::vector<Point2f> key_points; // 图像坐标系下，目标伪3D的关键点
    float vheight;                   // 图像坐标系下，目标的高度

    float occlusion{-1};  // 遮挡比例, -1表示无效，程序并未做该功能
    float truncation{-1}; // 截断比例, -1表示无效，程序并未做该功能
    MSGPACK_DEFINE(obj_id, child_id, prob, prob2d, prob3d, x, y, w, h,
                   orientation_enable, orientation_id, orientation_score,
                   vrotate_enable, vrotate, vrotate_score, key_points_enable,
                   key_points, vheight, occlusion, truncation);
};

// 视觉目标检测跟踪相关字段
struct TrackState {
    bool enable{false};
    uint64_t track_id{0};
    uint64_t fusion_id{0};
    uint32_t track_age{0};
    uint32_t track_time_since_update{0};
    float track_score{0};
    MSGPACK_DEFINE(enable, track_id, fusion_id, track_age,
                   track_time_since_update, track_score);
};

struct Object3D {
    bool enable{false};
    float x{0}; // 目标在3d坐标系下的中线点 (x,y,z)
    float y{0};
    float z{0};
    float width{0}; // 目标的物理尺寸
    float height{0};
    float length{0};
    float protate{0};   // 目标的在3D坐标下的航向角度
    float cx_score{-1}; // 小于0时，表示置信度信息无效，程序并未做该功能
    float cy_score{-1};
    float cz_score{-1};
    float width_score{-1};
    float height_score{-1};
    float length_score{-1};
    float protate_score{-1};
    MSGPACK_DEFINE(enable, x, y, z, width, height, length, protate, cx_score,
                   cy_score, cz_score, width_score, height_score, length_score,
                   protate_score);
};

// 目标实例分割结果
struct InstanceMask {
    bool enable{false};
    uint32_t mask_w{0};
    uint32_t mask_h{0};
    std::vector<uint8_t> instance_mask;
    MSGPACK_DEFINE(enable, mask_w, mask_h, instance_mask);
};

// 车辆尾灯状态
struct CarLightState {
    bool enable{false};
    uint32_t tail_light_state;
    float tail_light_score;
    MSGPACK_DEFINE(enable, tail_light_state, tail_light_score);
};

// 行人状态
struct PedestrianState {
    bool enable{false};
    uint32_t pedestrian_pose{0};
    float pedestrian_pose_score{0};
    MSGPACK_DEFINE(enable, pedestrian_pose, pedestrian_pose_score);
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
    MSGPACK_DEFINE(source_crop, object_2d, track_state, object_3d,
                   instance_mask, car_light_state, pedestrian_state);
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
    MSGPACK_DEFINE(obj_id, prob, x, y, w, h, offset_x, offset_y);
};

struct WheelPoints {
    uint32_t obj_id{0};
    float prob{0};
    float x{0};
    float y{0};
    float offset_x{0};
    float offset_y{0};
    MSGPACK_DEFINE(obj_id, prob, x, y, offset_x, offset_y);
};

struct VisionSegment {
    int camera_width = 0; // 相机原图的分辨率
    int camera_height = 0;
    bool resa_valid = false;
    Rect resa_roi;
    Mat resa; // 车道线和杆
    bool seg_valid = false;
    Rect seg_roi;
    Mat seg; // 其他场景分割
    // Mat resacrop;
    // Mat segcrop;
    void add_refcount() {
        resa.add_refcount();
        seg.add_refcount();
    }
    MSGPACK_DEFINE(camera_width, camera_height, resa_valid, resa_roi, resa,
                   seg_valid, seg_roi, seg);
};

struct VisionInstSegment {
    int width;
    int height;
    int inst_num{0};
    std::vector<int> inst_class;
    std::vector<float> inst_score;
    Mat mask;
    void add_refcount() { mask.add_refcount(); }
    MSGPACK_DEFINE(width, height, inst_num, inst_class, inst_score, mask);
};

// 目标检测，单个相机检测结果结构体
// 一个相机会检测出多个目标
// CameraPerceptionResult中的时间戳和所属
// NodePerceptionResult的时间戳保持一致
struct CameraPerceptionResult {
    std::string camera_name{""};
    bool valid{false};
    double trigger_stamp{0.0};
    double time{0.0}; // 注意：用于同步数据局，time==trigger_stamp
    double receive_stamp{0.0};
    double send_stamp{0.0};
    std::vector<VisionObject> vision_objects;
    VisionSegment vision_segment;
    std::vector<std::vector<Point2f>> lane_inst;
    VisionInstSegment camera_instseg_result;
    std::vector<PairObject2D> wheels;
    std::vector<WheelPoints> wheel_points;
    std::vector<PairObject2D> vehicle_hts;
    void add_refcount() {
        vision_segment.add_refcount();
        camera_instseg_result.add_refcount();
    }
    MSGPACK_DEFINE(camera_name, valid, trigger_stamp, time, receive_stamp,
                   send_stamp, vision_objects, vision_segment, lane_inst,
                   camera_instseg_result, wheels, wheel_points, vehicle_hts);
};

// 一个检测节点的检测结果结构体
// 一个检测节点会检测1个或多个相机
struct NodePerceptionResult {
    std::string node_name;
    double trigger_stamp{0.0};
    double receive_stamp{0.0};
    double send_stamp{0.0};
    std::vector<CameraPerceptionResult> camera_vision_objects;
    void add_refcount() {
        for (auto &camera_vision_object : camera_vision_objects) {
            camera_vision_object.add_refcount();
        }
    }

    MSGPACK_DEFINE(node_name, trigger_stamp, receive_stamp, send_stamp,
                   camera_vision_objects);
};

} // namespace perception
} // namespace avos

#endif