#ifndef XDRAW_DRIVABLE_AREA_H
#define XDRAW_DRIVABLE_AREA_H

#include "../VideoParameter.h"
#include "PerceptionVPResult.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_msgcameravpresult.h"
#include "ara/adsfi/impl_type_msgimagedata.h"

/**
 * @brief 道路可通行区域叠加功能
 * 
 * 功能说明：
 * 1. 根据AppVideoProfile中的drivable_area开关状态（0/1）决定是否叠加
 * 2. 上游感知节点输出视觉检测结果，使用语义分割算法
 * 3. 取像素值为1的区域作为可通行区域
 * 4. 仅处理front相机的检测叠加（frameID == "put_front"）
 * 5. 需要对加密的语义分割数据进行RLE解密
 * 6. 用绿色图层叠加当前车辆可通行区域
 * 
 * 数据流程：
 * MsgCameraVPResult -> PerceptionVPResult (转换+解密) -> 提取seg数据 -> 叠加到图像
 * 
 * 调用位置：
 * 应作为独立线程调用，类似XDrawAssistLines，在draw_obstacles之后、layout之前
 * 
 * @param image_data 图像数据（输入输出）
 * @param vp_result 视觉感知结果（包含语义分割数据）
 * @param video_profile 视频配置参数（包含drivable_area开关）
 * @return true表示处理成功
 */
bool XDrawDrivableArea(const ara::adsfi::MsgImageData& image_data, const ara::adsfi::MsgCameraVPResult& vp_result,
                       const compatible::VideoParameter& video_parameter);

#endif // XDRAW_DRIVABLE_AREA_H
