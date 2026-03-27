#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
#include "PerceptionVisionObject.h"
#include "PerceptionVisionSegment.h"
#include "PerceptionSingleLaneInst.h"
#include "PerceptionCameraInstSegResult.h"
#include "PerceptionPairObject2D.h"
#include "PerceptionWheelPoints.h"
/*
单个相机感知视觉输出
*/

typedef struct PerceptionCameraResult {
    std::vector<msghandle::type::fix_int32> key_hash_list; //
    std::string camera_name; //相机名称
    bool valid; //该相机结果是否有效，默认需要给false
    std::vector<PerceptionVisionObject> vision_objects; //视觉目标检测（所有类别，包括mono3D）
    PerceptionVisionSegment vision_segment; //车道线和语义分割结果
    std::vector<PerceptionSingleLaneInst> lane_inst; //车道线实例检测
    PerceptionCameraInstSegResult camera_instseg_result; //实例分割输出结果
    std::vector<PerceptionPairObject2D> wheels; //车轮检测
    std::vector<PerceptionWheelPoints> wheel_points; //车轮接地点检测
    std::vector<PerceptionPairObject2D> vehicle_hts; //车辆高度检测（修正注释）
    double stamp; //新增字段：时间戳（秒）

    PerceptionCameraResult() {
        key_hash_list.resize(14);
        key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
        key_hash_list.push_back(msghandle::type::fix_int32(1651279343));
        key_hash_list.push_back(msghandle::type::fix_int32(2073588530));
        key_hash_list.push_back(msghandle::type::fix_int32(948847434));
        key_hash_list.push_back(msghandle::type::fix_int32(1859379969));
        key_hash_list.push_back(msghandle::type::fix_int32(1678245969));
        key_hash_list.push_back(msghandle::type::fix_int32(2127318826));
        key_hash_list.push_back(msghandle::type::fix_int32(1674061705));
        key_hash_list.push_back(msghandle::type::fix_int32(1119874181));
        key_hash_list.push_back(msghandle::type::fix_int32(962137087));
        key_hash_list.push_back(msghandle::type::fix_int32(962137099));
        Clear(); //初始化所有字段（包括stamp）
    }

    void Clear() {
        camera_name = {};
        valid = {};
        vision_objects = {};
        vision_segment = {};
        lane_inst = {};
        camera_instseg_result = {};
        wheels = {};
        wheel_points = {};
        vehicle_hts = {};
        stamp = 0.0; //重置时间戳
    }

    MSGHANDLE_IS_CONTINUOUS_MEM(0)
    MSGHANDLE_DEFINE(key_hash_list, camera_name, valid, vision_objects, vision_segment, 
                    lane_inst, camera_instseg_result, wheels, wheel_points, vehicle_hts, stamp); //添加stamp
    MSGHANDLE_PARAKLIST_DEFINE("key_hash_list", "camera_name", "valid", "vision_objects", 
                              "vision_segment", "lane_inst", "camera_instseg_result", 
                              "wheels", "wheel_points", "vehicle_hts", "stamp"); //添加stamp键名
    MSGHANDLE_PARAVLIST_DEFINE(key_hash_list, camera_name, valid, vision_objects, 
                              vision_segment, lane_inst, camera_instseg_result, 
                              wheels, wheel_points, vehicle_hts, stamp); //添加stamp值
} PerceptionCameraResult;

REGISTER(PerceptionCameraResult, PerceptionCameraResult_Func);
