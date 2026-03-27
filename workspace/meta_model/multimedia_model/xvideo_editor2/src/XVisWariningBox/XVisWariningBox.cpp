#include <algorithm>
#include <iomanip> // 添加这行
#include <sstream> // 通常也需要

#include "DatetimeUtil.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_msgcameravpresult.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "custom_stack.h"
#include "fmt/format.h"
// 定义边界框结构体 (左上角(x1,y1), 右下角(x2,y2))
struct BoundingBox {
    float x1, y1, x2, y2;
};

// 快速判断两个框是否可能重叠（无分支，高效）
inline bool may_overlap(const BoundingBox& a, const BoundingBox& b) {
    return !(a.x2 < b.x1 || b.x2 < a.x1 || a.y2 < b.y1 || b.y2 < a.y1);
}

// 计算两个边界框的IoU (Intersection over Union)
float iou(const BoundingBox& box1, const BoundingBox& box2) {
    // 计算交集区域
    float inter_x1 = std::max(box1.x1, box2.x1);
    float inter_y1 = std::max(box1.y1, box2.y1);
    float inter_x2 = std::min(box1.x2, box2.x2);
    float inter_y2 = std::min(box1.y2, box2.y2);

    float inter_width = std::max(0.0f, inter_x2 - inter_x1);
    float inter_height = std::max(0.0f, inter_y2 - inter_y1);
    float inter_area = inter_width * inter_height;

    // 计算并集区域
    float area1 = (box1.x2 - box1.x1) * (box1.y2 - box1.y1);
    float area2 = (box2.x2 - box2.x1) * (box2.y2 - box2.y1);
    float union_area = area1 + area2 - inter_area;

    // 防止除0（比如两个点框）
    return (union_area <= 0.0f) ? 0.0f : (inter_area / union_area);
}

void XVisWariningBox(const ara::adsfi::MsgCameraVPResult&   dl_vision_result, // 视觉检测结果
                     const ara::adsfi::PerceptionStaticEnv& out_sematic_map,
                     ara::adsfi::MsgHafFusionOutArray&      tracking_result) {
    // add your code here
    // 图像框输出
    ApInfo("video_editor") << fmt::format(
        "XVisWariningBox start, dl_vision_result, frameID: {}, timestamp: {:20.8f}, size: {}",
        dl_vision_result.header.frameID,
        app_common::DatetimeUtil::double_time_from_sec_nsec(dl_vision_result.header.timestamp),
        dl_vision_result.vp_result_vec.size());
    ApInfo("video_editor") << fmt::format(
        "XVisWariningBox start, out_sematic_map, timestamp: {:20.8f}, size: {}",
        app_common::DatetimeUtil::double_time_from_sec_nsec(out_sematic_map.header.time),
        out_sematic_map.semantic_objs.size());

    static bool init = false;
    static bool vis_obj = false;
    if (!init) {
        init = true;
        CustomStack::Instance()->GetProjectConfigValue("Xvis_obj/filter_obj", vis_obj);
    }

    for (auto& cam : dl_vision_result.vp_result_vec) {
        for (auto& obj : cam.image_objects.detectionOut2d) {
            // cam.camera_name = ;
            // if(obj.object_3d.enable) continue;
            ApInfo("video_editor") << "cam.camera_name" << cam.header.frameID;
            if (cam.header.frameID == "front_h100" && (obj.objectId <= 9)) {
                ara::adsfi::MsgHafFusionOut p_obj;
                // TODO
                p_obj.fusionType = obj.cls;
                ApInfo("video_editor") << "p_obj.type: " << static_cast<int32_t>(p_obj.fusionType);
                ara::adsfi::MsgPoint2D point;
                point.x = obj.rect.location.x;
                point.y = obj.rect.location.y;
                p_obj.rect_2d.corners.push_back(point);
                point.x = obj.rect.location.x + obj.rect.size.x;
                point.y = obj.rect.location.y + obj.rect.size.y;
                p_obj.rect_2d.corners.push_back(point);
                // p_obj.reserved.push_back(obj.object_2d.prob2d);
                tracking_result.fusionOut.push_back(p_obj);
            }
        }
    }

    for (auto& obj : out_sematic_map.semantic_objs) {
        bool iou_flag = false;
        if (obj.type == 26 || obj.type == 27) { //正负障碍
            if (obj.cells.size() < 2)
                continue; // 跳过无效障碍物
            BoundingBox box1;
            box1.x1 = obj.cells[0].x;
            box1.y1 = obj.cells[0].y;
            box1.x2 = obj.cells[1].x;
            box1.y2 = obj.cells[1].y;
            for (auto& obj_old : tracking_result.fusionOut) {
                if (obj_old.rect_2d.corners.size() < 2)
                    continue; // 跳过无效框
                BoundingBox box_temp;
                box_temp.x1 = obj_old.rect_2d.corners[0].x;
                box_temp.y1 = obj_old.rect_2d.corners[0].y;
                box_temp.x2 = obj_old.rect_2d.corners[1].x;
                box_temp.y2 = obj_old.rect_2d.corners[1].y;
                if (vis_obj && may_overlap(box1, box_temp)) {
                    float iou_temp = iou(box1, box_temp);
                    if (iou_temp >= 0.05) {
                        iou_flag = true;
                        break;
                    }
                }
                // else {
                //     iou_flag = false;
                // }
            }

            ara::adsfi::MsgHafFusionOut p_obj;
            p_obj.fusionType = obj.type;
            // ApInfo("video_editor") << "p_obj.type" << p_obj.type;
            ara::adsfi::MsgPoint2D point;
            for (auto& cell : obj.cells) {
                point.x = cell.x;
                point.y = cell.y;
                p_obj.rect_2d.corners.push_back(point);
            }
            // p_obj.reserved.push_back(obj.object_2d.prob2d);
            tracking_result.fusionOut.push_back(p_obj);
        }
    }

    tracking_result.stamp = dl_vision_result.header.timestamp; // 时间戳
    tracking_result.frameID = dl_vision_result.header.frameID; // 帧ID
    tracking_result.seq = dl_vision_result.header.seq;         // 序列号
    tracking_result.count = tracking_result.fusionOut.size();

    ApInfo("video_editor") << "XVisWariningBox end";
}
