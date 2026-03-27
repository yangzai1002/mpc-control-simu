#ifndef __ADSFI_STRUCT_ADAPTER_H__
#define __ADSFI_STRUCT_ADAPTER_H__

// 引入所有需要的源类型和目标类型头文件
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "PlanningTrajectory.h"
#include "PerceptionFinalObjects.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"   
#include "ara/adsfi/impl_type_vehicleinformation.h"  
#include "ara/adsfi/impl_type_businesscommand.h"         
#include "ara/adsfi/impl_type_perceptionstaticenv.h" 
#include "ara/adsfi/impl_type_faultdata.h"   
#include "ara/adsfi/impl_type_planningstatus.h"

#include <cmath>
#include <memory>
#include <vector>

namespace adsfi {
namespace utils {

    // =========================================================
    // 转换 1: 官方 MsgHafEgoTrajectory -> 内部 PlanningTrajectory
    // =========================================================
    inline void ConvertMsgHafToPlanningTraj(const ara::adsfi::MsgHafEgoTrajectory& in, PlanningTrajectory& out)
    {
        // 1. 转换头部时间戳
        out.header.time = TimeToSec(in.egoTrajectoryHeader.timestamp);
        out.header.seq = in.egoTrajectoryHeader.seq;
        out.header.frame_id = in.egoTrajectoryHeader.frameID;

        // 2. 解析方向 (基于档位)
        // 内部定义: direction 0 = 前进, 1 = 倒车
        int internal_direction = 0;
        if (in.gear == 1) { 
            internal_direction = 1; 
        }

        // 3. 填充附加信息
        out.additional_info.direction = internal_direction;
        out.additional_info.emergency_stop = (in.estop.isEstop > 0); 

        // 4. 转换轨迹点
        size_t point_size = in.trajectoryPoints.size();
        out.trajectory.clear();
        out.trajectory.reserve(point_size);

        for (const auto& src_pt : in.trajectoryPoints) {
            PlanningTrajectoryPoint dst_pt;

            //运动学属性
            dst_pt.velocity = static_cast<float>(src_pt.speed);
            dst_pt.a = static_cast<float>(src_pt.acc);
            dst_pt.t = static_cast<float>(src_pt.timeRelative);
            
            // 绝对坐标 (UTM)
            dst_pt.xg = src_pt.wayPoint.xg;
            dst_pt.yg = src_pt.wayPoint.yg;
            dst_pt.angleglobal = static_cast<float>(src_pt.wayPoint.theta); // theta 为全局航向

            // 相对坐标
            dst_pt.x = static_cast<float>(src_pt.wayPoint.x);
            dst_pt.y = static_cast<float>(src_pt.wayPoint.y);
            dst_pt.angle = static_cast<float>(src_pt.wayPoint.angle); // 相对角度

            // 路径属性
            dst_pt.curvature = static_cast<float>(src_pt.wayPoint.curvature);
             dst_pt.length = src_pt.length;
             // dst_pt.length = static_cast<float>(src_pt.wayPoint.s);
            // 状态属性
            dst_pt.direction = static_cast<char>(internal_direction);
            dst_pt.risk_level = static_cast<int>(src_pt.risk_level);

            out.trajectory.push_back(dst_pt);
        }
    }
    // =========================================================
    // 转换 2: 内部 PlanningTrajectory -> 官方 MsgHafEgoTrajectory
    // =========================================================
    inline void ConvertTrajectoryToMsgHafEgoTrajectory(const PlanningTrajectory & in, ara::adsfi::MsgHafEgoTrajectory &out)
    {
        // 1. 转换头部信息
        // 使用辅助函数转换时间戳 (double -> MsgHafTime)
        out.egoTrajectoryHeader.timestamp = DoubleToHafTime(in.header.time);
        out.egoTrajectoryHeader.seq = static_cast<uint32_t>(in.header.seq);
        out.egoTrajectoryHeader.frameID = in.header.frame_id;

        // 2. 准备容器
        size_t point_size = in.trajectory.size();
        out.trajectoryPoints.clear();
        out.trajectoryPoints.reserve(point_size);
        out.wayPoints.clear();
        out.wayPoints.reserve(point_size);

        // 3. 遍历并转换轨迹点
        for (const auto& src_pt : in.trajectory) {
            ara::adsfi::MsgHafTrajectoryPoint dst_pt;

            // --- 填充 Header (复用外层 Header) ---
            dst_pt.header = out.egoTrajectoryHeader;
            dst_pt.wayPoint.header = out.egoTrajectoryHeader;

            // --- 填充物理/运动学属性 ---
            dst_pt.speed = static_cast<double>(src_pt.velocity);
            dst_pt.acc = static_cast<double>(src_pt.a);
            dst_pt.timeRelative = static_cast<double>(src_pt.t);
            dst_pt.length = src_pt.length;
            dst_pt.risk_level = static_cast<int32_t>(src_pt.risk_level);
            // dst_pt.deltaAcc = 0.0;
            // dst_pt.steerAngle = 0.0; 

            // --- 填充 WayPoint 信息 ---
            dst_pt.wayPoint.x = static_cast<double>(src_pt.x);
            dst_pt.wayPoint.y = static_cast<double>(src_pt.y);
            dst_pt.wayPoint.angle = static_cast<double>(src_pt.angle);

            // 绝对坐标 (UTM/Global)
            dst_pt.wayPoint.xg = src_pt.xg;
            dst_pt.wayPoint.yg = src_pt.yg;
            // dst_pt.wayPoint.zg = 0.0;

            // 路径属性
            // angleglobal 对应绝对航向 theta
            dst_pt.wayPoint.theta = static_cast<double>(src_pt.angleglobal);
            dst_pt.wayPoint.curvature = static_cast<double>(src_pt.curvature);
            // length 对应累积里程 s
            dst_pt.wayPoint.s = static_cast<double>(src_pt.length);

            // 推入向量
            out.trajectoryPoints.push_back(dst_pt);
            // 通常 MsgHafEgoTrajectory 的 wayPoints 也需要同步填充?
            // out.wayPoints.push_back(dst_pt.wayPoint);
        }

        // 5. 填充统计信息
        if (!in.trajectory.empty()) {
            out.trajectoryLength = static_cast<double>(in.trajectory.back().length);
            out.trajectoryPeriod = static_cast<double>(in.trajectory.back().t);
        } else {
            out.trajectoryLength = 0.0;
            out.trajectoryPeriod = 0.0;
        }

        // 6. 填充档位与状态
        // internal_direction: 0=前进, 1=倒车
        int internal_direction = in.additional_info.direction;
        
        if (internal_direction == 0) {
            out.gear = 3; // Drive (前进)
        } else if (internal_direction == 1) {
            out.gear = 1; // Reverse (倒车)
        } else {
            out.gear = 0; // Parking
        }

        // Estop 状态
        out.estop.isEstop = (in.additional_info.emergency_stop) ? 1 : 0;
    }

    // 2. 外部 MsgHafFusionOutArray -> 内部 PerceptionFinalObjects
    inline void ConvertFusionToFinalObjects(const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> in_ptr, PerceptionFinalObjects & out)
    {
    // 1. 转换头部信息
    // 注意: MsgHafTime 是结构体 {sec, nsec}，内部 header.time 是 double
    double timestamp = static_cast<double>(in_ptr->stamp.sec) + 
                       static_cast<double>(in_ptr->stamp.nsec) * 1e-9;
    
    out.header.time = timestamp;
    out.header.seq = in_ptr->seq;
    out.header.data_name = in_ptr->frameID;
    
    // 转换时间链信息
    if (!in_ptr->time_chain_vec.empty()) {
        out.start_stamp = in_ptr->time_chain_vec[0].t1;
        out.send_stamp = in_ptr->time_chain_vec[0].t2;
    }
    
    // 2. 转换对象列表
    out.objs.clear();
    
    for (const auto& in_obj : in_ptr->fusionOut) {
        PerceptionObject out_obj;
        
        // --- 基础信息 ---
        out_obj.id = in_obj.objectID;
        out_obj.type = in_obj.cs; // 类型字段对应 cs
        
        // --- 相对坐标系信息 (VCS / rect) ---
        // 位置: 使用 rect.center (MsgPoint3D -> double x, y, z)
        out_obj.vcs_info.x = in_obj.rect.center.x;
        out_obj.vcs_info.y = in_obj.rect.center.y;
        out_obj.vcs_info.z = in_obj.rect.center.z;
        
        // 航向角: 使用 rect.orientation 注意单位确认
        out_obj.vcs_info.heading = in_obj.rect.orientation; 
        
        // 速度: 使用 velocity (MsgPoint3f)
        out_obj.vcs_info.vx = static_cast<float>(in_obj.velocity.x);
        out_obj.vcs_info.vy = static_cast<float>(in_obj.velocity.y);
        
        // 尺寸: 使用 rect.size (MsgPoint3D)
        out_obj.obj_size.length = static_cast<float>(in_obj.rect.size.x); // x 是长
        out_obj.obj_size.width = static_cast<float>(in_obj.rect.size.y);  // y 是宽
        out_obj.obj_size.height = static_cast<float>(in_obj.rect.size.z);
        
        // --- DR 坐标信息 (绝对坐标对应 dr_info) ---
        out_obj.dr_info.valid = true;
        out_obj.dr_info.xg = in_obj.absRect.center.x;
        out_obj.dr_info.yg = in_obj.absRect.center.y;
        out_obj.dr_info.heading = in_obj.absRect.orientation;
        out_obj.dr_info.vxg = static_cast<float>(in_obj.absVelocity.x);
        out_obj.dr_info.vyg = static_cast<float>(in_obj.absVelocity.y);
        out_obj.dr_info.speed = std::hypot(out_obj.dr_info.vxg, out_obj.dr_info.vyg);
        
        // --- UTM 坐标信息 (同 dr_info) ---
        out_obj.utm_info = out_obj.dr_info;
        
        // --- 状态标志 ---
        // carlight 映射: blinkerFlag
        out_obj.carlight = in_obj.blinkerFlag;
        
        // 运动状态: 可以根据速度判断
        if (out_obj.dr_info.speed < 0.1) {
            out_obj.motion_state = 1; // 静止
        } else {
            out_obj.motion_state = 3; // 运动
        }
        
        // 读取 rect.corners 并赋值给 PerceptionObject 的 cells
        out_obj.vcs_info.cells.clear();
        for(const auto& corner : in_obj.rect.corners) {
            CommonPoint3f p;
            p.x = static_cast<float>(corner.x);
            p.y = static_cast<float>(corner.y);
            p.z = static_cast<float>(corner.z);
            out_obj.vcs_info.cells.push_back(p);
        }
        
        // 读取 rect_2d 转换为 img_info.rect (2D框信息)
        if (in_obj.rect_2d.corners.size() >= 4) {
            // 从四个角点恢复左上和右下
            float x_min = in_obj.rect_2d.location.x;
            float y_min = in_obj.rect_2d.location.y;
            float x_max = x_min + in_obj.rect_2d.size.x;
            float y_max = y_min + in_obj.rect_2d.size.y;
            
            out_obj.img_info.rect.clear();
            CommonPoint2f p1, p2;
            p1.x = x_min;
            p1.y = y_min;
            p2.x = x_max;
            p2.y = y_max;
            out_obj.img_info.rect.push_back(p1);
            out_obj.img_info.rect.push_back(p2);
        }

        out.objs.push_back(out_obj);
    }
    }


} // namespace utils
} // namespace adsfi

#endif // __ADSFI_STRUCT_ADAPTER_H__
