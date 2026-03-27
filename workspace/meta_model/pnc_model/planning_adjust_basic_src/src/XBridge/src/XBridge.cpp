#include "ap_log/ap_log_interface.h"
#include "XBridge.h"
#include "xkit/planning/datapool/task_struct.h"
#include "xkit/planning/datapool/data_pool.h"
#include "common_header/geotool.h"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "faulthandle_api.hpp"
#include "custom_stack.h"
#include "base_control/lat/latcontrol_header.h"
#include "base_control/adsfi_struct_adapter.h"

avos::planning::CoordTransform m_transform;
static bool is_in_utm_ = false; //感知
static bool use_dr_follow_ = false;
static bool use_dr_track_ = false;

void CallbackLocalization(int business_mode, const ara::adsfi::MsgHafLocation& loc, const bool use_dr_follow, 
                          avos::planning::LocalizationData& localization_data) {
    // --- 1. 赋值通用车辆状态 ---
    localization_data.time = TimeToSec(loc.header.timestamp);
    localization_data.velocity = loc.v;
    localization_data.vx = loc.velocity.twist.linear.x;
    localization_data.vy = loc.velocity.twist.linear.y;
    localization_data.yawrate = loc.velocity.twist.angular.z;
    localization_data.gps_state = loc.locationState;

    // --- 2. 根据标志位，选择坐标源对核心定位字段进行赋值 ---
    if (use_dr_follow) {
        // --- 场景一: 使用DR航位推算定位信息 ---
        AINFO_IF(FLAGS_enable_log) << "[Localization Bridge] Using DR data source based on task command.";
        localization_data.xg = loc.dr_pose.pose.position.x;
        localization_data.yg = loc.dr_pose.pose.position.y;
        localization_data.zg = loc.dr_pose.pose.position.z;
        localization_data.roll = loc.dr_pose.pose.orientation.x;
        localization_data.pitch = loc.dr_pose.pose.orientation.y;
        localization_data.yaw = loc.dr_pose.pose.orientation.z;
        // 当使用DR时，清除绝对定位异常的故障码
        FaultHandle::FaultApi::Instance()->ResetFaultCode("3072016");
    } else {
        // --- 场景二: 使用UTM绝对定位信息 ---
        AINFO_IF(FLAGS_enable_log) << "[Localization Bridge] Using UTM data source based on task command.";
        localization_data.xg = loc.pose.pose.position.x;
        localization_data.yg = loc.pose.pose.position.y;
        localization_data.zg = loc.pose.pose.position.z;
        localization_data.roll = loc.pose.pose.orientation.x;
        localization_data.pitch = loc.pose.pose.orientation.y;
        localization_data.yaw = loc.pose.pose.orientation.z;
        localization_data.loc_xg_dr = loc.dr_pose.pose.position.x;
        localization_data.loc_yg_dr = loc.dr_pose.pose.position.y;
        localization_data.loc_yaw_dr = loc.dr_pose.pose.orientation.z;
        // localization_data.dr_time = loc.dr_pose.dr_time;

        // 使用UTM时，才检查其数值有效性
        if (loc.pose.pose.position.x < 1000 || loc.pose.pose.position.y < 1000) {
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3072016", "绝对定位数据异常");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3082002", "绝对定位数据异常");
            }
        } else {
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3072016");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3082002");
            }
        }
    }

    // 消息超时检查逻辑
    static double last_loc_time = 0.0;
    double current_time = avos::common::NodeTime::Now().ToSecond();
    if (last_loc_time > 0.0) {
        double time_diff = std::fabs(current_time - last_loc_time);
        if (time_diff> 0.4) {
            AERROR_IF(FLAGS_enable_log) << "定位消息超时 " << time_diff << "秒。";
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3072015", "定位数据接收超时400ms");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3082003", "绝对定位数据异常");
            }
        } else {
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3072015");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3082003");
            }
        }
    }
    last_loc_time = TimeToSec(loc.header.timestamp);
}

void CallbackSemanticMap(int business_mode, const ara::adsfi::PerceptionStaticEnv &msg, avos::planning::Perception& perception){
    static double last_per_time = 0.0;
    perception.perception_semantic_map.perception_semantic_map_data.semantic_map.clear();
    perception.perception_semantic_map.perception_semantic_map_data.timestamp = TimeToSec(msg.header.time);

    std::string find_str = "out_to_utm";
    std::string data_name = msg.header.data_name;
    is_in_utm_ = false;
    if(data_name.find(find_str) != std::string::npos)
    {
        is_in_utm_=true;
    }
  std::cout << "is_in_utm_:" << is_in_utm_ << std::endl;
    
    // AINFO_IF(FLAGS_enable_log) << "msg.cells size = " << msg.semantic_objs.size();
    for (const auto &obj : msg.semantic_objs) {
        avos::planning::SemanticCell s_cell;
        for (auto &p : obj.cells) {
            if (obj.type == 70 || obj.type == 71 || obj.type == 72 || obj.type == 73) {
                // AINFO_IF(FLAGS_enable_log) << "obj.type = " << obj.type;
                continue;
                if (std::fabs(p.dh + 3.0) > -0.0000001) {
                    s_cell.type = obj.type;
                    s_cell.speed = obj.speed;
                    s_cell.x = p.x;
                    s_cell.y = p.y;
                    s_cell.width = obj.obj_size.width;
                    s_cell.length = obj.obj_size.length;
                    perception.perception_semantic_map.perception_semantic_map_data.semantic_map.push_back(s_cell);
                }

            } else {
                s_cell.type = obj.type;
                s_cell.speed = obj.speed;
                s_cell.x = p.x;
                s_cell.y = p.y;
                s_cell.width = obj.obj_size.width;
                s_cell.length = obj.obj_size.length;
                perception.perception_semantic_map.perception_semantic_map_data.semantic_map.push_back(s_cell);
            }
        }
    }
    perception.perception_semantic_map.perception_semantic_map_data.semantic_edge.clear();
    if (last_per_time > 0.0) {
        double time_diff = TimeToSec(msg.header.time) - last_per_time;
        if (time_diff > 0.5) {
            AERROR_IF(FLAGS_enable_log) << "感知消息超时 " << time_diff << "秒。";
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3072017", "感知数据接收超时500ms");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3082004", "感知数据接收超时500ms");
            }
        } else {
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3072017");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3082004");
            }
        }
    }
    last_per_time = TimeToSec(msg.header.time);
    return;
}

#include <queue>

class FixedSizeQueue {
private:
    std::queue<avos::planning::LocalizationData> queue;
    size_t maxSize;

public:
    FixedSizeQueue(size_t size) : maxSize(size) {}

    void push(avos::planning::LocalizationData value) {
        if (queue.size() >= maxSize) {
            queue.pop(); // 删除最旧的数据
        }
        queue.push(value); // 添加新数据
    }

    void MatchSemanticmap(const int business_mode, bool is_semantic_map, double time, avos::planning::LocalizationData &loc) const {
        std::queue<avos::planning::LocalizationData> temp = queue;
        AINFO_IF(FLAGS_enable_log) << "time size = " <<  temp.size() << "," << queue.size();
        bool weak_loc = false; 
        CustomStack::Instance()->GetProjectConfigValue("planning/cognition_config/weak_localization", weak_loc);
        double min_time = __DBL_MAX__;
        while (!temp.empty()) {
            double diff_time = std::abs(time - temp.front().time);
            if(weak_loc && use_dr_track_) {
                diff_time = std::abs(time - temp.front().dr_time);
            }
            if(diff_time < min_time){
                min_time = diff_time;
                loc = temp.front();
            }
            // AERROR_IF(FLAGS_enable_log) << "time: " << temp.front().time << ", diff time: " << diff_time ;
            temp.pop();
        }
        if (is_semantic_map) {
            if(min_time > 0.5){
                AERROR_IF(FLAGS_enable_log) << "语义地图匹配失败，时间差大于0.5秒";
                if(business_mode == 1) {
                    FaultHandle::FaultApi::Instance()->SetFaultCode("3072002", "语义地图定位时同匹配失败，时间差大于0.5秒");
                }
                else if(business_mode == 3 || business_mode == 31) {
                    FaultHandle::FaultApi::Instance()->SetFaultCode("3082011", "语义地图定位时同匹配失败，时间差大于0.5秒");
                }
            }
            else {
                if(business_mode == 1) {
                    FaultHandle::FaultApi::Instance()->ResetFaultCode("3072002");
                }
                else if(business_mode == 3 || business_mode == 31) {
                    FaultHandle::FaultApi::Instance()->ResetFaultCode("3082011");
                }
            }
        }
    }
};

//TODO非多线程，数据接收会存在丢失的情况
void ChangeSemanticMap(avos::planning::Perception& perception, avos::planning::LocalizationData& per_loc, avos::planning::LocalizationData& cur_loc){
    avos::vectormap::PointGCCS tmp_per_loc, sem_global_cell, tmp_cur_loc;
    avos::vectormap::PointVCS sem_cell;
    bool weak_loc = false; 
    CustomStack::Instance()->GetProjectConfigValue("planning/cognition_config/weak_localization", weak_loc);
    if(weak_loc && use_dr_track_) {
        tmp_per_loc.xg = per_loc.loc_xg_dr;
        tmp_per_loc.yg = per_loc.loc_yg_dr;
        tmp_per_loc.angle = per_loc.loc_yaw_dr;
        tmp_cur_loc.xg = cur_loc.loc_xg_dr;
        tmp_cur_loc.yg = cur_loc.loc_yg_dr;
        tmp_cur_loc.angle = cur_loc.loc_yaw_dr;
    }
    else {
        tmp_per_loc.xg = per_loc.xg;
        tmp_per_loc.yg = per_loc.yg;
        tmp_per_loc.angle = per_loc.yaw;
        tmp_cur_loc.xg = cur_loc.xg;
        tmp_cur_loc.yg = cur_loc.yg;
        tmp_cur_loc.angle = cur_loc.yaw;        
    }
    for(auto& cell : perception.perception_semantic_map.perception_semantic_map_data.semantic_map){
        if(is_in_utm_) {
            sem_global_cell.xg = cell.x;
            sem_global_cell.yg = cell.y;
            // AINFO_IF(FLAGS_enable_log) << "sem_global_cell.xg = " << sem_global_cell.xg << ",sem_global_cell.yg = " << sem_global_cell.yg << ",cell type = " << cell.type;
           m_transform.GCCS2VCS(tmp_cur_loc, sem_global_cell, sem_cell);
           cell.x = sem_cell.x;
           cell.y = sem_cell.y;
        }
        else {
            sem_cell.x = cell.x;
            sem_cell.y = cell.y;
            // AINFO_IF(FLAGS_enable_log) << "sem_global_cell.xg = " << sem_global_cell.xg << ",sem_global_cell.yg = " << sem_global_cell.yg << ",cell type = " << cell.type;
            m_transform.VCS2GCCS(tmp_per_loc, sem_cell, sem_global_cell);
            m_transform.GCCS2VCS(tmp_cur_loc, sem_global_cell, sem_cell);
            cell.x = sem_cell.x;
            cell.y = sem_cell.y;
        }
        
    //    AINFO_IF(FLAGS_enable_log) << "cell.x = " << cell.x << ",cell.y = " << cell.y << ",cell type = " << cell.type;

    }
}

void ChangePerceptionFusion(avos::planning::Perception &perception, avos::planning::LocalizationData &per_loc,
                            avos::planning::LocalizationData &cur_loc) {
    avos::vectormap::PointGCCS tmp_per_loc, sem_global_cell, tmp_cur_loc;
    avos::vectormap::PointVCS  sem_cell;
    bool                       weak_loc = false;
    CustomStack::Instance()->GetProjectConfigValue("planning/cognition_config/weak_localization", weak_loc);
    if (weak_loc && use_dr_track_) {
        tmp_per_loc.xg = per_loc.loc_xg_dr;
        tmp_per_loc.yg = per_loc.loc_yg_dr;
        tmp_per_loc.angle = per_loc.loc_yaw_dr;
        tmp_cur_loc.xg = cur_loc.loc_xg_dr;
        tmp_cur_loc.yg = cur_loc.loc_yg_dr;
        tmp_cur_loc.angle = cur_loc.loc_yaw_dr;

    } else {
        tmp_per_loc.xg = per_loc.xg;
        tmp_per_loc.yg = per_loc.yg;
        tmp_per_loc.angle = per_loc.yaw;
        tmp_cur_loc.xg = cur_loc.xg;
        tmp_cur_loc.yg = cur_loc.yg;
        tmp_cur_loc.angle = cur_loc.yaw;
    }
    for (auto &obj : perception.perception_fusion.perception_fusion_data.objects) {
        for (auto &cell : obj.cells) {
            // if(is_in_utm_) {
                sem_global_cell.xg = cell.x;
                sem_global_cell.yg = cell.y;
                m_transform.GCCS2VCS(tmp_cur_loc, sem_global_cell, sem_cell);
                cell.x = sem_cell.x;
                cell.y = sem_cell.y;
            // }
            // else {
            //     sem_cell.x = cell.x;
            //     sem_cell.y = cell.y;
            //     m_transform.VCS2GCCS(tmp_per_loc, sem_cell, sem_global_cell);
            //     m_transform.GCCS2VCS(tmp_cur_loc, sem_global_cell, sem_cell);
            //     cell.x = sem_cell.x;
            //     cell.y = sem_cell.y;
            // }
            
        }
    }
}

void CallbackVehicleStatus(int business_mode, const ara::adsfi::VehicleInformation& msg,
                           avos::planning::VehicleStatus& vehicle_status) {
    static double last_carstatus_time = 0.0;
    // vehicle_status.control_emergency = msg.VehicleActState.control_emergency;//暂未找到对应值
    vehicle_status.current_drive_mode = msg.vehicle_act_state.drive_mode;
    // vehicle_status.control_switch_state = msg.control_switch_state;//暂未找到对应值
    vehicle_status.epb_status = msg.vehicle_act_state.epb_status;
    vehicle_status.shift_position = msg.vehicle_act_state.shift_position;
    vehicle_status.speed = msg.vehicle_act_state.speed;
    AINFO_IF(FLAGS_enable_log) << "msg.velocity = " << msg.vehicle_act_state.speed;
    vehicle_status.steer_angle = msg.vehicle_act_state.steer_angle;
    // vehicle_status.vehicle_accel = msg.vehicle_accel;//暂未找到对应值
    if (last_carstatus_time > 0.0) {
        double time_diff = TimeToSec(msg.header.time) - last_carstatus_time;
        if (time_diff > 0.2) {
            AERROR_IF(FLAGS_enable_log) << "车辆状态消息超时 " << time_diff << "秒。";
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3072014", "车辆状态消息超时200ms");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3082001", "车辆状态消息超时200ms");
            }
        } else {
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3072014");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3082001");
            }
        }
    }
    last_carstatus_time = TimeToSec(msg.header.time);
}

/**
 * @brief 对单个动态障碍物先进行运动预测，再做坐标转换
 * @param raw_obj 输入的原始障碍物数据
 * @param cur_loc 当前时刻的定位
 * @param use_dr_follow 是否使用DR坐标系的标志位
 * @param time_diff 感知和当前的时间差 (秒)
 * @param final_obj 经过预测和转换后的最终障碍物数据
*/
void PredictAndTransformObject(const PerceptionObject& raw_obj,
                               const avos::planning::LocalizationData& cur_loc,
                               const bool use_dr_follow,
                               const double time_diff,
                               avos::planning::Object& final_obj) {
    // --- 1. 基础信息直接赋值 ---
    final_obj.id = raw_obj.id;
    final_obj.type = raw_obj.type;
    final_obj.age = raw_obj.age;
    final_obj.source = raw_obj.fusion_source;
    final_obj.length = raw_obj.obj_size.length;
    final_obj.width = raw_obj.obj_size.width;
    final_obj.height = raw_obj.obj_size.height;
    final_obj.is_static = (raw_obj.motion_state == 1); // 1:静止

    // --- 2. 先预测，后转换 ---
    // --- 2.1. 根据模式选择“过去时刻”的绝对坐标数据源 ---
    PerceptionAbsInfo past_abs_info; // 存储 t1 时刻的绝对信息
    if (use_dr_follow) {
        past_abs_info = raw_obj.dr_info;
    } else {
        past_abs_info = raw_obj.utm_info;
    }

    // 使用匀速模型预测障碍物在“当前时刻”的绝对位置
    // P_abs(t2) = P_abs(t1) + V_abs(t1) * Δt
    double predicted_xg = past_abs_info.xg + past_abs_info.vxg * time_diff;
    double predicted_yg = past_abs_info.yg + past_abs_info.vyg * time_diff;
    
    // 将预测出的绝对位置和速度赋值给最终的障碍物对象
    final_obj.xabs = predicted_xg;
    final_obj.yabs = predicted_yg;
    // 假设速度和航向角在短时间内保持不变
    final_obj.vxabs = past_abs_info.vxg;
    final_obj.vyabs = past_abs_info.vyg;
    final_obj.global_angle = past_abs_info.heading;
    final_obj.speed = past_abs_info.speed;

    // 将预测出的“当前绝对位置”转换到“当前车身坐标系”下
    avos::vectormap::PointGCCS obj_predicted_global_point;
    avos::vectormap::PointVCS obj_final_vehicle_point;
    avos::vectormap::PointGCCS cur_vehicle_pose;

    obj_predicted_global_point.xg = final_obj.xabs;
    obj_predicted_global_point.yg = final_obj.yabs;
    cur_vehicle_pose.xg = cur_loc.xg;
    cur_vehicle_pose.yg = cur_loc.yg;
    cur_vehicle_pose.angle = cur_loc.yaw;

    m_transform.GCCS2VCS(cur_vehicle_pose, obj_predicted_global_point, obj_final_vehicle_point);
    final_obj.x = obj_final_vehicle_point.x;
    final_obj.y = obj_final_vehicle_point.y;

    // --- 3. 相对速度赋值 ---
    final_obj.vxrel = raw_obj.vcs_info.vx;
    final_obj.vyrel = raw_obj.vcs_info.vy;
}

/**
 * @brief 回调函数，处理和补偿感知融合的动态障碍物
 * @param raw_objs 输入的原始障碍物列表
 * @param matched_loc 障碍物感知时刻匹配到的定位
 * @param current_loc 当前时刻的定位
 * @param perception 输出给规划的感知数据结构
*/
void CallbackPerceptionObjects(int business_mode, const PerceptionFinalObjects& raw_objs,
                               const avos::planning::LocalizationData& current_loc,
                               const bool use_dr_follow,
                               avos::planning::Perception& perception) {
    // --- 1. 计算时间差 ---
    double perception_time = raw_objs.header.time;
    double current_time = current_loc.time;
    AINFO_IF(FLAGS_enable_log) << "current_time = " << current_time << ", perception_time = " << perception_time;
    double time_diff = current_time - perception_time;

    // 清空上一帧数据，并更新时间戳
    perception.perception_fusion.perception_fusion_data.objects.clear();
    perception.perception_fusion.perception_fusion_data.timestamp = perception_time;

    // --- 2. 分情况处理 ---
    for (const auto& raw_obj : raw_objs.objs) {
        avos::planning::Object final_obj;
        // 基础信息直接赋值
        final_obj.id = raw_obj.id;
        final_obj.type = raw_obj.type;

        final_obj.length = raw_obj.obj_size.length;
        final_obj.width = raw_obj.obj_size.width;
        final_obj.height = raw_obj.obj_size.height;
        final_obj.is_static = (raw_obj.motion_state == 1); // 1:静止
        // 直接使用原始的相对坐标和绝对坐标，不进行任何时空转换
        final_obj.x = raw_obj.vcs_info.x;
        final_obj.y = raw_obj.vcs_info.y;
        final_obj.v_global_angle = raw_obj.vcs_info.heading;

        final_obj.vxrel = raw_obj.vcs_info.vx;
        final_obj.vyrel = raw_obj.vcs_info.vy;
    //TODO 此处暂时假定使用DR模式，后续根据实际情况调整
    // PerceptionAbsInfo abs_info = use_dr_follow ? raw_obj.dr_info : raw_obj.utm_info;
    PerceptionAbsInfo abs_info = raw_obj.dr_info;
        final_obj.xabs = abs_info.xg;
        final_obj.yabs = abs_info.yg;
        // AINFO_IF(FLAGS_enable_log) << "final_obj.xabs = " << final_obj.xabs  << ",final_obj.yabs = " <<final_obj.yabs;
        final_obj.vxabs = abs_info.vxg;
        final_obj.vyabs = abs_info.vyg;
        final_obj.global_angle = abs_info.heading;
        final_obj.speed = abs_info.speed;
    
        final_obj.cells.clear();
        avos::planning::ObjectCell cell;
        int num_points = 15;
        for (int i = 0; i < num_points; ++i) {
            double theta = 2.0 * M_PI * i / num_points;
            cell.x = final_obj.xabs +  (final_obj.width * 0.5 + 0.4)  * std::cos(theta);
            cell.y = final_obj.yabs +  (final_obj.width * 0.5 + 0.4) * std::sin(theta);
            final_obj.cells.push_back(cell);
        }
        // AINFO_IF(FLAGS_enable_log) << "final_obj.cells SIZE = " << final_obj.cells.size();
      
        
        perception.perception_fusion.perception_fusion_data.objects.push_back(final_obj);
    }
    if (time_diff < 0.5) {
        // 感知时间戳比定位时间戳更新，直接采纳感知数据。
        AWARN_IF(FLAGS_enable_log) << "[Data Sync] Perception data is newer than localization by " << -time_diff 
              << "s. Using raw perception data without transformation.";
        // 清除可能存在的超时故障码
        // FaultHandle::FaultApi::Instance()->ResetFaultCode("3072019");
            // 感知延迟在 0 到 0.2 秒的正常范围内
            if(business_mode == 1)
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3072019");
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3082010");
            }


        return; //直接返回
    } else if (time_diff >= 0.5) {
        // 感知延迟超过0.5秒
        AERROR_IF(FLAGS_enable_log) << "融合障碍物定位时同差距: " << time_diff << "s.";
        if(business_mode == 1) {
            FaultHandle::FaultApi::Instance()->SetFaultCode("3072019", "融合障碍物定位时同匹配失败，时间差大于0.5秒");
        }
        else if(business_mode == 3 || business_mode == 31) {
            FaultHandle::FaultApi::Instance()->SetFaultCode("3082010", "融合障碍物定位时同匹配失败，时间差大于0.5秒");
        }
    }

    // --- 3. 执行预测+转换---
    for (const auto& raw_obj : raw_objs.objs) {
        avos::planning::Object final_obj;
        PredictAndTransformObject(raw_obj, current_loc, use_dr_follow, time_diff, final_obj);
        perception.perception_fusion.perception_fusion_data.objects.push_back(final_obj);
    }

    // 增加对障碍物消息的超时检测
    static double last_obj_time = 0.0;
    if (last_obj_time > 0.0) {
        double time_diff = raw_objs.header.time - last_obj_time;
        if (time_diff > 0.5) {
            AERROR_IF(FLAGS_enable_log) << "感知融合障碍物消息超时 " << time_diff << "秒。";
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3072018", "感知障碍物数据接收超时500ms");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->SetFaultCode("3082009", "感知障碍物数据接收超时500ms");
            }
        } else {
            if(business_mode == 1) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3072018");
            }
            else if(business_mode == 3 || business_mode == 31) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode("3082009");
            }
        }
    }
    last_obj_time = raw_objs.header.time;
}


void XBridge(const ara::adsfi::BusinessCommand& task_cmd ,  // 业务命令 
            const ara::adsfi::MsgHafLocation& loc ,    // 定位原始数据   
            const ara::adsfi::PerceptionStaticEnv& semanticmap ,    // 语义地图数据   
            const PerceptionFinalObjects& perception_objs ,     // 感知融合障碍物数据    
            const ara::adsfi::VehicleInformation& car_status ,  // 底盘状态 
            avos::planning::Perception& perception ,    // 感知数据 
            avos::planning::LocalizationData& localization_data ,   // 定位数据 
            avos::planning::VehicleStatus& vehicle_status) {
                auto start = std::chrono::high_resolution_clock::now();
    use_dr_follow_ = (task_cmd.business_mode == 31);
    use_dr_track_ = (task_cmd.business_mode == 11);

    static FixedSizeQueue local_queue(10);
    // 将use_dr_follow传入定位回调函数
    CallbackLocalization(task_cmd.business_mode, loc, use_dr_follow_, localization_data);
    local_queue.push(localization_data);
    // --- 处理语义地图 ---
    CallbackSemanticMap(task_cmd.business_mode, semanticmap, perception);
    avos::planning::LocalizationData match_loc;
    local_queue.MatchSemanticmap(task_cmd.business_mode, true, perception.perception_semantic_map.perception_semantic_map_data.timestamp, match_loc);
    ChangeSemanticMap(perception, match_loc, localization_data);
    // localization_data = match_loc;
    // --- 处理动态障碍物 ---
    CallbackPerceptionObjects(task_cmd.business_mode, perception_objs, localization_data, use_dr_follow_, perception);
    local_queue.MatchSemanticmap(task_cmd.business_mode, false, perception.perception_fusion.perception_fusion_data.timestamp, match_loc);
    ChangePerceptionFusion(perception, match_loc, localization_data);

    CallbackVehicleStatus(task_cmd.business_mode, car_status, vehicle_status);
    auto end = std::chrono::high_resolution_clock::now();
    auto cpu_time_used = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    AINFO_IF(FLAGS_enable_log) << "bridge time used: " << cpu_time_used.count() / 1000.0 << " ms\n";
}
